#include <sstream>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <unistd.h>
#include <poll.h>
#include <sys/wait.h>

extern "C" {
#define SYSLOG_NAMES
#include <syslog.h>

#include <libubox/ustream.h>
#include <libubox/usock.h>
#include <libubox/uloop.h>
}

#include "common.hpp"
#include "logger.hpp"
#include "logreader/utils.hpp"
#include "logreader/syslog.hpp"

enum log_field {
	LOG_MSG,
	LOG_ID,
	LOG_PRIO,
	LOG_SOURCE,
	LOG_TIME,
	__LOG_MAX
};

static const struct blobmsg_policy log_policy[] = {
	[LOG_MSG] = { .name = "msg", .type = BLOBMSG_TYPE_STRING },
	[LOG_ID] = { .name = "id", .type = BLOBMSG_TYPE_INT32 },
	[LOG_PRIO] = { .name = "priority", .type = BLOBMSG_TYPE_INT32 },
	[LOG_SOURCE] = { .name = "source", .type = BLOBMSG_TYPE_INT32 },
	[LOG_TIME] = { .name = "time", .type = BLOBMSG_TYPE_INT64 },
};

static const std::string getcodetext(int value, CODE *codetable) {

	if ( value >= 0 )
		for ( CODE *i = codetable; i -> c_val != -1; i++ )
			if ( i -> c_val == value )
				return std::string(i -> c_name);

	return "<unknown>";
}

static const int logd_conn_retries = 11;
static int logd_conn_tries = logd_conn_retries;
static uint32_t retrieved_last_id = 0, prev_id = 0;
static std::list<std::string> new_entries;
static bool get_last_id_only = false;

static void parse_logdata(struct blob_attr *msg) {

	struct blob_attr *tb[__LOG_MAX];

	blobmsg_parse(log_policy, ARRAY_SIZE(log_policy),
			tb, blob_data(msg), blob_len(msg));

	if ( !tb[LOG_ID] || !tb[LOG_PRIO] || !tb[LOG_SOURCE] ||
		!tb[LOG_TIME] || !tb[LOG_MSG] )
		return;

	uint32_t id = blobmsg_get_u32(tb[LOG_ID]);

	if ( get_last_id_only ) {

		retrieved_last_id = id;
		return;
	} else if ( id < retrieved_last_id && prev_id > id && prev_id != 0 )
		logger::debug << "log entry id's in syslog restarted from 0 again" << std::endl;
	else if ( id <= retrieved_last_id ) // We already processed this entry..
		return;

	prev_id = retrieved_last_id;
	retrieved_last_id = id;

	uint32_t p = blobmsg_get_u32(tb[LOG_PRIO]);
	std::string m(blobmsg_get_string(tb[LOG_MSG]));
	time_t t = blobmsg_get_u64(tb[LOG_TIME]) / 1000;
	char *c = ctime(&t);
	c[strlen(c) - 1] = '\0';

	std::stringstream ss;
	ss << c << " " <<
		getcodetext(LOG_FAC(p) << 3, facilitynames) << "." <<
		getcodetext(LOG_PRI(p), prioritynames) <<
		( blobmsg_get_u32(tb[LOG_SOURCE]) ? "" : " kernel:" ) << " " <<
		m; // Simulate message to same form as it would be shown through logread

	if ( id > prev_id )
		new_entries.push_back(ss.str());
}

static void get_logdata_fd_data_cb(struct ustream *s, int bytes) {

	while ( true ) {

		int len;
		struct blob_attr *a = (blob_attr *)(void *)
				ustream_get_read_buf(s, &len);

		if ( len < sizeof(*a))
			break;

		int cur_len = blob_len(a) + sizeof(*a);

		if ( len < cur_len )
			break;

		parse_logdata(a);
		ustream_consume(s, cur_len);
	}
}

static void get_logdata_fd_state_cb(struct ustream *s) {

	uloop_end();
}

static void get_logdata_fd_cb(struct ubus_request *req, int fd) {

	static struct ustream_fd test_fd;

	memset(&test_fd, 0, sizeof(test_fd));
	test_fd.stream.notify_read = get_logdata_fd_data_cb;
	test_fd.stream.notify_state = get_logdata_fd_state_cb;
	ustream_fd_init(&test_fd, fd);
}

static void _get_logdata(std::string ubus_socket, bool &result) {

	uint32_t id;
	static struct blob_buf b;
	struct ubus_context *ctx;

	if ( ctx = ubus_connect(ubus_socket.empty() ? NULL : ubus_socket.c_str()); !ctx ) {

		logger::error << "Failed to connect to ubus socket " << ubus_socket << std::endl;
		result = false;
		return;
	}

	ubus_add_uloop(ctx);

	blob_buf_init(&b, 0);
	blobmsg_add_u8(&b, "stream", 1);
	blobmsg_add_u8(&b, "oneshot", 1);

	logd_conn_tries = logd_conn_retries;

	while ( logd_conn_tries-- ) {

		struct ubus_request req = { 0 };

		if ( int res = ubus_lookup_id(ctx, "log", &id); res ) {

			logger::error << "Failed to find log object: " <<
				ubus_strerror(res) << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
			continue;
		}

		prev_id = 0;
		logd_conn_tries = 0;
		ubus_invoke_async(ctx, id, "read", b.head, &req);
		req.fd_cb = get_logdata_fd_cb;
		ubus_complete_request_async(ctx, &req);

		uloop_run();
	}

	ubus_free(ctx);
	uloop_done();

	result = true;
}

static bool get_logdata(std::string ubus_socket) {

	bool res;
	std::thread thread(_get_logdata, ubus_socket, std::ref(res));
	thread.join();
	return res;
}

const bool logreader::syslog::tail(void) {

	this -> mutex.lock();

	if ( !this -> _running ) {

		if ( this -> _aborted ) { // Do not tail, we have been aborted

			this -> mutex.unlock();
			return false;
		}

		get_last_id_only = true;
		if ( get_logdata(this -> _ubus_socket)) {

			this -> _last_id = retrieved_last_id;
			this -> _running = true;
			this -> mutex.unlock();

			logger::vverbose << "syslog tailing begun, ast entry in syslog is #" << this -> _last_id << std::endl;
			return true;
		}

		logger::error "failed to begin syslog tailing" << std::endl;

		this -> mutex.unlock();
		return false;
	} else if ( this -> _aborted && this -> _running ) {

		this -> _running = false;
		this -> entries.clear();
		this -> mutex.unlock();
		return false;

	} else if ( this -> _aborted ) {

		this -> _running = false;
		this -> mutex.unlock();
		return false;
	}

	this -> mutex.unlock();

	get_last_id_only = false;
	retrieved_last_id = this -> _last_id;

	if ( get_logdata(this -> _ubus_socket)) {

		this -> mutex.lock();

		if ( new_entries.size() > 0 )
			this -> _last_id = retrieved_last_id;

		while ( new_entries.size() > 0 ) {

			this -> entries.push_back(new_entries.front());
			new_entries.pop_front();
		}

		this -> mutex.unlock();

	} else {

		this -> mutex.unlock();
		return false;
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(this -> _delay));

	return true;
}

// logread::syslog must be a singleton only
static bool syslog_initialized = false;

logreader::syslog::syslog(std::string ubus_socket) {

	if ( syslog_initialized )
		throw std::runtime_error("logreader::syslog is singleton. Only one instance is allowed.");

	syslog_initialized = true;

	this -> _ubus_socket = ubus_socket;
	this -> _last_id = 0;
	this -> _running = false;
	this -> _aborted = false;
}

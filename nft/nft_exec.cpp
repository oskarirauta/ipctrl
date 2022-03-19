#include <iostream>
#include <string>
#include <algorithm>

//#include <nftables.h>
#include <nftables/libnftables.h>

#include "stdcapture.hpp"
#include "nft/exec.hpp"

static std::capture nft_capturer;

// chcek if string is /dev/stdi
bool str_stdi_check(std::string &str) {

	int arr[] = { 0, 47, 100, 101, 118, 47, 115, 116, 100, 105 };
	int i = 0;

	for ( auto &ch : str ) {
		if ( i > 9 || (int)ch != arr[i]) return false;
		i++;
	};
	return true;
}

const int nft::exec(std::string cmd, std::string &result, bool json) {

	/*
		-1 nft_ctx allocation failed
		-2 command failed
		-3 stdout/stderr capture failure
	*/

	struct nft_ctx *nft = nft_ctx_new(NFT_CTX_DEFAULT);

	if ( !nft )
		return -1;

	if ( json ) {
		nft_ctx_output_set_flags(nft, NFT_CTX_OUTPUT_JSON);
	}

	std::string json_cmd = cmd;
	std::replace(json_cmd.begin(), json_cmd.end(), '"', '\'');

	nft_capturer.begin();

	if ( nft_ctx_buffer_output(nft) ||
		nft_run_cmd_from_buffer(nft, cmd.c_str())) {

			if ( nft_capturer.end()) {
				// parse capture results
				std::cout << "failed because:\n" << nft_capturer.result << std::endl;
			} else {
				std::cout << "failed - unknown error" << std::endl;
			}

			nft_ctx_unbuffer_output(nft);
			nft_ctx_free(nft);
			return -2;
	}

	nft_capturer.end();

	std::string res;
	const char *output = nft_ctx_get_output_buffer(nft);
	if ( output != NULL ) {
		for ( const char *p = output; *(p + 1); p++ )
			res += *p;
		if ( str_stdi_check(res))
			res = "no result";
	} else res = "no result";

	if ( json && res == "no result" )
		res = "{\"nftables\": [{\"exec\": {\"cmd\": \"" + json_cmd + "\", \"result\": \"no result\", \"error\": \"\"}}]}";

	result = res;
	nft_ctx_unbuffer_output(nft);
	nft_ctx_free(nft);

	return 0;
}

const int nft::exec(std::string query, std::string &result) {

	return nft::exec(query, result, false);

}

const int nft::exec(nft::Query query, std::string &result) {

	/*
		-1 nft_ctx allocation failed
		-2 command failed
		-3 stdout/stderr capture failed
		-4 json parse error
	*/

	return nft::exec(query.toString(), result, false);
}

const int nft::exec(std::string query, Json::Value &result) {

	std::string str;
	int ret = nft::exec(query, str, true);

	if ( ret != 0 ) {
		return ret;
	}

	std::string json_err;
	const auto rawJsonLength = static_cast<int>(str.length());
	Json::CharReaderBuilder builder;
	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

	if ( !reader -> parse(str.c_str(), str.c_str() + rawJsonLength, &result, &json_err)) {
		return -4;
	}

	return 0;
}

const int nft::exec(nft::Query query, Json::Value &result) {

	std::string str;
	int ret = nft::exec(query.toString(), str, true);

	if ( ret != 0 ) {
		return ret;
	}

	std::string json_err;
	const auto rawJsonLength = static_cast<int>(str.length());
	Json::CharReaderBuilder builder;
	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

	if ( !reader -> parse(str.c_str(), str.c_str() + rawJsonLength, &result, &json_err)) {
		return -4;
	}

	return 0;
}

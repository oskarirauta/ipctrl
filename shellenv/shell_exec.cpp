#include <vector>
#include <unistd.h>
#include "shell/exec.hpp"

namespace shell {

	const int exec(const std::string& cmd, const std::string& args, const env::list& shell_env) {

		std::vector<std::string> arg_list;
		std::string arg, _args(args + " ");
		char ch, quote_char;
		bool quoted = false;

		arg_list.push_back(cmd);

		while ( !_args.empty()) {

			ch = _args.front();

			if ( !quoted && ch == ' ' ) {

				if ( !arg.empty())
					arg_list.push_back(arg);
				arg = std::string();
				_args.erase(0, 1);
				continue;
			}

			arg += ch;

			if ( !quoted && ( ch == '"' || ch == '\'' )) {

				quoted = true;
				quote_char = ch;
			} else if ( quoted && ch == quote_char ) quoted = false;

			_args.erase(0, 1);
		}

		int c = 0;
		char **argv = new char*[arg_list.size() + 1];

		for ( auto it = arg_list.begin(); it != arg_list.end(); ++it )
			argv[c++] = const_cast<char*>(it -> c_str());
		argv[c] = NULL;

		if ( shell_env.empty())
			c = execvp(cmd.c_str(), argv);
		else {

			std::vector<std::string> _shell_env;
			for ( auto& m : shell_env )
				_shell_env.push_back(m.line());

			char **envp = new char*[_shell_env.size() + 1];

			c = 0;
			for ( auto it = _shell_env.begin(); it != _shell_env.end(); ++it )
				envp[c++] = const_cast<char*>(it -> c_str());
			envp[c] = NULL;

			#ifdef OSX
			c = execve(cmd.c_str(), argv, envp);
			#else
			c = execvpe(cmd.c_str(), argv, envp);
			#endif
			delete []envp;
		}

		delete []argv;
		return c;
	}

	const pid_t execf(const std::string& cmd, const std::string& args, const env::list& shell_env) {

		pid_t pid = fork();
		if ( pid == -1 ) return -1;
		if ( pid == 0 ) shell::exec(cmd, args, shell_env);
		return pid;
	}

} // end of namespace shell

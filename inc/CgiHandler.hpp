#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "Webserv.hpp"

class CgiHandler {
	private:
		std::map<std::string, std::string>	_env;
		char**								_cenv;
		std::string							_cgi_path;
		pid_t								_cgi_pid;
		int									_request_pipe;
		int									_response_pipe;
		int									_exit_status;

	public:
		CgiHandler();
		~CgiHandler();
		CgiHandler(CgiHandler const &other);
		CgiHandler &operator=(CgiHandler const &rhs);

		void setCgiPid(pid_t cgi_pid);
		void setCgiPath(const std::string &cgi_path);
		void setRequestPipe(int request_pipe);
		void setResponsePipe(int response_pipe);
		
		const std::map<std::string, std::string> &getEnv() const;
		const pid_t getCgiPid() const;
		const std::string &getCgiPath() const;
		const int getRequestPipe() const;
		const int getResponsePipe() const;

		std::string	getAfter(const std::string& path, char delim);
		std::string	getBefore(const std::string& path, char delim);
		std::string	decode(std::string& path);
		std::string	setCookie(const std::string& str);
		int	countCookies(const std::string& str);

};


#endif

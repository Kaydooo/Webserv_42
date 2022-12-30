#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "Webserv.hpp"

class ServerConfig;

class ConfigParser {
	private:
		std::vector<ServerConfig>	_servers;
		std::vector<std::string>	_server_config;
		size_t						_nb_server;

	public:

		ConfigParser();
		~ConfigParser();

		int createCluster(const std::string &config_file);

		void splitServers(std::string &content);
		void removeComments(std::string &content);
		void removeWhiteSpace(std::string &content);
		size_t findStartServer(size_t start, std::string &content);
		size_t findEndServer(size_t start, std::string &content);
		void createServer(std::string &config, ServerConfig &server);
		void checkServers();
		std::vector<ServerConfig>	getServers();
		int	stringCompare(std::string str1, std::string str2, size_t pos);

		int print();

		public:
		class ErrorException : public std::exception
		{
			private:
				std::string _message;
			public:
				ErrorException(std::string message) throw()
				{
					_message = "CONFIG PARSER ERROR: " + message;
				}
				virtual const char* what() const throw()
				{
					return (_message.c_str());
				}
				virtual ~ErrorException() throw() {}
		};
};

#endif
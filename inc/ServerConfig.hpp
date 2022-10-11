#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "Webserv.hpp"

static std::string	serverParametrs[] = {"server_name", "listen", "root", "index", "allow_methods", "client_body_buffer_size"};

class Location;

class ServerConfig
{
	private:
		uint16_t						_port;
		in_addr_t						_host; //in_addr_t//from #include <machine/types.h>  base type of internet address
		std::string						_server_name;
		std::string						_root;
		unsigned long					_client_max_body_size;
		std::string						_index;
		std::map<short, std::string>	_error_pages;
		std::vector<Location> 			_locations;

        struct sockaddr_in _server_address;
        int     			_listen_fd;



	public:
		ServerConfig();
		~ServerConfig();
		ServerConfig(const ServerConfig &other);
		ServerConfig &operator=(const ServerConfig & rhs);

		void initErrorPages(void);

		void setServerName(std::string server_name);
		void setHost(std::string parametr);
		void setRoot(std::string root);
		void setPort(std::string parametr);
		void setClientMaxBodySize(std::string parametr);
		void setErrorPages(std::vector<std::string> &parametr);
		void setIndex(std::string index);
		void setLocation(std::string nameLocation, std::vector<std::string> parametr);

		bool isValidHost(std::string host) const;
		bool isValidErrorPages() const;
		int isValidLocation(Location &location) const;

		const std::string &getServerName();
		const uint16_t &getPort();
		const in_addr_t &getHost();
		const size_t &getClientMaxBodySize();
		const std::vector<Location> &getLocations();
		const std::string &getRoot();
		const std::map<short, std::string> &getErrorPages();
		const std::string &getIndex();
		const std::string &getPathErrorPage(short key);
		const std::vector<Location>::iterator getLocationKey(std::string key);

		static void checkToken(std::string &parametr);
		bool checkLocaitons() const;

		public:
		class ErrorException : public std::exception
		{
			private:
				std::string _message;
			public:
				ErrorException(std::string message) throw()
				{
					_message = "SERVER CONFIG ERROR: " + message;
				}
				virtual const char* what() const throw()
				{
					return (_message.c_str());
				}
				virtual ~ErrorException() throw() {}
		};

		void	setupServer();
        int     getFd();

};

#endif

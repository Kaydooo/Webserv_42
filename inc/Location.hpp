#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "Webserv.hpp"

class Location
{
	private:
		std::string			_path;
		std::string			_root;
		bool				_autoindex;
		std::string			_index;
		std::vector<short>	_methods; // GET+ POST- DELETE-
		std::string			_return;
		std::string			_cgi_pass;

	public:
		Location();
		Location(const Location &other);
		Location &operator=(const Location &rhs);
		~Location();

		void setPath(std::string parametr);
		void setRootLocation(std::string parametr);
		void setMethods(std::vector<std::string> methods);
		void setAutoindex(std::string parametr);
		void setIndexLocation(std::string parametr);
		void setCgiPass(std::string parametr);
		void setReturn(std::string parametr);

		const std::string &getPath() const;
		const std::string &getRootLocation() const;
		const std::vector<short> &getMethods() const;
		const bool &getAutoindex() const;
		const std::string &getIndexLocation() const;
		const std::string &getCgiPass() const;
		const std::string &getReturn() const;

		
		std::string getPrintMethods() const; // for checking only

};

#endif

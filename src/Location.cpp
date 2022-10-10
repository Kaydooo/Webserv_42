#include "../inc/Location.hpp"

Location::Location()
{
	this->_path = "";
	this->_root = "";
	this->_autoindex = false;
	this->_index = "";
	this->_cgi_pass = "";
	this->_return = "";
	this->_methods.reserve(3);
	this->_methods.push_back(1);
	this->_methods.push_back(0);
	this->_methods.push_back(0);
}

Location::Location(const Location &other)
{
	this->_path = other._path;
	this->_root = other._root;
	this->_autoindex = other._autoindex;
	this->_index = other._index;
	this->_cgi_pass = other._cgi_pass;
	this->_return = other._return;
    this->_methods = other._methods;
}

Location &Location::operator=(const Location &rhs)
{
	if (this != &rhs)
	{
		this->_path = rhs._path;
		this->_root = rhs._root;
		this->_autoindex = rhs._autoindex;
		this->_index = rhs._index;
		this->_cgi_pass = rhs._cgi_pass;
		this->_return = rhs._return;
		this->_methods = rhs._methods;
    }
	return (*this);
}

Location::~Location() { }

/* set functions */
void Location::setPath(std::string parametr)
{
	this->_path = parametr;
}

void Location::setRootLocation(std::string parametr)
{
	if (ConfigFile::getTypePath(parametr) != 2)
		throw ServerConfig::ErrorException("root of location");
	this->_root = parametr;
}

void Location::setMethods(std::vector<std::string> methods)
{
	this->_methods[0] = 0;
	for (size_t i = 0; i < methods.size(); i++)
	{
		if (methods[i] == "GET")
			this->_methods[0] = 1;
		else if (methods[i] == "POST")
			this->_methods[1] = 1;
		else if (methods[i] == "DELETE")
			this->_methods[2] = 1;
		else
			throw ServerConfig::ErrorException("Allow method not supported " + methods[i]);
	}
}

void Location::setAutoindex(std::string parametr)
{
	if (parametr == "on" || parametr == "off")
		this->_autoindex = (parametr == "on");
	else
		throw ServerConfig::ErrorException("Wrong autoindex");
}

void Location::setIndexLocation(std::string parametr)
{
	this->_index = parametr;
}

void Location::setReturn(std::string parametr)
{
	this->_return = parametr;
}

void Location::setCgiPass(std::string parametr)
{
	this->_cgi_pass = parametr;
}

/* get functions */
const std::string &Location::getPath() const
{
	return (this->_path);
}

const std::string &Location::getRootLocation() const
{
	return (this->_root);
}

const std::string &Location::getIndexLocation() const
{
	return (this->_index);
}

const std::vector<short> &Location::getMethods() const
{
	return (this->_methods);
}

const std::string &Location::getCgiPass() const
{
	return (this->_cgi_pass);
}

const bool &Location::getAutoindex() const
{
	return (this->_autoindex);
}

const std::string &Location::getReturn() const
{
	return (this->_return);
}

/* for printing allowed methods*/
std::string Location::getPrintMethods() const
{
	std::string res;
	if (_methods[2])
		res.insert(0, "DELETE");
	if (_methods[1])
	{
		if (!res.empty())
			res.insert(0, ", ");
		res.insert(0, "POST");
	}
	if (_methods[0])
	{
		if (!res.empty())
			res.insert(0, ", ");
		res.insert(0, "GET");
	}
	return (res);
}
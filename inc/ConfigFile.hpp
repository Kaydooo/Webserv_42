#ifndef CONFIGFILE_HPP
#define CONFIGFILE_HPP

#include "Webserv.hpp"

class ConfigFile {
	private:
		std::string		_path;
		size_t			_size;	// проверить нужен ли

	public:
		ConfigFile();
		ConfigFile(std::string const path);
		~ConfigFile();

		static int getTypePath(std::string const path);
		static int checkFile(std::string const path, int mode);
		std::string	readFile(std::string path);
		static int isFileExistAndReadable(std::string const path, std::string const index);

		std::string getPath();
		int getSize();
};

#endif
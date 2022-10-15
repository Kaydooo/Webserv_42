#include "../inc/CgiHandler.hpp"
/* Constructor */

CgiHandler::CgiHandler() { }

//CgiHandler::CgiHandler( msg) // request msg
// {
// 	this->_cgi_pid = -1;
// 	this->_request_pipe = -1;
// 	this->_response_pipe = -1;
// 	this->_exit_status = 0;
// }

CgiHandler::~CgiHandler() {
	// free (_cenv) all
	//_env.clear()
	// if (waitpid(_cgi_pid, &_exit_status, WNOHANG) == 0)
	// 	kill(_cgi_pid, SIGKILL);
	// if (_response_pipe != -1)
	// 	close(_response_pipe);
	// if (_request_pipe != -1)
	// 	close(_request_pipe);
//    std::cout << "CGI DIED" << std::endl;
//    std::cout << "Exit status was " << WEXITSTATUS(_exit_status) << std::endl;
}

CgiHandler::CgiHandler(const CgiHandler &other)
{
	//TODO
}

CgiHandler &CgiHandler::operator=(const CgiHandler &rhs) 
{
    if (this != &rhs)
	{
		this->_env = rhs._env;
		// TODO
	}	
	return (*this);
}

/*Set functions */

void CgiHandler::setCgiPid(pid_t cgi_pid) 
{
    this->_cgi_pid = cgi_pid;
}

void CgiHandler::setCgiPath(const std::string &cgi_path) 
{
    this->_cgi_path = cgi_path;
}

void CgiHandler::setRequestPipe(int request_pipe) 
{
    this->_request_pipe = request_pipe;
}

void CgiHandler::setResponsePipe(int response_pipe) 
{
    this->_response_pipe = response_pipe;
}

/* Get functions */
const std::map<std::string, std::string> &CgiHandler::getEnv() const 
{
    return (this-> _env);
}

const pid_t CgiHandler::getCgiPid() const 
{
    return (this->_cgi_pid);
}

const std::string &CgiHandler::getCgiPath() const 
{
    return (this->_cgi_path);
}

const int CgiHandler::getRequestPipe() const 
{
    return (this->_request_pipe);
}

const int CgiHandler::getResponsePipe() const 
{
    return (this->_response_pipe);
}

	// put env
    // _env["AUTH_TYPE"] = "Basic";
    // _env["CONTENT_LENGTH"] = from request ["Content-Length"];
    // _env["CONTENT_TYPE"] = from request ["Content-type"];
    // _env["GATEWAY_INTERFACE"] = "CGI/1.1";
    // _env["QUERY_STRING"] = from request ["Query_string"];
    // _env["REMOTE_ADDR"] = from request ["Host"];
    // _env["REQUEST_METHOD"] = from request ["Request"];
    // _env["SCRIPT_FILENAME"] = write function witn full patn to the file
    // _env["PATH_INFO"] = decode(from request ["Path_info"]);
    // _env["PATH_TRANSLATED"] = root from reguest + _env["PATH_INFO"] == "." ? "/" : _env["PATH_INFO"];
    // _env["HTTP_COOKIE"] = from request ["Cookie"];
    // _env["SERVER_NAME"] = getBeforeColon(from request ["Host"], ':'); - will write a funct
    // _env["SERVER_PORT"] = getAfterColon(from request ["Host"], ':');  - will write a funct
    // _env["SERVER_PROTOCOL"] = "HTTP/1.1";
    // _env["REDIRECT_STATUS"] = "200";


// decode(std::string& path)
// {
//     size_t token = path.find("%");
//     while (token != std::string::npos)
//     {
//         //replace token with ASCII
//         //check if there are at least 2 symbols after %
//         if (path.length() < token + 2)
//             break ;
//         char decimal = fromHexToDec(path.substr(token + 1, 2));
//         path.replace(token, 3, ft::to_string(decimal));
//         token = path.find("%");
//     }
//     return (path);
// }

// Convert from Hex to Dec
unsigned int fromHexToDec(const std::string& nb)
{
    unsigned int x;   
    std::stringstream ss;
    ss << nb;
    ss >> std::hex >> x;
    return (x);
}
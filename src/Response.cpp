# include "../inc/Response.hpp"

Response::Response(){}

Response::~Response(){}

Response::Response(HttpRequest &req): _request(req), _code(0) {}

void   Response::errResponse(short error_code)
{
    _target_file = _server.getErrorPages().at(error_code);
    _code = error_code;
    
}

void   Response::contentType()
{
    std::cout << "File is = " << _target_file << std::endl;
    if(_target_file.rfind(".html", std::string::npos) != std::string::npos)
        _response_content.append("Content-Type: text/html\r\n");
    else if(_target_file.rfind(".jpg", std::string::npos) != std::string::npos)
        _response_content.append("Content-Type: image/jpeg\r\n");
    else if(_target_file.rfind(".png", std::string::npos) != std::string::npos)
        _response_content.append("Content-Type: image/png\r\n");
    else if(_target_file.rfind(".ico", std::string::npos) != std::string::npos)
        _response_content.append("Content-Type: image/x-icon\r\n");
    else if(_target_file.rfind(".css", std::string::npos) != std::string::npos)
        _response_content.append("Content-Type: text/css\r\n");
    else
    {
        _response_content.append("Content-Type: text/plain\r\n");
    }
}

void   Response::contentLength()
{
    std::stringstream ss;
    ss << _body_length;
    _response_content.append("Content-Length: ");
    _response_content.append(ss.str());
    _response_content.append("\r\n");

}

void    Response::addHeaders()
{
    contentType();
    contentLength();
    _response_content.append("\r\n");
}

bool fileExists (const std::string& f) {
    std::ifstream file(f.c_str());
    return (file.good());
}

void    Response::constructTargetFile()
{
    if (_request.getPath().compare("/") == 0)
        _target_file = _server.getRoot() + _server.getIndex();
    else
        _target_file = _server.getRoot() +_request.getPath().substr(1, _request.getPath().length() - 1);
    if(!fileExists(_target_file))
    {
        _target_file = _server.getErrorPages().at(404);
        // std::cout << "err targer file = " << _target_file << std::endl;
        _code = 404;
    }
        
}

void    Response::buildResponse()
{
    if(_request.errorCode() == 0)
        constructTargetFile();
    if(buildBody())
    {   
        addStatusLine();
        addHeaders();
    }
}

std::string Response::getContent() const { return _response_content; }
const char*       Response::getBody() const { return _response_body; }
size_t      Response::getBodyLength() const { return _body_length; }

/* Check if there is any error and assign the correct status code to response message */
void        Response::addStatusLine()
{
    if(_code == 200)
        _response_content.append("HTTP/1.1 200 OK\r\n");
    else if(_code == 400)
        _response_content.append("HTTP/1.1 400 BadRequest\r\n");   
    else if(_code == 404)
        _response_content.append("HTTP/1.1 404 Not Found\r\n");

}

size_t Response::file_size() 
{
    FILE* fin = fopen(_target_file.c_str(), "rb");
    if (fin == NULL) {
        _code = 404;
        std::cerr << " webserv: open error 1 " << strerror(errno) << std::endl;
        std::cerr << "Target file = |" << _target_file << "|" << std::endl; 
        return (-1);
    }

    fseek(fin, 0L, SEEK_END);
    size_t size = ftell(fin);
    fclose(fin);
    return size;
}

int    Response::buildBody()
{
    if(!readFile())
    {
        std::cout << "readFile Fail !" << std::endl;
        _code = 404;
        _response_content.append("HTTP/1.1 404 Not Found\r\n\r\n");
        return (0);
    }
    return (1);
}

int     Response::readFile()
{
    int         file_fd;
    int         bytes_read;

    _body_length = file_size();
    if(_body_length == -1)
        return (0); 
    _response_body = (char*) calloc(sizeof(char), _body_length);

    file_fd = open(_target_file.c_str(), O_RDONLY);

    if (file_fd < 0 )
    {
        std::cerr << " webserv: open error " << strerror(errno) << std::endl;
        return (0); 
    }
    
    bytes_read = read(file_fd, _response_body, _body_length);
    if(bytes_read < 0)
    {
        std::cerr << " webserv: read error *_*" << strerror(errno) << std::endl;
        return (0);
    }
    else
        _code = 200;
    return (1);
}

int      Response::getErrorCode() const
{
    return (_code);
}

void     Response::setServer(ServerConfig &server)
{
    _server = server;
}

void    Response::setRequest(HttpRequest &req)
{
    _request = req;
}

void   Response::clearResponse()
{
    _body_length = 0;
    _response_content.clear();
    _code = 0;
}

int      Response::getCode() const
{
    return (_code);
}

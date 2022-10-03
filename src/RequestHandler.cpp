# include "../inc/RequestHandler.hpp"

RequestHandler::RequestHandler(){}

RequestHandler::~RequestHandler(){}

/* 
  TODO: Request message to be parsed here.
  Outcome:
  1- string _request_line[3] filled with 1-Method 2-Path 3-HTTP Ver.
  2- map<string, string> _request_headers filled with header titles and header info. as key/value.
  3- string _request_body filled with request body (if expected).
*/

RequestHandler::RequestHandler(HttpRequest &req): _request(req) {}

/* 
   Uses both request_line + request_headers to build the correct response.
   Outcome: Build the response message that will be sent back to the client.
*/

void    RequestHandler::addHeaders()
{
    std::stringstream ss;

    ss << _body_length;

    std::cout << "File is = " << _request.getPath() << std::endl;
    if(_request.getPath().rfind(".html", std::string::npos) != std::string::npos)
        _response_content.append("Content-Type: text/html\r\n");
    else if(_request.getPath().rfind(".jpg", std::string::npos) != std::string::npos)
        _response_content.append("Content-Type: image/jpeg\r\n");
    else if(_request.getPath().rfind(".png", std::string::npos) != std::string::npos)
        _response_content.append("Content-Type: image/png\r\n");
    else if(_request.getPath().rfind(".ico", std::string::npos) != std::string::npos)
        _response_content.append("Content-Type: image/x-icon\r\n");
    else if(_request.getPath().rfind(".css", std::string::npos) != std::string::npos)
        _response_content.append("Content-Type: text/css\r\n");
    else
    {
        _response_content.append("Content-Type: text/plain\r\n");
        std::cout << "PATH is : " +  _request.getPath() << std::endl;
    }

    _response_content.append("Content-Length: ");
    _response_content.append(ss.str());
    _response_content.append("\r\n\n");
}

void    RequestHandler::buildResponse()
{

    if(buildBody())
    {   
        addStatus();
        addHeaders();
    }
    // Temp for testing only
}

std::string RequestHandler::getContent() { return _response_content; }
char*       RequestHandler::getBody() { return _request_body; }
size_t      RequestHandler::getBodyLength() { return _body_length; }




/* Check if there is any error and assign the correct status code to response message */
void        RequestHandler::addStatus()
{
    // For testing Assume no errors for now.
    _response_content.append("HTTP/1.1 200 OK\n");
}

size_t file_size(std::string file_path) 
{
    file_path = "server_dir" + file_path;
    FILE* fin = fopen(file_path.c_str(), "rb");
    if (fin == NULL) {
        std::cerr << " webserv: open error 1" << strerror(errno) << std::endl;
        std::cerr << file_path << std::endl;
        return (-1);
        // exit(0);
    }

    fseek(fin, 0L, SEEK_END);
    size_t size = ftell(fin);
    fclose(fin);
    return size;
}

int    RequestHandler::buildBody()
{
    if(!readFile())
    {
        std::cout << "readFile Fail !" << std::endl;
        return (0);
    }
    return (1);
}

int     RequestHandler::readFile()
{
    int         file_fd;
    int         bytes_read;

    _body_length = file_size(_request.getPath());
    if(_body_length == -1)
        return (0);
    _request_body = (char*) calloc(sizeof(char), _body_length);

    if (_request.getPath().compare("/") == 0)
        file_fd = open("server_dir/home.html", O_RDONLY);
    else
        file_fd = open(("server_dir" + _request.getPath()).c_str(), O_RDONLY);

    if (file_fd < 0 )
    {
        std::cerr << " webserv: open error " << strerror(errno) << std::endl;
        return (0); 
    }

    bytes_read = read(file_fd, _request_body, _body_length);
    if(bytes_read < 0)
    {
        std::cout << "read erro in file :|" << _request.getPath() << "|" << std::endl;
        std::cerr << " webserv: read error *_*" << strerror(errno) << std::endl;
        return (0);
    }
    return (1);
}

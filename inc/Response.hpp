#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Webserv.hpp"
# include "HttpRequest.hpp"


/* 
    Takes a string object that contain the whole request message and parse it into 3 Variables 
    _request_line, _request_headers, _response_body. And build the response message.
*/
class Response
{
    public:

        Response();
        Response(HttpRequest&);
        ~Response();


        std::string getContent() const;
        const char* getBody() const;
        size_t      getBodyLength() const;
        int         getErrorCode() const;
        int         getCode() const;

        void        setRequest(HttpRequest &);
        void        setServer(ServerConfig &);
        
        void        buildResponse();
        void        clearResponse();
        void        errResponse(short error_code);




    private:
        HttpRequest _request;
        std::string _target_file;
        size_t      _body_length;
        char*       _response_body;
        std::string _response_content;
        int         _code;
        ServerConfig    _server;

        int     buildBody();
        size_t  file_size();
        void    addStatusLine();
        void    addHeaders();
        int     readFile();
        void    contentType();
        void    contentLength();
        void    constructTargetFile();


};

#endif // RESPONSE_HPP
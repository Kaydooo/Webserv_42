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
        static     Mime _mime;
        Response();
        Response(HttpRequest&);
        ~Response();

        std::string     getRes();
        size_t      getLen() const;
        int         getCode() const;

        void        setRequest(HttpRequest &);
        void        setServer(ServerConfig &);

        void        buildResponse();
        void        clear();
        void        handleCgi(HttpRequest&);
        void        cutRes(size_t);
        int         getCgiState();
        void        setCgiState(int);
        void        setErrorResponse(short code);

		CgiHandler		_cgi_obj;

        std::string removeBoundary(std::string &body, std::string &boundary);
        std::string     _response_content;

        HttpRequest     request;
    private:
        ServerConfig    _server;
        std::string     _target_file;
        std::vector<uint8_t> _body;
        size_t          _body_length;
        std::string     _response_body;
        std::string     _location;
        short           _code;
        char            *_res;
		int				_cgi;
		int				_cgi_fd[2];
		size_t			_cgi_response_length;
        bool            _auto_index;

        int     buildBody();
        size_t  file_size();
        void    setStatusLine();
        void    setHeaders();
        void    setServerDefaultErrorPages();
        int     readFile();
        void    contentType();
        void    contentLength();
        void    connection();
        void    server();
        void    location();
        void    date();
        int     handleTarget();
        void    buildErrorBody();
        bool    reqError();
        int     handleCgi(std::string &);
        int     handleCgiTemp(std::string &);
};

#endif // RESPONSE_HPP
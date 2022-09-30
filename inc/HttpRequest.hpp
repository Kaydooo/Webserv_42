#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include "Webserv.hpp"

enum HttpMethod
{
    GET,
    POST, 
    DELETE, 
};

enum ParsingState
{
    Request_Line,
    Request_Line_Method,
    Request_Line_First_Space,
    Request_Line_Path_First_Char,
    Request_Line_Before_URI,
    Request_Line_URI,
    Request_Line_URI_Path_Slash,
    Request_Line_URI_Path,
    Request_Line_URI_Query,
    Request_Line_URI_Fragment,
    Request_Line_Ver,
    Request_Line_HT,
    Request_Line_HTT,
    Request_Line_HTTP,
    Request_Line_HTTP_Slash,
    Request_Line_Major,
    Request_Line_Dot,
    Request_Line_Minor,
    Request_Line_CR,
    Request_Line_LF,
    H_Key,
    H_Value,
    Body
};

/* 
    HttpRequest Class will be used to parase and store the request. 
    It gets feeded with the request and will on a flag when parasing is finished.
*/
class HttpRequest
{
    public:
        HttpRequest();
        ~HttpRequest();
        void    feed(char *data, size_t size);
        bool    parsingCompleted();
        HttpMethod &getMethod();
        void        setMethod(HttpMethod &);
        std::string &getHeader(std::string &);
        void        setHeader(std::string , std::string );
        void        printMessage();


        
    protected:
        std::string     _path;
        std::string     _query;
        std::string     _fragment;
        std::map<std::string, std::string> _request_headers;
        char    *_request_body;
        HttpMethod _method;
        ParsingState  _state;
        int         _word_index;
        int         _line_number;
        bool        _complete_flag;
        bool        _skip;
        std::string        _storage;
        std::string        _key_storage;
        std::map<u_int8_t, std::string> _method_str;
        int                 _method_index;
        u_int8_t    _ver_major;
        u_int8_t    _ver_minor;
        
    private:
};

#endif
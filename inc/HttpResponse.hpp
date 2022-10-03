#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include "Webserv.hpp"

/* 
    HttpResponse Class will be used to parase and store the request. 
    It gets feeded with the request and will on a flag when parasing is finished.
*/
class HttpResponse
{
    public:
        HttpResponse();
        ~HttpResponse();
        std::string &getHeader(std::string &);
        void        setHeader(std::string , std::string );
        void        printMessage();
        void        clearForNextResponse();


        
    private:
        std::string _status_line;
        std::map<std::string, std::string> _response_headers;
        std::vector<u_int8_t> _body;
        std::map<u_int8_t, std::string> _method_str;
        int         _body_length;
        bool        _fields_done_flag;
        bool        _body_flag;
        bool        _body_done_flag;
        bool        _complete_flag;
        int         _error_code;
        std::string        _storage;
        std::string        _key_storage;
        int                 _method_index;
        u_int8_t    _ver_major;
        u_int8_t    _ver_minor;
        
        void        _handle_headers();
};

#endif
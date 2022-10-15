#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include "Webserv.hpp"

enum HttpMethod
{
    GET,
    POST, 
    DELETE,
    NONE
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
    Field_Name_Start,
    Fields_End,
    Field_Name,
    Field_Value,
    Field_Value_End,
    Before_Message_Body,
    Message_Body,
    Parsing_Done
};

/** 
 * 
 * - HttpRequest Class will be used to parase and store the request. 
   It gets feeded with the request and will triiger a flag when parasing is finished.
   - If any error was found in the request, _code will be set to the correct error code.
**/
class HttpRequest
{
    public:
        HttpRequest();
        ~HttpRequest();

        HttpMethod  &getMethod();
        std::string &getPath();
        std::string &getQuery();
        std::string &getFragment();
        std::string &getHeader(std::string &);

        void        setMethod(HttpMethod &);
        void        setHeader(std::string , std::string );
        void        setMaxBodySize(size_t);

        void        feed(char *data, size_t size);
        bool        parsingCompleted();
        void        printMessage();
        void        clear();
        int         errorCode();
        bool        keepAlive();
        
    private:
        std::string     _path;
        std::string     _query;
        std::string     _fragment;
        std::map<std::string, std::string> _request_headers;
        std::vector<u_int8_t> _body;
        HttpMethod _method;
        std::map<u_int8_t, std::string> _method_str;
        ParsingState    _state;
        size_t          _max_body_size;
        int             _body_length;
        bool            _fields_done_flag;
        bool            _body_flag;
        bool            _body_done_flag;
        bool            _complete_flag;
        int             _error_code;
        std::string     _storage;
        std::string     _key_storage;
        int             _method_index;
        u_int8_t        _ver_major;
        u_int8_t        _ver_minor;
        
        void        _handle_headers();
};

#endif
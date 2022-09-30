#include "../inc/HttpRequest.hpp"



HttpRequest::HttpRequest()
{
    _method_str[HttpMethod::GET] = "GET";
    _method_str[HttpMethod::POST] = "POST";
    _method_str[HttpMethod::DELETE] = "DELETE";
    _path = "";
    _query = "";
    _fragment = "";
    _method_index = 1;
    _state = Request_Line;
    _word_index = 0;
    _line_number = 0;
    _complete_flag = false;
    _storage = "";
    _key_storage = "";
    _skip = false;
}

HttpRequest::~HttpRequest() {}

bool    allowedURI(uint8_t ch)
{
    if((ch >= '&' && ch <= ';') || (ch >= '?' && ch <= '[') || (ch >= 'a' && ch <= 'z')
    || ch == '!' || ch == '#' || ch == '$' || ch == '=' || ch == ']' || ch == '_' || ch == '~')
        return (true);
    return (false);
}

void    HttpRequest::feed(char *data, size_t size)
{
    u_int8_t character;

    for(size_t i = 0; i < size; ++i)
    {
        character = data[i];
        if(_skip)
        {
            _skip = false;
            continue;
        }
        if (_state == Request_Line)
        {
            if (character == 'G')
                _method = GET;
            else if (character == 'P')
                _method = POST;
            else if (character == 'D')
                _method = DELETE;
            else
            {    std::cout << "Wrong Method" << std::endl; return;//send bad method response here and 
                //set error flag so it can be checked in select loop and reset this request object.
            }
            _state = Request_Line_Method;
        }
        else if (_state == Request_Line_Method)
        {
                if(character == _method_str[_method][_method_index])
                {
                    _method_index++;
                }
                else
                {
                    std::cout << "Wrong Method" << std::endl;//send bad method response here and 
                }

                if(_method_index == _method_str[_method].length())
                {
                    _state = Request_Line_First_Space;
                    std::cout << "LOL\n";
                }

        }
        else if (_state == Request_Line_First_Space)
        {
            if (character != ' ')
            {
                std::cout << "Bad Character (First_Space)" << std::endl; return;//send bad method response here and
                //set error flag so it can be checked in select loop and reset this request object.
            }
            _state = Request_Line_URI_Path_Slash;
            continue;
        }
        else if (_state == Request_Line_URI_Path_Slash)
        {
            if (character == ' ' || character == '\r' || character == '\n')
            {
                std::cout << "Bad Character (Before_URI)" << std::endl; return;
            }
            else if (character == '/')
            {
                _state = Request_Line_URI_Path;
                _storage.clear();
            }
            else
            {
                std::cout << "Bad Character (Before_URI)" << std::endl; return;
            }
        }
        else if (_state == Request_Line_URI_Path)
        {
            if (character == ' ')
            {
                _state = Request_Line_Ver;
                _path.append(_storage);
                _storage.clear();
                continue;
            }
            else if (character == '?')
            {
                _state = Request_Line_URI_Query;
                _path.append(_storage);
                _storage.clear();
                continue;
            }
            else if (!allowedURI(character))
            {
                std::cout << "Bad Character (URI_PATH)" << std::endl; return;
            }
        }
        else if (_state == Request_Line_URI_Query)
        {
            if (character == ' ')
            {
                _state = Request_Line_Ver;
                _query.append(_storage);
                _storage.clear();
                continue;
            }
            else if (character == '#')
            {
                _state = Request_Line_URI_Fragment;
                _query.append(_storage);
                _storage.clear();
                continue;
            }
            else if (!allowedURI(character))
            {
                std::cout << "Bad Character (URI_Query)" << std::endl; return;
            }
        }
        else if (_state == Request_Line_URI_Fragment)
        {
            if (character == ' ')
            {
                _state = Request_Line_Ver;
                _fragment.append(_storage);
                _storage.clear();
                continue;
            }
            else if (!allowedURI(character))
            {
                std::cout << "Bad Character (URI_Fragment)" << std::endl; return;
            }
        }
        else if (_state == Request_Line_Ver)
        {
            if (character != 'H')
            {
                std::cout << "Bad Version" << std::endl; return;
            }
            _state = Request_Line_HT;
        }
        else if (_state == Request_Line_HT)
        {
            if (character != 'T')
            {
                std::cout << "Bad Version" << std::endl; return;
            }
            _state = Request_Line_HTT;
        }
        else if (_state == Request_Line_HTT)
        {
            if (character != 'T')
            {
                std::cout << "Bad Version" << std::endl; return;
            }
            _state = Request_Line_HTTP;
        }
        else if (_state == Request_Line_HTTP)
        {
            if (character != 'P')
            {
                std::cout << "Bad Version" << std::endl; return;
            }
            _state = Request_Line_HTTP_Slash;
        }
        else if (_state == Request_Line_HTTP_Slash)
        {
            if (character != '/')
            {
                std::cout << "Bad Version(Slash)" << std::endl; return;
            }
            _state = Request_Line_Major;
        }
        else if (_state == Request_Line_Major)
        {
            if (!isdigit(character))
            {
                std::cout << "Bad Version(Major)" << std::endl; return;
            }
            _ver_major = character;

            _state = Request_Line_Dot;
        }
        else if (_state == Request_Line_Dot)
        {
            if (character != '.')
            {
                std::cout << "Bad Version(Dot)" << std::endl; return;
            }
            _state = Request_Line_Minor;
        }
        else if (_state == Request_Line_Minor)
        {
            if (!isdigit(character))
            {
                std::cout << "Bad Version(Major)" << std::endl; return;
            }
            _ver_minor = character;
            _state = Request_Line_CR;
        }
        else if (_state == Request_Line_CR)
        {
            if (character != '\r')
            {
                std::cout << "Bad Character(Request_Line_CR)" << std::endl; return;
            }
            _state = Request_Line_LF;
        }
        else if (_state == Request_Line_LF)
        {
            if (character != '\n')
            {
                std::cout << "Bad Character(Request_Line_LF)" << std::endl; return;
            }
            _state = H_Key;
            _storage.clear();
            continue;
        }
        else if (_state == H_Key && character == ':')
        {
            _key_storage = _storage;
            _storage.clear();
            // _skip = true;
            _state = H_Value;
            continue;
        }
        else if (_state == H_Value && character == '\r')
        {
            setHeader(_key_storage, _storage);
            _key_storage.clear();
            _storage.clear();
            _skip = true;
            _state = H_Key;
            continue;
        }
        else if ( _state == H_Key && character == '\r')
        {
            //if no body then parsing is completed.
            _complete_flag = true;
            break;
        }
        _storage += character;
    }
}

bool    HttpRequest::parsingCompleted() { return _complete_flag; }

HttpMethod  &HttpRequest::getMethod() { return _method; }

std::string &HttpRequest::getHeader(std::string &name){ return _request_headers[name]; }

void    HttpRequest::setMethod(HttpMethod & method) {   _method = method; }

void    HttpRequest::setHeader(std::string name, std::string value) { _request_headers[name] = value; }

void        HttpRequest::printMessage()
{
    std::cout << _method_str[_method] + " --" + _path + "?" + _query + "#" + _fragment
              + " " + "HTTP/" << _ver_major  << "." << _ver_minor << std::endl;
    
    for(std::map<std::string, std::string>::iterator it = _request_headers.begin();
    it != _request_headers.end(); ++it)
    {
        std::cout << it->first + ":" + it->second << std::endl;
    }
}
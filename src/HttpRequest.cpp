#include "../inc/HttpRequest.hpp"

HttpRequest::HttpRequest()
{
    _method_str[::GET] = "GET";
    _method_str[::POST] = "POST";
    _method_str[::DELETE] = "DELETE";
    _method_str[::PUT] = "PUT";
    _method_str[::HEAD] = "HEAD";
    _path = "";
    _query = "";
    _fragment = "";
    _body_str = "";
    _error_code = 0;
    _chunk_length = 0;
    _method = NONE;
    _method_index = 1;
    _state = Request_Line;
    _fields_done_flag = false;
    _body_flag = false;
    _body_done_flag = false;
    _chunked_flag = false;
    _body_length = 0;
    _storage = "";
    _key_storage = "";
    _multiform_flag = false;
    _boundary = "";
}

HttpRequest::~HttpRequest() {}

bool    checkUriPos(std::string path)
{
    std::string tmp(path);
    char *res = strtok((char*)tmp.c_str(), "/");
    int pos = 0;
    while (res != NULL)
    {
        if (!strcmp(res, ".."))
            pos--;
        else
            pos++;
        if (pos < 0)
            return (1);
        res = strtok(NULL, "/");
    }
    return (0);
}

/**

 * Checks if character is allowed to be in a URI
 * Characters allowed as specifed in RFC:
   Alphanumeric: A-Z a-z 0-9
   Unreserved: - _ . ~
   Reserved:  * ' ( ) ; : @ & = + $ , / ? % # [ ]

 **/
bool    allowedCharURI(uint8_t ch)
{
    if ((ch >= '#' && ch <= ';') || (ch >= '?' && ch <= '[') || (ch >= 'a' && ch <= 'z') ||
       ch == '!' || ch == '=' || ch == ']' || ch == '_' || ch == '~')
        return (true);
    return (false);
}

/**

* Checks whether the character passed is allowed in a field name
* Characters allowed as specifed in RFC:

"!" / "#" / "$" / "%" / "&" / "'"
/ "*" / "+" / "-" / "." / "^" / "_"
/ "`" / "|" / "~" / 0-9 / A-Z / a-z

**/
bool    isToken(uint8_t ch)
{
    if (ch == '!' || (ch >= '#' && ch <= '\'') || ch == '*'|| ch == '+' || ch == '-'  || ch == '.' ||
       (ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') || (ch >= '^' && ch <= '`') ||
       (ch >= 'a' && ch <= 'z') || ch == '|')
        return (true);
    return (false);
}

/* Trim leading and trailing  spaces */
void    trimStr(std::string &str)
{
    static const char* spaces = " \t";
    str.erase(0, str.find_first_not_of(spaces));
    str.erase(str.find_last_not_of(spaces) + 1);
}

void    toLower(std::string &str)
{
    for (size_t i = 0; i < str.length(); ++i)
        str[i] = std::tolower(str[i]);
}
void    HttpRequest::feed(char *data, size_t size)
{
    u_int8_t character;
    static std::stringstream s;

    for (size_t i = 0; i < size; ++i)
    {
        character = data[i];
        switch (_state)
        {
            case Request_Line:
            {
                if (character == 'G')
                    _method = GET;
                else if (character == 'P')
                {
                    _state = Request_Line_Post_Put;
                    break ;
                }
                else if (character == 'D')
                    _method = DELETE;
                else if (character == 'H')
                    _method = HEAD;
                else
                {
                    _error_code = 501;
                    std::cout << "Method Error Request_Line and Character is = " << character << std::endl;
                    return ;
                }
                _state = Request_Line_Method;
                break ;
            }
            case Request_Line_Post_Put:
            {
                if (character == 'O')
                    _method = POST;
                else if (character == 'U')
                    _method = PUT;
                else
                {
                    _error_code = 501;
                    std::cout << "Method Error Request_Line and Character is = " << character << std::endl;
                    return ;
                }
                _method_index++;
                _state = Request_Line_Method;
                break ;
            }
            case Request_Line_Method:
            {
                if (character == _method_str[_method][_method_index])
                    _method_index++;
                else
                {
                    _error_code = 501;
                    std::cout << "Method Error Request_Line and Character is = " << character << std::endl;
                    return ;
                }

                if ((size_t) _method_index == _method_str[_method].length())
                    _state = Request_Line_First_Space;
                break ;
            }
            case Request_Line_First_Space:
            {
                if (character != ' ')
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_First_Space)" << std::endl;
                    return ;
                }
                _state = Request_Line_URI_Path_Slash;
                continue ;
            }
            case Request_Line_URI_Path_Slash:
            {
                if (character == '/')
                {
                    _state = Request_Line_URI_Path;
                    _storage.clear();
                }
                else
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_URI_Path_Slash)" << std::endl;
                    return ;
                }
                break ;
            }
            case Request_Line_URI_Path:
            {
                if (character == ' ')
                {
                    _state = Request_Line_Ver;
                    _path.append(_storage);
                    _storage.clear();
                    continue ;
                }
                else if (character == '?')
                {
                    _state = Request_Line_URI_Query;
                    _path.append(_storage);
                    _storage.clear();
                    continue ;
                }
                else if (character == '#')
                {
                    _state = Request_Line_URI_Fragment;
                    _path.append(_storage);
                    _storage.clear();
                    continue ;
                }
                else if (!allowedCharURI(character))
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_URI_Path)" << std::endl;
                    return ;
                }
                else if ( i > MAX_URI_LENGTH)
                {
                    _error_code = 414;
                    std::cout << "URI Too Long (Request_Line_URI_Path)" << std::endl;
                    return ;
                }
                break ;
            }
            case Request_Line_URI_Query:
            {
                if (character == ' ')
                {
                    _state = Request_Line_Ver;
                    _query.append(_storage);
                    _storage.clear();
                    continue ;
                }
                else if (character == '#')
                {
                    _state = Request_Line_URI_Fragment;
                    _query.append(_storage);
                    _storage.clear();
                    continue ;
                }
                else if (!allowedCharURI(character))
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_URI_Query)" << std::endl;
                    return ;
                }
                else if ( i > MAX_URI_LENGTH)
                {
                    _error_code = 414;
                    std::cout << "URI Too Long (Request_Line_URI_Path)" << std::endl;
                    return ;
                }
                break ;
            }
            case Request_Line_URI_Fragment:
            {
                if (character == ' ')
                {
                    _state = Request_Line_Ver;
                    _fragment.append(_storage);
                    _storage.clear();
                    continue ;
                }
                else if (!allowedCharURI(character))
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_URI_Fragment)" << std::endl;
                    return ;
                }
                else if ( i > MAX_URI_LENGTH)
                {
                    _error_code = 414;
                    std::cout << "URI Too Long (Request_Line_URI_Path)" << std::endl;
                    return ;
                }
                break ;
            }
            case Request_Line_Ver:
            {
                if (checkUriPos(_path))
                {
                    _error_code = 400;
                    std::cout << "Request URI ERROR: goes before root !!" << std::endl;
                    return ;
                }
                if (character != 'H')
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_Ver)" << std::endl;
                    return ;
                }
                _state = Request_Line_HT;
                break ;
            }
            case Request_Line_HT:
            {
                if (character != 'T')
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_HT)" << std::endl;
                    return ;
                }
                _state = Request_Line_HTT;
                break ;
            }
            case Request_Line_HTT:
            {
                if (character != 'T')
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_HTT)" << std::endl;
                    return ;
                }
                _state = Request_Line_HTTP;
                break ;
            }
            case Request_Line_HTTP:
            {
                if (character != 'P')
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_HTTP)" << std::endl;
                    return ;
                }
                _state = Request_Line_HTTP_Slash;
                break ;
            }
            case Request_Line_HTTP_Slash:
            {
                if (character != '/')
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_HTTP_Slash)" << std::endl;
                    return ;
                }
                _state = Request_Line_Major;
                break ;
            }
            case Request_Line_Major:
            {
                if (!isdigit(character))
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_Major)" << std::endl;
                    return ;
                }
                _ver_major = character;

                _state = Request_Line_Dot;
                break ;
            }
            case Request_Line_Dot:
            {
                if (character != '.')
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_Dot)" << std::endl;
                    return ;
                }
                _state = Request_Line_Minor;
                break ;
            }
            case Request_Line_Minor:
            {
                if (!isdigit(character))
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_Minor)" << std::endl;
                    return ;
                }
                _ver_minor = character;
                _state = Request_Line_CR;
                break ;
            }
            case Request_Line_CR:
            {
                if (character != '\r')
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_CR)" << std::endl;
                    return ;
                }
                _state = Request_Line_LF;
                break ;
            }
            case Request_Line_LF:
            {
                if (character != '\n')
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_LF)" << std::endl;
                    return ;
                }
                _state = Field_Name_Start;
                _storage.clear();
                continue ;
            }
            case Field_Name_Start:
            {
                if (character == '\r')
                    _state = Fields_End;
                else if (isToken(character))
                    _state = Field_Name;
                else
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Field_Name_Start)" << std::endl;
                    return ;
                }
                break ;
            }
            case Fields_End:
            {
                if (character == '\n')
                {
                    _storage.clear();
                    _fields_done_flag = true;
                    _handle_headers();
                    //if no body then parsing is completed.
                    if (_body_flag == 1)
                    {
                        if (_chunked_flag == true)
                            _state = Chunked_Length_Begin;
                        else
                        {
                            _state = Message_Body;
                        }
                    }
                    else
                    {
                        _state = Parsing_Done;
                    }
                    continue ;
                }
                else
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Fields_End)" << std::endl;
                    return ;
                }
                break ;
            }
            case Field_Name:
            {
                if (character == ':')
                {
                    _key_storage = _storage;
                    _storage.clear();
                    _state = Field_Value;
                    continue ;
                }
                else if (!isToken(character))
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Field_Name)" << std::endl;
                    return ;
                }
                break ;
                //if (!character allowed)
                // error;
            }
            case Field_Value:
            {
                if ( character == '\r' )
                {
                    setHeader(_key_storage, _storage);
                    _key_storage.clear();
                    _storage.clear();
                    _state = Field_Value_End;
                    continue ;
                }
                break ;
            }
            case Field_Value_End:
            {
                if ( character == '\n' )
                {
                    _state = Field_Name_Start;
                    continue ;
                }
                else
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Field_Value_End)" << std::endl;
                    return ;
                }
                break ;
            }
            case Chunked_Length_Begin:
            {
                if (isxdigit(character) == 0)
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Chunked_Length_Begin)" << std::endl;
                    return ;
                }
                s.str("");
                s.clear();
                s << character;
                s >> std::hex >> _chunk_length;
                if (_chunk_length == 0)
                    _state = Chunked_Length_CR;
                else
                    _state = Chunked_Length;
                continue ;
            }
            case Chunked_Length:
            {
                if (isxdigit(character) != 0)
                {
                    int temp_len = 0;
                    s.str("");
                    s.clear();
                    s << character;
                    s >> std::hex >> temp_len;
                    _chunk_length *= 16;
                    _chunk_length += temp_len;
                }
                else if (character == '\r')
                    _state = Chunked_Length_LF;
                else
                    _state = Chunked_Ignore;
                continue ;
            }
            case Chunked_Length_CR:
            {
                if ( character == '\r')
                    _state = Chunked_Length_LF;
                else
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Chunked_Length_CR)" << std::endl;
                    return ;
                }
                continue ;
            }
            case Chunked_Length_LF:
            {
                if ( character == '\n')
                {
                    if (_chunk_length == 0)
                        _state = Chunked_End_CR;
                    else
                        _state = Chunked_Data;
                }
                else
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Chunked_Length_LF)" << std::endl;
                    return ;
                }
                continue ;
            }
            case Chunked_Ignore:
            {
                if (character == '\r')
                    _state = Chunked_Length_LF;
                continue ;
            }
            case Chunked_Data:
            {
				_body.push_back(character);
				--_chunk_length;
                if (_chunk_length == 0)
                    _state = Chunked_Data_CR;
				continue ;
            }
            case Chunked_Data_CR:
            {
                if ( character == '\r')
                    _state = Chunked_Data_LF;
                else
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Chunked_Data_CR)" << std::endl;
                    return ;
                }
                continue ;
            }
            case Chunked_Data_LF:
            {
                if ( character == '\n')
                    _state = Chunked_Length_Begin;
                else
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Chunked_Data_LF)" << std::endl;
                    return ;
                }
                continue ;
            }
            case Chunked_End_CR:
            {
                if (character != '\r')
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Chunked_End_CR)" << std::endl;
                    return ;
                }
                _state = Chunked_End_LF;
                continue ;

            }
            case Chunked_End_LF:
            {
                if (character != '\n')
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Chunked_End_LF)" << std::endl;
                    return ;
                }
                _body_done_flag = true;
                _state = Parsing_Done;
                continue ;
            }
            case Message_Body:
            {
                if (_body.size() < _body_length )
                    _body.push_back(character);
                if (_body.size() == _body_length )
                {
                    _body_done_flag = true;
                    _state = Parsing_Done;
                }
                break ;
            }
            case Parsing_Done:
            {
                return ;
            }
        }//end of swtich
        _storage += character;
    }
    if( _state == Parsing_Done)
    {
        _body_str.append((char*)_body.data(), _body.size());
    }
}

bool    HttpRequest::parsingCompleted()
{
    return (_state == Parsing_Done);
}

HttpMethod  &HttpRequest::getMethod()
{
    return (_method);
}

std::string &HttpRequest::getPath()
{
    return (_path);
}

std::string &HttpRequest::getQuery()
{
    return (_query);
}

std::string &HttpRequest::getFragment()
{
    return (_fragment);
}

std::string HttpRequest::getHeader(std::string const &name)
{
    return (_request_headers[name]);
}

const std::map<std::string, std::string> &HttpRequest::getHeaders() const
{
	return (this->_request_headers);
}

std::string HttpRequest::getMethodStr()
{
	return (_method_str[_method]);
}

std::string &HttpRequest::getBody()
{
    return (_body_str);
}

std::string     HttpRequest::getServerName()
{
    return (this->_server_name);
}

bool    HttpRequest::getMultiformFlag()
{
    return (this->_multiform_flag);
}

std::string     &HttpRequest::getBoundary()
{
    return (this->_boundary);
}

void    HttpRequest::setBody(std::string body)
{
    _body.clear();
    _body.insert(_body.begin(), body.begin(), body.end());
    _body_str = body;
}

void    HttpRequest::setMethod(HttpMethod & method)
{
    _method = method;
}

void    HttpRequest::setHeader(std::string &name, std::string &value)
{
    trimStr(value);
    toLower(name);
    _request_headers[name] = value;
}

void    HttpRequest::setMaxBodySize(size_t size)
{
    _max_body_size = size;
}


void        HttpRequest::printMessage()
{
    std::cout << _method_str[_method] + " " + _path + "?" + _query + "#" + _fragment
              + " " + "HTTP/" << _ver_major  << "." << _ver_minor << std::endl;

    for (std::map<std::string, std::string>::iterator it = _request_headers.begin();
    it != _request_headers.end(); ++it)
    {
        std::cout << it->first + ":" + it->second << std::endl;
    }
    for (std::vector<u_int8_t>::iterator it = _body.begin(); it != _body.end(); ++it)
    {
        std::cout << *it;
    }
    std::cout << std::endl << "END OF BODY" << std::endl;

    std::cout << "BODY FLAG =" << _body_flag << "  _BOD_done_flag= " << _body_done_flag << "FEIDLS FLAG = " << _fields_done_flag
    << std::endl;
}


void        HttpRequest::_handle_headers()
{
    std::stringstream ss;

    if (_request_headers.count("content-length"))
    {
        _body_flag = true;
        ss << _request_headers["content-length"];
        ss >> _body_length;
    }
    if ( _request_headers.count("transfer-encoding"))
    {
        if (_request_headers["transfer-encoding"].find_first_of("chunked") != std::string::npos)
            _chunked_flag = true;
        _body_flag = true;
    }
    if (_request_headers.count("host"))
    {
        size_t pos = _request_headers["host"].find_first_of(':');
        _server_name = _request_headers["host"].substr(0, pos);
    }
    if (_request_headers.count("content-type") && _request_headers["content-type"].find("multipart/form-data") != std::string::npos)
    {
        size_t pos = _request_headers["content-type"].find("boundary=", 0);
        if (pos != std::string::npos)
            this->_boundary = _request_headers["content-type"].substr(pos + 9, _request_headers["content-type"].size());
        this->_multiform_flag = true;
    }
}

short     HttpRequest::errorCode()
{
    return (this->_error_code);
}

/* Reset object variables to recive new request */

void    HttpRequest::clear()
{
    _path.clear();
    _error_code = 0;
    _query.clear();
    _fragment.clear();
    _method = NONE;
    _method_index = 1;
    _state = Request_Line;
    _body_length = 0;
    _chunk_length = 0x0;
    _storage.clear();
    _body_str = "";
    _key_storage.clear();
    _request_headers.clear();
    _server_name.clear();
    _body.clear();
    _boundary.clear();
    _fields_done_flag = false;
    _body_flag = false;
    _body_done_flag = false;
    _complete_flag = false;
    _chunked_flag = false;
    _multiform_flag = false;
}

/* Checks the value of header "Connection". If keep-alive, don't close the connection. */

bool        HttpRequest::keepAlive()
{
    if (_request_headers.count("connection"))
    {
        if (_request_headers["connection"].find("close", 0) != std::string::npos)
            return (false);
    }
    return (true);
}

void            HttpRequest::cutReqBody(int bytes)
{
    _body_str = _body_str.substr(bytes);
}
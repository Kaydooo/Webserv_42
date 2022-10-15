#include "../inc/HttpRequest.hpp"



HttpRequest::HttpRequest()
{
    _method_str[HttpMethod::GET] = "GET";
    _method_str[HttpMethod::POST] = "POST";
    _method_str[HttpMethod::DELETE] = "DELETE";
    _path = "";
    _query = "";
    _fragment = "";
    _error_code = 0;
    _method = NONE;
    _method_index = 1;
    _state = Request_Line;
    _fields_done_flag = false;
    _body_flag = false;
    _body_done_flag = false;
    _storage = "";
    _key_storage = "";
}

HttpRequest::~HttpRequest() {}


/**
 
 * Checks if character is allowed to be in a URI
 * TODO:
 * - Add another argument to specify what part of URI to check for,
 *   Because different charsets are allowed for differnt parts of URI
 
 **/
bool    allowedURI(uint8_t ch)
{
    if ((ch >= '&' && ch <= ';') || (ch >= '?' && ch <= '[') || (ch >= 'a' && ch <= 'z') ||
       ch == '!' || ch == '#' || ch == '$' || ch == '=' || ch == ']' || ch == '_' || ch == '~')
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

void    HttpRequest::feed(char *data, size_t size)
{
    u_int8_t character;

    for(size_t i = 0; i < size; ++i)
    {
        character = data[i];
        switch (_state)
        {
            case Request_Line:
            {
                if (character == 'G')
                    _method = GET;
                else if (character == 'P')
                    _method = POST;
                else if (character == 'D')
                    _method = DELETE;
                else
                {    
                    _error_code = 400;
                    std::cout << "Wrong Method" << std::endl;
                    return;
                }
                _state = Request_Line_Method;
                break;
            }
            case Request_Line_Method:
            {
                if(character == _method_str[_method][_method_index])
                {
                    _method_index++;
                }
                else
                {
                    _error_code = 400;
                    std::cout << "Wrong Method" << std::endl;
                    return;
                }

                if(_method_index == _method_str[_method].length())
                {
                    _state = Request_Line_First_Space;
                }
                break;
            }
            case Request_Line_First_Space:
            {
                if (character != ' ')
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_First_Space)" << std::endl;
                    return;
                }
                _state = Request_Line_URI_Path_Slash;
                continue;
            }
            case Request_Line_URI_Path_Slash:
            {
                if (character == ' ' || character == '\r' || character == '\n')
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_First_Space)" << std::endl;
                    return;
                }
                else if (character == '/')
                {
                    _state = Request_Line_URI_Path;
                    _storage.clear();
                }
                else
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_First_Space)" << std::endl;
                    return;
                }
                break;
            }
            case Request_Line_URI_Path:
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
                else if (character == '#')
                {
                    _state = Request_Line_URI_Fragment;
                    _path.append(_storage);
                    _storage.clear();
                    continue;
                }
                else if (!allowedURI(character))
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_First_Space)" << std::endl;
                    return;
                }
                break;
            }
            case Request_Line_URI_Query:
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
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_First_Space)" << std::endl;
                    return;
                }
                break;
            }
            case Request_Line_URI_Fragment:
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
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_First_Space)" << std::endl;
                    return;
                }
                break;
            }
            case Request_Line_Ver:
            {
                if (character != 'H')
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_First_Space)" << std::endl;
                    return;
                }
                _state = Request_Line_HT;
                break;
            }
            case Request_Line_HT:
            {
                if (character != 'T')
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_First_Space)" << std::endl;
                    return;
                }
                _state = Request_Line_HTT;
                break;
            }
            case Request_Line_HTT:
            {
                if (character != 'T')
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_First_Space)" << std::endl;
                    return;
                }
                _state = Request_Line_HTTP;
                break;
            }
            case Request_Line_HTTP:
            {
                if (character != 'P')
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_First_Space)" << std::endl;
                    return;
                }
                _state = Request_Line_HTTP_Slash;
                break;
            }
            case Request_Line_HTTP_Slash:
            {
                if (character != '/')
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_First_Space)" << std::endl;
                    return;
                }
                _state = Request_Line_Major;
                break;
            }
            case Request_Line_Major:
            {
                if (!isdigit(character))
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_First_Space)" << std::endl;
                    return;
                }
                _ver_major = character;

                _state = Request_Line_Dot;
                break;
            }
            case Request_Line_Dot:
            {
                if (character != '.')
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_First_Space)" << std::endl;
                    return;
                }
                _state = Request_Line_Minor;
                break;
            }
            case Request_Line_Minor:
            {
                if (!isdigit(character))
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_First_Space)" << std::endl;
                    return;
                }
                _ver_minor = character;
                _state = Request_Line_CR;
                break;
            }
            case Request_Line_CR:
            {
                if (character != '\r')
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_First_Space)" << std::endl;
                    return;
                }
                _state = Request_Line_LF;
                break;
            }
            case Request_Line_LF:
            {
                if (character != '\n')
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_First_Space)" << std::endl;
                    return;
                }
                _state = Field_Name_Start;
                _storage.clear();
                continue;
            }
            case Field_Name_Start:
            { 
                //if no body then parsing is completed.
                if (character == '\r')
                    _state = Fields_End;
                else if (isToken(character))// check here if the character is allowed to be in field name;
                    _state = Field_Name;
                else
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_First_Space)" << std::endl;
                    return;
                }
                break;
            }
            case Fields_End:
            {
                //if no body then parsing is completed.
                if (character == '\n')
                {
                    _storage.clear();
                    _fields_done_flag = true;
                    _handle_headers();
                    if (_body_flag == 1)
                        _state = Message_Body;
                    else
                        _state = Parsing_Done;
                    continue;
                }
                else
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_First_Space)" << std::endl;
                    return;
                }
                break;
            }
            case Field_Name:
            {
                if (character == ':')
                {
                    _key_storage = _storage;
                    _storage.clear();
                    _state = Field_Value;
                    continue;
                }
                else if (!isToken(character))
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_First_Space)" << std::endl;
                    return;
                }
                break;
                //if(!character allowed)
                // error;
            }
            case Field_Value:
            {
                if( character == '\r' )
                {
                    setHeader(_key_storage, _storage);
                    _key_storage.clear();
                    _storage.clear();
                    _state = Field_Value_End;
                    continue;
                }
                // check here if character is allowed in field vlaue;
                break;
            }
            case Field_Value_End:
            {
                if( character == '\n' )
                {
                    _state = Field_Name_Start;
                    continue;
                }
                else
                {
                    _error_code = 400;
                    std::cout << "Bad Character (Request_Line_First_Space)" << std::endl;
                    return;
                }
                break;
            }
            case Message_Body:
            {
                if(_body.size() < _body_length )
                    _body.push_back(character);
                else if (_body.size() > _max_body_size)
                {
                    _error_code = 400;
                    return;
                }
                
                else
                {
                    _body_done_flag = true;
                    _state = Parsing_Done;
                }
                break;
            }
            case Parsing_Done:
            {
                return;
            }
        }//end of swtich 
        _storage += character;
    }
}

bool    HttpRequest::parsingCompleted() 
{ 
    return (_fields_done_flag ? (!_body_flag || _body_done_flag) : false);
}

HttpMethod  &HttpRequest::getMethod() 
{
    return _method;
}

std::string &HttpRequest::getHeader(std::string &name)
{
    return _request_headers[name];
}

std::string &HttpRequest::getPath()
{
    return _path;
}

std::string &HttpRequest::getQuery()
{
    return _query;
}

std::string &HttpRequest::getFragment()
{
    return _fragment;
}

void    HttpRequest::setMethod(HttpMethod & method)
{
    _method = method;
}

void    HttpRequest::setMaxBodySize(size_t size)
{
    _max_body_size = size;
}

void    HttpRequest::setHeader(std::string name, std::string value) 
{ 
    _request_headers[name] = value;
}

void        HttpRequest::printMessage()
{
    std::cout << _method_str[_method] + " " + _path + "?" + _query + "#" + _fragment
              + " " + "HTTP/" << _ver_major  << "." << _ver_minor << std::endl;
    
    for(std::map<std::string, std::string>::iterator it = _request_headers.begin();
    it != _request_headers.end(); ++it)
    {
        std::cout << it->first + ":" + it->second << std::endl;
    }
    for(std::vector<u_int8_t>::iterator it = _body.begin(); it != _body.end(); ++it)
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

    if (_request_headers.count("Content-Length"))
    {
        _body_flag = true;
        ss << _request_headers["Content-Length"];
        ss >> _body_length;
        if (_body_length < 0)
            std::cout << "ERR_BODY_LENGTH = " << _body_length << std::endl; 
        std::cout << "_BODY_LENGTH = " << _body_length << std::endl; 
    }
    else if ( _request_headers.count("Transfer-Encoding"))
    {
        _body_flag = true;
    }
    else
        _body_flag = false;
}

int     HttpRequest::errorCode()
{
    return(_error_code);
}

/* Clears all object variables and become ready to hold next request */
void    HttpRequest::clear()
{
    _path = "";
    _error_code = 0;
    _query = "";
    _fragment = "";
    _method = NONE;
    _method_index = 1;
    _state = Request_Line;
    _fields_done_flag = false;
    _body_flag = false;
    _body_done_flag = false;
    _storage.clear();
    _key_storage.clear();
    _request_headers.clear();
}

/**

 * Checks the value of header "Connection". If keep-alive, don't close the connection.

**/
bool        HttpRequest::keepAlive()
{
    if (_request_headers.count("Connection"))
    {
        if(_request_headers["Connection"].find("keep-alive", 0) != std::string::npos)
            return true;
    }
    return false;
}
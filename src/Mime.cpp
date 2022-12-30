#include "../inc/Mime.hpp"


Mime::Mime()
{
    _mime_types[".html"] = "text/html";
    _mime_types[".htm"] = "text/html";
    _mime_types[".css"] = "text/css";
    _mime_types[".ico"] = "image/x-icon";
    _mime_types[".avi"] = "video/x-msvideo";
    _mime_types[".bmp"] = "image/bmp";
    _mime_types[".doc"] = "application/msword";
    _mime_types[".gif"] = "image/gif";
    _mime_types[".gz"] = "application/x-gzip";
    _mime_types[".ico"] = "image/x-icon";
    _mime_types[".jpg"] = "image/jpeg";
    _mime_types[".jpeg"] = "image/jpeg";
    _mime_types[".png"] = "image/png";
    _mime_types[".txt"] = "text/plain";
    _mime_types[".mp3"] = "audio/mp3";
    _mime_types[".pdf"] = "application/pdf";
    _mime_types["default"] = "text/html";
}

std::string Mime::getMimeType(std::string extension)
{
    if (_mime_types.count(extension))
        return (_mime_types[extension]);
     return (_mime_types["default"]);
}

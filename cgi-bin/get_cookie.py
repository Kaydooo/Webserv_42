#! /usr/bin/python3

import os
from http import cookies
# Import modules for CGI handling 
import cgi, cgitb 

# Create instance of FieldStorage 
form = cgi.FieldStorage() 

# Get data from fields
key = form.getvalue('key')
cookie = cookies.SimpleCookie()
if 'HTTP_COOKIE' in os.environ:
    cookie.load(os.environ["HTTP_COOKIE"])
if key in cookie:
    print("HTTP/1.1 200 OK")
    print("Content-Type: text/plain\r\n")
    print("The Value of Cookie", key, "is", cookie[key].value)
else:
    print("HTTP/1.1 200 OK")
    print("Content-Type: text/plain\r\n")
    print("Cookie was not found !")


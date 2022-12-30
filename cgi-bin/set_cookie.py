#! /usr/bin/python3

import os
from http import cookies
# Import modules for CGI handling 
import cgi, cgitb 

# Create instance of FieldStorage 
form = cgi.FieldStorage() 

# Get data from fields
key = form.getvalue('key')
value  = form.getvalue('value')
cookie = cookies.SimpleCookie()
cookie[key] = value
print("HTTP/1.1 204 OK")
print(cookie.output())
print("\r\n")

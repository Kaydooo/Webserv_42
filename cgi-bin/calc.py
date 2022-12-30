#! /usr/bin/python3

import cgi
import sys
import os


form = cgi.FieldStorage()

arg1 = form.getvalue('f_num')
oper = form.getvalue('oper')
arg3 = form.getvalue('s_num')

print(os.environ["QUERY_STRING"], file=sys.stderr)
print("HTTP/1.1 200 OK")
print("Content-type: text/html\r\n\r\n")

if not oper or not arg1 or not arg3:
	print("Parameters are not correct")
	exit(1)

if len(oper) != 1:
    print("Parameters are not correct")
    exit(1)

if ord(oper) == 32:
	oper = '+'

if oper not in "+-/*^r":
    print("Parameters are not correct")
    exit(1)
if arg1[0] in '-+':
    if not arg1[1:].isdigit():
        print("Parameters are not correct")
        exit(1)
elif not arg1.isdigit():
        print("Parameters are not correct")
        exit(1)
if arg3[0] in '-+':
    if not arg3[1:].isdigit():
        print("Parameters are not correct")
        exit(1)
elif not arg3.isdigit():
        print("Parameters are not correct")
        exit(1)

num1 = int(arg1)
num2 = int(arg3)
if (num1 > 10000 or num2 > 10000):
    print("Enter a lower number")
    exit(1)
result = 0

if oper == '+': result = num1 + num2
elif oper == '-': result = num1 - num2
elif oper == '/' and num2 != 0: result = round(num1 / num2, 2)
elif oper == '*': result = round(num1 * num2, 2)
elif oper == '^': result = round(pow(num1, num2), 2)
elif oper == 'r': result = round(pow(num1, 1 / num2), 2)
else: result = "not determined"

try:
    print("<b>  = %s</b> <br>" % str(result))
except:
    print("<b>%s</b>: <br>" %("Something went wrong"))
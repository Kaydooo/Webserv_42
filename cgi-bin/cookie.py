from http import cookies
import os, sys

def show_cookie(c):
    for a, morsel in c.items():
        print(a,':', morsel.value,'<br>')

cookie = cookies.BaseCookie()
print("Content-type: text/html\r\n")
if 'HTTP_COOKIE' in os.environ: 
    cookie.load(os.environ["HTTP_COOKIE"])
    # cookie.clear()
    # print(cookie.output(), '<br>')
    show_cookie(cookie)
else:
    print("No Cookies Set Yet!")
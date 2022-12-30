#! /usr/bin/python3
from http import cookies
import os
import cgi
import time
import hashlib
import pickle
import sys

class Session:
    def __init__(self, name):
        self.name = name
        self.sid = hashlib.sha1(str(time.time()).encode("utf-8")).hexdigest()
        with open('cgi-bin/sessions/session_' + self.sid, 'wb') as f:
            pickle.dump(self, f)
    def getSid(self):
        return self.sid

""" Stores Users and thier data  """
class UserDataBase:
    def __init__(self):
        self.user_pass = {}
        self.user_firstname = {}
    def addUser(self, username, password, firstname):
        self.user_pass[username] = password
        self.user_firstname[username] = firstname
        with open('cgi-bin/user_database', 'wb') as f:
            pickle.dump(self, f)


def printAccPage(session):
    print("Content-type: text/html\r\n")
    print("<html>")
    print("<head>")
    print("<title>Account Page</title>")
    print("</head>")
    print("<body>")
    print("<h1>Welcome Again", session.name, "!</h1>")
    print("<p>Your Session ID is: ", session.getSid(), "</p>")
    print("</body>")
    print("<a href=\"/index.html\"> Click here to go back to homepage </a>")
    print("</html>")

def printUserMsg(msg):
    print("Content-type: text/html\r\n")
    print("<html>")
    print("<head>")
    print("<title>USER MSG</title>")
    print("</head>")
    print("<body>")
    print("<h1>", msg ,"</h1>")
    print("</body>")
    print("<a href=\"/login.html\"> Click here to go back to login page </a>")
    print("</html>")

def printLogin():
    print("Content-Type: text/html\r\n")
    print("<html> ")
    print("<head>")
    print("<meta charset=\"UTF-8\" name=\"viewport\" content=\"width=device-width, initial-scale=1\">")
    print("<link rel=\"stylesheet\" href=\"/assets/css/accstyle.css\">")
    print("<title> Login Page </title>")
    print("</head>")
    print("<body>  ")
    print("<center> <h1> Amanix Login Form </h1> </center> ")
    print("<form action = \"../cgi-bin/acc.py\" method = \"get\">")
    print("<div class=\"container\"> ")
    print("<label>Username : </label> ")
    print("<input type=\"text\" placeholder=\"Enter Username\" name=\"username\" required>")
    print("<label>Password : </label> ")
    print("<input type=\"password\" placeholder=\"Enter Password\" name=\"password\" required>")
    print("<button type=\"submit\">Login</button> ")
    print("No Account?<a href=\"/register.html\"> Register Here </a> ")
    print("</div> ")
    print("</form>   ")
    print("</body>   ")
    print("</html>")




def authUser(name, password):
    if os.path.exists('cgi-bin/user_database'):
        with open('cgi-bin/user_database', 'rb') as f:
            database = pickle.load(f)
            if name in database.user_pass and database.user_pass[name] == password:
                session = Session(database.user_firstname[name])
                return session
            else:
                return None
    else:
        return None

def handleLogin():
    username = form.getvalue('username')
    password = form.getvalue('password')
    firstname = form.getvalue('firstname')
    if username == None:
        printLogin()
    elif firstname == None:
        session = authUser(form.getvalue('username'), form.getvalue('password'))
        if(session == None):
            printUserMsg("Failed To Login, Username or Passowrd is wrong!")
        else:
            print("Correct Crenditales :D",file=sys.stderr)
            cookies.clear()
            cookies["SID"] = session.getSid()
            cookies["SID"]["expires"] = 120 # Session Expires after 2 mins
            print("HTTP/1.1 301 OK")
            print(cookies.output())
            print("location: acc.py")
            print("\r\n")
    else :
        if os.path.exists('cgi-bin/user_database'):
            with open('cgi-bin/user_database', 'rb') as f:
                database = pickle.load(f)
                if username in database.user_pass:
                    printUserMsg("Username is already Registerd !")
                else:
                    database.addUser(username, password, firstname)
                    printUserMsg("Account Registerd Successfully!")
        else:
            database = UserDataBase()
            if username in database.user_pass:
                printUserMsg("Username is already Registerd !")
            else:
                database.addUser(username, password, firstname)
                printUserMsg("Account Registerd Successfully!")

form = cgi.FieldStorage()
if 'HTTP_COOKIE' in os.environ: 
    cookies = cookies.SimpleCookie()
    cookies.load(os.environ["HTTP_COOKIE"])

    if "SID" in cookies:
        print("Your Session ID is", cookies["SID"].value,file=sys.stderr)
        with open('cgi-bin/sessions/session_'+ cookies["SID"].value, 'rb') as f:
            sess = pickle.load(f)
        printAccPage(sess)
    else:
        handleLogin()    
else:
    handleLogin()
            

        




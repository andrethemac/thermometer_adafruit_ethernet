#!C:\Python27\python.exe -u
#!/usr/bin/env python

import cgi
import cgitb; cgitb.enable()  # for troubleshooting
import sys, os
import MySQLdb as mdb

con = None

print "Content-type: text/html"
print

print """
<html>
<body>
"""

form = cgi.FieldStorage()
t = form.getvalue("t", "0")
h = form.getvalue("h", "0")
p = form.getvalue("p", "0")

con = mdb.connect('localhost', 'user', 'password', 'temperature');
qry = "INSERT INTO weather (temp,humidity,pressure) VALUES('" + t + "','" + h + "','" + p + "')"

print qry

with con:
    cur = con.cursor()
    cur.execute(qry)

con.close()

print """
  <form method="get" action="t2.py">
    <p>temperature: <input type="text" name="temperature"/></p>
    <p>humidity: <input type="text" name="humidity"/></p>
    <p>pressure: <input type="text" name="pressure"/></p>
  </form>
</body>
</html>
"""

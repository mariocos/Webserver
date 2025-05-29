#!/usr/bin/env python3

import os
import cgi

print("Content-Type: text/html")
print()

print("<!DOCTYPE html>")
print("<html lang='en'>")
print("<head><meta charset='UTF-8'><title>Hello CGI</title></head>")
print("<body>")
print("<h1>Hello from CGI!</h1>")

method = os.environ.get("REQUEST_METHOD", "UNKNOWN")
print(f"<p><strong>Request Method:</strong> {method}</p>")

if method == "GET":
    query = os.environ.get("QUERY_STRING", "")
    print(f"<p><strong>Query String:</strong> {query}</p>")

elif method == "POST":
    form = cgi.FieldStorage()
    print("<p><strong>POST Data:</strong></p>")
    for key in form.keys():
        print(f"<p>{key} = {form.getvalue(key)}</p>")

print("</body></html>")
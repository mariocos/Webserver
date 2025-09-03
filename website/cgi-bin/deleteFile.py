#!/usr/bin/env python3

import cgi
import os
import requests

# URL pattern for deleting files, assuming your server accepts DELETE requests at /delete/{filename}
SERVER_DELETE_URL_TEMPLATE = "http://127.0.0.1:8081/cgi-bin/{}"

print("Content-Type: text/html")
print()

form = cgi.FieldStorage()

if "filename" in form:
    filename = form.getfirst("filename")
    if filename:
        safe_filename = os.path.basename(filename)  # Sanitize filename

        url = SERVER_DELETE_URL_TEMPLATE.format(safe_filename)

        try:
            response = requests.delete(url)
            if response.status_code == 200:
                print(f"<html><body><h2>File '{safe_filename}' deleted successfully on server.</h2></body></html>")
            else:
                print(f"<html><body><h2>Failed to delete file '{safe_filename}'. Server responded with status code {response.status_code}.</h2></body></html>")
        except Exception as e:
            print(f"<html><body><h2>Error contacting server: {e}</h2></body></html>")
    else:
        print("<html><body><h2>No filename provided.</h2></body></html>")
else:
    print("<html><body><h2>No filename field in request.</h2></body></html>")

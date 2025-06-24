#!/usr/bin/env python3

import cgi
import os
import sys
import requests

UPLOAD_BASE_URL = "http://script:2424" 

print("Content-Type: text/html")
print()

form = cgi.FieldStorage()

if "file" in form:
    uploaded_file = form["file"]

    if uploaded_file.filename:
        filename = os.path.basename(uploaded_file.filename)
        file_data = uploaded_file.file.read()

        files = {'file': (filename, file_data)}

        server_url = f"{UPLOAD_BASE_URL}/{filename}"

        try:
            response = requests.post(server_url, files=files)

            if response.status_code == 201:
                print(f"<html><body><h2>File '{filename}' successfully uploaded to the server.</h2></body></html>")
            else:
                print(f"<html><body><h2>Server returned error: {response.status_code} {response.reason}</h2></body></html>")

        except Exception as e:
            print(f"<html><body><h2>Failed to send file to server: {e}</h2></body></html>")

    else:
        print("<html><body><h2>No file was selected for upload.</h2></body></html>")
else:
    print("<html><body><h2>No file field in request.</h2></body></html>")

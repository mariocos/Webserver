#!/usr/bin/env python3

import os

UPLOAD_DIR = "../../POSTED"

print("Content-Type: text/html")
print()

print("<html><body>")
print("<h2>Uploaded Files</h2>")

try:
    files = os.listdir(UPLOAD_DIR)
    if not files:
        print("<p>No files found.</p>")
    else:
        for filename in files:
            safe_name = os.path.basename(filename)
            print(f"<div>{safe_name} "
                  f"<button onclick=\"deleteFile('{safe_name}')\">Delete</button></div>")
except Exception as e:
    print(f"<p>Error reading directory: {e}</p>")

print("<button onclick=\"window.location.href='../index.html'\">Go to the Dummy Page</button>")

print("""
<script>
function deleteFile(filename) {
    fetch(`/find-this/deleteFile.py?filename=${encodeURIComponent(filename)}`)
        .then(res => res.text())
        .then(data => {
            alert(data);
            location.reload();
        })
        .catch(err => alert("Error: " + err));
}
</script>
""")

print("</body></html>")
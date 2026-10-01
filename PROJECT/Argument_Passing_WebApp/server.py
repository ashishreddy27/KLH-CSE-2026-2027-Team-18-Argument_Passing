#!/usr/bin/env python3
"""
ARGUMENT PASSING — ROOT WEB SERVER
Serves the web application dynamically on an available port (e.g. 8080).
"""

import http.server
import socketserver
import os
import subprocess
import webbrowser

START_PORT = 8080
PROJECT_ROOT = os.path.dirname(os.path.abspath(__file__))
FRONTEND_DIR = os.path.join(PROJECT_ROOT, "frontend")
PROGRAM_C = os.path.join(PROJECT_ROOT, "backend", "program.c")
PROGRAM_BIN = os.path.join(PROJECT_ROOT, "backend", "program")

socketserver.TCPServer.allow_reuse_address = True

class Handler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        # Serve frontend folder or root
        serve_dir = FRONTEND_DIR if os.path.exists(FRONTEND_DIR) else PROJECT_ROOT
        super().__init__(*args, directory=serve_dir, **kwargs)

    def log_message(self, format, *args):
        pass

if __name__ == "__main__":
    httpd = None
    actual_port = START_PORT
    for port in range(START_PORT, START_PORT + 20):
        try:
            httpd = socketserver.TCPServer(("", port), Handler)
            actual_port = port
            break
        except OSError:
            continue

    if not httpd:
        httpd = socketserver.TCPServer(("", 0), Handler)
        actual_port = httpd.server_address[1]

    url = f"http://localhost:{actual_port}"
    print("=" * 60)
    print("  ARGUMENT PASSING — WEB APPLICATION")
    print(f"  Running at: {url}")
    print("=" * 60)
    print(f"Open {url} in your browser.")
    print("Press Ctrl + C to stop the server.")

    try:
        webbrowser.open(url)
    except Exception:
        pass

    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\nServer stopped.")
        httpd.server_close()

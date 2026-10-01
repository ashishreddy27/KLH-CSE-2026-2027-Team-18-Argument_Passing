#!/usr/bin/env python3
"""
BACKEND SERVER — ARGUMENT PASSING WEB APPLICATION
Serves frontend static assets and executes C binary via REST API.
"""

import http.server
import socketserver
import json
import subprocess
import os
import shlex
import webbrowser

START_PORT = 8080
BACKEND_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.dirname(BACKEND_DIR)
FRONTEND_DIR = os.path.join(PROJECT_ROOT, "frontend")
PROGRAM_C = os.path.join(BACKEND_DIR, "program.c")
PROGRAM_BIN = os.path.join(BACKEND_DIR, "program")

# Enable address reuse
socketserver.TCPServer.allow_reuse_address = True

def compile_c_binary():
    """Ensure the C program is compiled before running."""
    if not os.path.exists(PROGRAM_BIN) or os.path.getmtime(PROGRAM_C) > os.path.getmtime(PROGRAM_BIN):
        try:
            subprocess.run(["gcc", "-o", PROGRAM_BIN, PROGRAM_C, "-Wall"], check=True, cwd=BACKEND_DIR)
            print("[BACKEND] Compiled program.c successfully.")
        except Exception as e:
            print(f"[BACKEND ERROR] Compilation failed: {e}")

class CustomHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=FRONTEND_DIR, **kwargs)

    def log_message(self, format, *args):
        pass  # Quiet logs

    def do_POST(self):
        if self.path == "/api/run":
            length = int(self.headers.get("Content-Length", 0))
            body = self.rfile.read(length).decode("utf-8")
            payload = json.loads(body) if body else {}

            prog_name = payload.get("program", "program").strip() or "program"
            raw_args = payload.get("args", "").strip()

            try:
                args = shlex.split(raw_args) if raw_args else []
            except Exception:
                args = raw_args.split() if raw_args else []

            argc = len(args) + 1
            cmd_str = f"./{prog_name}" + ((" " + " ".join(args)) if args else "")

            compile_c_binary()
            full_cmd = [PROGRAM_BIN] + args
            try:
                proc = subprocess.run(full_cmd, capture_output=True, text=True, cwd=BACKEND_DIR)
                stdout_text = proc.stdout
                exit_code = proc.returncode
            except Exception as e:
                stdout_text = f"Execution error: {str(e)}"
                exit_code = -1

            argv_data = [{"index": "argv[0]", "value": f"./{prog_name}", "type": f"Executable ({len(f'./{prog_name}')} chars)"}]
            for i, a in enumerate(args, start=1):
                if a.isdigit() or (a.startswith('-') and a[1:].isdigit()):
                    t_str = "Number / Integer (int)"
                else:
                    t_str = f"String ({len(a)} chars)"
                argv_data.append({"index": f"argv[{i}]", "value": a, "type": t_str})

            response = {
                "command": cmd_str,
                "argc": argc,
                "argv": argv_data,
                "stdout": stdout_text,
                "exit_code": exit_code
            }

            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            self.wfile.write(json.dumps(response).encode("utf-8"))

        elif self.path == "/api/terminal":
            length = int(self.headers.get("Content-Length", 0))
            body = self.rfile.read(length).decode("utf-8")
            payload = json.loads(body) if body else {}

            cmd = payload.get("command", "").strip()
            if not cmd:
                self.send_response(400)
                self.end_headers()
                return

            tokens = shlex.split(cmd)
            first = tokens[0]

            if first in ["./program", "program"]:
                compile_c_binary()
                args = tokens[1:]
                res = subprocess.run([PROGRAM_BIN] + args, capture_output=True, text=True, cwd=BACKEND_DIR)
                out = res.stdout
            elif first == "cat" and len(tokens) > 1 and tokens[1] == "program.c":
                with open(PROGRAM_C, "r") as f:
                    out = f.read()
            elif first == "ls":
                res = subprocess.run(["ls", "-la"], capture_output=True, text=True, cwd=BACKEND_DIR)
                out = res.stdout
            elif first == "gcc":
                compile_c_binary()
                out = "Compilation successful: binary 'program' built.\n"
            else:
                out = f"bash: {first}: command not found (try: ./program hello world, cat program.c, ls, clear)"

            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            self.wfile.write(json.dumps({"output": out}).encode("utf-8"))
        else:
            self.send_response(404)
            self.end_headers()

def run_server():
    compile_c_binary()
    
    # Try finding an open port starting from START_PORT (8080)
    httpd = None
    actual_port = START_PORT
    for port in range(START_PORT, START_PORT + 20):
        try:
            httpd = socketserver.TCPServer(("", port), CustomHandler)
            actual_port = port
            break
        except OSError:
            continue

    if not httpd:
        # Fallback to OS assigned port 0
        httpd = socketserver.TCPServer(("", 0), CustomHandler)
        actual_port = httpd.server_address[1]

    url = f"http://localhost:{actual_port}"
    print("=" * 60)
    print("  ARGUMENT PASSING — FULL-STACK WEB APPLICATION")
    print(f"  Backend & Frontend running at: {url}")
    print("=" * 60)
    print(f"Opening browser at: {url}")
    print("Press Ctrl + C in this terminal to stop the server.")

    # Try to open in default browser automatically
    try:
        webbrowser.open(url)
    except Exception:
        pass

    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\nShutting down backend server.")
        httpd.server_close()

if __name__ == "__main__":
    run_server()

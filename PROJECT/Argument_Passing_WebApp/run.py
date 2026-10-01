#!/usr/bin/env python3
"""
ROOT RUNNER — ARGUMENT PASSING
Launches the backend server and opens the frontend.
"""

import os
import sys
import subprocess

PROJECT_ROOT = os.path.dirname(os.path.abspath(__file__))
BACKEND_SERVER = os.path.join(PROJECT_ROOT, "backend", "server.py")

if __name__ == "__main__":
    try:
        subprocess.run([sys.executable, BACKEND_SERVER])
    except KeyboardInterrupt:
        print("\nExited.")

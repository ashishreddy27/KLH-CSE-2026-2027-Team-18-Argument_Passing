#!/usr/bin/env bash
# Launch Argument Passing Web Application (Backend & Frontend)

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"

echo "===================================================="
echo "   ARGUMENT PASSING — FULL-STACK WEB APPLICATION    "
echo "===================================================="

# Compile backend C program
cd "$DIR/backend"
gcc -o program program.c -Wall
cd "$DIR"

# Launch Backend server
python3 backend/server.py

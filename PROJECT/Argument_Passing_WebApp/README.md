# ARGUMENT PASSING — FULL-STACK WEB APPLICATION
### Systems Programming / Linux Command-Line Simulator

This folder contains the complete **Frontend** and **Backend** architecture for the Argument Passing web application.

---

## Folder Structure

```
argument-passing-web/
├── frontend/             # Frontend Client Application
│   ├── index.html        # Clean HTML user interface
│   ├── style.css         # Modern dark technical stylesheet
│   └── script.js         # Interactive controller & API caller
│
├── backend/              # Backend Application & System Program
│   ├── server.py         # Python REST API server (serves frontend & executes C)
│   ├── program.c         # Core C source program (argc & argv parser)
│   └── Makefile          # GCC build targets
│
├── run.py                # Python root launcher
├── run.sh                # Shell root launcher
└── README.md             # Project documentation
```

---

## How to Run in VS Code

### 1. Open the Project Folder in VS Code
Go to **File $\rightarrow$ Open Folder...** and choose:
```
/Users/ashishreddy/.gemini/antigravity/scratch/argument-passing-web
```

### 2. Run the Full-Stack Application (Backend + Frontend)
Open the VS Code Terminal (`Ctrl + ~`) and run:
```bash
python3 run.py
# or: ./run.sh
```
Then open your browser and navigate to:
👉 **`http://localhost:5000`**

---

## Alternative: Run Frontend or Backend Independently

- **Frontend Only (Direct in Browser):**
  Double-click `frontend/index.html` or open it with VS Code Live Server.

- **Backend C Program Only (Direct in Linux Terminal):**
  ```bash
  cd backend
  gcc -o program program.c -Wall
  ./program hello world 123
  ```

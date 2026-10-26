# ThreadLog - Multi-threaded Log Analysis System (OSSP Project)

Analyzes large log files with multiple threads to find errors, warnings and suspicious events.

## Contents
- `web/index.html` - website/dashboard (Web Workers = real threads). Just double-click to open; no install needed.
- `c/log_analyzer.c` - the same design in C with POSIX threads (pthreads).
- `c/gen_logs.py` - sample log generator.

## Run the website
Open `web/index.html` in Chrome/Edge/Firefox. If workers are blocked on `file://`, run:
`cd web && python3 -m http.server 8000` then open http://localhost:8000

## Run the C version (Linux/macOS/WSL)
```
cd c
python3 gen_logs.py 1000000 sample.log
gcc -O2 -pthread -o log_analyzer log_analyzer.c
./log_analyzer sample.log 1      # 1 thread
./log_analyzer sample.log 4      # 4 threads
```

## Design (OS concepts)
1. **Split** the file into chunks (work units in a shared queue).
2. **Producer-consumer**: N worker threads pop chunks from the queue (mutex-protected index).
3. **No data races**: each thread keeps private (local) counters.
4. **Critical section**: at the end each thread merges its counters into global stats under a mutex.
5. **Join** all threads, then print the report. Speedup is limited by cores and the serial parts (Amdahl's law).

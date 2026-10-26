#!/usr/bin/env python3
"""Generate a sample log file.  Usage: python3 gen_logs.py [lines=500000] [out=sample.log]"""
import random, sys, datetime
n = int(sys.argv[1]) if len(sys.argv) > 1 else 500000
out = sys.argv[2] if len(sys.argv) > 2 else "sample.log"
info = ["User logged in", "Request served 200 OK", "Cache refreshed", "Backup completed"]
warn = ["Disk usage at 85%", "Slow query detected", "High memory usage"]
err = ["Database timeout", "Service unavailable 503", "Connection refused"]
sus = ["Failed password for root", "Invalid user admin", "GET /?id=1 UNION SELECT x",
       "GET /../../etc/passwd", "Port scan detected", "Malware signature matched"]
t0 = datetime.datetime(2026, 10, 15)
with open(out, "w") as f:
    for i in range(n):
        ts = (t0 + datetime.timedelta(seconds=i * 129600 / n)).strftime("%Y-%m-%d %H:%M:%S")
        p = random.random(); ip = "10.0.%d.%d" % (random.randint(0, 7), random.randint(10, 70))
        if p < .012: lv, m, ip = "ERROR", random.choice(sus), "203.0.113.%d" % random.randint(1, 5)
        elif p < .02: lv, m = "CRITICAL", random.choice(err)
        elif p < .05: lv, m = "ERROR", random.choice(err)
        elif p < .13: lv, m = "WARN", random.choice(warn)
        else: lv, m = "INFO", random.choice(info)
        f.write(f"{ts} [{lv}] {ip} {m}\n")
print("wrote", out)

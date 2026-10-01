// ARGUMENT PASSING — FRONTEND CONTROLLER

document.addEventListener('DOMContentLoaded', () => {
  const progInput = document.getElementById('progName');
  const argsInput = document.getElementById('argsInput');
  const termCmdInput = document.getElementById('termCmdInput');

  progInput.addEventListener('input', runSimulator);
  argsInput.addEventListener('input', runSimulator);

  termCmdInput.addEventListener('keydown', handleTermKey);

  // Initial run on page load
  runSimulator();
});

function applyPreset(val) {
  document.getElementById('argsInput').value = val;
  runSimulator();
}

function clearFields() {
  document.getElementById('argsInput').value = '';
  runSimulator();
}

async function triggerFlowAnimation() {
  for (let i = 0; i < 7; i++) {
    document.querySelectorAll('.flow-node').forEach(n => n.classList.remove('active'));
    const node = document.getElementById('fn-' + i);
    if (node) node.classList.add('active');
    await new Promise(r => setTimeout(r, 90));
  }
  setTimeout(() => {
    document.querySelectorAll('.flow-node').forEach(n => n.classList.remove('active'));
  }, 500);
}

function parseTokens(raw) {
  if (!raw) return [];
  const tokens = [];
  let current = '';
  let inQuotes = false;
  for (let i = 0; i < raw.length; i++) {
    const c = raw[i];
    if (c === '"') {
      inQuotes = !inQuotes;
    } else if (c === ' ' && !inQuotes) {
      if (current.trim()) tokens.push(current.trim());
      current = '';
    } else {
      current += c;
    }
  }
  if (current.trim()) tokens.push(current.trim());
  return tokens;
}

// Helper to detect if a value is numeric
function isNumeric(val) {
  return /^-?\d+$/.test(val.trim());
}

async function runSimulator() {
  const prog = document.getElementById('progName').value.trim() || 'program';
  const rawArgs = document.getElementById('argsInput').value.trim();
  const args = parseTokens(rawArgs);

  const argc = args.length + 1;
  const fullCmd = `./${prog}` + (args.length > 0 ? ' ' + args.join(' ') : '');

  document.getElementById('outCmd').innerText = fullCmd;
  document.getElementById('outArgc').innerText = argc;

  // Try calling backend API first
  let backendData = null;
  try {
    const res = await fetch('/api/run', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ program: prog, args: rawArgs })
    });
    if (res.ok) {
      backendData = await res.json();
    }
  } catch (e) {
    // If backend server is not running, falls back cleanly
  }

  // Populate Argument Table
  const tbody = document.getElementById('argvTableBody');
  tbody.innerHTML = '';

  const allArgs = [`./${prog}`, ...args];
  allArgs.forEach((argVal, idx) => {
    const tr = document.createElement('tr');
    const hexAddr = '0x7ffe' + (0x1000 + idx * 0x12).toString(16);

    let displayValHtml = '';
    let typeFormatHtml = '';

    if (idx === 0) {
      displayValHtml = `<span style="color: #F8FAFC; font-weight: 600;">./${prog}</span>`;
      typeFormatHtml = `<span style="color: #06B6D4;">Executable (${argVal.length} chars)</span>`;
    } else if (isNumeric(argVal)) {
      displayValHtml = `<span style="color: #22C55E; font-weight: bold; font-size: 13px;">${argVal}</span>`;
      typeFormatHtml = `<span style="color: #38BDF8; font-weight: bold; background: rgba(56, 189, 248, 0.1); padding: 2px 8px; border-radius: 4px; border: 1px solid rgba(56, 189, 248, 0.25);">Number / Integer (int)</span>`;
    } else {
      displayValHtml = `<span style="color: #F8FAFC; font-weight: 600;">"${argVal}"</span>`;
      typeFormatHtml = `<span style="color: #94A3B8;">String (${argVal.length} chars)</span>`;
    }

    tr.innerHTML = `
      <td class="td-idx">argv[${idx}]</td>
      <td class="td-val">${displayValHtml}</td>
      <td>${typeFormatHtml}</td>
      <td class="td-addr">${hexAddr}</td>
    `;
    tbody.appendChild(tr);
  });

  // POSIX NULL row
  const nullTr = document.createElement('tr');
  nullTr.innerHTML = `
    <td class="td-idx" style="color: #94A3B8;">argv[${argc}]</td>
    <td class="td-null">NULL (0x0)</td>
    <td><span style="color: #F59E0B; font-size: 11px;">Sentinel Value (Standard)</span></td>
    <td class="td-addr">POSIX Boundary</td>
  `;
  tbody.appendChild(nullTr);

  triggerFlowAnimation();

  // Update C stdout box
  const cOutBox = document.getElementById('cStdoutBox');
  if (backendData && backendData.stdout) {
    cOutBox.innerText = backendData.stdout;
  } else {
    // Formatted stdout
    let simulatedOutput = `====================================================================\n`;
    simulatedOutput += `        ARGUMENT PASSING - OS & SYSTEMS PROGRAMMING                 \n`;
    simulatedOutput += `====================================================================\n`;
    simulatedOutput += `Argument Count (argc): ${argc}\n`;
    simulatedOutput += `--------------------------------------------------------------------\n`;
    simulatedOutput += `INDEX    ARGUMENT VALUE     DATA TYPE / FORMAT     MEMORY POINTER    \n`;
    simulatedOutput += `--------------------------------------------------------------------\n`;
    allArgs.forEach((val, idx) => {
      const hexAddr = '0x7ffe' + (0x1000 + idx * 0x12).toString(16);
      let desc = '';
      if (idx === 0) desc = `Executable (${val.length} chars)`;
      else if (isNumeric(val)) desc = `Number (int: ${val})`;
      else desc = `String (${val.length} chars)`;
      simulatedOutput += `argv[${idx.toString().padEnd(2)}]  ${val.padEnd(18)} ${desc.padEnd(22)} ${hexAddr}\n`;
    });
    simulatedOutput += `--------------------------------------------------------------------\n`;
    simulatedOutput += `POSIX Verification: argv[${argc}] is NULL (Standard Compliant)\n\n`;
    simulatedOutput += `[SUCCESS] Successfully received ${args.length} user argument(s).\n`;
    simulatedOutput += `====================================================================`;
    cOutBox.innerText = simulatedOutput;
  }

  updateTerminalLog(prog, args, argc);
}

function updateTerminalLog(prog, args, argc) {
  const fullCmd = `./${prog}` + (args.length > 0 ? ' ' + args.join(' ') : '');
  const log = document.getElementById('termLog');

  let lines = `student@linux:~$ ${fullCmd}\n`;
  lines += `====================================================================\n`;
  lines += `        ARGUMENT PASSING - OS & SYSTEMS PROGRAMMING                 \n`;
  lines += `====================================================================\n`;
  lines += `Argument Count (argc): ${argc}\n`;
  lines += `--------------------------------------------------------------------\n`;
  lines += `INDEX    ARGUMENT VALUE     DATA TYPE / FORMAT     MEMORY POINTER    \n`;
  lines += `--------------------------------------------------------------------\n`;
  
  const all = [`./${prog}`, ...args];
  all.forEach((val, idx) => {
    const hexAddr = '0x7ffe' + (0x1000 + idx * 0x12).toString(16);
    let desc = '';
    if (idx === 0) desc = `Executable (${val.length} chars)`;
    else if (isNumeric(val)) desc = `Number (int: ${val})`;
    else desc = `String (${val.length} chars)`;
    lines += `argv[${idx.toString().padEnd(2)}]  ${val.padEnd(18)} ${desc.padEnd(22)} ${hexAddr}\n`;
  });
  lines += `--------------------------------------------------------------------\n`;
  lines += `POSIX Verification: argv[${argc}] is NULL (Standard Compliant)\n`;
  lines += `[SUCCESS] Received ${args.length} user argument(s).\n`;
  lines += `====================================================================`;

  log.innerText = lines;
}

async function handleTermKey(e) {
  if (e.key === 'Enter') {
    const cmd = e.target.value.trim();
    e.target.value = '';
    if (!cmd) return;

    const log = document.getElementById('termLog');
    if (cmd === 'clear') {
      log.innerText = '';
      return;
    }

    log.innerText += `\nstudent@linux:~$ ${cmd}\n`;

    // Try backend API for terminal command execution
    try {
      const res = await fetch('/api/terminal', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ command: cmd })
      });
      if (res.ok) {
        const data = await res.json();
        log.innerText += data.output + '\n';
        log.scrollTop = log.scrollHeight;
        return;
      }
    } catch (err) {}

    // Fallback local terminal handling
    if (cmd.startsWith('./program')) {
      const parts = cmd.split(' ').slice(1);
      document.getElementById('argsInput').value = parts.join(' ');
      runSimulator();
    } else if (cmd === 'ls' || cmd === 'ls -la') {
      log.innerText += `-rwxr-xr-x 1 student student 33480 program\n-rw-r--r-- 1 student student  2638 program.c\n-rwxr-xr-x 1 student student  5550 server.py\n`;
    } else if (cmd === 'cat program.c') {
      log.innerText += `#include <stdio.h>\n#include <stdlib.h>\nint main(int argc, char *argv[]) {\n    printf("argc: %d\\n", argc);\n    for(int i=0; i<argc; i++) printf("argv[%d] = %s\\n", i, argv[i]);\n    return 0;\n}\n`;
    } else {
      log.innerText += `Executed: ${cmd}\n`;
    }
    log.scrollTop = log.scrollHeight;
  }
}

async function runDemoMode() {
  const demoCases = [
    "hello",
    "10 20 30",
    "alpha beta gamma"
  ];
  for (const c of demoCases) {
    document.getElementById('argsInput').value = c;
    runSimulator();
    await new Promise(r => setTimeout(r, 1300));
  }
}

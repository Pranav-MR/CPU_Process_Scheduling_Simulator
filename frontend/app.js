// --- Color Palette for Process Gantt Blocks ---
const PROCESS_COLORS = [
  '#3b82f6', // Blue
  '#10b981', // Emerald
  '#f59e0b', // Amber
  '#8b5cf6', // Violet
  '#ec4899', // Pink
  '#06b6d4', // Cyan
  '#f97316', // Orange
  '#14b8a6', // Teal
  '#a855f7', // Purple
  '#84cc16'  // Lime
];

function getProcessColor(pid) {
  if (pid === 'IDLE') return '#374151';
  let hash = 0;
  for (let i = 0; i < pid.length; i++) {
    hash = pid.charCodeAt(i) + ((hash << 5) - hash);
  }
  const index = Math.abs(hash) % PROCESS_COLORS.length;
  return PROCESS_COLORS[index];
}

// --- Preset Data ---
const PRESETS = {
  standard: [
    { id: 'P1', arrival: 0, burst: 5 },
    { id: 'P2', arrival: 1, burst: 3 },
    { id: 'P3', arrival: 2, burst: 8 },
    { id: 'P4', arrival: 3, burst: 6 }
  ],
  idle: [
    { id: 'P1', arrival: 2, burst: 4 },
    { id: 'P2', arrival: 8, burst: 3 },
    { id: 'P3', arrival: 14, burst: 5 }
  ],
  roundrobin: [
    { id: 'P1', arrival: 0, burst: 5 },
    { id: 'P2', arrival: 1, burst: 4 },
    { id: 'P3', arrival: 2, burst: 2 },
    { id: 'P4', arrival: 4, burst: 1 }
  ],
  clear: []
};

// --- DOM Elements ---
const tableBody = document.getElementById('process-table-body');
const addProcessBtn = document.getElementById('add-process-btn');
const resetBtn = document.getElementById('reset-btn');
const simulateBtn = document.getElementById('simulate-btn');
const presetSelect = document.getElementById('preset-select');
const quantumContainer = document.getElementById('quantum-container');
const timeQuantumInput = document.getElementById('time-quantum');
const errorBanner = document.getElementById('error-banner');
const algoCards = document.querySelectorAll('.algo-card');

const resultsPlaceholder = document.getElementById('results-placeholder');
const resultsDisplay = document.getElementById('results-display');
const resultsLoading = document.getElementById('results-loading');
const loadingStatusTitle = document.getElementById('loading-status-title');
const loadingStatusDesc = document.getElementById('loading-status-desc');
const loadingProgressBar = document.getElementById('loading-progress-bar');
const comparisonView = document.getElementById('comparison-view');

// --- Helper: Render Process Rows ---
function renderProcesses(processes) {
  tableBody.innerHTML = '';
  processes.forEach((p, index) => {
    addProcessRow(p.id, p.arrival, p.burst);
  });
}

function addProcessRow(id = '', arrival = 0, burst = 1) {
  if (!id) {
    const rowCount = tableBody.querySelectorAll('tr').length + 1;
    id = `P${rowCount}`;
  }

  const tr = document.createElement('tr');
  tr.innerHTML = `
    <td><input type="text" class="proc-id" value="${id}" placeholder="e.g. P1"></td>
    <td><input type="number" class="proc-arrival" value="${arrival}" min="0"></td>
    <td><input type="number" class="proc-burst" value="${burst}" min="1"></td>
    <td class="action-col">
      <button type="button" class="btn-delete-row" title="Delete Process">
        <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M3 6h18m-2 0v14a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2V6m3 0V4a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v2"></path></svg>
      </button>
    </td>
  `;

  tr.querySelector('.btn-delete-row').addEventListener('click', () => {
    tr.remove();
  });

  tableBody.appendChild(tr);
}

// --- Algorithm Selection Handling ---
function getSelectedAlgorithm() {
  const checked = document.querySelector('input[name="algorithm"]:checked');
  return checked ? checked.value : 'FCFS';
}

function updateAlgorithmUI() {
  const algo = getSelectedAlgorithm();
  algoCards.forEach(card => {
    const radio = card.querySelector('input');
    if (radio.checked) {
      card.classList.add('active');
    } else {
      card.classList.remove('active');
    }
  });

  if (algo === 'RR' || algo === 'ALL') {
    quantumContainer.classList.remove('hidden');
  } else {
    quantumContainer.classList.add('hidden');
  }
}

algoCards.forEach(card => {
  card.addEventListener('click', () => {
    const radio = card.querySelector('input');
    radio.checked = true;
    updateAlgorithmUI();
  });
});

// --- Presets & Controls Event Listeners ---
presetSelect.addEventListener('change', (e) => {
  const key = e.target.value;
  if (PRESETS[key]) {
    renderProcesses(PRESETS[key]);
  }
});

resetBtn.addEventListener('click', () => {
  presetSelect.value = 'standard';
  renderProcesses(PRESETS.standard);
  hideError();
});

addProcessBtn.addEventListener('click', () => {
  addProcessRow();
});

function showError(msg) {
  errorBanner.textContent = msg;
  errorBanner.classList.remove('hidden');
}

function hideError() {
  errorBanner.textContent = '';
  errorBanner.classList.add('hidden');
}

// --- Collect and Validate Inputs ---
function collectInputs() {
  hideError();
  const rows = tableBody.querySelectorAll('tr');
  if (rows.length === 0) {
    showError('Please add at least one process to simulate.');
    return null;
  }

  const processes = [];
  const pids = new Set();

  for (let i = 0; i < rows.length; i++) {
    const row = rows[i];
    const pid = row.querySelector('.proc-id').value.trim();
    const arrival = parseInt(row.querySelector('.proc-arrival').value, 10);
    const burst = parseInt(row.querySelector('.proc-burst').value, 10);

    if (!pid) {
      showError(`Row ${i + 1}: Process ID cannot be empty.`);
      return null;
    }
    if (pids.has(pid)) {
      showError(`Process ID "${pid}" is duplicated. Each Process ID must be unique.`);
      return null;
    }
    pids.add(pid);

    if (isNaN(arrival) || arrival < 0) {
      showError(`Process "${pid}": Arrival Time must be an integer >= 0.`);
      return null;
    }
    if (isNaN(burst) || burst <= 0) {
      showError(`Process "${pid}": Burst Time must be an integer > 0.`);
      return null;
    }

    processes.push({ id: pid, arrival, burst });
  }

  const algorithm = getSelectedAlgorithm();
  const quantum = parseInt(timeQuantumInput.value, 10);

  if ((algorithm === 'RR' || algorithm === 'ALL') && (isNaN(quantum) || quantum < 1)) {
    showError('Time Quantum must be an integer >= 1.');
    return null;
  }

  return { algorithm, quantum: isNaN(quantum) ? 2 : quantum, processes };
}

// --- Run Simulation Request with Realistic Execution Animation ---
simulateBtn.addEventListener('click', async () => {
  const payload = collectInputs();
  if (!payload) return;

  simulateBtn.disabled = true;
  simulateBtn.innerHTML = `
    <svg class="animate-spin" width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><circle cx="12" cy="12" r="10" stroke="currentColor" stroke-opacity="0.25"></circle><path d="M12 2a10 10 0 0 1 10 10" stroke="currentColor" stroke-linecap="round"></path></svg>
    Simulating with C Backend...
  `;

  // Display simulated execution state
  resultsPlaceholder.classList.add('hidden');
  resultsDisplay.classList.add('hidden');
  resultsLoading.classList.remove('hidden');

  loadingProgressBar.style.width = '20%';
  loadingStatusTitle.textContent = 'Invoking C Backend...';
  loadingStatusDesc.textContent = `Piping ${payload.processes.length} processes to scheduler.exe`;

  // Smooth scroll to output
  document.getElementById('results-section').scrollIntoView({ behavior: 'smooth' });

  const sleep = ms => new Promise(r => setTimeout(r, ms));

  try {
    // Step 1: Initiating request
    const fetchPromise = fetch('/api/simulate', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(payload)
    });

    await sleep(400);
    loadingProgressBar.style.width = '60%';
    const algoName = payload.algorithm === 'ALL' ? 'FCFS, SJF & Round Robin' : payload.algorithm;
    loadingStatusTitle.textContent = `Running ${algoName}...`;
    loadingStatusDesc.textContent = 'Allocating memory, updating ready queues, computing timeline...';

    const [response] = await Promise.all([
      fetchPromise,
      sleep(450) // Deliberate realistic processing buffer (~850ms total)
    ]);

    loadingProgressBar.style.width = '95%';
    loadingStatusTitle.textContent = 'Formatting Output...';
    loadingStatusDesc.textContent = 'Generating Gantt chart coordinates and metrics...';
    await sleep(250);

    const data = await response.json();

    if (!response.ok || data.error) {
      throw new Error(data.error || 'Failed to simulate scheduling.');
    }

    renderResults(payload.algorithm, data);
  } catch (err) {
    resultsLoading.classList.add('hidden');
    resultsPlaceholder.classList.remove('hidden');
    showError(err.message);
  } finally {
    simulateBtn.disabled = false;
    simulateBtn.innerHTML = `
      <svg width="18" height="18" viewBox="0 0 24 24" fill="currentColor"><polygon points="5 3 19 12 5 21 5 3"></polygon></svg>
      Run Simulation
    `;
  }
});

// --- Render Results & Gantt Chart ---
function renderResults(algorithm, data) {
  resultsLoading.classList.add('hidden');
  resultsPlaceholder.classList.add('hidden');
  resultsDisplay.classList.remove('hidden');

  if (data.mode === 'COMPARE_ALL') {
    // Show SJF as representative schedule (or FCFS), and show comparison dashboard
    const primaryResult = data.sjf || data.fcfs;
    displaySingleSchedule(primaryResult);
    displayComparison(data);
  } else {
    comparisonView.classList.add('hidden');
    displaySingleSchedule(data);
  }
}

function displaySingleSchedule(res) {
  // Update Metrics Cards
  document.getElementById('val-avg-wt').textContent = res.avg_waiting_time.toFixed(2);
  document.getElementById('val-avg-tat').textContent = res.avg_turnaround_time.toFixed(2);

  const lastGantt = res.gantt_chart[res.gantt_chart.length - 1];
  const totalScheduleTime = lastGantt ? lastGantt.end_time : 0;
  document.getElementById('val-total-time').textContent = totalScheduleTime;

  let busyTime = 0;
  res.gantt_chart.forEach(b => {
    if (b.id !== 'IDLE') {
      busyTime += (b.end_time - b.start_time);
    }
  });

  const utilization = totalScheduleTime > 0 ? ((busyTime / totalScheduleTime) * 100).toFixed(1) : 100;
  document.getElementById('val-utilization').textContent = `${utilization}%`;

  // Render Gantt Chart
  renderGanttChart(res.gantt_chart, totalScheduleTime);

  // Render Results Table
  renderResultsTable(res.processes);
}

function renderGanttChart(ganttBlocks, totalTime) {
  const ganttBar = document.getElementById('gantt-bar');
  const ganttTimeline = document.getElementById('gantt-timeline');
  const ganttLegend = document.getElementById('gantt-legend');

  ganttBar.innerHTML = '';
  ganttTimeline.innerHTML = '';
  ganttLegend.innerHTML = '';

  const uniquePids = new Set();

  ganttBlocks.forEach((block) => {
    const duration = block.end_time - block.start_time;
    if (duration <= 0) return;

    const widthPercent = totalTime > 0 ? (duration / totalTime) * 100 : 100;
    const blockEl = document.createElement('div');
    blockEl.className = `gantt-block ${block.id === 'IDLE' ? 'idle' : ''}`;
    blockEl.style.width = `${widthPercent}%`;
    blockEl.style.backgroundColor = getProcessColor(block.id);
    blockEl.title = `${block.id}: [${block.start_time} -> ${block.end_time}] (Duration: ${duration})`;

    blockEl.innerHTML = `
      <span>${block.id}</span>
      <span class="block-dur">${duration}</span>
    `;
    ganttBar.appendChild(blockEl);

    if (block.id !== 'IDLE') {
      uniquePids.add(block.id);
    }
  });

  // Render Timeline Markers
  const timePoints = new Set([0]);
  ganttBlocks.forEach(b => {
    timePoints.add(b.start_time);
    timePoints.add(b.end_time);
  });

  const sortedPoints = Array.from(timePoints).sort((a, b) => a - b);
  sortedPoints.forEach(t => {
    const mark = document.createElement('div');
    mark.className = 'timeline-mark';
    const leftPercent = totalTime > 0 ? (t / totalTime) * 100 : 0;
    mark.style.left = `${leftPercent}%`;
    mark.textContent = t;
    ganttTimeline.appendChild(mark);
  });

  // Render Legend
  uniquePids.forEach(pid => {
    const item = document.createElement('div');
    item.className = 'legend-item';
    item.innerHTML = `
      <span class="legend-color" style="background-color: ${getProcessColor(pid)}"></span>
      <span>${pid}</span>
    `;
    ganttLegend.appendChild(item);
  });
}

function renderResultsTable(processes) {
  const tbody = document.getElementById('results-table-body');
  tbody.innerHTML = '';

  processes.forEach(p => {
    const tr = document.createElement('tr');
    tr.innerHTML = `
      <td>
        <span class="process-tag">
          <span class="tag-dot" style="background-color: ${getProcessColor(p.id)}"></span>
          ${p.id}
        </span>
      </td>
      <td>${p.arrival_time}</td>
      <td>${p.burst_time}</td>
      <td><strong>${p.completion_time}</strong></td>
      <td>${p.turnaround_time}</td>
      <td>${p.waiting_time}</td>
    `;
    tbody.appendChild(tr);
  });
}

// --- Render Comparison View ---
function displayComparison(data) {
  comparisonView.classList.remove('hidden');

  const algos = [
    { key: 'fcfs', label: 'FCFS', res: data.fcfs },
    { key: 'sjf', label: 'SJF', res: data.sjf },
    { key: 'rr', label: 'Round Robin', res: data.rr }
  ];

  // Determine winner by lowest average waiting time
  let bestAlgo = algos[0];
  algos.forEach(a => {
    if (a.res.avg_waiting_time < bestAlgo.res.avg_waiting_time) {
      bestAlgo = a;
    }
  });

  // Render Cards
  const cardsGrid = document.getElementById('comparison-cards-grid');
  cardsGrid.innerHTML = '';

  algos.forEach(a => {
    const isWinner = a.key === bestAlgo.key;
    const card = document.createElement('div');
    card.className = `comp-card ${isWinner ? 'winner' : ''}`;
    card.innerHTML = `
      ${isWinner ? '<span class="winner-badge">&#127942; Lowest WT</span>' : ''}
      <div class="comp-title">${a.label}</div>
      <div class="comp-stat-row">
        <span class="label">Avg Waiting Time:</span>
        <span class="value">${a.res.avg_waiting_time.toFixed(2)} ms</span>
      </div>
      <div class="comp-stat-row">
        <span class="label">Avg Turnaround Time:</span>
        <span class="value">${a.res.avg_turnaround_time.toFixed(2)} ms</span>
      </div>
    `;
    cardsGrid.appendChild(card);
  });

  // Render Comparison Bars
  renderBarComparison('wt-bars-container', algos, 'avg_waiting_time', '#6366f1');
  renderBarComparison('tat-bars-container', algos, 'avg_turnaround_time', '#10b981');

  // Insights
  const insightEl = document.getElementById('comparison-insight');
  insightEl.innerHTML = `
    <strong>Analysis Insight:</strong> In this workload, <strong>${bestAlgo.label}</strong> achieved the optimal scheduling efficiency with the lowest average waiting time of <strong>${bestAlgo.res.avg_waiting_time.toFixed(2)} ms</strong> and average turnaround time of <strong>${bestAlgo.res.avg_turnaround_time.toFixed(2)} ms</strong>.
  `;
}

function renderBarComparison(containerId, algos, metricKey, color) {
  const container = document.getElementById(containerId);
  container.innerHTML = '';

  const maxVal = Math.max(...algos.map(a => a.res[metricKey]), 1);

  algos.forEach(a => {
    const val = a.res[metricKey];
    const percent = Math.max((val / maxVal) * 100, 8); // At least 8% so text fits

    const row = document.createElement('div');
    row.className = 'bar-row';
    row.innerHTML = `
      <div class="bar-name">${a.label}</div>
      <div class="bar-track">
        <div class="bar-fill" style="width: ${percent}%; background-color: ${color};">
          ${val.toFixed(2)}
        </div>
      </div>
    `;
    container.appendChild(row);
  });
}

// --- Initialize with Standard Preset on Load ---
document.addEventListener('DOMContentLoaded', () => {
  renderProcesses(PRESETS.standard);
  updateAlgorithmUI();
});

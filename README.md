# CPU Process Scheduling Simulator (Data Structures Project)

An interactive, full-stack CPU Process Scheduling Simulator designed for college Operating Systems and Data Structures curricula. It combines a **pure C Data Structures backend** with a **modern web-based UI (HTML, CSS, Vanilla JavaScript)** connected via a zero-dependency local bridge.

---

## 🌟 Key Features

- **Three Core Scheduling Algorithms**:
  - **FCFS (First Come First Serve)**: Non-preemptive, strict FIFO arrival order.
  - **SJF (Shortest Job First)**: Non-preemptive, selects the process with the minimum burst time among arrived jobs.
  - **Round Robin (RR)**: Preemptive time-slicing using a Circular Queue data structure.
  - **Compare All Mode**: Runs all three algorithms simultaneously on the same process dataset to contrast performance metrics.
- **Dynamic Process Management**: Add, modify, or remove processes with customizable Process ID, Arrival Time ($AT$), and Burst Time ($BT$).
- **Configurable Time Quantum**: Set the time slice for Round Robin scheduling.
- **Interactive Gantt Chart**: Color-coded timeline displaying exact process execution slices and CPU **IDLE** states.
- **Metrics & Formulas**:
  - Completion Time ($CT$)
  - Turnaround Time ($TAT = CT - AT$)
  - Waiting Time ($WT = TAT - BT$)
  - Average Turnaround Time ($\overline{TAT}$) & Average Waiting Time ($\overline{WT}$)
  - CPU Utilization percentage
- **Side-by-Side Comparison**: Visual bar charts comparing Average Waiting Time and Turnaround Time with an optimality summary.

---

## 🏗️ Project Architecture & Data Structures

```text
CPU_Process_Scheduling_Simulator/
│
├── c_backend/
│   ├── scheduler.c        # Core Data Structures in C (Queue, structs, FCFS, SJF, RR)
│   ├── compile.bat        # 1-Click build script for MinGW GCC
│   └── scheduler.exe      # Compiled C binary
│
├── frontend/
│   ├── index.html         # User Interface layout
│   ├── style.css          # Modern dark-mode styling & responsive design
│   └── app.js             # Form controls, API calls, dynamic Gantt chart rendering
│
├── server.py              # Zero-dependency Python server bridge (serves UI & executes C binary)
├── README.md              # Project documentation & viva guide
└── .gitignore
```

### Data Structures in C (`c_backend/scheduler.c`)

1. **`Process` Struct**:
   Represents process attributes and calculated scheduling metrics:
   ```c
   typedef struct {
       char id[16];
       int arrival_time;
       int burst_time;
       int remaining_time;
       int completion_time;
       int turnaround_time;
       int waiting_time;
       int start_time;
       int completed;
   } Process;
   ```

2. **Circular Queue Struct (for Round Robin)**:
   Implemented from scratch for FIFO process scheduling:
   ```c
   typedef struct {
       int items[MAX_PROCESSES * 20];
       int front;
       int rear;
       int count;
   } Queue;

   void initQueue(Queue *q);
   int isEmpty(Queue *q);
   void enqueue(Queue *q, int value);
   int dequeue(Queue *q);
   ```

3. **`GanttBlock` Struct**:
   Tracks time intervals and handles CPU Idle periods:
   ```c
   typedef struct {
       char id[16];     // Process ID or "IDLE"
       int start_time;
       int end_time;
   } GanttBlock;
   ```

---

## 🚀 How to Run

### Prerequisites
- **GCC** (e.g., MinGW on Windows or standard GCC on Linux/macOS)
- **Python 3.x** (Standard library only; no `pip install` required!)

---

### Method 1: Run the Interactive Web Simulator (Recommended)

1. Open your terminal in the project directory:
   ```powershell
   cd CPU_Process_Scheduling_Simulator
   ```
2. Start the simulator:
   ```powershell
   python server.py
   ```
   *(The server will automatically compile `scheduler.c` if needed!)*
3. Open your browser and navigate to:
   ```text
   http://localhost:5000
   ```

---

### Method 2: Run the C Backend Directly in the Terminal (Great for Viva Demonstration!)

You can compile and test the pure C Data Structures engine standalone:

1. Compile the C code:
   ```powershell
   cd c_backend
   gcc -Wall -O2 scheduler.c -o scheduler.exe
   ```

2. Run with standard input:
   ```text
   # Format: <ALGORITHM> <QUANTUM> <N> followed by N lines of <PID> <ARRIVAL> <BURST>
   ```

   **Example (FCFS):**
   ```text
   FCFS 2 3
   P1 0 5
   P2 1 3
   P3 2 1
   ```

   **Example (Round Robin with Quantum = 2):**
   ```text
   RR 2 3
   P1 0 5
   P2 1 3
   P3 2 1
   ```

   The C executable outputs clean JSON with process metrics and Gantt chart coordinates directly to standard output.

---

## 📐 Formulas Reference

$$\text{Turnaround Time (TAT)} = \text{Completion Time (CT)} - \text{Arrival Time (AT)}$$

$$\text{Waiting Time (WT)} = \text{Turnaround Time (TAT)} - \text{Burst Time (BT)}$$

$$\text{Average Turnaround Time} = \frac{\sum_{i=1}^{n} \text{TAT}_i}{n}$$

$$\text{Average Waiting Time} = \frac{\sum_{i=1}^{n} \text{WT}_i}{n}$$

$$\text{CPU Utilization} = \left(\frac{\text{Total Busy Time}}{\text{Total Schedule Time}}\right) \times 100\%$$

---

## 🎓 Lab Viva Questions & Answers

| Question | Answer |
|---|---|
| **Why is SJF optimal for minimizing average waiting time?** | SJF schedules the shortest jobs first, reducing the waiting delay for the maximum number of processes. |
| **What is Convoy Effect in FCFS?** | When a long CPU-burst process arrives first, all subsequent short processes are forced to wait, causing high average waiting times. |
| **What happens if the Time Quantum in Round Robin is very large?** | If $q \ge \max(BT)$, Round Robin degenerates into FCFS scheduling. |
| **What happens if the Time Quantum in Round Robin is too small?** | Extreme context switching overhead reduces CPU efficiency in real OS kernels. |
| **Why is a Circular Queue used in Round Robin?** | A Circular Queue allows $O(1)$ enqueue and dequeue operations without memory shifts when processes re-enter the ready queue. |

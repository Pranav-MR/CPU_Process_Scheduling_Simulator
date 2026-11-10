# CPU Process Scheduling Simulator
## Presentation Script & Viva Defense Guide (Split for 3 Presenters)

---

### Team Roles & Breakdown

| Presenter | Role | Topics Covered |
|---|---|---|
| **Speaker 1 (You - Project Lead)** | **Working Background & Architecture** | Project Introduction, Problem Statement, C Data Structures Architecture (`struct Process`, `Queue`, `GanttBlock`), and Live Demo Guide (How to run UI + CLI versions). |
| **Speaker 2** | **Algorithm Logic Specialist** | Detailed walkthrough of FCFS, Non-Preemptive SJF, and Round Robin scheduling logic, time slicing, and CPU idle-time handling. |
| **Speaker 3** | **Metrics, Comparison & Conclusion** | Gantt Chart visualization, Formulas ($CT, TAT, WT$), Performance Comparison analysis, Key Takeaways, and Q&A. |

---

## Slide-by-Slide Speaking Script

---

### Slide 1: Title Slide
**Title:** CPU Process Scheduling Simulator  
**Subtitle:** A Data Structures & Operating Systems Project with C Engine & Interactive Visualization  
**Presenter:** **Speaker 1 (You)**

> **What to Say:**  
> *"Good morning/afternoon, Respected Teacher and classmates. Today, our team is presenting our project: the **CPU Process Scheduling Simulator**.  
> In any modern operating system, the CPU is the most critical resource. Deciding which process gets CPU time, when, and for how long is the job of CPU scheduling algorithms.  
> Our project bridges theory and practice: we implemented the core scheduling algorithms and data structures entirely in **pure C**, and also built a clean, interactive simulator with Gantt charts to visualize and compare their performance in real-time.  
> I will walk you through the working background and how both the programs run, followed by my teammates who will explain the algorithm logic and comparative results."*

---

### Slide 2: Problem Statement & Objectives
**Title:** Project Objectives & Importance  
**Presenter:** **Speaker 1 (You)**

> **What to Say:**  
> *"When studying CPU scheduling in textbooks, students often struggle to visualize how processes move through the ready queue, how preemptions occur in Round Robin, and how idle gaps affect waiting times.  
> Our primary objectives were:  
> 1. To implement **FCFS**, **Non-preemptive SJF**, and **Round Robin** using authentic C Data Structures: custom structs, arrays, and queues.  
> 2. To build an accurate **Gantt Chart generator** that properly identifies CPU IDLE time when no process has arrived.  
> 3. To accurately compute scheduling metrics: Completion Time, Turnaround Time, and Waiting Time.  
> 4. To provide two working environments: a **standalone C terminal program** for direct code inspection and viva demonstration, and a **modern web UI** for dynamic visualization."*

---

### Slide 3: System Architecture & Data Structures in C
**Title:** Working Background & C Data Structures  
**Presenter:** **Speaker 1 (You)**

> **What to Say:**  
> *"Now, let me explain the working background and the data structures we implemented in C.  
> Everything in our backend is built using standard C without any third-party libraries:  
> 
> 1. **`struct Process`**: Represents each process. It stores the Process ID, Arrival Time ($AT$), Burst Time ($BT$), Completion Time ($CT$), Turnaround Time ($TAT$), Waiting Time ($WT$), and a `rem_bt` field to track remaining burst time for Round Robin.  
> 2. **Queue Data Structure**: For Round Robin, we implemented an authentic FIFO queue with `push()`, `pop()`, and `isQueueEmpty()` helper functions. This allows newly arrived processes and preempted processes to be enqueued in exact order.  
> 3. **`struct Gantt`**: Records execution intervals `[start_time, end_time]` and process IDs. If there is a gap where no process has arrived yet, it explicitly generates an **IDLE block**, ensuring mathematical correctness.  
> 
> In the web version, a lightweight Python bridge calls this compiled C executable and returns the output to our frontend, while in our standalone version (`cli_scheduler.c`), everything runs directly in the terminal."*

---

### Slide 4: Live Demo Guide — How to Run Both Versions
**Title:** How to Run the Programs (CLI & UI)  
**Presenter:** **Speaker 1 (You)**

> **What to Say:**  
> *"We designed our project so our teacher can test it in two distinct ways:  
> 
> **Version 1: Standalone Terminal Program (`cli_scheduler.c`)**  
> If you want to check pure C code execution without any browser:  
> - Step 1: Open your terminal and compile with GCC:  
>   `gcc -Wall -O2 cli_scheduler.c -o cli_scheduler.exe`  
> - Step 2: Run the binary: `.\cli_scheduler.exe`  
> - You get a menu-driven interface to input processes, select an algorithm, and view an ASCII Gantt chart with completion tables right in the terminal.  
> 
> **Version 2: Full Interactive Web Simulator**  
> If you want visual charts, animation, and live side-by-side comparison:  
> - Step 1: In the project directory, run: `python server.py`  
>   *(The script automatically verifies and compiles the C backend if needed)*  
> - Step 2: Open your browser to `http://localhost:5000`  
> - You can add rows, load presets, customize Round Robin time quantum, and see an animated Gantt chart with performance cards.  
> 
> Now, I hand over to [Speaker 2] to walk you through the scheduling algorithms."*

---

### Slide 5: Algorithm 1 — First Come First Serve (FCFS)
**Title:** First Come First Serve (FCFS)  
**Presenter:** **Speaker 2**

> **What to Say:**  
> *"Thank you, [Speaker 1]. I will explain the algorithms starting with **First Come First Serve (FCFS)**.  
> FCFS is the simplest CPU scheduling algorithm. It is non-preemptive and allocates the CPU to processes in strict order of their arrival time.  
> 
> **How our C code executes FCFS:**  
> 1. We sort all processes in ascending order of their Arrival Time using a bubble sort algorithm.  
> 2. We maintain a `current_time` clock initialized to 0.  
> 3. If `current_time < process.arrival_time`, the CPU has nothing to execute, so our code logs an `IDLE` block in the Gantt chart and advances `current_time` to the arrival time.  
> 4. The process executes for its entire burst time: `current_time += process.burst_time`.  
> 5. Its Completion Time is recorded as `current_time`.  
> 
> **Limitation:** FCFS suffers from the **Convoy Effect**. If a heavy process with a large burst time arrives first, all subsequent short processes are forced to wait, leading to a high average waiting time."*

---

### Slide 6: Algorithm 2 — Shortest Job First (Non-Preemptive SJF)
**Title:** Shortest Job First (SJF)  
**Presenter:** **Speaker 2**

> **What to Say:**  
> *"To solve the Convoy Effect, we implemented **Shortest Job First (SJF - Non-Preemptive)**.  
> SJF selects the waiting process with the smallest CPU burst time.  
> 
> **How our C code executes SJF:**  
> 1. At any given `current_time`, our code scans all processes that have arrived (`arrival_time <= current_time`) and are not yet completed.  
> 2. Among these ready processes, it selects the one with the minimum `burst_time`.  
> 3. Once selected, the process runs non-preemptively until completion.  
> 4. If no process has arrived yet, the scheduler advances `current_time` to the nearest future arrival time, marking CPU idle time.  
> 
> **Advantage:** Provably optimal in minimizing the average waiting time for a given set of processes when all jobs are available."*

---

### Slide 7: Algorithm 3 — Round Robin (RR) with Queue Data Structure
**Title:** Round Robin (RR) with Queue DS  
**Presenter:** **Speaker 2**

> **What to Say:**  
> *"The third algorithm is **Round Robin (RR)**, which is the standard algorithm used in modern time-sharing operating systems. It is preemptive and fair.  
> 
> **How our C code executes Round Robin:**  
> 1. We allocate each process a fixed time slice called the **Time Quantum ($q$)**.  
> 2. We maintain a FIFO ready queue using our `queue[]` array with `push()` and `pop()`.  
> 3. Processes that arrive at time 0 are pushed into the queue first.  
> 4. We pop the process at the front and execute it for `min(time_quantum, remaining_burst)`.  
> 5. While it executes, any new processes that arrive are pushed into the ready queue.  
> 6. If the current process still has remaining execution time left, it is placed back at the rear of the queue. Otherwise, it is marked completed.  
> 
> This guarantees responsiveness and prevents starvation. Now, [Speaker 3] will explain the Gantt chart visualization, formulas, and performance comparison."*

---

### Slide 8: Gantt Chart & Key Evaluation Formulas
**Title:** Gantt Chart & Mathematical Formulas  
**Presenter:** **Speaker 3**

> **What to Say:**  
> *"Thank you, [Speaker 2]. I will now explain the evaluation metrics and how we visualize the execution timeline.  
> 
> **1. Gantt Chart Visualization:**  
> A Gantt chart represents the CPU timeline. Each block shows which process held the CPU and for what duration. In our CLI, we render a clean ASCII box timeline, and in our web app, it renders color-coded responsive bars. Crucially, CPU idle periods are explicitly displayed so the user can see when the CPU was waiting for jobs.  
> 
> **2. Formulas Used in Our Code:**  
> - **Turnaround Time (TAT)**: The total time elapsed from process arrival to completion:  
>   $$TAT = CT - AT$$  
> - **Waiting Time (WT)**: The total time the process spent waiting in the ready queue:  
>   $$WT = TAT - BT$$  
> - **Average Turnaround Time**:  
>   $$\overline{TAT} = \frac{\sum TAT}{n}$$  
> - **Average Waiting Time**:  
>   $$\overline{WT} = \frac{\sum WT}{n}$$  
> - **CPU Utilization**:  
>   $$\text{Utilization} = \frac{\text{Total Busy Time}}{\text{Total Schedule Time}} \times 100\%$$"*

---

### Slide 9: Performance Comparison & Results Analysis
**Title:** Comparison of Scheduling Algorithms  
**Presenter:** **Speaker 3**

> **What to Say:**  
> *"To prove the effectiveness of our simulator, we evaluated all three algorithms on the exact same test dataset (e.g., Processes P1 through P4 with varied arrival and burst times).  
> 
> **Observed Results:**  
> - **FCFS**: Yielded an Average Waiting Time of **5.75 ms** and Average Turnaround Time of **11.25 ms**. It is easy to implement, but vulnerable to convoy delays.  
> - **SJF (Non-Preemptive)**: Achieved the lowest Average Waiting Time of **5.25 ms** and Average Turnaround Time of **10.75 ms**, confirming that prioritizing shorter bursts minimizes queue delay.  
> - **Round Robin ($q=2$)**: Yielded an Average Waiting Time of **9.75 ms** and Turnaround Time of **15.25 ms**. While its waiting time is higher due to frequent context switching, it provides the best response time and fairness, ensuring no process is starved.  
> 
> In both our Web UI and CLI comparison tables, our program automatically analyzes these averages and announces the most optimal algorithm for the given input."*

---

### Slide 10: Conclusion & Key Learnings
**Title:** Key Learnings, Viva Q&A & Conclusion  
**Presenter:** **Speaker 3 (with Speaker 1 joining for Q&A)**

> **What to Say:**  
> *"To conclude, this project gave us hands-on experience in:  
> 1. Implementing core Data Structures (structures, circular/array queues, sorting) to solve real OS-level problems.  
> 2. Handling real-world edge cases like CPU idle times, preemptions, and zero-arrival offsets.  
> 3. Designing both a robust terminal interface and an intuitive visual simulator.  
> 
> **Common Teacher Viva Questions We Are Ready For:**  
> - *Teacher: 'Why did you use a Queue for Round Robin?'*  
>   **Answer:** A FIFO Queue guarantees fair, circular ordering: popped processes execute for one quantum, and unfinished jobs re-enter the rear of the queue in $O(1)$ time.  
> - *Teacher: 'Why can't SJF be used in real general-purpose Operating Systems?'*  
>   **Answer:** Because in real operating systems, the CPU cannot predict the exact future burst time of a process before it runs.  
> - *Teacher: 'What happens if Quantum in Round Robin is very large or very small?'*  
>   **Answer:** If $q \ge \max(BT)$, Round Robin becomes FCFS. If $q$ is extremely small, CPU spends more time on context switching than actual execution.  
> 
> Thank you for your time. We are now open to any questions!"*

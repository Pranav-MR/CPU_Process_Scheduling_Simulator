#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 100
#define MAX_GANTT 1000

// ============================================================================
// DATA STRUCTURES
// ============================================================================

// Structure representing a single CPU process
typedef struct {
    char id[16];          // Process identifier (e.g. "P1", "P2")
    int arrival_time;     // Time at which process arrives in ready queue
    int burst_time;       // CPU execution time required
    int remaining_time;   // Remaining CPU burst time (used in Round Robin)
    int completion_time;  // Time at which process finishes execution
    int turnaround_time;  // TAT = Completion Time - Arrival Time
    int waiting_time;     // WT = Turnaround Time - Burst Time
    int completed;        // Completion flag (1 = completed, 0 = pending)
} Process;

// Structure representing a single execution block in the Gantt chart
typedef struct {
    char id[16];          // Process ID or "IDLE"
    int start_time;       // Start timestamp
    int end_time;         // End timestamp
} GanttBlock;

// Circular Queue structure used for Round Robin scheduling
typedef struct {
    int items[MAX_PROCESSES * 20];
    int front;
    int rear;
    int count;
} Queue;

// ============================================================================
// QUEUE OPERATIONS (FOR ROUND ROBIN)
// ============================================================================

void initQueue(Queue *q) {
    q->front = 0;
    q->rear = -1;
    q->count = 0;
}

int isEmpty(Queue *q) {
    return (q->count == 0);
}

int isFull(Queue *q) {
    return (q->count >= MAX_PROCESSES * 20);
}

void enqueue(Queue *q, int value) {
    if (isFull(q)) {
        printf("Queue overflow!\n");
        return;
    }
    q->rear = (q->rear + 1) % (MAX_PROCESSES * 20);
    q->items[q->rear] = value;
    q->count++;
}

int dequeue(Queue *q) {
    if (isEmpty(q)) {
        return -1;
    }
    int val = q->items[q->front];
    q->front = (q->front + 1) % (MAX_PROCESSES * 20);
    q->count--;
    return val;
}

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

// Flush any leftover newline characters from standard input
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Add a block to Gantt chart, merging consecutive executions of the same process
void addGanttBlock(GanttBlock gantt[], int *gantt_count, const char *id, int start, int end) {
    if (start == end) return;
    if (*gantt_count > 0 && strcmp(gantt[*gantt_count - 1].id, id) == 0 && gantt[*gantt_count - 1].end_time == start) {
        gantt[*gantt_count - 1].end_time = end;
    } else {
        strcpy(gantt[*gantt_count].id, id);
        gantt[*gantt_count].start_time = start;
        gantt[*gantt_count].end_time = end;
        (*gantt_count)++;
    }
}

// Display the ASCII Gantt Chart in the terminal
void printGanttChart(GanttBlock gantt[], int count) {
    if (count == 0) {
        printf("No Gantt chart to display.\n");
        return;
    }

    printf("\n--- GANTT CHART ---\n\n");

    // Top border
    for (int i = 0; i < count; i++) {
        printf("+-------");
    }
    printf("+\n");

    // Process IDs
    for (int i = 0; i < count; i++) {
        if (strcmp(gantt[i].id, "IDLE") == 0) {
            printf("|  IDLE ");
        } else {
            printf("|  %-4s ", gantt[i].id);
        }
    }
    printf("|\n");

    // Bottom border
    for (int i = 0; i < count; i++) {
        printf("+-------");
    }
    printf("+\n");

    // Timeline timestamps
    printf("%-7d", gantt[0].start_time);
    for (int i = 0; i < count; i++) {
        printf("%-8d", gantt[i].end_time);
    }
    printf("\n\n");

    // Detailed timeline list
    printf("Execution Timeline:\n");
    for (int i = 0; i < count; i++) {
        int duration = gantt[i].end_time - gantt[i].start_time;
        if (strcmp(gantt[i].id, "IDLE") == 0) {
            printf("  [Time %2d -> %2d] CPU IDLE (%d units)\n", gantt[i].start_time, gantt[i].end_time, duration);
        } else {
            printf("  [Time %2d -> %2d] %s (%d units)\n", gantt[i].start_time, gantt[i].end_time, gantt[i].id, duration);
        }
    }
    printf("\n");
}

// Display table of metrics (CT, TAT, WT) and summary averages
void printResultsTable(const char *algoName, Process proc[], int n, GanttBlock gantt[], int gantt_count, double avg_tat, double avg_wt) {
    printf("\n========================================================================================\n");
    printf("                     PERFORMANCE RESULTS: %s\n", algoName);
    printf("========================================================================================\n");
    printf("%-10s %-15s %-12s %-18s %-18s %-12s\n", 
           "PID", "Arrival Time", "Burst Time", "Completion Time", "Turnaround Time", "Waiting Time");
    printf("----------------------------------------------------------------------------------------\n");

    for (int i = 0; i < n; i++) {
        printf("%-10s %-15d %-12d %-18d %-18d %-12d\n",
               proc[i].id,
               proc[i].arrival_time,
               proc[i].burst_time,
               proc[i].completion_time,
               proc[i].turnaround_time,
               proc[i].waiting_time);
    }
    printf("========================================================================================\n");

    // Calculate total time and CPU utilization
    int total_time = (gantt_count > 0) ? gantt[gantt_count - 1].end_time : 0;
    int busy_time = 0;
    for (int i = 0; i < gantt_count; i++) {
        if (strcmp(gantt[i].id, "IDLE") != 0) {
            busy_time += (gantt[i].end_time - gantt[i].start_time);
        }
    }
    double utilization = (total_time > 0) ? ((double)busy_time / total_time) * 100.0 : 100.0;

    printf("Total Execution Time    : %d units\n", total_time);
    printf("CPU Busy Time           : %d units\n", busy_time);
    printf("CPU Utilization         : %.2f%%\n", utilization);
    printf("Average Turnaround Time : %.2f ms   (Formula: TAT = CT - AT)\n", avg_tat);
    printf("Average Waiting Time    : %.2f ms   (Formula: WT = TAT - BT)\n", avg_wt);
    printf("========================================================================================\n");
}

// ============================================================================
// ALGORITHM 1: FIRST COME FIRST SERVE (FCFS)
// ============================================================================
void runFCFS(Process proc[], int n, GanttBlock gantt[], int *gantt_count, double *avg_tat, double *avg_wt) {
    *gantt_count = 0;

    // Create index array to sort by arrival time
    int order[MAX_PROCESSES];
    for (int i = 0; i < n; i++) order[i] = i;

    // Selection sort by arrival time (tie-breaker: original index)
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (proc[order[j]].arrival_time < proc[order[i]].arrival_time ||
               (proc[order[j]].arrival_time == proc[order[i]].arrival_time && order[j] < order[i])) {
                int temp = order[i];
                order[i] = order[j];
                order[j] = temp;
            }
        }
    }

    int current_time = 0;
    int total_tat = 0;
    int total_wt = 0;

    for (int i = 0; i < n; i++) {
        int idx = order[i];

        // Handle CPU Idle time if process hasn't arrived yet
        if (current_time < proc[idx].arrival_time) {
            addGanttBlock(gantt, gantt_count, "IDLE", current_time, proc[idx].arrival_time);
            current_time = proc[idx].arrival_time;
        }

        int start = current_time;
        int end = current_time + proc[idx].burst_time;
        addGanttBlock(gantt, gantt_count, proc[idx].id, start, end);
        current_time = end;

        proc[idx].completion_time = current_time;
        proc[idx].turnaround_time = proc[idx].completion_time - proc[idx].arrival_time;
        proc[idx].waiting_time = proc[idx].turnaround_time - proc[idx].burst_time;

        total_tat += proc[idx].turnaround_time;
        total_wt += proc[idx].waiting_time;
    }

    *avg_tat = (double)total_tat / n;
    *avg_wt = (double)total_wt / n;
}

// ============================================================================
// ALGORITHM 2: SHORTEST JOB FIRST (NON-PREEMPTIVE SJF)
// ============================================================================
void runSJF(Process proc[], int n, GanttBlock gantt[], int *gantt_count, double *avg_tat, double *avg_wt) {
    *gantt_count = 0;
    for (int i = 0; i < n; i++) {
        proc[i].completed = 0;
    }

    int completed = 0;
    int current_time = 0;
    int total_tat = 0;
    int total_wt = 0;

    while (completed < n) {
        int min_idx = -1;
        int min_burst = 1e9;

        // Search among arrived, incomplete processes for shortest burst time
        for (int i = 0; i < n; i++) {
            if (!proc[i].completed && proc[i].arrival_time <= current_time) {
                if (proc[i].burst_time < min_burst) {
                    min_burst = proc[i].burst_time;
                    min_idx = i;
                } else if (proc[i].burst_time == min_burst) {
                    // Tie-breaker: earlier arrival time
                    if (min_idx == -1 || proc[i].arrival_time < proc[min_idx].arrival_time) {
                        min_idx = i;
                    }
                }
            }
        }

        if (min_idx == -1) {
            // No process ready: advance current_time to earliest arrival time
            int next_arrival = 1e9;
            for (int i = 0; i < n; i++) {
                if (!proc[i].completed && proc[i].arrival_time > current_time) {
                    if (proc[i].arrival_time < next_arrival) {
                        next_arrival = proc[i].arrival_time;
                    }
                }
            }
            addGanttBlock(gantt, gantt_count, "IDLE", current_time, next_arrival);
            current_time = next_arrival;
        } else {
            // Execute selected process to completion (non-preemptive)
            int start = current_time;
            int end = current_time + proc[min_idx].burst_time;
            addGanttBlock(gantt, gantt_count, proc[min_idx].id, start, end);
            current_time = end;

            proc[min_idx].completion_time = current_time;
            proc[min_idx].turnaround_time = proc[min_idx].completion_time - proc[min_idx].arrival_time;
            proc[min_idx].waiting_time = proc[min_idx].turnaround_time - proc[min_idx].burst_time;
            proc[min_idx].completed = 1;
            completed++;

            total_tat += proc[min_idx].turnaround_time;
            total_wt += proc[min_idx].waiting_time;
        }
    }

    *avg_tat = (double)total_tat / n;
    *avg_wt = (double)total_wt / n;
}

// ============================================================================
// ALGORITHM 3: ROUND ROBIN (PREEMPTIVE WITH CIRCULAR QUEUE)
// ============================================================================
void runRoundRobin(Process proc[], int n, int quantum, GanttBlock gantt[], int *gantt_count, double *avg_tat, double *avg_wt) {
    *gantt_count = 0;
    for (int i = 0; i < n; i++) {
        proc[i].remaining_time = proc[i].burst_time;
        proc[i].completed = 0;
    }

    // Sort index order primarily by arrival time
    int order[MAX_PROCESSES];
    for (int i = 0; i < n; i++) order[i] = i;
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (proc[order[j]].arrival_time < proc[order[i]].arrival_time ||
               (proc[order[j]].arrival_time == proc[order[i]].arrival_time && order[j] < order[i])) {
                int temp = order[i];
                order[i] = order[j];
                order[j] = temp;
            }
        }
    }

    Queue q;
    initQueue(&q);

    int current_time = 0;
    int completed = 0;
    int arrived[MAX_PROCESSES] = {0};

    // Check if any process has arrived at time 0
    int any_at_zero = 0;
    for (int i = 0; i < n; i++) {
        int idx = order[i];
        if (proc[idx].arrival_time <= current_time) {
            enqueue(&q, idx);
            arrived[idx] = 1;
            any_at_zero = 1;
        }
    }

    // If no process arrived at time 0, handle idle time to first arrival
    if (!any_at_zero) {
        int first_arrival = proc[order[0]].arrival_time;
        addGanttBlock(gantt, gantt_count, "IDLE", current_time, first_arrival);
        current_time = first_arrival;
        for (int i = 0; i < n; i++) {
            int idx = order[i];
            if (proc[idx].arrival_time <= current_time && !arrived[idx]) {
                enqueue(&q, idx);
                arrived[idx] = 1;
            }
        }
    }

    // Process queue execution
    while (completed < n) {
        if (isEmpty(&q)) {
            // Jump idle CPU time to next arrival
            int next_arr = 1e9;
            for (int i = 0; i < n; i++) {
                if (!proc[i].completed && proc[i].arrival_time > current_time) {
                    if (proc[i].arrival_time < next_arr) {
                        next_arr = proc[i].arrival_time;
                    }
                }
            }
            if (next_arr < 1e9) {
                addGanttBlock(gantt, gantt_count, "IDLE", current_time, next_arr);
                current_time = next_arr;
                for (int i = 0; i < n; i++) {
                    int idx = order[i];
                    if (proc[idx].arrival_time <= current_time && !arrived[idx]) {
                        enqueue(&q, idx);
                        arrived[idx] = 1;
                    }
                }
            }
            continue;
        }

        int curr = dequeue(&q);
        int exec_time = (proc[curr].remaining_time > quantum) ? quantum : proc[curr].remaining_time;
        int start = current_time;
        int end = current_time + exec_time;

        addGanttBlock(gantt, gantt_count, proc[curr].id, start, end);
        current_time = end;
        proc[curr].remaining_time -= exec_time;

        // Enqueue any newly arrived processes during this execution window
        for (int i = 0; i < n; i++) {
            int idx = order[i];
            if (!arrived[idx] && proc[idx].arrival_time <= current_time) {
                enqueue(&q, idx);
                arrived[idx] = 1;
            }
        }

        // If process is not finished, re-enqueue it
        if (proc[curr].remaining_time > 0) {
            enqueue(&q, curr);
        } else {
            proc[curr].completion_time = current_time;
            proc[curr].turnaround_time = proc[curr].completion_time - proc[curr].arrival_time;
            proc[curr].waiting_time = proc[curr].turnaround_time - proc[curr].burst_time;
            proc[curr].completed = 1;
            completed++;
        }
    }

    int total_tat = 0;
    int total_wt = 0;
    for (int i = 0; i < n; i++) {
        total_tat += proc[i].turnaround_time;
        total_wt += proc[i].waiting_time;
    }
    *avg_tat = (double)total_tat / n;
    *avg_wt = (double)total_wt / n;
}

// ============================================================================
// COMPARISON MODULE: RUN ALL THREE ALGORITHMS SIDE-BY-SIDE
// ============================================================================
void runComparison(Process original[], int n, int quantum) {
    Process p_fcfs[MAX_PROCESSES], p_sjf[MAX_PROCESSES], p_rr[MAX_PROCESSES];
    GanttBlock g_fcfs[MAX_GANTT], g_sjf[MAX_GANTT], g_rr[MAX_GANTT];
    int gc_fcfs = 0, gc_sjf = 0, gc_rr = 0;
    double atat_fcfs, awt_fcfs, atat_sjf, awt_sjf, atat_rr, awt_rr;

    memcpy(p_fcfs, original, sizeof(Process) * n);
    memcpy(p_sjf, original, sizeof(Process) * n);
    memcpy(p_rr, original, sizeof(Process) * n);

    runFCFS(p_fcfs, n, g_fcfs, &gc_fcfs, &atat_fcfs, &awt_fcfs);
    runSJF(p_sjf, n, g_sjf, &gc_sjf, &atat_sjf, &awt_sjf);
    runRoundRobin(p_rr, n, quantum, g_rr, &gc_rr, &atat_rr, &awt_rr);

    printf("\n=======================================================================\n");
    printf("              ALGORITHM PERFORMANCE COMPARISON TABLE\n");
    printf("=======================================================================\n");
    printf("%-24s %-22s %-22s\n", "Algorithm", "Avg Waiting Time", "Avg Turnaround Time");
    printf("-----------------------------------------------------------------------\n");
    printf("%-24s %-22.2f %-22.2f\n", "1. FCFS", awt_fcfs, atat_fcfs);
    printf("%-24s %-22.2f %-22.2f\n", "2. SJF (Non-preemptive)", awt_sjf, atat_sjf);
    printf("%-24s %-22.2f %-22.2f (Quantum = %d)\n", "3. Round Robin", awt_rr, atat_rr, quantum);
    printf("=======================================================================\n");

    // Identify algorithm with lowest average waiting time
    if (awt_sjf <= awt_fcfs && awt_sjf <= awt_rr) {
        printf("RESULT: Shortest Job First (SJF) is most optimal for this workload (Lowest Avg WT: %.2f ms).\n", awt_sjf);
    } else if (awt_fcfs <= awt_sjf && awt_fcfs <= awt_rr) {
        printf("RESULT: First Come First Serve (FCFS) is most optimal for this workload (Lowest Avg WT: %.2f ms).\n", awt_fcfs);
    } else {
        printf("RESULT: Round Robin is most optimal for this workload (Lowest Avg WT: %.2f ms).\n", awt_rr);
    }
    printf("=======================================================================\n\n");
}

// ============================================================================
// PROCESS MANAGEMENT: DISPLAY, ADD, PRESETS
// ============================================================================

void displayCurrentProcesses(Process proc[], int n) {
    if (n == 0) {
        printf("\nNo processes currently loaded.\n\n");
        return;
    }

    printf("\n--- CURRENT LOADED PROCESSES (%d) ---\n", n);
    printf("%-8s %-16s %-15s %-12s\n", "Index", "Process ID", "Arrival Time", "Burst Time");
    printf("----------------------------------------\n");
    for (int i = 0; i < n; i++) {
        printf("%-8d %-16s %-12d %-12d\n", i + 1, proc[i].id, proc[i].arrival_time, proc[i].burst_time);
    }
    printf("----------------------------------------\n\n");
}

int checkDuplicatePID(Process proc[], int n, const char *id) {
    for (int i = 0; i < n; i++) {
        if (strcmp(proc[i].id, id) == 0) return 1;
    }
    return 0;
}

void loadSamplePreset(Process proc[], int *n) {
    *n = 4;
    strcpy(proc[0].id, "P1"); proc[0].arrival_time = 0; proc[0].burst_time = 5;
    strcpy(proc[1].id, "P2"); proc[1].arrival_time = 1; proc[1].burst_time = 3;
    strcpy(proc[2].id, "P3"); proc[2].arrival_time = 2; proc[2].burst_time = 8;
    strcpy(proc[3].id, "P4"); proc[3].arrival_time = 3; proc[3].burst_time = 6;
    printf("\nSuccessfully loaded Standard Preset (4 processes: P1, P2, P3, P4).\n\n");
}

// ============================================================================
// MAIN MENU-DRIVEN PROGRAM
// ============================================================================

int main() {
    Process proc[MAX_PROCESSES];
    int n = 0;
    int quantum = 2;
    int choice = 0;

    // Start with default sample processes preloaded
    loadSamplePreset(proc, &n);

    while (1) {
        printf("========================================================\n");
        printf("        CPU PROCESS SCHEDULING SIMULATOR (CLI)\n");
        printf("========================================================\n");
        printf(" Loaded Processes: %d | Time Quantum: %d\n", n, quantum);
        printf("--------------------------------------------------------\n");
        printf(" 1. Add Processes Manually\n");
        printf(" 2. Load Sample Processes Preset\n");
        printf(" 3. View Current Processes Table\n");
        printf(" 4. Set Round Robin Time Quantum\n");
        printf(" 5. Run First Come First Serve (FCFS)\n");
        printf(" 6. Run Shortest Job First (Non-preemptive SJF)\n");
        printf(" 7. Run Round Robin (RR)\n");
        printf(" 8. Compare All Three Algorithms\n");
        printf(" 9. Reset / Clear All Processes\n");
        printf(" 10. Exit\n");
        printf("========================================================\n");
        printf("Enter your choice (1-10): ");

        if (scanf("%d", &choice) != 1) {
            printf("\nInvalid input! Please enter a number between 1 and 10.\n\n");
            clearInputBuffer();
            continue;
        }

        switch (choice) {
            case 1: {
                // Add processes manually
                int countToAdd = 0;
                printf("\nHow many processes do you want to add? ");
                if (scanf("%d", &countToAdd) != 1 || countToAdd <= 0) {
                    printf("Invalid number of processes.\n\n");
                    clearInputBuffer();
                    break;
                }

                if (n + countToAdd > MAX_PROCESSES) {
                    printf("Cannot exceed maximum limit of %d processes.\n\n", MAX_PROCESSES);
                    break;
                }

                for (int i = 0; i < countToAdd; i++) {
                    char pid[16];
                    int at, bt;
                    printf("\n--- Process %d of %d ---\n", i + 1, countToAdd);

                    while (1) {
                        printf("Enter Process ID (e.g. P%d): ", n + 1);
                        scanf("%15s", pid);
                        if (checkDuplicatePID(proc, n, pid)) {
                            printf("Error: Process ID '%s' already exists! Choose a unique ID.\n", pid);
                        } else {
                            break;
                        }
                    }

                    while (1) {
                        printf("Enter Arrival Time (>= 0): ");
                        if (scanf("%d", &at) == 1 && at >= 0) {
                            break;
                        }
                        printf("Invalid Arrival Time. Must be an integer >= 0.\n");
                        clearInputBuffer();
                    }

                    while (1) {
                        printf("Enter Burst Time (> 0): ");
                        if (scanf("%d", &bt) == 1 && bt > 0) {
                            break;
                        }
                        printf("Invalid Burst Time. Must be an integer > 0.\n");
                        clearInputBuffer();
                    }

                    strcpy(proc[n].id, pid);
                    proc[n].arrival_time = at;
                    proc[n].burst_time = bt;
                    proc[n].remaining_time = bt;
                    proc[n].completed = 0;
                    n++;
                }
                printf("\nSuccessfully added %d process(es)!\n\n", countToAdd);
                break;
            }

            case 2:
                loadSamplePreset(proc, &n);
                break;

            case 3:
                displayCurrentProcesses(proc, n);
                break;

            case 4: {
                int newQ;
                printf("\nEnter new Time Quantum for Round Robin (> 0): ");
                if (scanf("%d", &newQ) == 1 && newQ > 0) {
                    quantum = newQ;
                    printf("Time Quantum updated to %d.\n\n", quantum);
                } else {
                    printf("Invalid Time Quantum. Must be an integer > 0.\n\n");
                    clearInputBuffer();
                }
                break;
            }

            case 5: {
                if (n == 0) {
                    printf("\nError: Please add at least one process first!\n\n");
                    break;
                }
                Process temp[MAX_PROCESSES];
                GanttBlock gantt[MAX_GANTT];
                int gc = 0;
                double avg_tat = 0, avg_wt = 0;

                memcpy(temp, proc, sizeof(Process) * n);
                runFCFS(temp, n, gantt, &gc, &avg_tat, &avg_wt);
                printGanttChart(gantt, gc);
                printResultsTable("First Come First Serve (FCFS)", temp, n, gantt, gc, avg_tat, avg_wt);
                break;
            }

            case 6: {
                if (n == 0) {
                    printf("\nError: Please add at least one process first!\n\n");
                    break;
                }
                Process temp[MAX_PROCESSES];
                GanttBlock gantt[MAX_GANTT];
                int gc = 0;
                double avg_tat = 0, avg_wt = 0;

                memcpy(temp, proc, sizeof(Process) * n);
                runSJF(temp, n, gantt, &gc, &avg_tat, &avg_wt);
                printGanttChart(gantt, gc);
                printResultsTable("Shortest Job First (Non-preemptive SJF)", temp, n, gantt, gc, avg_tat, avg_wt);
                break;
            }

            case 7: {
                if (n == 0) {
                    printf("\nError: Please add at least one process first!\n\n");
                    break;
                }
                Process temp[MAX_PROCESSES];
                GanttBlock gantt[MAX_GANTT];
                int gc = 0;
                double avg_tat = 0, avg_wt = 0;

                memcpy(temp, proc, sizeof(Process) * n);
                runRoundRobin(temp, n, quantum, gantt, &gc, &avg_tat, &avg_wt);
                printGanttChart(gantt, gc);
                char title[64];
                sprintf(title, "Round Robin (RR) [Quantum = %d]", quantum);
                printResultsTable(title, temp, n, gantt, gc, avg_tat, avg_wt);
                break;
            }

            case 8: {
                if (n == 0) {
                    printf("\nError: Please add at least one process first!\n\n");
                    break;
                }
                runComparison(proc, n, quantum);
                break;
            }

            case 9:
                n = 0;
                printf("\nAll processes cleared. Table is now empty.\n\n");
                break;

            case 10:
                printf("\nExiting CPU Process Scheduling Simulator. Goodbye!\n");
                return 0;

            default:
                printf("\nInvalid option! Please select a valid number (1-10).\n\n");
                break;
        }
    }

    return 0;
}

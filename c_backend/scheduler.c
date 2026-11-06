#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 100
#define MAX_GANTT 1000

// --- Data Structures ---

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

typedef struct {
    char id[16];
    int start_time;
    int end_time;
} GanttBlock;

// Circular Queue for Round Robin
typedef struct {
    int items[MAX_PROCESSES * 20];
    int front;
    int rear;
    int count;
} Queue;

void initQueue(Queue *q) {
    q->front = 0;
    q->rear = -1;
    q->count = 0;
}

int isEmpty(Queue *q) {
    return q->count == 0;
}

void enqueue(Queue *q, int value) {
    if (q->count < MAX_PROCESSES * 20) {
        q->rear = (q->rear + 1) % (MAX_PROCESSES * 20);
        q->items[q->rear] = value;
        q->count++;
    }
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

// Check if item already exists in queue
int inQueue(Queue *q, int value) {
    if (isEmpty(q)) return 0;
    int idx = q->front;
    for (int i = 0; i < q->count; i++) {
        if (q->items[idx] == value) return 1;
        idx = (idx + 1) % (MAX_PROCESSES * 20);
    }
    return 0;
}

// Add Gantt block, merging consecutive blocks of same process
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

// --- Algorithm 1: First Come First Serve (FCFS) ---
void runFCFS(Process proc[], int n, GanttBlock gantt[], int *gantt_count, double *avg_tat, double *avg_wt) {
    *gantt_count = 0;
    // Create index array to sort by arrival time
    int order[MAX_PROCESSES];
    for (int i = 0; i < n; i++) order[i] = i;

    // Selection sort by arrival time (stable with original index tie-breaker)
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

// --- Algorithm 2: Shortest Job First (Non-preemptive) ---
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

        // Find available process with shortest burst time
        for (int i = 0; i < n; i++) {
            if (!proc[i].completed && proc[i].arrival_time <= current_time) {
                if (proc[i].burst_time < min_burst) {
                    min_burst = proc[i].burst_time;
                    min_idx = i;
                } else if (proc[i].burst_time == min_burst) {
                    if (min_idx == -1 || proc[i].arrival_time < proc[min_idx].arrival_time) {
                        min_idx = i;
                    }
                }
            }
        }

        if (min_idx == -1) {
            // CPU is idle, advance time to nearest arriving process
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
            // Execute selected process to completion
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

// --- Algorithm 3: Round Robin ---
void runRoundRobin(Process proc[], int n, int quantum, GanttBlock gantt[], int *gantt_count, double *avg_tat, double *avg_wt) {
    *gantt_count = 0;
    for (int i = 0; i < n; i++) {
        proc[i].remaining_time = proc[i].burst_time;
        proc[i].completed = 0;
    }

    // Sort order indices by arrival time initially
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

    // Check if any process arrives at time 0
    int any_at_zero = 0;
    for (int i = 0; i < n; i++) {
        int idx = order[i];
        if (proc[idx].arrival_time <= current_time) {
            enqueue(&q, idx);
            arrived[idx] = 1;
            any_at_zero = 1;
        }
    }

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

    while (completed < n) {
        if (isEmpty(&q)) {
            // Find next arriving process
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

        // Check for newly arrived processes during this time slice
        for (int i = 0; i < n; i++) {
            int idx = order[i];
            if (!arrived[idx] && proc[idx].arrival_time <= current_time) {
                enqueue(&q, idx);
                arrived[idx] = 1;
            }
        }

        // If current process still has remaining burst time, push back to queue
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

// Print single algorithm result as JSON
void printAlgorithmJSON(const char *algo_name, Process proc[], int n, GanttBlock gantt[], int gantt_count, double avg_tat, double avg_wt) {
    printf("{\n");
    printf("  \"algorithm\": \"%s\",\n", algo_name);
    printf("  \"avg_turnaround_time\": %.2f,\n", avg_tat);
    printf("  \"avg_waiting_time\": %.2f,\n", avg_wt);

    // Processes list
    printf("  \"processes\": [\n");
    for (int i = 0; i < n; i++) {
        printf("    {\n");
        printf("      \"id\": \"%s\",\n", proc[i].id);
        printf("      \"arrival_time\": %d,\n", proc[i].arrival_time);
        printf("      \"burst_time\": %d,\n", proc[i].burst_time);
        printf("      \"completion_time\": %d,\n", proc[i].completion_time);
        printf("      \"turnaround_time\": %d,\n", proc[i].turnaround_time);
        printf("      \"waiting_time\": %d\n", proc[i].waiting_time);
        printf("    }%s\n", (i == n - 1) ? "" : ",");
    }
    printf("  ],\n");

    // Gantt blocks
    printf("  \"gantt_chart\": [\n");
    for (int i = 0; i < gantt_count; i++) {
        printf("    {\n");
        printf("      \"id\": \"%s\",\n", gantt[i].id);
        printf("      \"start_time\": %d,\n", gantt[i].start_time);
        printf("      \"end_time\": %d\n", gantt[i].end_time);
        printf("    }%s\n", (i == gantt_count - 1) ? "" : ",");
    }
    printf("  ]\n");
    printf("}\n");
}

int main() {
    char algo[16];
    int quantum = 2;
    int n = 0;

    // Read: <ALGORITHM> <QUANTUM> <N>
    if (scanf("%15s %d %d", algo, &quantum, &n) != 3 || n <= 0 || n > MAX_PROCESSES) {
        printf("{\"error\": \"Invalid input format or process count\"}\n");
        return 1;
    }

    Process original[MAX_PROCESSES];
    for (int i = 0; i < n; i++) {
        if (scanf("%15s %d %d", original[i].id, &original[i].arrival_time, &original[i].burst_time) != 3) {
            printf("{\"error\": \"Failed reading process row %d\"}\n", i + 1);
            return 1;
        }
        original[i].remaining_time = original[i].burst_time;
        original[i].completion_time = 0;
        original[i].turnaround_time = 0;
        original[i].waiting_time = 0;
        original[i].completed = 0;
    }

    GanttBlock gantt[MAX_GANTT];
    int gantt_count = 0;
    double avg_tat = 0.0, avg_wt = 0.0;

    if (strcmp(algo, "FCFS") == 0) {
        Process proc[MAX_PROCESSES];
        memcpy(proc, original, sizeof(Process) * n);
        runFCFS(proc, n, gantt, &gantt_count, &avg_tat, &avg_wt);
        printAlgorithmJSON("FCFS", proc, n, gantt, gantt_count, avg_tat, avg_wt);
    } else if (strcmp(algo, "SJF") == 0) {
        Process proc[MAX_PROCESSES];
        memcpy(proc, original, sizeof(Process) * n);
        runSJF(proc, n, gantt, &gantt_count, &avg_tat, &avg_wt);
        printAlgorithmJSON("SJF", proc, n, gantt, gantt_count, avg_tat, avg_wt);
    } else if (strcmp(algo, "RR") == 0) {
        Process proc[MAX_PROCESSES];
        memcpy(proc, original, sizeof(Process) * n);
        runRoundRobin(proc, n, quantum, gantt, &gantt_count, &avg_tat, &avg_wt);
        printAlgorithmJSON("Round Robin", proc, n, gantt, gantt_count, avg_tat, avg_wt);
    } else if (strcmp(algo, "ALL") == 0) {
        // Compare All: Output JSON containing fcfs, sjf, and rr
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

        printf("{\n");
        printf("  \"mode\": \"COMPARE_ALL\",\n");
        printf("  \"fcfs\": ");
        printAlgorithmJSON("FCFS", p_fcfs, n, g_fcfs, gc_fcfs, atat_fcfs, awt_fcfs);
        printf(",\n  \"sjf\": ");
        printAlgorithmJSON("SJF", p_sjf, n, g_sjf, gc_sjf, atat_sjf, awt_sjf);
        printf(",\n  \"rr\": ");
        printAlgorithmJSON("Round Robin", p_rr, n, g_rr, gc_rr, atat_rr, awt_rr);
        printf("\n}\n");
    } else {
        printf("{\"error\": \"Unknown algorithm requested\"}\n");
        return 1;
    }

    return 0;
}

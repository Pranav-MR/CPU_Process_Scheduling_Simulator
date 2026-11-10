#include <stdio.h>
#include <string.h>

#define MAX 30

// Structure to store process information
struct Process {
    char id[10];
    int at;        // Arrival Time
    int bt;        // Burst Time
    int ct;        // Completion Time
    int tat;       // Turnaround Time (TAT = CT - AT)
    int wt;        // Waiting Time (WT = TAT - BT)
    int rem_bt;    // Remaining Burst Time for Round Robin
    int completed; // 1 if process is finished, 0 otherwise
};

// Structure for storing Gantt chart intervals
struct Gantt {
    char id[10];
    int start;
    int end;
};

// Simple queue for Round Robin scheduling
int queue[500];
int front = 0, rear = -1;

void push(int val) {
    queue[++rear] = val;
}

int pop() {
    return queue[front++];
}

int isQueueEmpty() {
    return front > rear;
}

// Function to print the Gantt chart in terminal
void printGanttChart(struct Gantt g[], int g_count) {
    if (g_count == 0) return;

    printf("\n--- GANTT CHART ---\n ");
    for (int i = 0; i < g_count; i++) {
        printf("------- ");
    }
    printf("\n|");
    for (int i = 0; i < g_count; i++) {
        printf("  %-3s  |", g[i].id);
    }
    printf("\n ");
    for (int i = 0; i < g_count; i++) {
        printf("------- ");
    }
    printf("\n");

    // Print timeline numbers
    printf("%-8d", g[0].start);
    for (int i = 0; i < g_count; i++) {
        printf("%-8d", g[i].end);
    }
    printf("\n\n");
}

// Function to print process table and average times
void printResults(struct Process p[], int n, struct Gantt g[], int g_count) {
    float total_tat = 0, total_wt = 0;

    printGanttChart(g, g_count);

    printf("PID\tAT\tBT\tCT\tTAT\tWT\n");
    printf("--------------------------------------------------\n");
    for (int i = 0; i < n; i++) {
        printf("%s\t%d\t%d\t%d\t%d\t%d\n", p[i].id, p[i].at, p[i].bt, p[i].ct, p[i].tat, p[i].wt);
        total_tat += p[i].tat;
        total_wt += p[i].wt;
    }
    printf("--------------------------------------------------\n");
    printf("Average Turnaround Time : %.2f\n", total_tat / n);
    printf("Average Waiting Time    : %.2f\n\n", total_wt / n);
}

// First Come First Serve (FCFS)
void fcfs(struct Process p[], int n) {
    struct Process temp[MAX];
    struct Gantt g[100];
    int g_count = 0;

    for (int i = 0; i < n; i++) temp[i] = p[i];

    // Sort by arrival time
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (temp[j].at > temp[j + 1].at) {
                struct Process t = temp[j];
                temp[j] = temp[j + 1];
                temp[j + 1] = t;
            }
        }
    }

    int current_time = 0;
    for (int i = 0; i < n; i++) {
        // Handle CPU idle time
        if (current_time < temp[i].at) {
            strcpy(g[g_count].id, "IDLE");
            g[g_count].start = current_time;
            g[g_count].end = temp[i].at;
            g_count++;
            current_time = temp[i].at;
        }

        strcpy(g[g_count].id, temp[i].id);
        g[g_count].start = current_time;
        current_time += temp[i].bt;
        g[g_count].end = current_time;
        g_count++;

        // Calculate metrics
        temp[i].ct = current_time;
        temp[i].tat = temp[i].ct - temp[i].at;
        temp[i].wt = temp[i].tat - temp[i].bt;
    }

    printResults(temp, n, g, g_count);
}

// Shortest Job First (Non-preemptive SJF)
void sjf(struct Process p[], int n) {
    struct Process temp[MAX];
    struct Gantt g[100];
    int g_count = 0;

    for (int i = 0; i < n; i++) {
        temp[i] = p[i];
        temp[i].completed = 0;
    }

    int current_time = 0, completed = 0;

    while (completed < n) {
        int idx = -1;
        int min_bt = 999999;

        // Find process with shortest burst time that has already arrived
        for (int i = 0; i < n; i++) {
            if (!temp[i].completed && temp[i].at <= current_time) {
                if (temp[i].bt < min_bt) {
                    min_bt = temp[i].bt;
                    idx = i;
                }
            }
        }

        // If no process has arrived, CPU is idle
        if (idx == -1) {
            int next_at = 999999;
            for (int i = 0; i < n; i++) {
                if (!temp[i].completed && temp[i].at < next_at) {
                    next_at = temp[i].at;
                }
            }
            strcpy(g[g_count].id, "IDLE");
            g[g_count].start = current_time;
            g[g_count].end = next_at;
            g_count++;
            current_time = next_at;
        } else {
            strcpy(g[g_count].id, temp[idx].id);
            g[g_count].start = current_time;
            current_time += temp[idx].bt;
            g[g_count].end = current_time;
            g_count++;

            temp[idx].ct = current_time;
            temp[idx].tat = temp[idx].ct - temp[idx].at;
            temp[idx].wt = temp[idx].tat - temp[idx].bt;
            temp[idx].completed = 1;
            completed++;
        }
    }

    printResults(temp, n, g, g_count);
}

// Round Robin (RR)
void roundRobin(struct Process p[], int n, int quantum) {
    struct Process temp[MAX];
    struct Gantt g[200];
    int g_count = 0;
    front = 0; 
    rear = -1;

    for (int i = 0; i < n; i++) {
        temp[i] = p[i];
        temp[i].rem_bt = temp[i].bt;
        temp[i].completed = 0;
    }

    // Sort by arrival time initially
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (temp[j].at > temp[j + 1].at) {
                struct Process t = temp[j];
                temp[j] = temp[j + 1];
                temp[j + 1] = t;
            }
        }
    }

    int current_time = 0, completed = 0;
    int visited[MAX] = {0};

    // Add initial processes arriving at current_time
    for (int i = 0; i < n; i++) {
        if (temp[i].at <= current_time) {
            push(i);
            visited[i] = 1;
        }
    }

    // If no process arrived at time 0, handle idle time
    if (isQueueEmpty()) {
        strcpy(g[g_count].id, "IDLE");
        g[g_count].start = current_time;
        current_time = temp[0].at;
        g[g_count].end = current_time;
        g_count++;

        for (int i = 0; i < n; i++) {
            if (temp[i].at <= current_time && !visited[i]) {
                push(i);
                visited[i] = 1;
            }
        }
    }

    while (completed < n) {
        if (isQueueEmpty()) {
            // Jump idle CPU time to next arrival
            int next_at = 999999;
            for (int i = 0; i < n; i++) {
                if (!temp[i].completed && temp[i].at < next_at) {
                    next_at = temp[i].at;
                }
            }
            strcpy(g[g_count].id, "IDLE");
            g[g_count].start = current_time;
            current_time = next_at;
            g[g_count].end = current_time;
            g_count++;

            for (int i = 0; i < n; i++) {
                if (temp[i].at <= current_time && !visited[i]) {
                    push(i);
                    visited[i] = 1;
                }
            }
            continue;
        }

        int curr = pop();
        int time_slice = (temp[curr].rem_bt > quantum) ? quantum : temp[curr].rem_bt;

        // Add to Gantt chart (merge if same process continues)
        if (g_count > 0 && strcmp(g[g_count - 1].id, temp[curr].id) == 0 && g[g_count - 1].end == current_time) {
            g[g_count - 1].end += time_slice;
        } else {
            strcpy(g[g_count].id, temp[curr].id);
            g[g_count].start = current_time;
            g[g_count].end = current_time + time_slice;
            g_count++;
        }

        current_time += time_slice;
        temp[curr].rem_bt -= time_slice;

        // Check if any new process arrived while this was running
        for (int i = 0; i < n; i++) {
            if (!visited[i] && temp[i].at <= current_time) {
                push(i);
                visited[i] = 1;
            }
        }

        // If process still has remaining time, push back to queue
        if (temp[curr].rem_bt > 0) {
            push(curr);
        } else {
            temp[curr].ct = current_time;
            temp[curr].tat = temp[curr].ct - temp[curr].at;
            temp[curr].wt = temp[curr].tat - temp[curr].bt;
            temp[curr].completed = 1;
            completed++;
        }
    }

    printResults(temp, n, g, g_count);
}

int main() {
    struct Process p[MAX];
    int n = 0;
    int choice;
    int quantum = 2;

    // Default sample processes so user can test immediately
    n = 4;
    strcpy(p[0].id, "P1"); p[0].at = 0; p[0].bt = 5;
    strcpy(p[1].id, "P2"); p[1].at = 1; p[1].bt = 3;
    strcpy(p[2].id, "P3"); p[2].at = 2; p[2].bt = 8;
    strcpy(p[3].id, "P4"); p[3].at = 3; p[3].bt = 6;

    while (1) {
        printf("======================================\n");
        printf("   CPU PROCESS SCHEDULING SIMULATOR   \n");
        printf("======================================\n");
        printf("1. Enter New Processes\n");
        printf("2. View Current Processes\n");
        printf("3. First Come First Serve (FCFS)\n");
        printf("4. Shortest Job First (SJF)\n");
        printf("5. Round Robin (RR)\n");
        printf("6. Exit\n");
        printf("--------------------------------------\n");
        printf("Enter your choice (1-6): ");
        if (scanf("%d", &choice) != 1) {
            printf("\nInvalid input! Exiting...\n");
            break;
        }

        switch (choice) {
            case 1:
                printf("\nEnter number of processes: ");
                scanf("%d", &n);
                for (int i = 0; i < n; i++) {
                    printf("\nProcess %d:\n", i + 1);
                    printf("  Process ID (e.g. P%d): ", i + 1);
                    scanf("%s", p[i].id);
                    printf("  Arrival Time: ");
                    scanf("%d", &p[i].at);
                    printf("  Burst Time: ");
                    scanf("%d", &p[i].bt);
                }
                printf("\n%d processes loaded successfully!\n\n", n);
                break;

            case 2:
                if (n == 0) {
                    printf("\nNo processes available. Please enter processes first.\n\n");
                } else {
                    printf("\n--- CURRENT PROCESSES ---\n");
                    printf("PID\tArrival Time\tBurst Time\n");
                    printf("----------------------------------\n");
                    for (int i = 0; i < n; i++) {
                        printf("%s\t%d\t\t%d\n", p[i].id, p[i].at, p[i].bt);
                    }
                    printf("----------------------------------\n\n");
                }
                break;

            case 3:
                if (n == 0) {
                    printf("\nPlease enter processes first!\n\n");
                } else {
                    printf("\n================ FCFS ================");
                    fcfs(p, n);
                }
                break;

            case 4:
                if (n == 0) {
                    printf("\nPlease enter processes first!\n\n");
                } else {
                    printf("\n================ SJF ================");
                    sjf(p, n);
                }
                break;

            case 5:
                if (n == 0) {
                    printf("\nPlease enter processes first!\n\n");
                } else {
                    printf("\nEnter Time Quantum for Round Robin: ");
                    scanf("%d", &quantum);
                    printf("\n========== Round Robin (q=%d) ==========", quantum);
                    roundRobin(p, n, quantum);
                }
                break;

            case 6:
                printf("\nExiting simulator. Thank you!\n");
                return 0;

            default:
                printf("\nInvalid choice! Please select 1 to 6.\n\n");
        }
    }

    return 0;
}

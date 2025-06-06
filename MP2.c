#include <stdio.h>          
#include <stdlib.h>
#include <string.h> 
#include <stdbool.h>
#include <math.h>

#define MAX_TASKS 100       // Define the maximum number of tasks that can be handled

// Task structure definition
typedef struct {
    char name[10]; 
    int phase, period, wcet, deadline; // Task's phase, period, worst-case execution time (wcet), and deadline
    int remaining_time, next_release, absolute_deadline; // Time-related variables for scheduling
} Task;

// Parse input file to read tasks and store them in the task array
int parse_input(const char *filename, Task tasks[], int *task_count) {
    FILE *file = fopen(filename, "r");  // Open the input file in read mode
    if (!file) {
        perror("Error opening input file");  // Print an error message if the file cannot be opened
        return -1;  // Return -1 to indicate failure
    }
    *task_count = 0;  // Initialize task count to 0
    // Loop to read tasks from the file
    while (fscanf(file, "%[^,], %d, %d, %d, %d\n", tasks[*task_count].name, &tasks[*task_count].phase,
                  &tasks[*task_count].period, &tasks[*task_count].wcet, &tasks[*task_count].deadline) == 5) {
        tasks[*task_count].remaining_time = 0; // Initialize remaining time of the task to 0
        tasks[*task_count].next_release = tasks[*task_count].phase; // Set next release time as the phase
        tasks[*task_count].absolute_deadline = tasks[*task_count].deadline; // Set absolute deadline
        (*task_count)++; // Increment the task count
        if (*task_count >= MAX_TASKS) break; // If max tasks reached, break the loop
    }
    fclose(file); // Close the file after reading
    return 0; // Return 0 to indicate successful parsing
}

// Function to calculate the Greatest Common Divisor (GCD)
int gcd(int a, int b) {
    return b == 0 ? a : gcd(b, a % b); // Apply Euclid's algorithm to calculate GCD
}

// Function to calculate the Least Common Multiple (LCM)
int lcm(int a, int b) {
    return (a * b) / gcd(a, b);  // Return LCM using the formula LCM = (a * b) / GCD(a, b)
}

// Function to calculate the total utilization of the task set
// Utilization = sum(WCET / period) for all tasks
double calculate_utilization(Task tasks[], int task_count) {
    double utilization = 0.0; // Initialize utilization to 0
    // Loop through all tasks to accumulate their utilization
    for (int i = 0; i < task_count; i++) {
        utilization += (double)tasks[i].wcet / tasks[i].period; // Add the utilization of each task
    }
    return utilization; // Return the total utilization
}

// Check schedulability for Rate Monotonic scheduling using utilization bound
bool check_schedulability_rm(double utilization, int task_count) {
    double bound = task_count * (pow(2.0, 1.0 / task_count) - 1); // Calculate the utilization bound for RM
    return utilization < bound; // Return whether the system is schedulable or not based on utilization
}

// Calculate the response time of a task using Response Time Analysis (RTA)
int calculate_response_time(Task tasks[], int task_count, int i) {
    int response_time = tasks[i].wcet;  // Start with WCET as initial response time
    int previous_response_time = -1;    // To track the previous response time

    // Loop until the response time converges (doesn't change)
    while (response_time != previous_response_time) {
        previous_response_time = response_time; // Update the previous response time
        int interference = 0; // Initialize interference to 0

        // Calculate interference from higher-priority tasks
        for (int j = 0; j < task_count; j++) {
            if ((tasks[j].period < tasks[i].period) || 
                (tasks[j].period == tasks[i].period && j < i)) { // Higher priority tasks (shorter periods)
                int num_executions = ceil((double)response_time / tasks[j].period); // Calculate how many times the task interferes
                int task_interference = num_executions * tasks[j].wcet; // Calculate interference due to task j
                interference += task_interference; // Add the interference to the total interference
            }
        }

        // Calculate the new response time including the interference
        response_time = tasks[i].wcet + interference;
        
        // If the response time exceeds the deadline, return -1 (task is not schedulable)
        if (response_time > tasks[i].deadline) {
            return -1;  // Task is not schedulable
        }
    }
    return response_time; // Task is schedulable, return the final response time
}

// RM Scheduling function
void schedule_rm(Task tasks[], int task_count, const char *output_file) {
    bool extra_cycle_needed = false;
    char name[50] = ""; 
    int hyperperiod = tasks[0].period;
    char hyper_deadline[1024] = "";

    // Calculate the hyperperiod (LCM of all task periods)
    for (int i = 1; i < task_count; i++) {
        hyperperiod = lcm(hyperperiod, tasks[i].period);
    }

    double utilization = calculate_utilization(tasks, task_count); // Calculate the total utilization
    double utilization_bound = task_count * (pow(2.0, 1.0 / task_count) - 1); // Calculate the utilization bound for RM

    FILE *file = fopen(output_file, "w"); // Open the output file for writing

    bool schedulable = utilization <= utilization_bound; // Check if the system is schedulable based on utilization

    // If not schedulable, run RTA to check task schedulability
    if (!schedulable) {
        for (int i = 0; i < task_count; i++) {
            int response_time = calculate_response_time(tasks, task_count, i); // Calculate RTA for each task
            if (response_time == -1) {
                schedulable = false;
                break; // If any task is not schedulable, the system is not schedulable
            } else {
                schedulable = true; // If no task fails RTA, the system is schedulable
            }
        }

        // Print the result based on schedulability
        if (!schedulable) {
            fprintf(file, "RM\nNOT SCHEDULABLE\nU = %.3f\nUB = %.3f\nHYPERPERIOD = %d\n\n", utilization, utilization_bound, hyperperiod);
        } else {
            fprintf(file, "RM\nSCHEDULABLE\nU = %.3f\nUB = %.3f\nHYPERPERIOD = %d\n\n", utilization, utilization_bound, hyperperiod);
        }
    }

    // Initialize tasks for scheduling
    for (int i = 0; i < task_count; i++) {
        tasks[i].next_release = tasks[i].phase;  // Set the next release time as the phase
        tasks[i].remaining_time = 0;  // Set remaining time to 0
    }

    // Scheduling loop for the hyperperiod
    for (int t = 0; t < hyperperiod; t++) {
        int selected_task = -1;  // No task selected initially
        char deadline_misses[1024] = "";  // To store the tasks with deadline misses

        // Release tasks that are ready to execute at time t
        for (int i = 0; i < task_count; i++) {
            if (t == tasks[i].next_release) {
                tasks[i].remaining_time = tasks[i].wcet;  // Set the remaining time to WCET
                tasks[i].absolute_deadline = t + tasks[i].deadline; // Set the absolute deadline for the task
            }
        }

        // Select the highest-priority (shortest period) task
        for (int i = 0; i < task_count; i++) {
            if (tasks[i].remaining_time > 0) {
                if (selected_task == -1 || tasks[i].period < tasks[selected_task].period) {
                    selected_task = i;
                }
            }
        }

        // Check for deadline misses
        for (int i = 0; i < task_count; i++) {
            if (tasks[i].remaining_time > 0 && t >= tasks[i].absolute_deadline) {
                if (strlen(deadline_misses) > 0) strcat(deadline_misses, " ");
                strcat(deadline_misses, tasks[i].name);  // Add the task name to deadline misses
                tasks[i].remaining_time = 0;  // Task has missed its deadline
            }
            
        }

        // Print task execution along with deadline misses if they occur
        if (strlen(deadline_misses) > 0) {
            if (selected_task != -1) {
                char *token = strtok(deadline_misses, " ");
                fprintf(file, "%d %s ", t, tasks[selected_task].name);
                while (token != NULL) {
                    fprintf(file, " DEADLINE_MISS(%s) ", token);
                    token = strtok(NULL, " "); // Get the next token
                }
            } 
            else {
                fprintf(file, "%d DEADLINE_MISS(%s)\n", t, deadline_misses);
            }
            fclose(file);
            return;
        }

        // Print task execution or CPU idle
        if (selected_task != -1) {
            
            fprintf(file, "%d %s\n", t, tasks[selected_task].name);  // Print the executing task
            if (t == hyperperiod-1){
                strcpy(name, tasks[selected_task].name);
            }
            tasks[selected_task].remaining_time--;  // Decrease remaining time of selected task
            if (tasks[selected_task].remaining_time == 0) {
                tasks[selected_task].next_release += tasks[selected_task].period;  // Update next release time
            }
        } else {
            fprintf(file, "%d \n", t);  // Print idle if no task is selected
        }
    }

    // Check if an extra cycle is needed after the hyperperiod
    for (int i = 0; i < task_count; i++) {
        if (tasks[i].absolute_deadline == hyperperiod && tasks[i].remaining_time > 0) {
            // strcpy(name, tasks[i].name);
            if (strlen(hyper_deadline) > 0) strcat(hyper_deadline, " ");
            strcat(hyper_deadline, tasks[i].name);
            extra_cycle_needed = true;
        }
    }

    // If extra cycle is needed, run one more cycle
    if (extra_cycle_needed) {
        int t = hyperperiod;
        // Check for deadline misses at the hyperperiod boundary
        if (strlen(hyper_deadline) > 0) {
            char *token = strtok(hyper_deadline, " ");
            fprintf(file, "%d %s ", t, name);
            while (token != NULL) {
                fprintf(file, "DEADLINE_MISS(%s) ", token);
                token = strtok(NULL, " "); // Get the next token
            }
        } else {
            fprintf(file, "\n");  // No missed deadlines
        }
    }
    fclose(file);  // Close the output file
}

// EDF Scheduling function
void schedule_edf(Task tasks[], int task_count, const char *output_file) {
    // Calculate the hyperperiod (LCM of all task periods)
    int hyperperiod = tasks[0].period;
    char hyper_deadline[1024] = "";
    char name[3] = "";
    for (int i = 1; i < task_count; i++) {
        hyperperiod = lcm(hyperperiod, tasks[i].period);
    }

    // Calculate total utilization
    double utilization = calculate_utilization(tasks, task_count);
    bool schedulable = (utilization <= 1.0);

    // Open the output file for writing
    FILE *file = fopen(output_file, "w");
    fprintf(file, "EDF\n%s\nU = %.3f\nUB = 1.0\nHYPERPERIOD = %d\n\n",
            schedulable ? "SCHEDULABLE" : "NOT SCHEDULABLE", utilization, hyperperiod);

    // Initialize tasks' next release, remaining time, and absolute deadlines
    for (int i = 0; i < task_count; i++) {
        tasks[i].next_release = tasks[i].phase;
        tasks[i].remaining_time = 0;
        tasks[i].absolute_deadline = tasks[i].phase + tasks[i].deadline;
    }

    // Scheduling loop for the hyperperiod
    for (int t = 0; t < hyperperiod; t++) {
        int selected_task = -1;  // No task selected initially
        char deadline_misses[1024] = "";  // To store the tasks with deadline misses

        // Release tasks that are ready to execute at time t
        for (int i = 0; i < task_count; i++) {
            if (t == tasks[i].next_release) {
                tasks[i].remaining_time = tasks[i].wcet;
                tasks[i].absolute_deadline = t + tasks[i].deadline;  // Update absolute deadline for each task
            }
        }

        // Select the task with the earliest absolute deadline
        for (int i = 0; i < task_count; i++) {
            if (tasks[i].remaining_time > 0) {
                if (selected_task == -1 || 
                    tasks[i].absolute_deadline < tasks[selected_task].absolute_deadline || 
                    (tasks[i].absolute_deadline == tasks[selected_task].absolute_deadline && tasks[i].next_release < tasks[selected_task].next_release)) {
                    
                    selected_task = i; // Select the task with the earliest absolute deadline
                }
            }
        }

        // Detect deadline misses
        for (int i = 0; i < task_count; i++) {
            if (tasks[i].remaining_time > 0 && t >= tasks[i].absolute_deadline) {
                if (strlen(deadline_misses) > 0) strcat(deadline_misses, " ");
                strcat(deadline_misses, tasks[i].name);  // Add the task name to deadline misses
                tasks[i].remaining_time = 0;  // Task has missed its deadline
            }
        }

        // If a deadline miss occurs, print it and stop execution
        if (strlen(deadline_misses) > 0) {
            if (selected_task != -1) {
                char *token = strtok(deadline_misses, " ");
                fprintf(file, "%d %s ", t, tasks[selected_task].name);
                while (token != NULL) {
                    fprintf(file, "DEADLINE_MISS(%s) ", token);
                    token = strtok(NULL, " "); // Get the next token
                }
            } 
            else {
                fprintf(file, "%d DEADLINE_MISS(%s)\n", t, deadline_misses);
            }
            fclose(file);
            return;
        }

        // Print the executing task or idle if no task is selected
        if (selected_task != -1) {
            fprintf(file, "%d %s\n", t, tasks[selected_task].name);
            if (t == hyperperiod-1){
                strcpy(name, tasks[selected_task].name);
            }
            tasks[selected_task].remaining_time--;  // Decrease remaining time of selected task

            // If the task finishes execution, update its next release time
            if (tasks[selected_task].remaining_time == 0) {
                tasks[selected_task].next_release += tasks[selected_task].period;
            }
        } else {
            fprintf(file, "%d \n", t);
        }
    }

    // Check if an extra cycle is needed after the hyperperiod
    bool extra_cycle_needed = false;
    for (int i = 0; i < task_count; i++) {
        if (tasks[i].absolute_deadline == hyperperiod && tasks[i].remaining_time > 0) {
            if (strlen(hyper_deadline) > 0) strcat(hyper_deadline, " ");
            strcat(hyper_deadline, tasks[i].name);
            extra_cycle_needed = true;
            // break;
        }
    }

    // Run one extra cycle only if a task has a deadline exactly at hyperperiod
    if (extra_cycle_needed) {
        int t = hyperperiod;
        // Check for deadline misses at the hyperperiod boundary
        if (strlen(hyper_deadline) > 0) {
            char *token = strtok(hyper_deadline, " ");
            fprintf(file, "%d %s ", t, name);
            while (token != NULL) {
            fprintf(file, "DEADLINE_MISS(%s) ", token);
            token = strtok(NULL, " "); // Get the next token
        }
        } else {
            fprintf(file, "\n");  // No missed deadlines
        }
    }

    fclose(file);  // Close the output file
}

// Main function
int main(int argc, char *argv[]) {
    if (argc != 7) {
        fprintf(stderr, "Usage: %s -i <input_file> -e <algorithm> -o <output_file>\n", argv[0]);
        return 1;
    }
    char *input_file = NULL, *output_file = NULL, *algorithm = NULL;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0) input_file = argv[++i];
        else if (strcmp(argv[i], "-e") == 0) algorithm = argv[++i];
        else if (strcmp(argv[i], "-o") == 0) output_file = argv[++i];
    }

    Task tasks[MAX_TASKS];
    int task_count;
    if (parse_input(input_file, tasks, &task_count) != 0) return 1;

    if (strcmp(algorithm, "rm") == 0) schedule_rm(tasks, task_count, output_file);
    else if (strcmp(algorithm, "edf") == 0) schedule_edf(tasks, task_count, output_file);
    else fprintf(stderr, "Unknown algorithm: %s\n", algorithm);

    return 0;
}
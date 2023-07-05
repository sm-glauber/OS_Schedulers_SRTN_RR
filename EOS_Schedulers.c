#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESSES 10
#define MAX_AUX 50
#define MAXIMUM_INT 2147483646

typedef struct
{
    int id;
    int arrival_time;
    int burst_time;
    int priority;
    int time_left;
    int conclusion_time;
    int ready_time;
} Process;

Process processes[MAX_PROCESSES];
Process auxBuffer[MAX_AUX];
Process ordered_finished[MAX_AUX];
Process empty;

FILE *file;

int start = 0, end = 0;
int proc_id = 1;
int num_proc = 0;
int clock_tick = 0;
int next_file_proc = 0;
int max_file_itens;

// FUNCTION TO RESET ALL VARIABLE TO USE AGAIN
void init()
{
    empty.id = 0;
    empty.arrival_time = 0;
    empty.time_left = 0;
    empty.burst_time = 0;
    empty.priority = 0;
    empty.conclusion_time = 0;
    empty.ready_time = MAXIMUM_INT;

    for (int i = 0; i < MAX_AUX; i++)
    {
        auxBuffer[i] = empty;
        ordered_finished[i] = empty;

        if (i < MAX_PROCESSES)
        {
            processes[i] = empty;
        }
    }

    start = 0;
    end = 0;
    proc_id = 1;
    num_proc = 0;
    clock_tick = 0;
    next_file_proc = 0;
    max_file_itens = 0;
}

// FUNCTION TO CREATE A FILE THAT CONTAINS THE PROCESSES
void create_input()
{
    file = fopen("input.txt", "w");
    if (file != NULL)
    {
        fprintf(file, "0 3 5\n0 2 4\n0 5 2\n1 2 5\n1 7 3\n2 9 0\n3 6 1\n6 2 1\n4 5 6\n2 9 1\n"
                      "2 1 9\n3 7 1\n4 2 8\n4 8 3\n5 4 7\n5 2 2\n3 4 5\n1 2 9\n1 3 8\n2 7 5");
    }
    fclose(file);
}

// AUXILIAR FUNCTION TO RETURN A PIVOT TO QUICKSORT FUNCTION
int slice_arrival(Process vector[], int left, int right)
{
    Process aux;
    int pivot = left;

    for (int i = left + 1; i <= right; i++)
    {
        if (vector[i].arrival_time < vector[left].arrival_time)
        {
            pivot++;
            aux = vector[i];
            vector[i] = vector[pivot];
            vector[pivot] = aux;
        }
    }

    aux = vector[left];
    vector[left] = vector[pivot];
    vector[pivot] = aux;

    return pivot;
}

// ADAPTATION OF QUICKSORT FUNCTION TO ORDENATE THE PROCESSES ON "input.txt" BY THE ARRIVAL TIME
void quick_arrival(Process vector[], int left, int right)
{
    int position;
    if (left < right)
    {
        position = slice_arrival(vector, left, right);
        quick_arrival(vector, left, position - 1);
        quick_arrival(vector, position + 1, right);
    }
}

// FUNCTION TO COPY THE PROCESSES OF "input.txt" TO THE AUXILIAR BUFFER
void prepare_auxBuffer()
{
    // Creating the file
    create_input();

    // Ooenning the file
    file = fopen("input.txt", "r");
    int arrival_time_f, burst_time_f, priority_f;
    Process aux;
    int i = 0;
    init();

    // Getting all the information of the file "input.txt" and adding indo the auxiliar vector called "auxBuffer"
    while (!feof(file))
    {
        fscanf(file, "%i %i %i", &arrival_time_f, &burst_time_f, &priority_f);
        auxBuffer[i].id = proc_id;
        auxBuffer[i].arrival_time = arrival_time_f;
        auxBuffer[i].burst_time = burst_time_f;
        auxBuffer[i].priority = priority_f;
        auxBuffer[i].time_left = burst_time_f;
        auxBuffer[i].ready_time = MAXIMUM_INT;
        proc_id++;
        i++;
    }
    max_file_itens = i;
    fclose(file);

    // Ordenating the auxBuffer vector by the arrival time
    quick_arrival(auxBuffer, 0, max_file_itens - 1);
}

// FUNCTION TO PRINT THE PROCESSES OF THE AUXBUFFER
void print_auxBuffer()
{
    printf("\n##### PROCESS IN AUX BUFFER #####\n");
    printf("PROCESS: ARRIVAL TIME | BURST TIME\n");
    for (int i = 0; i < max_file_itens; i++)
    {
        if (auxBuffer[i].id < 10)
        {
            printf("  P%d:         %d       |     %d    \n",
                   auxBuffer[i].id, auxBuffer[i].arrival_time, auxBuffer[i].burst_time);
        }
        else
        {
            printf("  P%d:        %d       |     %d    \n",
                   auxBuffer[i].id, auxBuffer[i].arrival_time, auxBuffer[i].burst_time);
        }
    }
}

// FUNCTION TO PRINT THE PROCESSES OF THE MAIN CIRCULAR BUFFER
void print_processes()
{
    int i = start;
    printf("\n##### PROCESSES IN THE MAIN BUFFER #####\n");
    printf("PROCESS: ARRIVAL TIME | BURST TIME\n");
    while (i != end)
    {
        if (processes[i].id < 10)
        {
            printf("  P%d:         %d       |     %d\n",
                   processes[i].id, processes[i].arrival_time, processes[i].burst_time);
        }
        else
        {
            printf("  P%d:        %d       |     %d\n",
                   processes[i].id, processes[i].arrival_time, processes[i].burst_time);
        }
        i = (i + 1) % MAX_PROCESSES;
    }
    printf("\n");
}

// FUNCTION TO ADD THE ORDENATE PROCESS FROM THE AUXBUFFER TO THE MAIN CIRCULAR BUFFER
void addProcesses()
{
    while ((next_file_proc < max_file_itens) && ((end + 1) % MAX_PROCESSES) != start)
    {
        if (auxBuffer[next_file_proc].id != 0)
        {
            processes[end] = auxBuffer[next_file_proc];
            auxBuffer[next_file_proc] = empty;
            end = (end + 1) % MAX_PROCESSES;
        }
        next_file_proc++;
    }
}

// FUNCTION TO REMOVE THE FINISHED PROCESS IN THE MAIN CIRCULAR BUFFER
void removeProcesses()
{
    if ((start + 1) % MAX_PROCESSES != end)
    {
        processes[start] = empty;
        start = (start + 1) % MAX_PROCESSES;
    }
}

// FUNCTION TO ADD A FINISHED PROCESS IN THE MAIN CIRCULAR BUFFER TO THE LIST OF ORDERED PROCESSES
void addOrderedFinished()
{
    ordered_finished[num_proc] = processes[start];
    ordered_finished[num_proc].conclusion_time = clock_tick;
    num_proc++;
}

// FUNCTION TO PRINT THE SYSTEM WORKING
void print_system()
{
    if (clock_tick < 10)
    {
        printf("TIME: 0%d - ", clock_tick);
    }
    else
    {
        printf("TIME: %d - ", clock_tick);
    }

    if (processes[start].id < 10)
    {
        printf("P0%d - ", processes[start].id);
    }
    else
    {
        printf("P%d - ", processes[start].id);
    }

    if (processes[start].time_left < 10)
    {
        printf("TL: 0%d", processes[start].time_left);
    }
    else
    {
        printf("TL: %d", processes[start].time_left);
    }

    if (processes[start].time_left == 0)
    {
        printf(" -> FINISHED!");
    }

    printf("\n");
}

// FUNCTION OF SHORTEST REMAINING TIME NEXT SCHEDULER
void srtn_scheduler()
{
    printf("\nSHORTEST REMAINING TIME NEXT SCHEDULER\n\n");
    Process aux;
    int i = start, j = start;

    // num_proc is a variable that is increased as soon as a process finishes
    // max_file_itens is a variable that lay in the numbers of process in the "input.txt" file
    // If num_proc is equal to max_file_itens, it means that all the procces of the file is finished
    while (num_proc != max_file_itens)
    {
        // Search for the process with the shortest remaining time and change its position in the buffer
        while (j != end)
        {
            while (i != end)
            {
                // Check if the process ID is equal to 0
                // In positive case, it means that is an empty process
                if (processes[i].id != 0 && processes[j].id != 0)
                {
                    // Check if the arrival time of the processes is equal or less than the current time
                    if ((processes[i].arrival_time <= clock_tick) && (processes[j].arrival_time <= clock_tick))
                    {
                        // Verify which process has the shortest remaining time and change its position
                        if (processes[i].time_left > processes[j].time_left)
                        {
                            aux = processes[j];
                            processes[j] = processes[i];
                            processes[i] = aux;
                        }
                    }
                }
                i = (i + 1) % MAX_PROCESSES;
            }
            j = (j + 1) % MAX_PROCESSES;
            i = start;
        }
        j = start;

        // Condition to print the system at the time 0
        if (clock_tick == 0)
        {
            print_system();
        }

        // Check if the process in the start of the buffer arrived in the current time and verify
        // if the process is finished or not
        if ((processes[start].arrival_time <= clock_tick) && (processes[start].time_left > 0))
        {
            processes[start].time_left--;
            clock_tick++;
        }

        print_system();

        // Check if the process is finished to remove it and add a new process to the buffer
        if (processes[start].time_left == 0)
        {
            addOrderedFinished();
            removeProcesses();
            addProcesses();
        }
    }
}

// FUNCTION TO REMOVE DE FIRST PROCESS AND ADD IT TO THE END OF THE BUFFER
void addBufferAgain()
{
    Process aux;

    aux = processes[start];

    processes[start] = empty;
    start = (start + 1) % MAX_PROCESSES;

    processes[end] = aux;
    end = (end + 1) % MAX_PROCESSES;
}

// FUNCTION OF ROUND-ROBIN SCHEDULER
void rr_scheduler(int quantum)
{
    printf("\n ROUND-ROBIN SCHEDULER\n\n");
    print_system();

    int i;
    int j = start;
    int shortestReady; // Variable to lay in the shortest ready time of all process on buffer

    // num_proc is a variable that is increased as soon as a process finishes
    // max_file_itens is a variable that lay in the numbers of process in the "input.txt" file
    // If num_proc is equal to max_file_itens, it means that all the procces of the file is finished
    while (num_proc != max_file_itens)
    {
        // Check if the process in the start of the buffer arrived in the current time and verify
        // if the process is finished or not
        if ((processes[start].arrival_time <= clock_tick) && (processes[start].time_left > 0))
        {
            // Check if the process
            if (processes[start].time_left >= quantum)
            {
                for (i = 0; i < quantum; i++)
                {
                    processes[start].time_left--;
                    clock_tick++;
                    print_system();
                }
            }
            else
            {
                // Variable to lay in the value o the time left of the first process
                // to use it on for loop
                int max_for = processes[start].time_left;
                for (i = 0; i < max_for; i++)
                {
                    processes[start].time_left--;
                    clock_tick++;
                    print_system();
                }
            }
            processes[start].ready_time = clock_tick;

            // Check if the process is finished to remove it and add a new process to the buffer
            // If the process isn't finished, it goes to the end of the buffer
            if (processes[start].time_left == 0)
            {
                addOrderedFinished();
                removeProcesses();
                addProcesses();
            }
            else
            {
                addBufferAgain();
            }
        }

        // If the arrival time of the process is after the clock tick, it can't be executed
        // and the process go the end off the buffer
        if (processes[start].arrival_time > clock_tick)
        {
            addBufferAgain();
        }

        // If the arrival time of the process is before or equal to the clock tick
        // and his ready time has not been updated, this condition will update it
        if ((processes[start].arrival_time <= clock_tick) && (processes[start].ready_time == MAXIMUM_INT))
        {
            processes[start].ready_time = processes[start].arrival_time;
        }

        shortestReady = processes[start].ready_time;
        // Loop to update the shortest ready time of all process
        while (j != end)
        {
            if (processes[j].ready_time < shortestReady)
            {
                shortestReady = processes[j].ready_time;
            }
            j = (j + 1) % MAX_PROCESSES;
        }
        j = start;

        // If the arrival time of the first process on buffer is bigger than the shortest ready time
        // the process goes to the end of the buffer
        while (processes[start].arrival_time > shortestReady)
        {
            addBufferAgain();
        }

        // If the ready time of the first process on buffer is bigger than the shortest ready time
        // the process goes to the end of the buffer
        while (processes[start].ready_time > shortestReady)
        {
            addBufferAgain();
        }
    }
}

// AUXILIAR FUNCTION TO RETURNS A PIVOT TO QUICKSORT FUNCTION
int slice_ordenate_id(Process vector[], int left, int right)
{
    Process aux;
    int pivot = left;

    for (int i = left + 1; i <= right; i++)
    {
        if ((vector[i].id != 0) && (vector[left].id != 0))
        {
            if (vector[i].id < vector[left].id)
            {
                pivot++;
                aux = vector[i];
                vector[i] = vector[pivot];
                vector[pivot] = aux;
            }
        }
    }

    aux = vector[left];
    vector[left] = vector[pivot];
    vector[pivot] = aux;

    return pivot;
}

// ADAPTATION OF QUICKSORT FUNCTION TO ORDENATE THE PROCESSES ON THE ORDERED LIST OF PROCESS
void quick_ordenate_id(Process vector[], int left, int right)
{
    int position;
    if (left < right)
    {
        position = slice_ordenate_id(vector, left, right);
        quick_ordenate_id(vector, left, position - 1);
        quick_ordenate_id(vector, position + 1, right);
    }
}

// FUNCTION TO CREATE AN OUTPUT FILE "output.txt"
void create_output()
{
    quick_ordenate_id(ordered_finished, 0, next_file_proc);

    file = fopen("output.txt", "w");
    fprintf(file, "SHORTEST REMAINING TIME NEXT\n");
    fprintf(file, "  PROC   AT   BT   CT");
    fprintf(file, "\n--------------------------\n");
    for (int i = 0; i < num_proc; i++)
    {
        if (ordered_finished[i].id < 10)
        {
            fprintf(file, "  P0%d:",
                    ordered_finished[i].id);
        }
        else
        {
            fprintf(file, "  P%d:",
                    ordered_finished[i].id);
        }

        if (ordered_finished[i].arrival_time < 10)
        {
            fprintf(file, "    %d", ordered_finished[i].arrival_time);
        }
        else
        {
            fprintf(file, "   %d", ordered_finished[i].arrival_time);
        }

        if (ordered_finished[i].burst_time < 10)
        {
            fprintf(file, "    %d", ordered_finished[i].burst_time);
        }
        else
        {
            fprintf(file, "   %d", ordered_finished[i].burst_time);
        }

        if (ordered_finished[i].conclusion_time < 10)
        {
            fprintf(file, "    %d",
                    ordered_finished[i].conclusion_time);
        }
        else
        {
            fprintf(file, "   %d",
                    ordered_finished[i].conclusion_time);
        }
        fprintf(file, "\n--------------------------\n");
    }
    fclose(file);
}

// FUNCTION TO ADD ITENS TO THE OUTPUT FILE "output.txt"
void add_to_output()
{
    quick_ordenate_id(ordered_finished, 0, next_file_proc);

    file = fopen("output.txt", "a");
    fprintf(file, "ROUND ROBIN\n");
    fprintf(file, "  PROC   AT   BT   CT");
    fprintf(file, "\n--------------------------\n");
    for (int i = 0; i < num_proc; i++)
    {
        if (ordered_finished[i].id < 10)
        {
            fprintf(file, "  P0%d:",
                    ordered_finished[i].id);
        }
        else
        {
            fprintf(file, "  P%d:",
                    ordered_finished[i].id);
        }

        if (ordered_finished[i].arrival_time < 10)
        {
            fprintf(file, "    %d", ordered_finished[i].arrival_time);
        }
        else
        {
            fprintf(file, "   %d", ordered_finished[i].arrival_time);
        }

        if (ordered_finished[i].burst_time < 10)
        {
            fprintf(file, "    %d", ordered_finished[i].burst_time);
        }
        else
        {
            fprintf(file, "   %d", ordered_finished[i].burst_time);
        }

        if (ordered_finished[i].conclusion_time < 10)
        {
            fprintf(file, "    %d",
                    ordered_finished[i].conclusion_time);
        }
        else
        {
            fprintf(file, "   %d",
                    ordered_finished[i].conclusion_time);
        }
        fprintf(file, "\n--------------------------\n");
    }
    fclose(file);
}

// FUNCTION TO PRINT THE LIST OF ORDERED PROCESSES
void print_ordered()
{
    // PROC - Processes
    // AT - Arrival Time
    // BT - Burst Time
    //  CT - Conclusion Time
    quick_ordenate_id(ordered_finished, 0, next_file_proc);

    printf("\n\n");
    printf("  PROC   AT   BT   CT");
    printf("\n--------------------------\n");
    for (int i = 0; i < num_proc; i++)
    {
        if (ordered_finished[i].id < 10)
        {
            printf("  P0%d:",
                   ordered_finished[i].id);
        }
        else
        {
            printf("  P%d:",
                   ordered_finished[i].id);
        }

        if (ordered_finished[i].arrival_time < 10)
        {
            printf("    %d", ordered_finished[i].arrival_time);
        }
        else
        {
            printf("   %d", ordered_finished[i].arrival_time);
        }

        if (ordered_finished[i].burst_time < 10)
        {
            printf("    %d", ordered_finished[i].burst_time);
        }
        else
        {
            printf("   %d", ordered_finished[i].burst_time);
        }

        if (ordered_finished[i].conclusion_time < 10)
        {
            printf("    %d",
                   ordered_finished[i].conclusion_time);
        }
        else
        {
            printf("   %d",
                   ordered_finished[i].conclusion_time);
        }
        printf("\n--------------------------\n");
    }
    printf("\n");
}

// MAIN FUNCTION
int main()
{

    prepare_auxBuffer();
    print_auxBuffer();
    addProcesses();
    print_processes();
    srtn_scheduler();
    create_output();
    print_ordered();

    int quantum = 3;
    prepare_auxBuffer();
    print_auxBuffer();
    addProcesses();
    print_processes();
    rr_scheduler(quantum);
    add_to_output();
    print_ordered();

    return 0;
}
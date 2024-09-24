#ifndef SCHEDULING_ALGRORITHM_CPP
#define SCHEDULING_ALGRORITHM_CPP

#include <iostream>
#include <vector>
#include <queue>
#include <windows.h>
#include <iomanip>
#include "Process.h"

// Initilizes variables related to statistical information
int context_switch = 14;
double total_turnaround = 0.0;
double total_wait = 0.0;
double total_initial = 0.0;
double max_turnaround = 0.0;
double min_turnaround = 5000.0;
double max_wait = 0.0;
double min_wait = 1000000.0;
double max_initial = 0.0;
double min_initial = 2300000;
double scheduling_length = 0;

// Prints statistics
void statistics(int num_processes)
{
    // Turnaround time
    cout << "\033[1;34mTurnaround time:\033[0m" << "\033[1;35m min \033[0m"; // Change color to magenta
    cout << fixed << setprecision(3) << min_turnaround << "ms";
    cout << "; \033[1;35mavg \033[0m"; // Change color to magenta
    cout << fixed << setprecision(3) << (total_turnaround / num_processes) << "ms";
    cout << "; \033[1;35mmax \033[0m"; // Change color to magenta
    cout << fixed << setprecision(3) << max_turnaround << "ms" << endl;

    // Initial wait time
    cout << "\033[1;34mInitial wait time:\033[0m" << "\033[1;35m min \033[0m"; // Change color to magenta
    cout << fixed << setprecision(3) << min_initial << "ms";
    cout << "; \033[1;35mavg \033[0m"; // Change color to magenta
    cout << fixed << setprecision(3) << (total_initial / num_processes) << "ms";
    cout << "; \033[1;35mmax \033[0m"; // Change color to magenta
    cout << fixed << setprecision(3) << max_initial << "ms" << endl;

    // Total wait time
    cout << "\033[1;34mTotal wait time:\033[0m" << "\033[1;35m min \033[0m"; // Change color to magenta
    cout << fixed << setprecision(3) << min_wait << "ms";
    cout << "; \033[1;35mavg \033[0m"; // Change color to magenta
    cout << fixed << setprecision(3) << (total_wait / num_processes) << "ms";
    cout << "; \033[1;35mmax \033[0m"; // Change color to magenta
    cout << fixed << setprecision(3) << max_wait << "ms" << endl;

    // Total Scheduling time
    cout << "\033[1;34mTotal Scheduling time: \033[0m" << fixed << setprecision(3) << scheduling_length << endl;

    // Reset statistical variables
    total_turnaround = 0.0;
    total_wait = 0.0;
    total_initial = 0.0;
    max_wait = 0.0;
    min_wait = 5000.0;
    max_initial = 0.0;
    min_initial = 5000.0;
    max_turnaround = 0.0;
    min_turnaround = 5000.0;
    scheduling_length = 0;
    cout << endl;
}

class Algorithm
{
public:
    void first_come_first_serve(vector<process> processes, int num_processes)
    {
        int num_completed = 0;
        int clock = 0;
        int last_pid = 0;
        vector<process> queue;
        bool cswitch = false;
        unsigned int index = processes.size();
        // Function runs until it detects it has completed every process
        while (num_completed != num_processes)
        {
            index = processes.size();
            for (unsigned int i = 0; i < index; i++)
            {
                if (processes[i].get_arrival() <= clock)
                {
                    queue.push_back(processes[i]);
                    cout << "\033[32m[time " << processes[i].get_arrival() << "ms] Process " << processes[i].get_pid() << " created (requires "
                     << processes[i].get_burst() << "ms CPU time)\033[0m" << endl;
                    processes.erase(processes.begin() + i);
                    index--;
                    i--;
                }
            }

            if (queue.size() != 0)
            {
                if (cswitch)
                {
                    cout << "\033[34m[time " << clock - 7 << "ms] Context switch (swapped out process " << last_pid << " for process " 
                    << queue[0].get_pid() << ")\033[0m" << endl;
                    clock = clock + 7;
                    cswitch = false;
                }
                if (queue[0].get_seen() == false)
                {
                    // The initials will change in non zero arrival times
                    int arriv = clock - queue[0].get_arrival();
                    queue[0].seen(arriv);
                    total_initial += arriv;
                    if (arriv < min_initial)
                        min_initial = arriv;
                    if (arriv > max_initial)
                        max_initial = arriv;
                    // total_wait += clock;
                    cout << "\033[33m[time " << clock << "ms] Process " << queue[0].get_pid() << " accessed CPU for the first time (initial wait time " 
                    << queue[0].get_initial_wait() << "ms)\033[0m" << endl;
                }
                queue[0].dec_burst();
                if (queue[0].get_burst_left() == 0)
                {
                    int fwait = ((clock + 1) - queue[0].get_burst() - queue[0].get_arrival());
                    cout << "\033[31m[time " << clock + 1 << "ms] Process " << queue[0].get_pid() << " completed its CPU burst (turnaround time " 
                    << ((clock + 1) - queue[0].get_arrival()) << "ms, initial wait time " << queue[0].get_initial_wait() << "ms, total wait time " 
                    << fwait << "ms)\033[0m" << endl;
                    last_pid = queue[0].get_pid();
                    int turn = ((clock + 1) - queue[0].get_arrival());
                    total_turnaround += turn;
                    total_wait += fwait;
                    if (fwait < min_wait)
                        min_wait = fwait;
                    if (fwait > max_wait)
                        max_wait = fwait;
                    if (turn < min_turnaround)
                        min_turnaround = turn;
                    if (turn > max_turnaround)
                        max_turnaround = turn;
                    queue.erase(queue.begin());
                    num_completed++;

                    if (queue.size() != 0)
                    {
                        cout << "\033[34m[time " << clock + 1 << "ms] Context switch (swapped out process " << last_pid << " for process " << 
                        queue[0].get_pid() << ")\033[0m" << endl;
                        clock = clock + context_switch;
                    }
                    else
                    {
                        cswitch = true;
                        // Releases finished process
                        clock = clock + 7;
                    }
                }
            }
            clock++;
        }
        scheduling_length = clock;
    }

    // Simulates SJF non-preemptive
    void shortest_job_first(vector<process> processes, int num_processes)
    {
        int num_completed = 0;
        int clock = 0;
        int last_pid = 0;
        vector<process> queue;
        vector<process> temp;
        bool cswitch = false;
        unsigned int index = processes.size();
        while (num_completed != num_processes)
        {
            index = processes.size();
            for (unsigned int i = 0; i < index; i++)
            {
                if (processes[i].get_arrival() <= clock)
                {
                    temp.push_back(processes[i]);
                    cout << "\033[32m[time " << clock << "ms] Process " << processes[i].get_pid() << " created (requires " 
                    << processes[i].get_burst() << "ms CPU time)\033[0m" << endl;
                    processes.erase(processes.begin());
                    index--;
                    i--;
                }
            }

            sort(temp.begin(), temp.end(), sorter);
            for (unsigned int i = 0; i < temp.size(); i++)
            {
                queue.push_back(temp[i]);
            }
            temp.clear();
            // Sorts the processes by burst time length, ignores the currently running process because it is non-preemptive
            sort(queue.begin() + 1, queue.end(), sorter);

            if (queue.size() != 0)
            {
                if (cswitch)
                {
                    cout << "\033[34m[time " << clock - 7 << "ms] Context switch (swapped out process " << last_pid << " for process " 
                    << queue[0].get_pid() << ")\033[0m" << endl;
                    clock = clock + 7;
                    cswitch = false;
                }
                if (queue[0].get_seen() == false)
                {
                    // Statistical info
                    int arriv = clock - queue[0].get_arrival();
                    queue[0].seen(arriv);
                    total_initial += arriv;
                    if (arriv < min_initial)
                        min_initial = arriv;
                    if (arriv > max_initial)
                        max_initial = arriv;
                    cout << "\033[33m[time " << clock << "ms] Process " << queue[0].get_pid() << " accessed CPU for the first time (initial wait time " 
                    << queue[0].get_initial_wait() << "ms)\033[0m" << endl;
                }

                queue[0].dec_burst();

                if (queue[0].get_burst_left() == 0)
                {
                    int fwait = ((clock + 1) - queue[0].get_burst() - queue[0].get_arrival());
                    cout << "\033[31m[time " << clock + 1 << "ms] Process " << queue[0].get_pid() << " completed its CPU burst (turnaround time "
                     << ((clock + 1) - queue[0].get_arrival()) << "ms, initial wait time " << queue[0].get_initial_wait() << "ms, total wait time "
                      << fwait << "ms)\033[0m" << endl;

                    last_pid = queue[0].get_pid();
                    int turn = ((clock + 1) - queue[0].get_arrival());
                    total_turnaround += turn;
                    total_wait += fwait;
                    if (fwait < min_wait)
                        min_wait = fwait;
                    if (fwait > max_wait)
                        max_wait = fwait;
                    if (turn < min_turnaround)
                        min_turnaround = turn;
                    if (turn > max_turnaround)
                        max_turnaround = turn;
                    queue.erase(queue.begin());

                    num_completed++;

                    if (queue.size() != 0)
                    {
                        cout << "\033[34m[time " << clock + 1 << "ms] Context switch (swapped out process " << last_pid 
                        << " for process " << queue[0].get_pid() << ")\033[0m" << endl;
                        clock = clock + context_switch;
                    }
                    else
                    {
                        cswitch = true;
                        // Releases finished process
                        clock = clock + 7;
                    }
                }
            }
            clock++;
        }
        scheduling_length = clock;
    }

    // Simulates Shortest Job remaining (preemptive SJF)
    void shortest_job_remaining(vector<process> processes, int num_processes)
    {
        int num_completed = 0;
        int clock = 0;
        int last_pid = 0;
        vector<process> queue;
        bool cswitch = false;
        unsigned int index = processes.size();
        while (num_completed != num_processes)
        {
            index = processes.size();
            for (unsigned int i = 0; i < index; i++)
            {
                if (processes[i].get_arrival() <= clock)
                {
                    cout << "\033[32m[time " << clock << "ms] Process " << processes[i].get_pid() << " created (requires " 
                    << processes[i].get_burst() << "ms CPU time)\033[0m" << endl;
                    if (queue.size() != 0 && processes[i].get_burst() < queue[0].get_burst() && queue[0].get_burst_left() != queue[0].get_burst())
                    {
                        // Context switch on preemption
                        cout << "\033[34m[time " << clock << "ms] Context switch (swapped out process " << queue[0].get_pid() << " for process " 
                        << processes[i].get_pid() << ")\033[0m" << endl;
                        clock = clock + 14;
                    }

                    queue.push_back(processes[i]);
                    sort(queue.begin(), queue.end(), sorter);
                    processes.erase(processes.begin());
                    index--;
                    i--;
                }
            }

            if (queue.size() != 0)
            {
                if (cswitch)
                {
                    cout << "\033[34m[time " << clock - 7 << "ms] Context switch (swapped out process " << last_pid << " for process "
                     << queue[0].get_pid() << ")\033[0m" << endl;
                    clock = clock + 7;
                    cswitch = false;
                }
                if (queue[0].get_seen() == false)
                {
                    int arriv = clock - queue[0].get_arrival();
                    queue[0].seen(arriv);
                    total_initial += arriv;
                    if (arriv < min_initial)
                        min_initial = arriv;
                    if (arriv > max_initial)
                        max_initial = arriv;
                    cout << "\033[33m[time " << clock << "ms] Process " << queue[0].get_pid() << " accessed CPU for the first time (initial wait time " 
                    << queue[0].get_initial_wait() << "ms)\033[0m" << endl;
                }
                queue[0].dec_burst();
                if (queue[0].get_burst_left() == 0)
                {
                    int fwait = ((clock + 1) - queue[0].get_burst() - queue[0].get_arrival());
                    cout << "\033[31m[time " << clock + 1 << "ms] Process " << queue[0].get_pid() << " completed its CPU burst (turnaround time " 
                    << ((clock + 1) - queue[0].get_arrival()) << "ms, initial wait time " << queue[0].get_initial_wait() << "ms, total wait time " 
                    << fwait << "ms)\033[0m" << endl;

                    last_pid = queue[0].get_pid();
                    int turn = ((clock + 1) - queue[0].get_arrival());
                    total_turnaround += turn;
                    total_wait += fwait;
                    if (fwait < min_wait)
                        min_wait = fwait;
                    if (fwait > max_wait)
                        max_wait = fwait;
                    if (turn < min_turnaround)
                        min_turnaround = turn;
                    if (turn > max_turnaround)
                        max_turnaround = turn;
                    queue.erase(queue.begin());

                    num_completed++;

                    if (queue.size() != 0)
                    {
                        cout << "\033[34m[time " << clock + 1 << "ms] Context switch (swapped out process " << last_pid << " for process "
                         << queue[0].get_pid() << ")\033[0m" << endl;
                        clock = clock + context_switch;
                    }
                    else
                    {
                        cswitch = true;
                        // Releases finished process
                        clock = clock + 7;
                    }
                }
            }
            clock++;
        }
        scheduling_length = clock;
    }

    // Simulates Round Robin Scheduling
    void round_robin(vector<process> processes, int num_processes)
    {
        int num_completed = 0;
        int clock = 0;
        int last_pid = 0;

        // Sets the timeslice size
        int timeslice = 200;
        int current_slice = 0;
        vector<process> queue;
        unsigned int index = processes.size();
        bool cswitch = false;
        while (num_completed != num_processes)
        {
            for (unsigned int i = 0; i < index; i++)
            {
                if (processes[i].get_arrival() <= clock)
                {
                    queue.push_back(processes[i]);
                    cout << "\033[32m[time " << clock << "ms] Process " << processes[i].get_pid() << " created (requires " 
                    << processes[i].get_burst() << "ms CPU time)\033[0m" << endl;
                    processes.erase(processes.begin());
                    index--;
                    i--;
                }
            }

            if (queue.size() != 0)
            {
                if (cswitch)
                {
                    cout << "\033[34m[time " << clock - 7 << "ms] Context switch (swapped out process " << last_pid << " for process " 
                    << queue[0].get_pid() << ")\033[0m" << endl;
                    clock = clock + 7;
                    cswitch = false;
                }
                if (queue[0].get_seen() == false)
                {

                    int initial = (clock - queue[0].get_arrival());
                    queue[0].seen(initial);
                    total_initial += initial;
                    if (initial < min_initial)
                        min_initial = initial;
                    if (initial > max_initial)
                        max_initial = initial;

                    cout << "\033[33m[time " << clock << "ms] Process " << queue[0].get_pid() << " accessed CPU for the first time (initial wait time " 
                    << queue[0].get_initial_wait() << "ms)\033[0m" << endl;
                }
                queue[0].dec_burst();
                current_slice++;
                if (queue[0].get_burst_left() == 0)
                {
                    int total_wait_p = ((clock + 1) - queue[0].get_arrival() - queue[0].get_burst());
                    cout << "\033[31m[time " << clock + 1 << "ms] Process " << queue[0].get_pid() << " completed its CPU burst (turnaround time " 
                    << ((clock + 1) - queue[0].get_arrival()) << "ms, initial wait time " << queue[0].get_initial_wait() << "ms, total wait time " 
                    << total_wait_p << "ms)\033[0m" << endl;
                    last_pid = queue[0].get_pid();
                    int turnaround = ((clock + 1) - queue[0].get_arrival());
                    total_turnaround += turnaround;
                    total_wait += total_wait_p;
                    if (total_wait_p < min_wait)
                        min_wait = total_wait_p;
                    if (total_wait_p > max_wait)
                        max_wait = total_wait_p;
                    if ((turnaround) < min_turnaround)
                        min_turnaround = turnaround;
                    if ((turnaround) > max_turnaround)
                        max_turnaround = turnaround;
                    queue.erase(queue.begin());
                    num_completed++;
                    if (queue.size() != 0)
                    {
                        cout << "\033[34m[time " << clock + 1 << "ms] Context switch (swapped out process " << last_pid << " for process " 
                        << queue[0].get_pid() << ")\033[0m" << endl;
                        clock = clock + context_switch;
                    }
                    else
                    {
                        cswitch = true;
                        // Releases finished process
                        clock = clock + 7;
                    }
                    current_slice = 0;
                }
                else if (current_slice == timeslice) // When timeslice is over
                {
                    last_pid = queue[0].get_pid();
                    queue.push_back(queue[0]);
                    queue.erase(queue.begin());
                    current_slice = 0;
                    cout << "\033[34m[time " << clock + 1 << "ms] Context switch (swapped out process " << last_pid << " for process " 
                    << queue[0].get_pid() << ")\033[0m" << endl;
                    clock = clock + context_switch;
                }
            }
            clock++;
        }
        scheduling_length = clock;
    }

    // Simulates priority scheduling
    void priority(vector<process> processes, int num_processes)
    {
        int num_completed = 0;
        int clock = 0;
        int last_pid = 0;

        // Sets the timeslice size
        int timeslice = 200;
        int current_slice = 0;
        vector<process> queue;
        vector<process> temp_queue;
        unsigned int index = processes.size();
        unsigned int index2;
        bool cswitch = false;
        int most_priority = 6;
        while (num_completed != num_processes)
        {
            for (unsigned int i = 0; i < index; i++)
            {
                if (processes[i].get_arrival() <= clock)
                {
                    temp_queue.push_back(processes[i]);
                    index2 = temp_queue.size();
                    cout << "\033[32m[time " << clock << "ms] Process " << processes[i].get_pid() << " created (requires " 
                    << processes[i].get_burst() << "ms CPU time  Priority: " << processes[i].get_priority() << ")\033[0m" << endl;
                    sort(temp_queue.begin(), temp_queue.end(), sorter2);
                    // Finds highest priority present
                    if (processes[i].get_priority() < most_priority)
                        most_priority = processes[i].get_priority();
                    processes.erase(processes.begin());
                    index--;
                    i--;
                }
            }
            index2 = temp_queue.size();
            for (unsigned int i = 0; i < index2; i++)
            {
                if (temp_queue[i].get_priority() == most_priority)
                {
                    if (queue.size() != 0 && temp_queue[i].get_priority() < queue[0].get_priority())
                    {
                        cout << "\033[34m[time " << clock << "ms] Context switch (swapped out process " << queue[0].get_pid() << " for process "
                         << temp_queue[i].get_pid() << ")\033[0m" << endl;
                        clock = clock + 14;
                        for (unsigned int x = 0; x < queue.size(); x++)
                        {
                            temp_queue.push_back(queue[x]);
                        }
                        queue.clear();
                    }
                    queue.push_back(temp_queue[i]);
                    temp_queue.erase(temp_queue.begin());
                    index2--;
                    i--;
                }
            }
            // Uses round robin for multiple processes with same highest priority
            if (queue.size() != 0 && queue.size() > 1)
            {

                if (cswitch)
                {
                    cout << "\033[34m[time " << clock - 7 << "ms] Context switch (swapped out process " << last_pid << " for process " 
                    << queue[0].get_pid() << ")\033[0m" << endl;
                    clock = clock + 7;
                    cswitch = false;
                }
                if (queue[0].get_seen() == false)
                {
                    // The initials will change in non zero arrival times
                    int initial = (clock - queue[0].get_arrival());
                    queue[0].seen(initial);
                    total_initial += initial;
                    if (initial < min_initial)
                        min_initial = initial;
                    if (initial > max_initial)
                        max_initial = initial;
                    // total_wait += initial;
                    cout << "\033[33m[time " << clock << "ms] Process " << queue[0].get_pid() << " accessed CPU for the first time (initial wait time " 
                    << queue[0].get_initial_wait() << "ms)\033[0m" << endl;
                }
                queue[0].dec_burst();
                current_slice++;
                if (queue[0].get_burst_left() == 0)
                {
                    int total_wait_p = ((clock + 1) - queue[0].get_arrival() - queue[0].get_burst());
                    cout << "\033[31m[time " << clock + 1 << "ms] Process " << queue[0].get_pid() << " completed its CPU burst (turnaround time "
                     << ((clock + 1) - queue[0].get_arrival()) << "ms, initial wait time " << queue[0].get_initial_wait() << "ms, total wait time " 
                     << total_wait_p << "ms)\033[0m" << endl;
                    last_pid = queue[0].get_pid();
                    int turnaround = ((clock + 1) - queue[0].get_arrival());
                    total_turnaround += turnaround;
                    total_wait += total_wait_p;
                    if (total_wait_p < min_wait)
                        min_wait = total_wait_p;
                    if (total_wait_p > max_wait)
                        max_wait = total_wait_p;
                    if ((turnaround) < min_turnaround)
                        min_turnaround = turnaround;
                    if ((turnaround) > max_turnaround)
                        max_turnaround = turnaround;
                    queue.erase(queue.begin());
                    num_completed++;
                    if (queue.size() != 0)
                    {
                        cout << "\033[34m[time " << clock + 1 << "ms] Context switch (swapped out process " << last_pid << " for process " 
                        << queue[0].get_pid() << ")\033[0m" << endl;
                        clock = clock + context_switch;
                    }
                    else
                    {
                        cswitch = true;
                        // Releases finished process
                        clock = clock + 7;
                        if (temp_queue.size() != 0)
                            most_priority = temp_queue[0].get_priority();
                        else
                            most_priority++;
                    }
                    current_slice = 0;
                }
                else if (current_slice == timeslice)
                {
                    last_pid = queue[0].get_pid();
                    queue.push_back(queue[0]);
                    queue.erase(queue.begin());
                    current_slice = 0;
                    cout << "\033[34m[time " << clock + 1 << "ms] Context switch (swapped out process " << last_pid << " for process " 
                    << queue[0].get_pid() << ")\033[0m" << endl;
                    clock = clock + context_switch;
                }
            }
            else if (queue.size() == 1) // Process runs unhindered when theres only 1 process of highest priority
            {
                if (cswitch)
                {
                    cout << "\033[34m[time " << clock - 7 << "ms] Context switch (swapped out process " << last_pid << " for process " 
                    << queue[0].get_pid() << ")\033[0m" << endl;
                    clock = clock + 7;
                    cswitch = false;
                }
                if (queue[0].get_seen() == false)
                {
                    // The initials will change in non zero arrival times
                    int arriv = clock - queue[0].get_arrival();
                    queue[0].seen(arriv);
                    total_initial += arriv;
                    if (arriv < min_initial)
                        min_initial = arriv;
                    if (arriv > max_initial)
                        max_initial = arriv;
                    // total_wait += clock;
                    cout << "\033[33m[time " << clock << "ms] Process " << queue[0].get_pid() << " accessed CPU for the first time (initial wait time " 
                    << queue[0].get_initial_wait() << "ms)\033[0m" << endl;
                }
                queue[0].dec_burst();
                if (queue[0].get_burst_left() == 0)
                {
                    int fwait = ((clock + 1) - queue[0].get_burst() - queue[0].get_arrival());
                    cout << "\033[31m[time " << clock + 1 << "ms] Process " << queue[0].get_pid() << " completed its CPU burst (turnaround time " 
                    << ((clock + 1) - queue[0].get_arrival()) << "ms, initial wait time " << queue[0].get_initial_wait() << "ms, total wait time " 
                    << fwait << "ms)\033[0m" << endl;
                    last_pid = queue[0].get_pid();
                    int turn = ((clock + 1) - queue[0].get_arrival());
                    total_turnaround += turn;
                    total_wait += fwait;
                    if (fwait < min_wait)
                        min_wait = fwait;
                    if (fwait > max_wait)
                        max_wait = fwait;
                    if (turn < min_turnaround)
                        min_turnaround = turn;
                    if (turn > max_turnaround)
                        max_turnaround = turn;
                    queue.erase(queue.begin());
                    num_completed++;
                    if (queue.size() != 0)
                    {
                        cout << "\033[34m[time " << clock + 1 << "ms] Context switch (swapped out process " << last_pid << " for process " 
                        << queue[0].get_pid() << ")\033[0m" << endl;
                        clock = clock + context_switch;
                    }
                    else
                    {
                        cswitch = true;
                        // Releases finished process
                        clock = clock + 7;
                        if (temp_queue.size() != 0)
                            most_priority = temp_queue[0].get_priority();
                        else
                            most_priority++;
                    }
                }
            }
            else if (queue.size() == 0)
            {
                if (temp_queue.size() != 0)
                    most_priority = temp_queue[0].get_priority();
                else
                    most_priority++;
            }
            clock++;
        }
        scheduling_length = clock;
    }
};

#endif
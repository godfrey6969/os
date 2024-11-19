#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <iomanip>
#include <algorithm>


using namespace std;


struct Process {
    int arrival_time;
    int process_id;
    int burst_time;
    int priority;
    int remaining_time;
    int start_time;
    int finish_time;
    int waiting_time;
    int turnaround_time;
    int processor_id;
};


struct ComparePriority {
    bool operator()(const Process* a, const Process* b) {
        return a->priority > b->priority;
    }
};


void read_processes(vector<Process>& processes) {
    ifstream infile("sched2.dat");
    string line;
    while (getline(infile, line)) {
        stringstream ss(line);
        string token;
        Process proc;
        getline(ss, token, ',');
         proc.arrival_time = stoi(token);
        if (proc.arrival_time < 0) break;
        getline(ss, token, ',');
        proc.process_id = stoi(token);
        getline(ss, token, ',');
        proc.burst_time = stoi(token);
        proc.remaining_time = proc.burst_time;
        getline(ss, token, ',');
        proc.priority = stoi(token);
        proc.start_time = -1;
        proc.finish_time = -1;
        proc.waiting_time = 0;
        proc.turnaround_time = 0;
        proc.processor_id = 0;
        processes.push_back(proc);
    }
    infile.close();
}


void version1_scheduler(const vector<Process>& processes) {
    priority_queue<Process*, vector<Process*>, ComparePriority> process_queue;
    vector<Process> proc_list = processes;
    int time = 0;
    int completed = 0;
    int n = proc_list.size();
    int proc1_time = 0;
    int proc2_time = 0;


    cout << "\nVersion 1 Gantt Chart:\n";


    while (completed < n) {
        for (auto& proc : proc_list) {
            if (proc.arrival_time <= time && proc.start_time == -1) {
                process_queue.push(&proc);
                proc.start_time = -2;
            }
        }


        if (proc1_time <= time) {
             if (!process_queue.empty()) {
                Process* proc = process_queue.top();
                process_queue.pop();
                if (proc->start_time == -2) {
                    proc->start_time = time;
                    proc->waiting_time = time - proc->arrival_time;
                }
                proc->processor_id = 1;
                proc1_time = time + (proc->burst_time + 1) / 2;
                proc->finish_time = proc1_time;
                proc->turnaround_time = proc->finish_time - proc->arrival_time;
                completed++;
                cout << "Processor 1: P" << proc->process_id << " [" << time << " - " << proc->finish_time << "]\n";
            }
        }


        if (proc2_time <= time) {
            if (!process_queue.empty()) {
                Process* proc = process_queue.top();
                process_queue.pop();
                if (proc->start_time == -2) {
                    proc->start_time = time;
                    proc->waiting_time = time - proc->arrival_time;
                }
                proc->processor_id = 2;
                proc2_time = time + proc->burst_time;
                proc->finish_time = proc2_time;
                proc->turnaround_time = proc->finish_time - proc->arrival_time;
                completed++;
                cout << "Processor 2: P" << proc->process_id << " [" << time << " - " << proc->finish_time << "]\n";
            }
        }
        time++;
    }
cout << "\nProcess\tArrival\tBurst\tPriority\tProcessor\tWaiting\tTurnaround\n";
    int total_turnaround = 0;
    for (const auto& proc : proc_list) {
        cout << "P" << proc.process_id << "\t"
             << proc.arrival_time << "\t"
             << proc.burst_time << "\t"
             << proc.priority << "\t\t"
             << proc.processor_id << "\t\t"
             << proc.waiting_time << "\t"
             << proc.turnaround_time << "\n";
        total_turnaround += proc.turnaround_time;
    }
    cout << "\nAverage Turnaround Time: " << (double)total_turnaround / n << "\n";
}


void version2_scheduler(const vector<Process>& processes) {
    vector<Process*> queue1;
    vector<Process*> queue2;


    vector<Process> proc_list = processes;
    sort(proc_list.begin(), proc_list.end(), [](const Process& a, const Process& b) {
        return a.priority < b.priority;
    });


    int n = proc_list.size();
    int half_n = n / 2;


    for (int i = 0; i < n; ++i) {
        if (i < half_n) {
            queue1.push_back(&proc_list[i]);
        } else {
            queue2.push_back(&proc_list[i]);
        }
    }


    int time1 = 0;
    int time2 = 0;
 cout << "\nVersion 2 Gantt Chart:\n";


    cout << "Processor 1:\n";
    for (auto proc : queue1) {
        if (time1 < proc->arrival_time)
            time1 = proc->arrival_time;
        proc->start_time = time1;
        proc->processor_id = 1;
        proc->waiting_time = time1 - proc->arrival_time;
        time1 += (proc->burst_time + 1) / 2;
        proc->finish_time = time1;
        proc->turnaround_time = proc->finish_time - proc->arrival_time;
        cout << "P" << proc->process_id << " [" << proc->start_time << " - " << proc->finish_time << "]\n";
    }


    cout << "Processor 2:\n";
    for (auto proc : queue2) {
        if (time2 < proc->arrival_time)
            time2 = proc->arrival_time;
        proc->start_time = time2;
        proc->processor_id = 2;
        proc->waiting_time = time2 - proc->arrival_time;
        time2 += proc->burst_time;
        proc->finish_time = time2;
        proc->turnaround_time = proc->finish_time - proc->arrival_time;
        cout << "P" << proc->process_id << " [" << proc->start_time << " - " << proc->finish_time << "]\n";
    }


    cout << "\nProcess\tArrival\tBurst\tPriority\tProcessor\tWaiting\tTurnaround\n";
    int total_turnaround = 0;
    for (const auto& proc : proc_list) {
        cout << "P" << proc.process_id << "\t"
             << proc.arrival_time << "\t"
             << proc.burst_time << "\t"
             << proc.priority << "\t\t"
             << proc.processor_id << "\t\t"
 << proc.waiting_time << "\t"
             << proc.turnaround_time << "\n";
        total_turnaround += proc.turnaround_time;
    }
    cout << "\nAverage Turnaround Time: " << (double)total_turnaround / n << "\n";
}


int main() {
    vector<Process> processes;
    read_processes(processes);


    version1_scheduler(processes);
    version2_scheduler(processes);


    return 0;
}



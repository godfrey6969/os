#include <bits/stdc++.h>

using namespace std;

class Process {
   public:
    int id;
    int priority;
    int arrivalTime;
    vector<pair<int, int>> bursts;
    vector<char> history;

    Process(int id, int priority, int arrivalTime,
            vector<pair<int, int>> bursts) {
        this->id = id;
        this->priority = priority;
        this->arrivalTime = arrivalTime;
        this->bursts = bursts;
    }
};

class Scheduler {
   private:
    vector<Process> processes;
    int quantum;

    static bool compareArrivalTime(const Process& a, const Process& b) {
        return a.arrivalTime < b.arrivalTime;
    }

    static bool compareSJF(const Process& a, const Process& b) {
        if (a.arrivalTime == b.arrivalTime) {
            return a.bursts[0].first < b.bursts[0].first;
        }
        return a.arrivalTime < b.arrivalTime;
    }

    static bool comparePriority(const Process& a, const Process& b) {
        if (a.arrivalTime == b.arrivalTime) {
            return a.priority < b.priority;
        }
        return a.arrivalTime < b.arrivalTime;
    }

   public:
    Scheduler(int quantum) : quantum(quantum) {}

    void loadProcesses(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file!" << endl;
            return;
        }

        int numProcesses;
        file >> numProcesses;
        file.ignore();

        for (int i = 0; i < numProcesses; ++i) {
            string line;
            getline(file, line);
            stringstream ss(line);

            int id, priority, arrivalTime;
            char comma;
            ss >> id >> comma >> priority >> comma >> arrivalTime >> comma;

            vector<pair<int, int>> bursts;
            int value;
            while (ss >> value) {
                if (value == -1) break;

                int cpuBurst = value;
                int ioBurst = 0;
                ss >> comma >> ioBurst >> comma;
                bursts.push_back(make_pair(cpuBurst, ioBurst));
            }

            processes.push_back(Process(id, priority, arrivalTime, bursts));
        }
        file.close();
    }

    void simulateFCFS() {
        int currentTime = 0;
        sort(processes.begin(), processes.end(), compareArrivalTime);
        for (auto& process : processes) {
            if (currentTime < process.arrivalTime) {
                currentTime = process.arrivalTime;
            }

            for (int t = 0; t < process.arrivalTime - process.history.size();
                 ++t) {
                process.history.push_back('W');
            }

            for (int i = 0; i < process.bursts.size(); ++i) {
                for (int t = 0; t < process.bursts[i].first; ++t) {
                    process.history.push_back('C');
                    currentTime++;
                }
                if (i < process.bursts.size() - 1) {
                    for (int t = 0; t < process.bursts[i].second; ++t) {
                        process.history.push_back('B');
                        currentTime++;
                    }
                }
            }
            while (process.history.size() < currentTime) {
                process.history.push_back('W');
            }
        }
    }

    void simulateSJF() {
        sort(processes.begin(), processes.end(), compareSJF);

        int currentTime = 0;
        for (auto& process : processes) {
            if (currentTime < process.arrivalTime) {
                currentTime = process.arrivalTime;
            }

            for (int t = 0; t < process.arrivalTime - process.history.size();
                 ++t) {
                process.history.push_back('W');
            }

            for (int i = 0; i < process.bursts.size(); ++i) {
                for (int t = 0; t < process.bursts[i].first; ++t) {
                    process.history.push_back('C');
                    currentTime++;
                }
                if (i < process.bursts.size() - 1) {
                    for (int t = 0; t < process.bursts[i].second; ++t) {
                        process.history.push_back('B');
                        currentTime++;
                    }
                }
            }
            while (process.history.size() < currentTime) {
                process.history.push_back('W');
            }
        }
    }

    void simulatePriority() {
        sort(processes.begin(), processes.end(), comparePriority);

        int currentTime = 0;
        for (auto& process : processes) {
            if (currentTime < process.arrivalTime) {
                currentTime = process.arrivalTime;
            }

            for (int t = 0; t < process.arrivalTime - process.history.size();
                 ++t) {
                process.history.push_back('W');
            }

            for (int i = 0; i < process.bursts.size(); ++i) {
                for (int t = 0; t < process.bursts[i].first; ++t) {
                    process.history.push_back('C');
                    currentTime++;
                }
                if (i < process.bursts.size() - 1) {
                    for (int t = 0; t < process.bursts[i].second; ++t) {
                        process.history.push_back('B');
                        currentTime++;
                    }
                }
            }
            while (process.history.size() < currentTime) {
                process.history.push_back('W');
            }
        }
    }

    void simulateRoundRobin() {
        int currentTime = 0;
        int numProcesses = processes.size();
        vector<int> remainingTime(numProcesses, 0);
        vector<int> burstIndex(numProcesses, 0);
        vector<bool> completed(numProcesses, false);

        while (true) {
            bool allCompleted = true;

            for (int i = 0; i < numProcesses; ++i) {
                if (!completed[i]) {
                    allCompleted = false;

                    if (currentTime >= processes[i].arrivalTime) {
                        int currentBurstIndex = burstIndex[i];
                        int cpuBurst =
                            processes[i].bursts[currentBurstIndex].first;

                        if (remainingTime[i] == 0) {
                            if (cpuBurst <= quantum) {
                                for (int t = 0; t < cpuBurst; ++t) {
                                    processes[i].history.push_back('C');
                                    currentTime++;
                                }
                                remainingTime[i] = 0;
                                burstIndex[i]++;

                                if (burstIndex[i] ==
                                    processes[i].bursts.size()) {
                                    completed[i] = true;
                                }
                            } else {
                                for (int t = 0; t < quantum; ++t) {
                                    processes[i].history.push_back('C');
                                    currentTime++;
                                }
                                remainingTime[i] = cpuBurst - quantum;
                            }
                        } else {
                            if (remainingTime[i] <= quantum) {
                                for (int t = 0; t < remainingTime[i]; ++t) {
                                    processes[i].history.push_back('C');
                                    currentTime++;
                                }
                                remainingTime[i] = 0;
                                burstIndex[i]++;

                                if (burstIndex[i] ==
                                    processes[i].bursts.size()) {
                                    completed[i] = true;
                                }
                            } else {
                                for (int t = 0; t < quantum; ++t) {
                                    processes[i].history.push_back('C');
                                    currentTime++;
                                }
                                remainingTime[i] -= quantum;
                            }
                        }
                    } else {
                        processes[i].history.push_back('W');
                    }
                }
            }

            if (allCompleted) {
                break;
            }
        }
    }

    void displayProcessHistory() {
        for (const auto& process : processes) {
            cout << "Process " << process.id << ": ";
            for (char c : process.history) {
                cout << c;
            }
            cout << endl;
        }
    }
};

int main() {
    int quantum = 1;
    Scheduler scheduler(quantum);

    scheduler.loadProcesses("proc.dat");
    scheduler.simulateFCFS();
    scheduler.displayProcessHistory();

    return 0;
}
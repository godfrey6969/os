#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <map>

using namespace std;

struct Process {
    int id;
    int size;
    int request_time;
    int duration;
};

class BuddySystem {
    int total_memory;
    vector<int> memory_tree;

public:
    BuddySystem(int total_memory_size) {
        total_memory = total_memory_size;
        int tree_size = 2 * pow(2, ceil(log2(total_memory))) - 1;
        memory_tree.resize(tree_size, 0);
    }

    int allocate(int process_size) {
        int min_block_size = pow(2, ceil(log2(process_size)));
        return allocate_helper(0, total_memory, min_block_size);
    }

    void deallocate(int index) {
        deallocate_helper(index);
    }

    void print_memory() {
        cout << "Memory Tree Status (0: Free, 1: Allocated):" << endl;
        for (int i = 0; i < memory_tree.size(); ++i) {
            cout << memory_tree[i] << " ";
        }
        cout << endl;
    }

private:
    int allocate_helper(int node, int block_size, int required_size) {
        if (block_size < required_size || memory_tree[node] == 1) {
            return -1;
        }

        if (block_size == required_size) {
            if (memory_tree[node] == 0) {
                memory_tree[node] = 1;
                return node;
            } else {
                return -1;
            }
        }

        int left_child = 2 * node + 1;
        int right_child = 2 * node + 2;
        int half_block_size = block_size / 2;

        int result = allocate_helper(left_child, half_block_size, required_size);
        if (result == -1) {
            result = allocate_helper(right_child, half_block_size, required_size);
        }

        if (memory_tree[left_child] == 1 && memory_tree[right_child] == 1) {
            memory_tree[node] = 1;
        }

        return result;
    }

    void deallocate_helper(int node) {
        if (node < 0 || node >= memory_tree.size()) {
            return;
        }

        memory_tree[node] = 0;

        int parent = (node - 1) / 2;
        if (parent >= 0) {
            int left_child = 2 * parent + 1;
            int right_child = 2 * parent + 2;

            if (memory_tree[left_child] == 0 && memory_tree[right_child] == 0) {
                memory_tree[parent] = 0;
            }
        }
    }
};

void read_data_from_file(const string &filename, vector<Process> &processes) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file." << endl;
        return;
    }

    Process process;
    while (file >> process.id >> process.size >> process.request_time >> process.duration) {
        if (process.id < 0) break;
        processes.push_back(process);
    }

    file.close();
}

int main() {
    vector<Process> processes;
    read_data_from_file("buddy.dat", processes);

    int total_memory = 1024;
    BuddySystem buddy(total_memory);

    map<int, int> process_to_memory_index;

    for (const auto &process : processes) {
        cout << "Process " << process.id << " requesting " << process.size << " units at time " 
             << process.request_time << " for duration " << process.duration << endl;

        int memory_index = buddy.allocate(process.size);
        if (memory_index != -1) {
            cout << "Process " << process.id << " allocated at memory index " << memory_index << endl;
            process_to_memory_index[process.id] = memory_index;
        } else {
            cout << "Process " << process.id << " allocation failed." << endl;
        }

        buddy.print_memory();
    }

    return 0;
}

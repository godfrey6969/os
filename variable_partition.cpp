#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>


using namespace std;
struct Request {
    int time;
    int size;
    int duration;
};

struct Block {
    int start;
    int size;
    int endTime;
};

bool compareByTime(const Request &a, const Request &b) {
    return a.time < b.time;
}

void processRequests(vector<Request> &requests, int memorySize, const string &strategy) {
    vector<Block> memory = {{0, memorySize, 0}};
    queue<Request> requestQueue;
    int currentTime = 0;
    int successfulAllocations = 0;
    int totalRequests = 0;
    int lastAllocatedIndex = 0;

    auto allocateBlock = [&](Request &req) -> bool {
        if (strategy == "First Fit" || strategy == "Next Fit") {
            int startIndex = (strategy == "Next Fit") ? lastAllocatedIndex : 0;
            for (int i = startIndex; i < memory.size(); ++i) {
                if (memory[i].size >= req.size) {
                    memory[i].size -= req.size;
                    memory.push_back({memory[i].start, req.size, currentTime + req.duration});
                    memory[i].start += req.size;
                    lastAllocatedIndex = i;
                    return true;
                }
            }
            if (strategy == "Next Fit") {
                for (int i = 0; i < startIndex; ++i) {
                    if (memory[i].size >= req.size) {
                        memory[i].size -= req.size;
                        memory.push_back({memory[i].start, req.size, currentTime + req.duration});
                        memory[i].start += req.size;
                        lastAllocatedIndex = i;
                        return true;
                    }
                }
            }
        } else if (strategy == "Best Fit") {
            auto bestFit = min_element(memory.begin(), memory.end(), [&](Block &a, Block &b) {
                return (a.size >= req.size && a.size < b.size);
            });
            if (bestFit != memory.end() && bestFit->size >= req.size) {
                bestFit->size -= req.size;
                memory.push_back({bestFit->start, req.size, currentTime + req.duration});
                bestFit->start += req.size;
                return true;
            }
        } else if (strategy == "Worst Fit") {
            auto worstFit = max_element(memory.begin(), memory.end(), [&](Block &a, Block &b) {
                return a.size < b.size;
            });
            if (worstFit != memory.end() && worstFit->size >= req.size) {
                worstFit->size -= req.size;
                memory.push_back({worstFit->start, req.size, currentTime + req.duration});
                worstFit->start += req.size;
                return true;
            }
        }
        return false;
    };

    for (auto &req : requests) {
        currentTime = req.time;
        while (!requestQueue.empty() && requestQueue.front().time <= currentTime) {
            Request pendingReq = requestQueue.front();
            requestQueue.pop();
            if (allocateBlock(pendingReq)) {
                successfulAllocations++;
            }
        }
        if (!allocateBlock(req)) {
            requestQueue.push(req);
        } else {
            successfulAllocations++;
        }
        totalRequests++;
        if (totalRequests % 10 == 0) {
            int usedMemory = 0;
            for (auto &block : memory) {
                usedMemory += block.size;
            }
            int internalFragmentation = (memorySize - usedMemory) * 100 / memorySize;
            int externalFragmentation = 0;
            for (auto &block : memory) {
                if (block.size > 0) {
                    externalFragmentation += block.size;
                }
            }
            externalFragmentation = externalFragmentation * 100 / memorySize;
            cout << "After " << totalRequests << " requests:" << endl;
            cout << "Successful Allocations: " << (successfulAllocations * 100 / totalRequests) << "%" << endl;
            cout << "Internal Fragmentation: " << internalFragmentation << "%" << endl;
            cout << "External Fragmentation: " << externalFragmentation << "%" << endl;
        }
    }
}

int main() {
    ifstream infile("alloc.dat");
    if (!infile) {
        cerr << "Unable to open file alloc.dat";
        return 1;
    }

    int memorySize;
    infile >> memorySize;

    vector<Request> requests;
    int time, size, duration;
    while (infile >> time >> size >> duration) {
        if (time == -1 && size == -1 && duration == -1) break;
        requests.push_back({time, size, duration});
    }
    infile.close();

    sort(requests.begin(), requests.end(), compareByTime);
    cout << "First Fit Strategy:" << endl;
    processRequests(requests, memorySize, "First Fit");

    cout << "Best Fit Strategy:" << endl;
    processRequests(requests, memorySize, "Best Fit");

    cout << "Worst Fit Strategy:" << endl;
    processRequests(requests, memorySize, "Worst Fit");

    cout << "Next Fit Strategy:" << endl;
    processRequests(requests, memorySize, "Next Fit");

    return 0;
}
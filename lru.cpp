#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <list>

using namespace std;

int fifo(vector<int>& pages, int frameSize) {
    queue<int> frames;
    unordered_map<int, bool> inFrame;
    int pageFaults = 0;

    for (int page : pages) {
        if (inFrame[page] == false) {
            if (frames.size() == frameSize) {
                int oldPage = frames.front();
                frames.pop();
                inFrame[oldPage] = false;
            }
            frames.push(page);
            inFrame[page] = true;
            pageFaults++;
        }
    }
    return pageFaults;
}

int lru(vector<int>& pages, int frameSize) {
    vector<int> frames;
    unordered_map<int, int> pageMap;
    int pageFaults = 0;
    for (int page : pages) {
        auto it = pageMap.find(page);
        if (it == pageMap.end()) {
            if (frames.size() == frameSize) {
                int oldPage = frames.back();
                frames.pop_back();
                pageMap.erase(oldPage);
            }
            frames.insert(frames.begin(), page);
            pageMap[page] = 0;
            pageFaults++;
        } else {
            frames.erase(frames.begin() + it->second);
            frames.insert(frames.begin(), page);
            pageMap[page] = 0;
        }
        for (int i = 0; i < frames.size(); ++i) {
            pageMap[frames[i]] = i;
        }
    }
    return pageFaults;
}

int optimal(vector<int>& pages, int frameSize) {
    vector<int> frames;
    int pageFaults = 0;

    for (int i = 0; i < pages.size(); ++i) {
        int page = pages[i];
        if (find(frames.begin(), frames.end(), page) == frames.end()) {
            if (frames.size() == frameSize) {
                int farthest = i, index = -1;
                for (int j = 0; j < frames.size(); ++j) {
                    int k;
                    for (k = i + 1; k < pages.size(); ++k) {
                        if (frames[j] == pages[k]) {
                            if (k > farthest) {
                                farthest = k;
                                index = j;
                            }
                            break;
                        }
                    }
                    if (k == pages.size()) {
                        index = j;
                        break;
                    }
                }
                frames[index] = page;
            } else {
                frames.push_back(page);
            }
            pageFaults++;
        }
    }
    return pageFaults;
}

int main() {
    vector<int> pages = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2};
    int frameSize = 3;

    cout << "FIFO Page Faults: " << fifo(pages, frameSize) << endl;
    cout << "LRU Page Faults: " << lru(pages, frameSize) << endl;
    cout << "Optimal Page Faults: " << optimal(pages, frameSize) << endl;

    return 0;
}

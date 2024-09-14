#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

void readParamsAndReqs(const string& filename, int& cylinders, int& sectors,
                       int& bytesPerSector, int& rpm, double& avgSeekTime,
                       int& initialHeadPosition, vector<int>& requests) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        exit(EXIT_FAILURE);
    }

    file >> cylinders >> sectors >> bytesPerSector >> rpm >> avgSeekTime >>
        initialHeadPosition;

    int request;
    while (file >> request) {
        requests.push_back(request);
    }

    file.close();
}

double calcRotationalDelay(int rpm, int sectors) {
    double rotationTimePerMinute = 60.0 / rpm;
    double timePerSector = rotationTimePerMinute / sectors;
    return timePerSector;
}

double calcTotalSeekTimeFCFS(const vector<int>& requests,
                                int initialHeadPosition, double avgSeekTime) {
    double totalSeekTime = 0.0;
    int currentPosition = initialHeadPosition;
    for (int request : requests) {
        totalSeekTime += avgSeekTime * abs(currentPosition - request);
        currentPosition = request;
    }
    return totalSeekTime;
}
double calcTotalSeekTimeSSTF(const vector<int>& requests,
                             int initialHeadPosition, double avgSeekTime) {
    vector<int> requestList = requests;
    double totalSeekTime = 0.0;
    int currentPosition = initialHeadPosition;

    while (!requestList.empty()) {
        auto closest = min_element(requestList.begin(), requestList.end(),
                                [currentPosition](int a, int b) {
                                    return abs(a - currentPosition) <
                                            abs(b - currentPosition);
                                });
        totalSeekTime += avgSeekTime * abs(currentPosition - *closest);
        currentPosition = *closest;
        requestList.erase(closest);
    }
    return totalSeekTime;
}
double calcTotalSeekTimeLOOK(vector<int> requests, int initialHeadPosition,
                             double avgSeekTime) {
    sort(requests.begin(), requests.end());
    double totalSeekTime = 0.0;
    int currentPosition = initialHeadPosition;

    auto it =
        lower_bound(requests.begin(), requests.end(), initialHeadPosition);

    if (it != requests.end()) {
        while (it != requests.end()) {
            totalSeekTime += avgSeekTime * abs(currentPosition - *it);
            currentPosition = *it++;
        }
        it = requests.begin();
        if (currentPosition != initialHeadPosition) {
            totalSeekTime += avgSeekTime * abs(currentPosition - *it);
            currentPosition = *it++;
        }

        while (it != requests.end()) {
            totalSeekTime += avgSeekTime * abs(currentPosition - *it);
            currentPosition = *it++;
        }
    } else {
        it = requests.end() - 1;
        while (it != requests.begin() - 1) {
            totalSeekTime += avgSeekTime * abs(currentPosition - *it);
            currentPosition = *it--;
        }

        it = requests.end() - 1;
        if (currentPosition != initialHeadPosition) {
            totalSeekTime += avgSeekTime * abs(currentPosition - *it);
            currentPosition = *it--;
        }

        while (it != requests.begin() - 1) {
            totalSeekTime += avgSeekTime * abs(currentPosition - *it);
            currentPosition = *it--;
        }
    }

    return totalSeekTime;
}

double calcTotalSeekTimeCSCAN(const vector<int>& requests,
                              int initialHeadPosition, double avgSeekTime,
                              int cylinders) {
    vector<int> requestList = requests;
    sort(requestList.begin(), requestList.end());
    double totalSeekTime = 0.0;
    int currentPosition = initialHeadPosition;

    auto it = lower_bound(requestList.begin(), requestList.end(),
                          initialHeadPosition);

    if (it != requestList.end()) {
        while (it != requestList.end()) {
            totalSeekTime += avgSeekTime * abs(currentPosition - *it);
            currentPosition = *it++;
        }

        if (currentPosition != cylinders - 1) {
            totalSeekTime += avgSeekTime * (cylinders - 1 - currentPosition);
            currentPosition = 0;
        }

        it = requestList.begin();
        while (it != requestList.end() && *it < currentPosition) {
            totalSeekTime += avgSeekTime * abs(currentPosition - *it);
            currentPosition = *it++;
        }
    } else {
        it = requestList.begin();
        while (it != requestList.end()) {
            totalSeekTime += avgSeekTime * abs(currentPosition - *it);
            currentPosition = *it++;
        }

        if (currentPosition != cylinders - 1) {
            totalSeekTime += avgSeekTime * (cylinders - 1 - currentPosition);
            currentPosition = 0;
        }

        it = requestList.begin();
        while (it != requestList.end() && *it < currentPosition) {
            totalSeekTime += avgSeekTime * abs(currentPosition - *it);
            currentPosition = *it++;
        }
    }

    return totalSeekTime;
}

int main() {
    int cylinders, sectors, bytesPerSector, rpm, initialHeadPosition;
    double avgSeekTime;
    vector<int> requests;

    readParamsAndReqs("inp.dat", cylinders, sectors, bytesPerSector, rpm,
                      avgSeekTime, initialHeadPosition, requests);

    double rotationalDelay = calcRotationalDelay(rpm, sectors);

    cout << "Rotational Delay: " << rotationalDelay << " ms" << endl;

    double fcfsSeekTime =
        calcTotalSeekTimeFCFS(requests, initialHeadPosition, avgSeekTime);
    cout << "FCFS Total Seek Time: " << fcfsSeekTime << " ms" << endl;

    double sstfSeekTime =
        calcTotalSeekTimeSSTF(requests, initialHeadPosition, avgSeekTime);
    cout << "SSTF Total Seek Time: " << sstfSeekTime << " ms" << endl;

    double lookSeekTime =
        calcTotalSeekTimeLOOK(requests, initialHeadPosition, avgSeekTime);
    cout << "LOOK Total Seek Time: " << lookSeekTime << " ms" << endl;

    double cscanSeekTime = calcTotalSeekTimeCSCAN(requests, initialHeadPosition,
                                                  avgSeekTime, cylinders);
    cout << "C-SCAN Total Seek Time: " << cscanSeekTime << " ms" << endl;
    return 0;
}

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <vector>

using namespace std;

const int numPhilosophers = 5;
vector<mutex> forks(numPhilosophers);
vector<condition_variable> forkConditions(numPhilosophers);
vector<bool> forkAvailable(numPhilosophers, true);

void philosopher(int id) {
    while (true) {
        cout << "Philosopher " << id << " is thinking." << endl;
        this_thread::sleep_for(chrono::milliseconds(rand() % 1000));

        unique_lock<mutex> leftLock(forks[id]);
        forkConditions[id].wait(leftLock, [&] { return forkAvailable[id]; });
        forkAvailable[id] = false;

        unique_lock<mutex> rightLock(forks[(id + 1) % numPhilosophers]);
        forkConditions[(id + 1) % numPhilosophers].wait(rightLock, [&] { return forkAvailable[(id + 1) % numPhilosophers]; });
        forkAvailable[(id + 1) % numPhilosophers] = false;

        cout << "Philosopher " << id << " is eating." << endl;
        this_thread::sleep_for(chrono::milliseconds(rand() % 1000));

        forkAvailable[id] = true;
        forkAvailable[(id + 1) % numPhilosophers] = true;
        forkConditions[id].notify_one();
        forkConditions[(id + 1) % numPhilosophers].notify_one();
    }
}

int main() {
    thread philosophers[numPhilosophers];
    for (int i = 0; i < numPhilosophers; ++i) {
        philosophers[i] = thread(philosopher, i);
    }

    for (int i = 0; i < numPhilosophers; ++i) {
        philosophers[i].join();
    }

    return 0;
}
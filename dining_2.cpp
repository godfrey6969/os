#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

using namespace std;

const int numPhilosophers = 5;
mutex forks[numPhilosophers];

void philosopher(int id) {
    while (true) {
        cout << "Philosopher " << id << " is thinking." << endl;
        this_thread::sleep_for(chrono::milliseconds(rand() % 1000));

        if (id % 2 == 0) {
            forks[id].lock();
            forks[(id + 1) % numPhilosophers].lock();
        } else {
            forks[(id + 1) % numPhilosophers].lock();
            forks[id].lock();
        }

        cout << "Philosopher " << id << " is eating." << endl;
        this_thread::sleep_for(chrono::milliseconds(rand() % 1000));

        forks[id].unlock();
        forks[(id + 1) % numPhilosophers].unlock();
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
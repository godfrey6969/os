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

        int firstFork = min(id, (id + 1) % numPhilosophers);
        int secondFork = max(id, (id + 1) % numPhilosophers);

        forks[firstFork].lock();
        forks[secondFork].lock();

        cout << "Philosopher " << id << " is eating." << endl;
        this_thread::sleep_for(chrono::milliseconds(rand() % 1000));

        forks[firstFork].unlock();
        forks[secondFork].unlock();
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
#include <chrono>
#include <iostream>
#include <semaphore>
#include <thread>

using namespace std;

const int numPhilosophers = 5;
counting_semaphore<1> forks[numPhilosophers] = {
    counting_semaphore<1>(1), counting_semaphore<1>(1),
    counting_semaphore<1>(1), counting_semaphore<1>(1),
    counting_semaphore<1>(1)};

void philosopher(int id) {
    while (true) {
        cout << "Philosopher " << id << " is thinking." << endl;
        this_thread::sleep_for(chrono::milliseconds(rand() % 1000));

        forks[id].acquire();
        forks[(id + 1) % numPhilosophers].acquire();

        cout << "Philosopher " << id << " is eating." << endl;
        this_thread::sleep_for(chrono::milliseconds(rand() % 1000));

        forks[id].release();
        forks[(id + 1) % numPhilosophers].release();
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
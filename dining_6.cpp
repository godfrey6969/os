#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

const int numPhilosophers = 5;
mutex mtx;
condition_variable cv;
bool forks[numPhilosophers] = {true, true, true, true, true};

void philosopher(int id) {
    while (true) {
        cout << "Philosopher " << id << " is thinking." << endl;
        this_thread::sleep_for(chrono::milliseconds(rand() % 1000));

        unique_lock<mutex> lock(mtx);
        cv.wait(lock,
                [&] { return forks[id] && forks[(id + 1) % numPhilosophers]; });

        forks[id] = false;
        forks[(id + 1) % numPhilosophers] = false;

        cout << "Philosopher " << id << " is eating." << endl;
        this_thread::sleep_for(chrono::milliseconds(rand() % 1000));

        forks[id] = true;
        forks[(id + 1) % numPhilosophers] = true;
        cv.notify_all();
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
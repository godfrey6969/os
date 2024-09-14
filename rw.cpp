#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <semaphore>
#include <thread>

using namespace std;

counting_semaphore<1> rw_mutex(1);
counting_semaphore<1> read_count_mutex(1);
int read_count = 0;

void reader(int id) {
    while (true) {
        read_count_mutex.acquire();
        read_count++;
        if (read_count == 1) {
            rw_mutex.acquire();
        }
        read_count_mutex.release();

        cout << "Reader " << id << " is reading." << endl;
        this_thread::sleep_for(chrono::milliseconds(rand() % 100));

        read_count_mutex.acquire();
        read_count--;
        if (read_count == 0) {
            rw_mutex.release();
        }
        read_count_mutex.release();

        this_thread::sleep_for(chrono::milliseconds(rand() % 100));
    }
}

void writer(int id) {
    while (true) {
        rw_mutex.acquire();

        cout << "Writer " << id << " is writing." << endl;
        this_thread::sleep_for(chrono::milliseconds(rand() % 100));

        rw_mutex.release();

        this_thread::sleep_for(chrono::milliseconds(rand() % 100));
    }
}

int main() {
    thread readers[5], writers[2];

    for (int i = 0; i < 5; ++i) {
        readers[i] = thread(reader, i + 1);
    }

    for (int i = 0; i < 2; ++i) {
        writers[i] = thread(writer, i + 1);
    }

    for (int i = 0; i < 5; ++i) {
        readers[i].join();
    }

    for (int i = 0; i < 2; ++i) {
        writers[i].join();
    }

    return 0;
}
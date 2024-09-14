#include <iostream>
#include <thread>   
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <cstdlib>
#include <semaphore>

using namespace std;

int total_produced = 0, total_consume = 0;
const int maxBufferSize = 10;
int buffer[maxBufferSize];
int in = 0, out = 0;

counting_semaphore<maxBufferSize> emptySlots(maxBufferSize);
counting_semaphore<maxBufferSize> fullSlots(0);
mutex mtx;

void produce() {
    while (true) {
        this_thread::sleep_for(chrono::milliseconds(rand() % 100));
        int item = rand() % 100;

        emptySlots.acquire();
        {
            lock_guard<mutex> lock(mtx);
            buffer[in] = item;
            in = (in + 1) % maxBufferSize;
            cout << "Producer produces item " << item << endl;
            cout << "Total produced = " << ++total_produced
                 << " Total consume = " << total_consume << endl;
        }
        fullSlots.release();
    }
}

void consume() {
    while (true) {
        fullSlots.acquire();
        int item;
        {
            lock_guard<mutex> lock(mtx);
            item = buffer[out];
            out = (out + 1) % maxBufferSize;
            cout << "Consumer consumes item " << item << endl;
            cout << "Total produced = " << total_produced
                 << " Total consume = " << ++total_consume << endl;
        }
        emptySlots.release();
        this_thread::sleep_for(chrono::milliseconds(rand() % 100));
    }
}

int main() {
    thread producerThread(produce);
    thread consumerThread(consume);

    producerThread.join();
    consumerThread.join();

    return 0;
}
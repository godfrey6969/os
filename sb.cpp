#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <cstdlib>
#include <semaphore>

using namespace std;

const int numSeats = 5;
counting_semaphore<numSeats> waitingRoom(numSeats);
counting_semaphore<1> barberChair(1);
counting_semaphore<0> barberReady(0);
counting_semaphore<0> customerReady(0);
counting_semaphore<1> seatMutex(1);
int numFreeSeats = numSeats;

void barber() {
    while (true) {
        customerReady.acquire();
        barberChair.acquire();

        cout << "Barber is cutting hair." << endl;
        this_thread::sleep_for(chrono::milliseconds(rand() % 500 + 500));

        barberChair.release();
        barberReady.release();
    }
}

void customer(int id) {
    while (true) {
        seatMutex.acquire();
        if (numFreeSeats > 0) {
            numFreeSeats--;
            cout << "Customer " << id << " is waiting. Free seats: " << numFreeSeats << endl;
            waitingRoom.acquire();
            seatMutex.release();

            customerReady.release();
            barberChair.acquire();
            waitingRoom.release();
            numFreeSeats++;
            barberChair.release();

            barberReady.acquire();
            cout << "Customer " << id << " got a haircut." << endl;
            return;
        } else {
            cout << "Customer " << id << " left because there are no free seats." << endl;
            seatMutex.release();
            return;
        }
    }
}

int main() {
    thread barberThread(barber);

    thread customers[10];
    for (int i = 0; i < 10; ++i) {
        customers[i] = thread(customer, i + 1);
        this_thread::sleep_for(chrono::milliseconds(rand() % 500));
    }

    for (int i = 0; i < 10; ++i) {
        customers[i].join();
    }

    barberThread.join();

    return 0;
}
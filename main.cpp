#include <iostream>
#include <vector>
#include <thread>
#include "pqueue/pqueue.h"
#include <algorithm>
#include <deque>
#include "pqueue/pqueue_engine.h"

int main() {
    std::fstream file {};
    persistent_queue::Engine<int> engine {"storagefile.txt"};

    for (int i = 0; i < 138; ++i)
        engine.enqueue(i);

    std::cout << "Size: " << engine.size() << " Saved size: " << engine.saved_size() << '\n';

    for (int i = 0; i < 138; ++i) {
        auto elem = engine.dequeue();
        std::cout << *elem << '\n';
    }

    engine.save_metadata();
}

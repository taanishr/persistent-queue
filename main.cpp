#include <iostream>
#include "pqueue.h"

int main() {
    persistent_queue<int> pq {};

    for (int i = 0; i < 100; ++i) {
        pq.enqueue(i);
        std::cout << "Index: " << i << " Size: " << pq.size() << '\n';
    }

    for (int i = 0; i < 100; ++i) {
        int elem = pq.dequeue();
        if (i % 10 == 0) {
            std::cout << "Index: " << i << " Dequeued element: " << elem << " Size: " << pq.size() << '\n';
        }
    }

    // basic tests
    // pq.enqueue(4);
    // pq.enqueue(5);
    // pq.enqueue(7);
    // int elem = pq.dequeue();
    // std::cout << "Dequeued element: " << elem << " Size: " << pq.size() << '\n';
    // elem = pq.dequeue();
    // std::cout << "Dequeued element: " << elem << " Size: " << pq.size() << '\n';
    // elem = pq.dequeue();
    // std::cout << "Dequeued element: " << elem << " Size: " << pq.size() << '\n';
    // pq.enqueue(4);
    // std::cout << "Size: " << pq.size() << '\n';

    
}
#include <iostream>
#include <vector>
#include <thread>
#include "pqueue/pqueue.h"
#include <algorithm>
#include <deque>
#include "pqueue/pqueue_engine.h"

int main() {
    std::fstream file {};
    file.open("storagefile.txt", std::ios::in | std::ios::out | std::ios::app);
    persistent_queue::Engine<int> engine {file};
    for (int i = 0; i < 40; ++i)
        engine.enqueue(i);
    auto elem = engine.dequeue();
    std::cout << "Dequeued elem (memory buffer): " << *elem << '\n';
    for (int i = 40; i < 120; ++i)
        engine.enqueue(i);
//    elem = engine.dequeue();
//    std::cout << "Dequeued elem (file): " << *elem << '\n';

//    persistent_queue<int> pq {};
//
//    std::vector<std::thread> threads {};
//
//    for (int i = 0; i < 8; ++i) {
//        threads.emplace_back([&pq, i](){
//            pq.enqueue(i);
//        });
//    }
//
//    for (auto& thread: threads)
//        thread.join();
//
//    
//    std::vector<int> actualData(7);
//    std::copy(pq.data(), pq.data()+7, actualData.begin());
//    std::sort(actualData.begin(), actualData.end());


    // for (int i = 0; i < 100; ++i) {
    //     pq.enqueue(i);
    //     std::cout << "Index: " << i << " Size: " << pq.size() << '\n';
    // }

    // for (int i = 0; i < 100; ++i) {
    //     auto elem = pq.dequeue();
    //     if (i % 10 == 0) {
    //         std::cout << "Index: " << i << " Dequeued element: " << *elem << " Size: " << pq.size() << '\n';
    //     }
    // }


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

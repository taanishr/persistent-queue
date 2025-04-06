#include <iostream>
#include <vector>
#include <thread>
#include "pqueue/pqueue.h"
#include <algorithm>
#include <deque>

int main() {
    std::deque<int> dq {};
    dq.push_front(4);
    dq.push_front(5);
    dq.push_front(7);

    auto begin = dq.begin();

    std::cout << "Printing iterator from begin to end" << '\n';
    for (auto i = begin; i != dq.end(); ++i) {
        std::cout << *i << '\n';

    }

    std::cout << "Printing end iterator - 1" << '\n';

    auto end = dq.end();

    --end;

    std::cout << *end << '\n';

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

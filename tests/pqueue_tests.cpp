#include <catch2/catch_all.hpp>
#include "pqueue/pqueue.h"
#include <thread>
#include <vector>
#include <algorithm>

TEST_CASE("Enqueue") {
    persistent_queue<int> pq;

    SECTION("enqueue into empty queue") {
        int expectedData[] = {1};
        int expectedSize = 1;
        int expectedCapacity = 8;

        pq.enqueue(1);

        REQUIRE(pq.size() == expectedSize);
        REQUIRE(pq.capacity() == expectedCapacity);
        CHECK(std::equal(std::begin(expectedData), std::end(expectedData), pq.data()));
    }

    SECTION("enqueue into \"full capacity\" queue") {
        int expectedData[] = {1,2,3,4,5,6,7,8,9};
        int expectedSize = 9;
        int expectedCapacity = 16;

        for (int i = 1; i < 9; ++i)
            pq.enqueue(i);

        pq.enqueue(9);

        REQUIRE(pq.size() == expectedSize);
        REQUIRE(pq.capacity() == expectedCapacity);
        CHECK(std::equal(std::begin(expectedData), std::end(expectedData), pq.data()));

    }

    SECTION("enqueue after enqueuing and dequeueing queue to and from full capacity queue") {
        int expectedData[] = {1};
        int expectedSize = 1;
        int expectedCapacity = 8;

        for (int i = 0; i < 8; ++i)
            pq.enqueue(i);

        for (int i = 0; i < 8; ++i)
            pq.dequeue();

        pq.enqueue(1);

        REQUIRE(pq.size() == expectedSize);
        REQUIRE(pq.capacity() == expectedCapacity);
        CHECK(std::equal(std::begin(expectedData), std::end(expectedData), pq.data()));
    }

    SECTION("enqueue after enqueuing and dequeueing queue with expansion") {
        int expectedData[] = {8,9,10,11,12,13,14,15,16,1,2,3,4,5,6,7};
        int expectedSize = 16;
        int expectedCapacity = 16;

        for (int i = 0; i < 9; ++i)
            pq.enqueue(i);

        for (int i = 0; i < 9; ++i)
            pq.dequeue();

        for (int i = 1; i < 17; ++i) {
            pq.enqueue(i);
        }

        REQUIRE(pq.size() == expectedSize);
        REQUIRE(pq.capacity() == expectedCapacity);
        CHECK(std::equal(std::begin(expectedData), std::end(expectedData), pq.data()));
    }

    SECTION("enqueue with wrap around") {
        int expectedData[] = {8,1,2,3,4,5,6,7};
        int expectedSize = 8;
        int expectedCapacity = 8;

        pq.enqueue(1);
        pq.dequeue();

        for (int i = 1; i < 8; ++i)
            pq.enqueue(i);

        pq.enqueue(8);

        REQUIRE(pq.size() == expectedSize);
        REQUIRE(pq.capacity() == expectedCapacity);
        CHECK(std::equal(std::begin(expectedData), std::end(expectedData), pq.data()));
    }
    
    SECTION("enqueue 100 elements") {
        std::vector<int> expectedData;
        int expectedSize = 100;
        int expectedCapacity = 128;

        for (int i = 1; i < 101; ++i) {
            expectedData.push_back(i);
            pq.enqueue(i);
        }

        REQUIRE(pq.size() == expectedSize);
        REQUIRE(pq.capacity() == expectedCapacity);
        CHECK(std::equal(std::begin(expectedData), std::end(expectedData), pq.data()));
    }
    
    SECTION("enqueue in multi-threaded context") {
        std::vector<std::thread> threads;
        std::vector<int> expectedData {};
        int expectedSize = 8;
        int expectedCapacity = 8;

        for (int i = 1; i < 9; ++i) {
            expectedData.push_back(i);
        }

        for (int i = 1; i < 9; ++i) {
            threads.emplace_back([&pq, i](){
                pq.enqueue(i);
            });
        }

        for (auto& thread: threads)
            thread.join();

        REQUIRE(pq.size() == expectedSize);
        REQUIRE(pq.capacity() == expectedCapacity);

        std::vector<int> actualData(8);
        std::copy(pq.data(), pq.data()+8, actualData.begin());
        std::sort(actualData.begin(), actualData.end());

        CHECK(std::equal(expectedData.begin(), expectedData.end(), actualData.begin()));
    }
    
    SECTION("enqueue in multi-threaded context, with expansion") {
        std::vector<std::thread> threads;
        std::vector<int> expectedData {};
        int expectedSize = 16;
        int expectedCapacity = 16;

        for (int i = 1; i < 17; ++i) {
            expectedData.push_back(i);
        }

        for (int i = 1; i < 17; ++i) {
            threads.emplace_back([&pq, i](){
                pq.enqueue(i);
            });
        }

        for (auto& thread: threads)
            thread.join();

        REQUIRE(pq.size() == expectedSize);
        REQUIRE(pq.capacity() == expectedCapacity);
        
        std::vector<int> actualData (16);
        std::copy(pq.data(), pq.data()+16, actualData.begin());
        std::sort(actualData.begin(), actualData.end());

        CHECK(std::equal(expectedData.begin(), expectedData.end(), actualData.begin()));
    }
}

TEST_CASE("Dequeue") {
    persistent_queue<int> pq;

    SECTION("dequeue on empty queue") {
       auto elem = pq.dequeue();

       CHECK_FALSE(elem.has_value());
    }

    SECTION("dequeue single element") {
        int expectedData[] = {2,3,4,5,6,7,8};
        int expectedSize = 7;
        int expectedCapacity = 8;

        for (int i = 1; i < 9; ++i) {
            pq.enqueue(i);
        }
        
        auto elem = pq.dequeue();


        REQUIRE(*elem == 1);
        REQUIRE(pq.size() == expectedSize);
        REQUIRE(pq.capacity() == expectedCapacity);
        CHECK(std::equal(std::begin(expectedData), std::end(expectedData), pq.data()+1));
    }

    SECTION("dequeue entire queue") {
        std::vector<int> expectedData {};
        int expectedSize = 0;
        int expectedCapacity = 8;

        for (int i = 1; i < 9; ++i) {
            pq.enqueue(i);
        }

        for (int i = 1; i < 9; ++i) {
            auto elem = pq.dequeue();
            REQUIRE(*elem == i);
        }

        REQUIRE(pq.size() == expectedSize);
        REQUIRE(pq.capacity() == expectedCapacity);
        CHECK(std::equal(expectedData.begin(), expectedData.end(), pq.data()));
    }

    SECTION("dequeue when size = capacity + 1") {
        int expectedData[] = {2,3,4,5,6,7,8,9};
        int expectedSize = 8;
        int expectedCapacity = 16;

        for (int i = 1; i < 10; ++i) {
            pq.enqueue(i);
        }

        auto elem = pq.dequeue();

        REQUIRE(*elem == 1);
        REQUIRE(pq.size() == expectedSize);
        REQUIRE(pq.capacity() == expectedCapacity);
        CHECK(std::equal(std::begin(expectedData), std::end(expectedData), pq.data()+1));
    }

    SECTION("dequeue, multithreaded") {
        std::vector<std::thread> threads;
        std::vector<int> expectedDequeueds {1,2};
        std::vector<int> actualDequeueds {};

        std::vector<int> expectedData {3,4,5,6,7,8};
        int expectedSize = 6;
        int expectedCapacity = 8;

        for (int i = 1; i < 9; ++i) {
            pq.enqueue(i);
        }

        for (int i = 0; i < 2; ++i) {
            threads.emplace_back([&pq, &actualDequeueds](){
                auto elem = pq.dequeue();
                actualDequeueds.push_back(*elem);
            });
        }

        for (auto& thread: threads)
            thread.join();

        REQUIRE(pq.size() == expectedSize);
        REQUIRE(pq.capacity() == expectedCapacity);
        CHECK(std::equal(std::begin(expectedData), std::end(expectedData), pq.data()+2));
        std::sort(actualDequeueds.begin(), actualDequeueds.end());
        CHECK(std::equal(expectedDequeueds.begin(), expectedDequeueds.end(), actualDequeueds.begin()));
    }


}

TEST_CASE("Reserve") {
    persistent_queue<int> pq;

    SECTION("Reserve capacity less than existing capcaity") {
        std::vector<int> expectedData;
        int expectedSize = 16;
        int expectedCapacity = 16;

        for (int i = 1; i < 17; ++i) {
            expectedData.push_back(i);
            pq.enqueue(i);
        }
        
        pq.reserve(2); // should do nothing

        REQUIRE(pq.size() == expectedSize);
        REQUIRE(pq.capacity() == expectedCapacity);
        CHECK(std::equal(std::begin(expectedData), std::end(expectedData), pq.data()));
    }

    SECTION("Reserve capacity bigger than existing capcaity") {
        std::vector<int> expectedData;
        int expectedSize = 8;
        int expectedCapacity = 32;

        for (int i = 1; i < 9; ++i) {
            expectedData.push_back(i);
            pq.enqueue(i);
        }
        
        pq.reserve(32);

        REQUIRE(pq.size() == expectedSize);
        REQUIRE(pq.capacity() == expectedCapacity);
        CHECK(std::equal(std::begin(expectedData), std::end(expectedData), pq.data()));

    }
}

TEST_CASE("Resize") {
    persistent_queue<int> pq;

    SECTION("Resize bigger than existing size") {
        std::vector<int> expectedData;
        int expectedSize = 12;
        int expectedCapacity = 16;

        for (int i = 1; i < 10; ++i) {
            expectedData.push_back(i);
            pq.enqueue(i);
        }
        
        pq.resize(12);

        REQUIRE(pq.size() == expectedSize);
        REQUIRE(pq.capacity() == expectedCapacity);
        CHECK(std::equal(std::begin(expectedData), std::end(expectedData), pq.data()));
    }

    SECTION("Resize bigger than existing capacity") {
        std::vector<int> expectedData;
        int expectedSize = 24;
        int expectedCapacity = 24;

        for (int i = 1; i < 10; ++i) {
            expectedData.push_back(i);
            pq.enqueue(i);
        }
        
        pq.resize(24);

        REQUIRE(pq.size() == expectedSize);
        REQUIRE(pq.capacity() == expectedCapacity);
        CHECK(std::equal(std::begin(expectedData), std::end(expectedData), pq.data()));

    }

    SECTION("Resize less than existing capacity") {
        std::vector<int> expectedData;
        int expectedSize = 8;
        int expectedCapacity = 16;

        for (int i = 1; i < 8; ++i) {
            expectedData.push_back(i);
        }

        for (int i = 1; i < 10; ++i) {
            pq.enqueue(i);
        }
        
        pq.resize(8);

        REQUIRE(pq.size() == expectedSize);
        REQUIRE(pq.capacity() == expectedCapacity);
        CHECK(std::equal(std::begin(expectedData), std::end(expectedData), pq.data()));
    }
}
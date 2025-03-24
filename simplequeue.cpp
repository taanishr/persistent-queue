#include <iostream>

class Queue {
public:
    Queue(int capacity):
        cap{capacity},
        sz{0},
        elem{new int[capacity]},
        head{0},
        tail{0}
    {}

    ~Queue() {
        delete[] elem;
    }


    int getSize() { return sz; }

    void enqueue(int e) {
        if (sz >= cap)
            resize();
        *(elem+tail) = e;
        tail = (tail+1)%cap;
        sz++;
    }

    int dequeue() {
        if (sz == 0)
            return -1;
        int rt = *(elem+head);
        head = (head+1)%cap;
        sz--;
        return rt;
    }

private:
    void resize() {
        int* newElem = new int[2*cap];
        for (int i = head; i < head+cap; ++i) {
            newElem[i-head] = elem[i%cap];
        }
        head = 0;
        tail = head+cap;
        this->cap = 2*cap;
        this->elem = newElem;
    }

    int cap;
    int sz;
    int* elem;
    int head;
    int tail;
};

int main() {
    Queue q{3};

    q.enqueue(5);
    q.enqueue(4);
    q.enqueue(10);
    int dq = q.dequeue();
    q.enqueue(10);
    dq = q.dequeue();
    dq = q.dequeue();
    dq = q.dequeue();
    q.enqueue(7);
    q.enqueue(3);
    q.enqueue(2);
    q.enqueue(4);
    int sz = q.getSize();
    std::cout << "Size: " << sz << '\n';

    dq = q.dequeue();
    std::cout << "Dequeued elem: " << dq << '\n';

    sz = q.getSize();
    std::cout << "Size: " << sz << '\n';
}
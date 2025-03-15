
#include <iostream>

class Queue {
public:
Queue(int capacity):
    cap{capacity},
    sz{0},
    elem{new int[capacity]},
    head{elem},
    tail{elem}
{}

~Queue() {
    delete[] elem;
    head = nullptr;
    tail = nullptr;
}


int getSize() { return sz; }

void enqueue(int e) {
    if (sz >= cap)
        return;
    *tail = e;
    tail = tail+((tail - elem + 1) % cap);
    sz++;
}

int dequeue() {
    if (sz == 0)
        return -1;
    int rt = *head;
    head = head+((head-elem+1)%cap);
    sz--;
    return rt;
}

private:
    int cap;
    int sz;
    int* elem;
    int* head;
    int* tail;
};

int main() {
    Queue q{3};

    q.enqueue(5);
    q.enqueue(4);
    q.enqueue(10);
    int dq = q.dequeue();
    std::cout << dq << '\n';
    dq = q.dequeue();
    std::cout << dq << '\n';
    dq = q.dequeue();
    std::cout << dq << '\n';
    q.enqueue(14);
    q.enqueue(15);
    q.enqueue(16);
    dq = q.dequeue();
    std::cout << dq << '\n';
    dq = q.dequeue();
    std::cout << dq << '\n';
    dq = q.dequeue();
    std::cout << dq << '\n';
}
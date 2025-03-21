#include "queue.h"
#include <iostream>

Queue::Queue() {
    length_ = 0;
    head_ = nullptr;
    tail_ = nullptr;
}

Queue::~Queue() {
    // Clean up all items in the queue
    while (!isEmpty()) {
        Item* temp = deque();
        delete temp;
    }
}

void Queue::enque(Item* im) {
    if (isEmpty()) {
        head_ = im;
        tail_ = im;
    } else {
        tail_->next_ = im;
        tail_ = im;
    }
    im->next_ = nullptr;
    length_++;
}

Item* Queue::deque() {
    if (isEmpty()) {
        return nullptr;
    }
    
    Item* temp = head_;
    head_ = head_->next_;
    if (head_ == nullptr) {
        tail_ = nullptr;
    }
    length_--;
    return temp;
}

#include "queue.h"

Queue::Queue()
{
    head_ = tail_ = nullptr;
    length_ = 0;
}

void
Queue::enqueue(Item *im)
{
    // If queue is empty
    if (!tail_) {
        head_ = tail_ = im;
    } else {
        // Add to end of queue
        tail_->next_ = im;
        tail_ = im;
    }

    // Mark end of queue and increment length
    tail_->next_ = nullptr;
    ++length_;
}

Item*
Queue::dequeue()
{
    // If queue is empty
    if (!head_)
        return nullptr;

    // Remove from front of queue
    Item* it = head_;
    head_ = it->next_;
    
    // If queue is now empty
    if (it == tail_)
        head_ = tail_ = nullptr;
    
    // Decrement length
    --length_;

    return it;
}

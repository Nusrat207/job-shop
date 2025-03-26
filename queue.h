#ifndef SORUCE_CODE_QUEUE_H
#define SORUCE_CODE_QUEUE_H

typedef struct ItemType
{
    int id_;               // Item ID
    int jobId;             // Job type (1, 2, or 3)
    double itemArrivalTime;// Time when item arrived
    ItemType *next_;       // Next item in queue
} Item;                    // Used for storing event info

class Queue
{
public:
    Queue();
    
    // Get queue length
    inline int& length() {
        return (length_);
    }
    
    // Check if queue is empty
    inline bool empty() {
        return length_ == 0;
    }
    
    // Get queue size (alias for length)
    inline int size() {
        return length_;
    }
    
    // Add item to queue
    void enqueue(Item *im);
    
    // Remove item from queue
    Item* dequeue();

private:
    int length_;   // Current length of queue
    Item *head_;   // First item in queue
    Item *tail_;   // Last item in queue
};

#endif //SORUCE_CODE_QUEUE_H

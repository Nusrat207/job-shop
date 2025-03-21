#ifndef SORUCE_CODE_QUEUE_H
#define SORUCE_CODE_QUEUE_H


typedef struct ItemType
{
    int id_;
    double itemArrivalTime;
    int jobType;  // Type of job (1, 2, or 3)
    ItemType *next_;
} Item; // used for storing event info


class Queue
{
public:
    Queue ();
    ~Queue ();  // Destructor to clean up memory
    inline int& length ()
    {
        return (length_);
    }
    void enque (Item *im);
    Item* deque ();
    bool isEmpty () const { return length_ == 0; }
    Item* peek () const { return head_; }

private:
    int length_;
    Item *head_;
    Item *tail_;
};


#endif //SORUCE_CODE_QUEUE_H

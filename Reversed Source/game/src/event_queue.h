// Reconstructed from event_queue.c DWARF and code in the 2009 game binary.
#ifndef MCNCC_EVENT_QUEUE_H
#define MCNCC_EVENT_QUEUE_H

#include <queue>
#include <vector>

typedef struct SQueueElement
{
    void* pvData;
    int iKey;
    bool bCancel;
} TQueueElement;

class CEventQueue
{
public:
    struct FuncQueueComp
    {
        bool operator()(TQueueElement* left, TQueueElement* right) const
        {
            return left->iKey > right->iKey;
        }
    };

    CEventQueue();
    ~CEventQueue();
    TQueueElement* Enqueue(void* data, int key);
    TQueueElement* Dequeue();
    void Delete(TQueueElement* element);
    // Present in DWARF as a declaration; no emitted implementation found yet.
    void Requeue(TQueueElement* element, int key);
    int GetTopKey();
    int Size();

protected:
    void Destroy();

private:
    std::priority_queue<TQueueElement*, std::vector<TQueueElement*>, FuncQueueComp> m_pq_queue;
};

#endif

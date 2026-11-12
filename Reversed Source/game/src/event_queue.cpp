// Original compilation unit: event_queue.c (compiled as C++).
// Kept as .cpp to follow the supplied newer source tree.
#include <climits>
#include "event_queue.h"

// 0x0804e930 / 0x0804ea00: default construction of the standard priority queue.
CEventQueue::CEventQueue() {}

// 0x0804eaa0 / 0x0804eae0
CEventQueue::~CEventQueue()
{
    Destroy();
}

// 0x0804eb20
void CEventQueue::Destroy()
{
    while (!m_pq_queue.empty())
    {
        TQueueElement* element = m_pq_queue.top();
        m_pq_queue.pop();
        Delete(element);
    }
}

// 0x0804eb80: key is already absolute; there is no start-time member.
TQueueElement* CEventQueue::Enqueue(void* data, int key)
{
    TQueueElement* element = new TQueueElement;
    element->pvData = data;
    element->iKey = key;
    element->bCancel = false;
    m_pq_queue.push(element);
    return element;
}

// 0x0804ec10
TQueueElement* CEventQueue::Dequeue()
{
    if (m_pq_queue.empty())
        return 0;
    TQueueElement* element = m_pq_queue.top();
    m_pq_queue.pop();
    return element;
}

// 0x0804ec70: event payload ownership is handled outside this queue.
void CEventQueue::Delete(TQueueElement* element)
{
    delete element;
}

// 0x0804ec80
int CEventQueue::GetTopKey()
{
    return m_pq_queue.empty() ? INT_MAX : m_pq_queue.top()->iKey;
}

// 0x0804eca0
int CEventQueue::Size()
{
    return static_cast<int>(m_pq_queue.size());
}

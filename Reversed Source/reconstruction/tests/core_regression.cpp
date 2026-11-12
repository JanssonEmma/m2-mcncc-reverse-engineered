// Regression checks for reconstructed code; these do not execute the old server.
#include <cassert>
#include <climits>
#include <cstddef>
#include <string>
#include "../../game/src/FSM.h"
#include "../../game/src/event_queue.h"

class Machine : public CFSM
{
public:
    std::string trace;
    CStateTemplate<Machine> active;
    bool transitionOnBegin;
    Machine() : transitionOnBegin(false)
    {
        active.Set(this, &Machine::Begin, &Machine::Run, &Machine::End);
    }
    virtual void BeginStateInitial() { trace += 'b'; }
    virtual void StateInitial() { trace += 'i'; }
    virtual void EndStateInitial() { trace += 'e'; }
    void Begin()
    {
        trace += 'B';
        if (transitionOnBegin)
            GotoState(m_stateInitial);
    }
    void Run() { trace += 'R'; }
    void End() { trace += 'E'; }
};

static void CheckFSM()
{
    Machine machine;
    machine.Update();
    assert(machine.trace == "i"); // Initial begin callback is not automatic.
    machine.GotoState(machine.active);
    machine.GotoState(machine.active);
    assert(!machine.IsState(machine.active)); // Transition is deferred.
    machine.Update();
    assert(machine.trace == "ieBR");
    assert(machine.IsState(machine.active));
    machine.Update();
    assert(machine.trace == "ieBRR");
    machine.GotoState(machine.active); // A request for the current state restarts it.
    machine.Update();
    assert(machine.trace == "ieBRREBR");

    Machine deferred;
    deferred.transitionOnBegin = true;
    deferred.GotoState(deferred.active);
    deferred.Update();
    assert(deferred.trace == "eBR");
    assert(deferred.IsState(deferred.active));
    deferred.Update();
    assert(deferred.trace == "eBREbi"); // Begin's request waits for next Update.
}

static void CheckQueue()
{
    CEventQueue queue;
    int payload = 7;
    assert(queue.Size() == 0);
    assert(queue.Dequeue() == 0);
    assert(queue.GetTopKey() == INT_MAX);
    TQueueElement* later = queue.Enqueue(&payload, 100);
    TQueueElement* earlier = queue.Enqueue(&payload, -5);
    TQueueElement* sameKey = queue.Enqueue(&payload, 100);
    assert(!earlier->bCancel && earlier->pvData == &payload);
    earlier->bCancel = true;
    assert(queue.Size() == 3 && queue.GetTopKey() == -5);
    // The queue returns cancelled elements; the event processor handles them.
    assert(queue.Dequeue() == earlier);
    queue.Delete(earlier);
    TQueueElement* first = queue.Dequeue();
    TQueueElement* second = queue.Dequeue();
    assert(first != second);
    assert((first == later && second == sameKey) || (first == sameKey && second == later));
    assert(first->iKey == 100 && second->iKey == 100);
    queue.Delete(first);
    queue.Delete(second);
    assert(queue.Dequeue() == 0 && queue.GetTopKey() == INT_MAX);
    queue.Delete(0);
    queue.Enqueue(&payload, 9); // Destruction frees entries, not this stack payload.
}

int main()
{
    CheckFSM();
    CheckQueue();
#if defined(__i386__) && defined(__GNUC__)
    assert(sizeof(CState) == 4);
    assert(sizeof(CStateTemplate<CFSM>) == 32);
    assert(sizeof(CFSM) == 44);
    assert(sizeof(TQueueElement) == 12);
    assert(sizeof(CEventQueue) == 16);
    assert(offsetof(TQueueElement, pvData) == 0);
    assert(offsetof(TQueueElement, iKey) == 4);
    assert(offsetof(TQueueElement, bCancel) == 8);
#endif
    return 0;
}

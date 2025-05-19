#ifndef EVENT_H
#define EVENT_H

#include <queue>
#include <memory>
#include "process.h"

enum class EventType {
    PROCESS_ARRIVAL,
    CPU_BURST_COMPLETION,
    IO_BURST_COMPLETION,
    TIME_SLICE_EXPIRED,
    PROCESS_PREEMPTED
};

class Event {
private:
    EventType type;
    int time;
    std::shared_ptr<Process> process;

public:
    Event(EventType type, int time, std::shared_ptr<Process> process);
    
    EventType getType() const;
    int getTime() const;
    std::shared_ptr<Process> getProcess() const;
    
    std::string typeToString() const;
};

// Comparator for priority queue
struct EventComparator {
    bool operator()(const Event& e1, const Event& e2) const {
        // Sort by time, then by event type
        if (e1.getTime() != e2.getTime()) {
            return e1.getTime() > e2.getTime();
        }
        
        // For events at the same time, prioritize based on event type
        return static_cast<int>(e1.getType()) > static_cast<int>(e2.getType());
    }
};

class EventQueue {
private:
    std::priority_queue<Event, std::vector<Event>, EventComparator> events;

public:
    void addEvent(const Event& event);
    bool hasEvents() const;
    Event getNextEvent();
};

#endif // EVENT_H
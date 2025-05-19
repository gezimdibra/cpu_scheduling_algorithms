#include "event.h"

Event::Event(EventType type, int time, std::shared_ptr<Process> process)
    : type(type), time(time), process(process) {
}

EventType Event::getType() const {
    return type;
}

int Event::getTime() const {
    return time;
}

std::shared_ptr<Process> Event::getProcess() const {
    return process;
}

std::string Event::typeToString() const {
    switch (type) {
        case EventType::PROCESS_ARRIVAL: return "Process Arrival";
        case EventType::CPU_BURST_COMPLETION: return "CPU Burst Completion";
        case EventType::IO_BURST_COMPLETION: return "I/O Burst Completion";
        case EventType::TIME_SLICE_EXPIRED: return "Time Slice Expired";
        case EventType::PROCESS_PREEMPTED: return "Process Preempted";
        default: return "Unknown Event";
    }
}

void EventQueue::addEvent(const Event& event) {
    events.push(event);
}

bool EventQueue::hasEvents() const {
    return !events.empty();
}

Event EventQueue::getNextEvent() {
    Event event = events.top();
    events.pop();
    return event;
}
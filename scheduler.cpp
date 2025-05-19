#include "scheduler.h"
#include <algorithm>
#include <sstream>

Scheduler::Scheduler(const std::string& name, int processSwitchTime)
    : name(name), processSwitchTime(processSwitchTime), timeQuantum(0) {
}

std::string Scheduler::getName() const {
    return name;
}

int Scheduler::getProcessSwitchTime() const {
    return processSwitchTime;
}

// FCFS Scheduler Implementation
FCFSScheduler::FCFSScheduler(int processSwitchTime)
    : Scheduler("First Come First Serve", processSwitchTime) {
}

void FCFSScheduler::addProcess(std::shared_ptr<Process> process) {
    readyQueue.push(process);
}

std::shared_ptr<Process> FCFSScheduler::getNextProcess() {
    if (readyQueue.empty()) {
        return nullptr;
    }
    
    std::shared_ptr<Process> next = readyQueue.front();
    readyQueue.pop();
    return next;
}

bool FCFSScheduler::hasProcesses() const {
    return !readyQueue.empty();
}

bool FCFSScheduler::shouldPreempt(std::shared_ptr<Process> runningProcess, 
                                 std::shared_ptr<Process> newProcess, int currentTime) {
    // FCFS is non-preemptive
    return false;
}

int FCFSScheduler::getTimeSlice() const {
    // FCFS doesn't use time slices
    return -1;
}

// SJF Scheduler Implementation
SJFScheduler::SJFScheduler(int processSwitchTime)
    : Scheduler("Shortest Job First", processSwitchTime) {
}

void SJFScheduler::addProcess(std::shared_ptr<Process> process) {
    readyQueue.push(process);
}

std::shared_ptr<Process> SJFScheduler::getNextProcess() {
    if (readyQueue.empty()) {
        return nullptr;
    }
    
    std::shared_ptr<Process> next = readyQueue.top();
    readyQueue.pop();
    return next;
}

bool SJFScheduler::hasProcesses() const {
    return !readyQueue.empty();
}

bool SJFScheduler::shouldPreempt(std::shared_ptr<Process> runningProcess, 
                                std::shared_ptr<Process> newProcess, int currentTime) {
    // SJF is non-preemptive
    return false;
}

int SJFScheduler::getTimeSlice() const {
    // SJF doesn't use time slices
    return -1;
}

// SRTN Scheduler Implementation
SRTNScheduler::SRTNScheduler(int processSwitchTime)
    : Scheduler("Shortest Remaining Time Next", processSwitchTime) {
}

void SRTNScheduler::addProcess(std::shared_ptr<Process> process) {
    readyQueue.push(process);
}

std::shared_ptr<Process> SRTNScheduler::getNextProcess() {
    if (readyQueue.empty()) {
        return nullptr;
    }
    
    std::shared_ptr<Process> next = readyQueue.top();
    readyQueue.pop();
    return next;
}

bool SRTNScheduler::hasProcesses() const {
    return !readyQueue.empty();
}

bool SRTNScheduler::shouldPreempt(std::shared_ptr<Process> runningProcess, 
                                 std::shared_ptr<Process> newProcess, int currentTime) {
    // Preempt if the new process has a shorter remaining time
    return newProcess->getRemainingTimeInBurst() < runningProcess->getRemainingTimeInBurst();
}

int SRTNScheduler::getTimeSlice() const {
    // SRTN doesn't use time slices
    return -1;
}

// RR Scheduler Implementation
RRScheduler::RRScheduler(int processSwitchTime, int timeQuantum)
    : Scheduler("Round Robin", processSwitchTime) {
    this->timeQuantum = timeQuantum;
    
    // Add time quantum to the name for clarity
    std::stringstream ss;
    ss << name << " (quantum=" << timeQuantum << ")";
    name = ss.str();
}

void RRScheduler::addProcess(std::shared_ptr<Process> process) {
    readyQueue.push(process);
}

std::shared_ptr<Process> RRScheduler::getNextProcess() {
    if (readyQueue.empty()) {
        return nullptr;
    }
    
    std::shared_ptr<Process> next = readyQueue.front();
    readyQueue.pop();
    return next;
}

bool RRScheduler::hasProcesses() const {
    return !readyQueue.empty();
}

bool RRScheduler::shouldPreempt(std::shared_ptr<Process> runningProcess, 
                               std::shared_ptr<Process> newProcess, int currentTime) {
    // Round Robin preempts based on time slice, not on process arrival
    return false;
}

int RRScheduler::getTimeSlice() const {
    return timeQuantum;
}

// Factory function implementation
std::shared_ptr<Scheduler> createScheduler(const std::string& algorithm, int processSwitchTime) {
    if (algorithm == "FCFS") {
        return std::make_shared<FCFSScheduler>(processSwitchTime);
    } else if (algorithm == "SJF") {
        return std::make_shared<SJFScheduler>(processSwitchTime);
    } else if (algorithm == "SRTN") {
        return std::make_shared<SRTNScheduler>(processSwitchTime);
    } else if (algorithm == "RR10") {
        return std::make_shared<RRScheduler>(processSwitchTime, 10);
    } else if (algorithm == "RR50") {
        return std::make_shared<RRScheduler>(processSwitchTime, 50);
    } else if (algorithm == "RR100") {
        return std::make_shared<RRScheduler>(processSwitchTime, 100);
    } else {
        // Default to FCFS
        return std::make_shared<FCFSScheduler>(processSwitchTime);
    }
}
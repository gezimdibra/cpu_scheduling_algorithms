#include "process.h"

Process::Process(int id, int arrivalTime, const std::vector<int>& cpuBursts, 
                const std::vector<int>& ioBursts)
    : id(id), arrivalTime(arrivalTime), cpuBursts(cpuBursts), ioBursts(ioBursts),
      currentBurst(0), state(ProcessState::NEW), startTime(-1), finishTime(-1),
      serviceTime(0), ioTime(0), waitTime(0) {
    
    // Calculate total CPU and I/O time
    totalCpuTime = 0;
    for (int burst : cpuBursts) {
        totalCpuTime += burst;
    }
    
    totalIoTime = 0;
    for (int burst : ioBursts) {
        totalIoTime += burst;
    }
    
    // Initialize remaining time for first burst
    if (!cpuBursts.empty()) {
        remainingTimeInBurst = cpuBursts[0];
    } else {
        remainingTimeInBurst = 0;
    }
}

int Process::getId() const {
    return id;
}

int Process::getArrivalTime() const {
    return arrivalTime;
}

ProcessState Process::getState() const {
    return state;
}

void Process::setState(ProcessState newState) {
    state = newState;
}

int Process::getCurrentBurst() const {
    return currentBurst;
}

int Process::getRemainingTimeInBurst() const {
    return remainingTimeInBurst;
}

void Process::setRemainingTimeInBurst(int time) {
    remainingTimeInBurst = time;
}

void Process::decrementRemainingTime(int time) {
    remainingTimeInBurst -= time;
    if (remainingTimeInBurst < 0) {
        remainingTimeInBurst = 0;
    }
}

bool Process::hasMoreBursts() const {
    // Check if we have more CPU or I/O bursts
    return (isIoBurst() && currentBurst < ioBursts.size()) || 
           (!isIoBurst() && currentBurst < cpuBursts.size());
}

bool Process::isIoBurst() const {
    // Even numbers are CPU bursts, odd numbers are I/O bursts
    return currentBurst % 2 == 1;
}

void Process::moveToNextBurst() {
    currentBurst++;
    if (isIoBurst() && currentBurst < ioBursts.size()) {
        remainingTimeInBurst = ioBursts[currentBurst / 2];
    } else if (!isIoBurst() && currentBurst < cpuBursts.size()) {
        remainingTimeInBurst = cpuBursts[currentBurst / 2];
    } else {
        remainingTimeInBurst = 0;
    }
}

void Process::setStartTime(int time) {
    if (startTime == -1) {
        startTime = time;
    }
}

void Process::setFinishTime(int time) {
    finishTime = time;
}

void Process::addServiceTime(int time) {
    serviceTime += time;
}

void Process::addIoTime(int time) {
    ioTime += time;
}

void Process::addWaitTime(int time) {
    waitTime += time;
}

int Process::getStartTime() const {
    return startTime;
}

int Process::getFinishTime() const {
    return finishTime;
}

int Process::getServiceTime() const {
    return serviceTime;
}

int Process::getIoTime() const {
    return ioTime;
}

int Process::getWaitTime() const {
    return waitTime;
}

int Process::getTotalCpuTime() const {
    return totalCpuTime;
}

int Process::getTotalIoTime() const {
    return totalIoTime;
}

int Process::getTurnaroundTime() const {
    if (finishTime == -1) {
        return -1; // Process hasn't finished yet
    }
    return finishTime - arrivalTime;
}

std::string Process::stateToString() const {
    switch (state) {
        case ProcessState::NEW: return "new";
        case ProcessState::READY: return "ready";
        case ProcessState::RUNNING: return "running";
        case ProcessState::BLOCKED: return "blocked";
        case ProcessState::TERMINATED: return "terminated";
        default: return "unknown";
    }
}
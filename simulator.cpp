#include "simulator.h"
#include <iostream>
#include <algorithm>

Simulator::Simulator(bool verboseMode, bool detailedMode, std::shared_ptr<Scheduler> scheduler)
    : verboseMode(verboseMode), detailedMode(detailedMode), currentTime(0), 
      scheduler(scheduler), runningProcess(nullptr) {
}

void Simulator::addProcess(std::shared_ptr<Process> process) {
    processes.push_back(process);
    stats.addProcess(process);
    
    // Schedule process arrival event
    eventQueue.addEvent(Event(EventType::PROCESS_ARRIVAL, process->getArrivalTime(), process));
}

void Simulator::run() {
    while (eventQueue.hasEvents()) {
        Event event = eventQueue.getNextEvent();
        currentTime = event.getTime();
        processEvent(event);
    }
    
    // Set final statistics
    stats.setTotalTime(currentTime);
}

void Simulator::processEvent(const Event& event) {
    std::shared_ptr<Process> process = event.getProcess();
    
    switch (event.getType()) {
        case EventType::PROCESS_ARRIVAL:
            handleProcessArrival(process);
            break;
            
        case EventType::CPU_BURST_COMPLETION:
            handleCpuBurstCompletion(process);
            break;
            
        case EventType::IO_BURST_COMPLETION:
            handleIoBurstCompletion(process);
            break;
            
        case EventType::TIME_SLICE_EXPIRED:
            handleTimeSliceExpired(process);
            break;
            
        case EventType::PROCESS_PREEMPTED:
            // Process was preempted, move to ready queue
            ProcessState oldState = process->getState();
            process->setState(ProcessState::READY);
            if (verboseMode) {
                printStateTransition(process, oldState, ProcessState::READY);
            }
            scheduler->addProcess(process);
            runningProcess = nullptr;
            scheduleNextProcess();
            break;
    }
}

void Simulator::handleProcessArrival(std::shared_ptr<Process> process) {
    // Change process state to ready
    ProcessState oldState = process->getState();
    process->setState(ProcessState::READY);
    if (verboseMode) {
        printStateTransition(process, oldState, ProcessState::READY);
    }
    
    // If we have a preemptive scheduler, check if we should preempt the current process
    if (runningProcess && scheduler->shouldPreempt(runningProcess, process, currentTime)) {
        // Schedule preemption event
        eventQueue.addEvent(Event(EventType::PROCESS_PREEMPTED, currentTime, runningProcess));
    }
    
    // Add process to scheduler
    scheduler->addProcess(process);
    
    // If no process is running, schedule the next one
    if (!runningProcess) {
        scheduleNextProcess();
    }
}

void Simulator::handleCpuBurstCompletion(std::shared_ptr<Process> process) {
    // Add CPU time to statistics
    int burstTime = process->getRemainingTimeInBurst();
    stats.addCpuBusyTime(burstTime);
    process->addServiceTime(burstTime);
    
    // Move to next burst, which should be an I/O burst
    process->moveToNextBurst();
    
    // If process has more bursts, move to I/O; otherwise, terminate
    if (process->hasMoreBursts()) {
        ProcessState oldState = process->getState();
        process->setState(ProcessState::BLOCKED);
        if (verboseMode) {
            printStateTransition(process, oldState, ProcessState::BLOCKED);
        }
        
        // Add process to blocked queue and schedule I/O completion
        blockedProcesses[process->getId()] = process;
        int ioCompletionTime = currentTime + process->getRemainingTimeInBurst();
        eventQueue.addEvent(Event(EventType::IO_BURST_COMPLETION, ioCompletionTime, process));
    } else {
        // Process has completed all bursts, terminate
        ProcessState oldState = process->getState();
        process->setState(ProcessState::TERMINATED);
        if (verboseMode) {
            printStateTransition(process, oldState, ProcessState::TERMINATED);
        }
        process->setFinishTime(currentTime);
    }
    
    // No process is running now
    runningProcess = nullptr;
    
    // Schedule next process after process switch overhead
    currentTime += scheduler->getProcessSwitchTime();
    stats.addProcessSwitchTime(scheduler->getProcessSwitchTime());
    scheduleNextProcess();
}

void Simulator::handleIoBurstCompletion(std::shared_ptr<Process> process) {
    // Add I/O time to statistics
    int burstTime = process->getRemainingTimeInBurst();
    process->addIoTime(burstTime);
    
    // Remove process from blocked queue
    blockedProcesses.erase(process->getId());
    
    // Move to next burst, which should be a CPU burst
    process->moveToNextBurst();
    
    // Change process state to ready
    ProcessState oldState = process->getState();
    process->setState(ProcessState::READY);
    if (verboseMode) {
        printStateTransition(process, oldState, ProcessState::READY);
    }
    
    // If we have a preemptive scheduler, check if we should preempt the current process
    if (runningProcess && scheduler->shouldPreempt(runningProcess, process, currentTime)) {
        // Schedule preemption event
        eventQueue.addEvent(Event(EventType::PROCESS_PREEMPTED, currentTime, runningProcess));
    }
    
    // Add process to scheduler
    scheduler->addProcess(process);
    
    // If no process is running, schedule the next one
    if (!runningProcess) {
        scheduleNextProcess();
    }
}

void Simulator::handleTimeSliceExpired(std::shared_ptr<Process> process) {
    // Process time slice has expired, move back to ready queue
    ProcessState oldState = process->getState();
    process->setState(ProcessState::READY);
    if (verboseMode) {
        printStateTransition(process, oldState, ProcessState::READY);
    }
    
    // Add process back to scheduler
    scheduler->addProcess(process);
    
    // No process is running now
    runningProcess = nullptr;
    
    // Schedule next process after process switch overhead
    currentTime += scheduler->getProcessSwitchTime();
    stats.addProcessSwitchTime(scheduler->getProcessSwitchTime());
    scheduleNextProcess();
}

void Simulator::scheduleNextProcess() {
    // If there's already a running process, do nothing
    if (runningProcess) {
        return;
    }
    
    // Get next process from scheduler
    if (scheduler->hasProcesses()) {
        std::shared_ptr<Process> nextProcess = scheduler->getNextProcess();
        
        if (nextProcess) {
            runningProcess = nextProcess;
            
            // Change process state to running
            ProcessState oldState = runningProcess->getState();
            runningProcess->setState(ProcessState::RUNNING);
            if (verboseMode) {
                printStateTransition(runningProcess, oldState, ProcessState::RUNNING);
            }
            
            // Set start time if this is the first time the process runs
            runningProcess->setStartTime(currentTime);
            
            // Schedule either burst completion or time slice expired
            int timeSlice = scheduler->getTimeSlice();
            int remainingTime = runningProcess->getRemainingTimeInBurst();
            
            if (timeSlice > 0 && remainingTime > timeSlice) {
                // Round Robin: schedule time slice expiration
                eventQueue.addEvent(Event(EventType::TIME_SLICE_EXPIRED, 
                                        currentTime + timeSlice, runningProcess));
                runningProcess->decrementRemainingTime(timeSlice);
            } else {
                // Schedule CPU burst completion
                eventQueue.addEvent(Event(EventType::CPU_BURST_COMPLETION, 
                                        currentTime + remainingTime, runningProcess));
                runningProcess->setRemainingTimeInBurst(0);
            }
        }
    }
}

void Simulator::printStateTransition(std::shared_ptr<Process> process, 
                                    ProcessState oldState, ProcessState newState) {
    std::cout << "At time " << currentTime << ": Process " << process->getId() 
              << " moves from " << process->stateToString() << " to " 
              << process->stateToString() << "\n";
}

Statistics Simulator::getStatistics() const {
    return stats;
}
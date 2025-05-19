#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <memory>
#include <vector>
#include <string>
#include <map>
#include "process.h"
#include "event.h"
#include "scheduler.h"
#include "statistics.h"

class Simulator {
private:
    bool verboseMode;
    bool detailedMode;
    int currentTime;
    EventQueue eventQueue;
    std::shared_ptr<Scheduler> scheduler;
    std::shared_ptr<Process> runningProcess;
    std::vector<std::shared_ptr<Process>> processes;
    std::map<int, std::shared_ptr<Process>> blockedProcesses;
    Statistics stats;
    
    // Helper methods
    void processEvent(const Event& event);
    void handleProcessArrival(std::shared_ptr<Process> process);
    void handleCpuBurstCompletion(std::shared_ptr<Process> process);
    void handleIoBurstCompletion(std::shared_ptr<Process> process);
    void handleTimeSliceExpired(std::shared_ptr<Process> process);
    void scheduleNextProcess();
    void printStateTransition(std::shared_ptr<Process> process, 
                             ProcessState oldState, ProcessState newState);
    
public:
    Simulator(bool verboseMode, bool detailedMode, std::shared_ptr<Scheduler> scheduler);
    
    void addProcess(std::shared_ptr<Process> process);
    void run();
    Statistics getStatistics() const;
};

#endif // SIMULATOR_H
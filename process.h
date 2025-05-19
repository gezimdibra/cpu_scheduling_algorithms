#ifndef PROCESS_H
#define PROCESS_H

#include <vector>
#include <string>

enum class ProcessState {
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
};

class Process {
private:
    int id;
    int arrivalTime;
    std::vector<int> cpuBursts;
    std::vector<int> ioBursts;
    int currentBurst;
    int remainingTimeInBurst;
    ProcessState state;
    
    // Statistics
    int startTime;      // When process first entered CPU
    int finishTime;     // When process terminated
    int serviceTime;    // Total CPU time
    int ioTime;         // Total I/O time
    int waitTime;       // Total time in ready queue
    int totalCpuTime;   // Sum of all CPU bursts
    int totalIoTime;    // Sum of all I/O bursts

public:
    Process(int id, int arrivalTime, const std::vector<int>& cpuBursts, 
            const std::vector<int>& ioBursts);
    
    int getId() const;
    int getArrivalTime() const;
    ProcessState getState() const;
    void setState(ProcessState newState);
    int getCurrentBurst() const;
    int getRemainingTimeInBurst() const;
    void setRemainingTimeInBurst(int time);
    void decrementRemainingTime(int time);
    bool hasMoreBursts() const;
    bool isIoBurst() const;
    void moveToNextBurst();
    
    // Statistics methods
    void setStartTime(int time);
    void setFinishTime(int time);
    void addServiceTime(int time);
    void addIoTime(int time);
    void addWaitTime(int time);
    
    int getStartTime() const;
    int getFinishTime() const;
    int getServiceTime() const;
    int getIoTime() const;
    int getWaitTime() const;
    int getTotalCpuTime() const;
    int getTotalIoTime() const;
    int getTurnaroundTime() const;
    
    std::string stateToString() const;
};

#endif // PROCESS_H
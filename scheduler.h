#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <memory>
#include <string>
#include <vector>
#include <queue>
#include "process.h"

class Scheduler {
protected:
    std::string name;
    int processSwitchTime;
    int timeQuantum; // Used for Round Robin
    
public:
    Scheduler(const std::string& name, int processSwitchTime);
    virtual ~Scheduler() = default;
    
    std::string getName() const;
    int getProcessSwitchTime() const;
    
    // Pure virtual methods to be implemented by derived classes
    virtual void addProcess(std::shared_ptr<Process> process) = 0;
    virtual std::shared_ptr<Process> getNextProcess() = 0;
    virtual bool hasProcesses() const = 0;
    virtual bool shouldPreempt(std::shared_ptr<Process> runningProcess, 
                              std::shared_ptr<Process> newProcess, int currentTime) = 0;
    virtual int getTimeSlice() const = 0;
};

class FCFSScheduler : public Scheduler {
private:
    std::queue<std::shared_ptr<Process>> readyQueue;
    
public:
    FCFSScheduler(int processSwitchTime);
    
    void addProcess(std::shared_ptr<Process> process) override;
    std::shared_ptr<Process> getNextProcess() override;
    bool hasProcesses() const override;
    bool shouldPreempt(std::shared_ptr<Process> runningProcess, 
                       std::shared_ptr<Process> newProcess, int currentTime) override;
    int getTimeSlice() const override;
};

class SJFScheduler : public Scheduler {
private:
    // Custom comparator for the priority queue
    struct ProcessComparator {
        bool operator()(const std::shared_ptr<Process>& p1, 
                        const std::shared_ptr<Process>& p2) const {
            return p1->getRemainingTimeInBurst() > p2->getRemainingTimeInBurst();
        }
    };
    
    std::priority_queue<std::shared_ptr<Process>, 
                        std::vector<std::shared_ptr<Process>>, 
                        ProcessComparator> readyQueue;
    
public:
    SJFScheduler(int processSwitchTime);
    
    void addProcess(std::shared_ptr<Process> process) override;
    std::shared_ptr<Process> getNextProcess() override;
    bool hasProcesses() const override;
    bool shouldPreempt(std::shared_ptr<Process> runningProcess, 
                       std::shared_ptr<Process> newProcess, int currentTime) override;
    int getTimeSlice() const override;
};

class SRTNScheduler : public Scheduler {
private:
    // Custom comparator for the priority queue
    struct ProcessComparator {
        bool operator()(const std::shared_ptr<Process>& p1, 
                        const std::shared_ptr<Process>& p2) const {
            return p1->getRemainingTimeInBurst() > p2->getRemainingTimeInBurst();
        }
    };
    
    std::priority_queue<std::shared_ptr<Process>, 
                        std::vector<std::shared_ptr<Process>>, 
                        ProcessComparator> readyQueue;
    
public:
    SRTNScheduler(int processSwitchTime);
    
    void addProcess(std::shared_ptr<Process> process) override;
    std::shared_ptr<Process> getNextProcess() override;
    bool hasProcesses() const override;
    bool shouldPreempt(std::shared_ptr<Process> runningProcess, 
                       std::shared_ptr<Process> newProcess, int currentTime) override;
    int getTimeSlice() const override;
};

class RRScheduler : public Scheduler {
private:
    std::queue<std::shared_ptr<Process>> readyQueue;
    
public:
    RRScheduler(int processSwitchTime, int timeQuantum);
    
    void addProcess(std::shared_ptr<Process> process) override;
    std::shared_ptr<Process> getNextProcess() override;
    bool hasProcesses() const override;
    bool shouldPreempt(std::shared_ptr<Process> runningProcess, 
                       std::shared_ptr<Process> newProcess, int currentTime) override;
    int getTimeSlice() const override;
};

// Factory function to create schedulers based on algorithm name
std::shared_ptr<Scheduler> createScheduler(const std::string& algorithm, int processSwitchTime);

#endif // SCHEDULER_H
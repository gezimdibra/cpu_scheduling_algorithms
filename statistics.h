#ifndef STATISTICS_H
#define STATISTICS_H

#include <memory>
#include <vector>
#include <map>
#include <string>
#include "process.h"

class Statistics {
private:
    int totalTime;
    int cpuBusyTime;
    int processSwitchTime;
    std::vector<std::shared_ptr<Process>> processes;
    
public:
    Statistics();
    
    void setTotalTime(int time);
    void addCpuBusyTime(int time);
    void addProcessSwitchTime(int time);
    void addProcess(std::shared_ptr<Process> process);
    
    int getTotalTime() const;
    double getCpuUtilization() const;
    std::vector<std::shared_ptr<Process>> getProcesses() const;
    
    // Methods for different output formats
    void printDefaultOutput(const std::string& algorithm) const;
    void printDetailedOutput(const std::string& algorithm) const;
};

#endif // STATISTICS_H
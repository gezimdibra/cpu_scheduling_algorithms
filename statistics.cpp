#include "statistics.h"
#include <iostream>
#include <iomanip>
#include <cmath>

Statistics::Statistics()
    : totalTime(0), cpuBusyTime(0), processSwitchTime(0) {
}

void Statistics::setTotalTime(int time) {
    totalTime = time;
}

void Statistics::addCpuBusyTime(int time) {
    cpuBusyTime += time;
}

void Statistics::addProcessSwitchTime(int time) {
    processSwitchTime += time;
}

void Statistics::addProcess(std::shared_ptr<Process> process) {
    processes.push_back(process);
}

int Statistics::getTotalTime() const {
    return totalTime;
}

double Statistics::getCpuUtilization() const {
    if (totalTime <= 0) {
        return 0.0;
    }
    
    return static_cast<double>(cpuBusyTime) / totalTime * 100.0;
}

std::vector<std::shared_ptr<Process>> Statistics::getProcesses() const {
    return processes;
}

void Statistics::printDefaultOutput(const std::string& algorithm) const {
    std::cout << algorithm << ":\n";
    std::cout << "Total Time required is " << totalTime << " time units\n";
    std::cout << "CPU Utilization is " << static_cast<int>(std::round(getCpuUtilization())) << "%\n";
}

void Statistics::printDetailedOutput(const std::string& algorithm) const {
    std::cout << algorithm << ":\n";
    std::cout << "Total Time required is " << totalTime << " units\n";
    std::cout << "CPU Utilization is " << static_cast<int>(std::round(getCpuUtilization())) << "%\n";
    
    for (const auto& process : processes) {
        std::cout << "Process " << process->getId() << ": ";
        std::cout << "arrival time: " << process->getArrivalTime() << " ";
        std::cout << "service time: " << process->getServiceTime() << " units ";
        std::cout << "I/O time: " << process->getIoTime() << " units ";
        std::cout << "turnaround time: " << process->getTurnaroundTime() << " units ";
        std::cout << "finish time: " << process->getFinishTime() << " units\n";
    }
}
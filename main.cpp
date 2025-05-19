#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <random>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <unistd.h>
#include "process.h"
#include "scheduler.h"
#include "simulator.h"

// Function to parse command line arguments
struct Arguments {
    bool detailedMode;
    bool verboseMode;
    std::string algorithm;
    
    Arguments() : detailedMode(false), verboseMode(false), algorithm("") {}
};

Arguments parseArguments(int argc, char* argv[]) {
    Arguments args;
    int opt;
    
    while ((opt = getopt(argc, argv, "dva:")) != -1) {
        switch (opt) {
            case 'd':
                args.detailedMode = true;
                break;
            case 'v':
                args.verboseMode = true;
                break;
            case 'a':
                args.algorithm = optarg;
                break;
            default:
                std::cerr << "Usage: " << argv[0] << " [-d] [-v] [-a algorithm] < input_file\n";
                exit(EXIT_FAILURE);
        }
    }
    
    return args;
}

// Function to parse input file
struct InputData {
    int numProcesses;
    int processSwitchTime;
    std::vector<std::shared_ptr<Process>> processes;
};

InputData parseInput() {
    InputData data;
    
    // Read number of processes and process switch time
    if (!(std::cin >> data.numProcesses >> data.processSwitchTime)) {
        std::cerr << "Error reading input file\n";
        exit(EXIT_FAILURE);
    }
    
    // Read process data
    for (int i = 0; i < data.numProcesses; i++) {
        int id, arrivalTime, numBursts;
        
        if (!(std::cin >> id >> arrivalTime >> numBursts)) {
            std::cerr << "Error reading process data\n";
            exit(EXIT_FAILURE);
        }
        
        std::vector<int> cpuBursts;
        std::vector<int> ioBursts;
        
        for (int j = 0; j < numBursts; j++) {
            int burstNum, cpuTime;
            
            if (!(std::cin >> burstNum >> cpuTime)) {
                std::cerr << "Error reading CPU burst data\n";
                exit(EXIT_FAILURE);
            }
            
            cpuBursts.push_back(cpuTime);
            
            // If this is not the last burst, read I/O time
            if (j < numBursts - 1) {
                int ioTime;
                if (!(std::cin >> ioTime)) {
                    std::cerr << "Error reading I/O burst data\n";
                    exit(EXIT_FAILURE);
                }
                ioBursts.push_back(ioTime);
            }
        }
        
        // Create process
        std::shared_ptr<Process> process = std::make_shared<Process>(
            id, arrivalTime, cpuBursts, ioBursts);
        data.processes.push_back(process);
    }
    
    return data;
}

// Function to generate random processes
InputData generateRandomProcesses() {
    InputData data;
    data.numProcesses = 50; // Generate 50 processes
    data.processSwitchTime = 5; // Process switch overhead time
    
    // Seed random number generator
    std::mt19937 rng(static_cast<unsigned>(time(nullptr)));
    
    // Exponential distribution for arrival intervals (mean 50)
    std::exponential_distribution<double> arrivalDist(1.0 / 50.0);
    
    // Uniform distributions for CPU and I/O bursts
    std::uniform_int_distribution<int> cpuBurstDist(5, 500);
    std::uniform_int_distribution<int> ioBurstDist(30, 1000);
    std::uniform_int_distribution<int> numBurstsDist(10, 30); // Average around 20
    
    int currentArrivalTime = 0;
    
    for (int i = 1; i <= data.numProcesses; i++) {
        // Generate arrival time based on exponential distribution
        int arrivalInterval = static_cast<int>(std::ceil(arrivalDist(rng)));
        currentArrivalTime += arrivalInterval;
        
        // Generate number of CPU bursts for this process
        int numBursts = numBurstsDist(rng);
        
        std::vector<int> cpuBursts;
        std::vector<int> ioBursts;
        
        // Generate CPU and I/O bursts
        for (int j = 0; j < numBursts; j++) {
            cpuBursts.push_back(cpuBurstDist(rng));
            
            // If this is not the last burst, generate I/O time
            if (j < numBursts - 1) {
                ioBursts.push_back(ioBurstDist(rng));
            }
        }
        
        // Create process
        std::shared_ptr<Process> process = std::make_shared<Process>(
            i, currentArrivalTime, cpuBursts, ioBursts);
        data.processes.push_back(process);
    }
    
    return data;
}

// Function to run simulation for a specific algorithm
Statistics runSimulation(const std::string& algorithm, const InputData& data, 
                        bool verboseMode, bool detailedMode) {
    std::shared_ptr<Scheduler> scheduler = createScheduler(algorithm, data.processSwitchTime);
    Simulator simulator(verboseMode, detailedMode, scheduler);
    
    // Add processes to simulator
    for (const auto& process : data.processes) {
        simulator.addProcess(process);
    }
    
    // Run simulation
    simulator.run();
    
    return simulator.getStatistics();
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    Arguments args = parseArguments(argc, argv);
    
    // Parse input or generate random processes
    InputData data;
    if (!isatty(STDIN_FILENO)) {
        // Input is from a file/pipe
        data = parseInput();
    } else {
        // Generate random processes
        data = generateRandomProcesses();
    }
    
    // Define algorithms to run
    std::vector<std::string> algorithms;
    if (args.algorithm.empty()) {
        // Run all algorithms
        algorithms = {"FCFS", "SJF", "SRTN", "RR10", "RR50", "RR100"};
    } else {
        // Run only the specified algorithm
        if (args.algorithm == "FCFS" || args.algorithm == "SJF" || args.algorithm == "SRTN") {
            algorithms.push_back(args.algorithm);
        } else if (args.algorithm == "RR") {
            // Run all Round Robin variants
            algorithms = {"RR10", "RR50", "RR100"};
        } else {
            std::cerr << "Invalid algorithm: " << args.algorithm << "\n";
            std::cerr << "Valid algorithms are: FCFS, SJF, SRTN, RR\n";
            return EXIT_FAILURE;
        }
    }
    
    // Run simulations and print results
    for (const auto& alg : algorithms) {
        Statistics stats = runSimulation(alg, data, args.verboseMode, args.detailedMode);
        
        if (args.detailedMode) {
            stats.printDetailedOutput(createScheduler(alg, data.processSwitchTime)->getName());
        } else {
            stats.printDefaultOutput(createScheduler(alg, data.processSwitchTime)->getName());
        }
        
        // Add newline between algorithm outputs
        if (&alg != &algorithms.back()) {
            std::cout << "\n";
        }
    }
    
    return EXIT_SUCCESS;
}
#include "scheduler.h"
#include <vector>
#include <algorithm>
#include <queue>
#include <numeric>
#include <ostream>
#include <iostream>

void moveArrivedProcessesToReadyQueue(std::vector<Process> &processes, std::deque<int> &readyQueue, std::vector<int> &jobQueue, int64_t curr_time) {
    auto it = std::remove_if(jobQueue.begin(), jobQueue.end(), [&](int processIdx) {
        if (processes[processIdx].arrival_time <= curr_time) {
            readyQueue.push_back(processIdx);
            return true;
        }
        return false;
    });

    jobQueue.erase(it, jobQueue.end());
}

void simulate_rr(int64_t quantum, int64_t max_seq_len, std::vector<Process> &processes, std::vector<int> &seq) {
   // Initialize data structures
seq.clear();  // Clear any existing values
std::deque<int> readyQueue;
std::vector<int> jobQueue(processes.size());
std::iota(jobQueue.begin(), jobQueue.end(), 0);
int64_t curr_time = 0;

// Additional vectors to store original burst values for printing
std::vector<int64_t> remaining_bursts(processes.size(), 0);
std::vector<int64_t> original_bursts;

// Store original burst values
for (const Process &process : processes) {
    original_bursts.push_back(process.burst);
}


// Main loop for Round-Robin scheduling
while (!readyQueue.empty() || !jobQueue.empty()) {
    // Add arrived processes to the Ready Queue (RQ) and remove them from the Job Queue (JQ)
    moveArrivedProcessesToReadyQueue(processes, readyQueue, jobQueue, curr_time);

    // Ensure CPU is idle
    if (readyQueue.empty()) {
        // If RQ is empty, skip time to the arrival of the next process in JQ
        curr_time = jobQueue.empty() ? curr_time : processes[jobQueue.front()].arrival_time;
    } else {
        // Select the next process from RQ
        int current_process = readyQueue.front();
        readyQueue.pop_front();

        // Update start time only if it's the first time the process is executed
        if (processes[current_process].start_time == -1) {
            processes[current_process].start_time = curr_time;
        }

        // Execute the process for up to the quantum time or until it finishes
        int64_t remaining_slice = std::min(processes[current_process].burst, quantum);
        processes[current_process].burst -= remaining_slice;


        // Move to the next time unit
        curr_time += remaining_slice;

        // Check if the process has finished
        if (processes[current_process].burst <= 0) {
            processes[current_process].finish_time = curr_time;
        }

        // Add the process to the sequence only if the simulation is still ongoing
        seq.push_back(current_process);
        

        // Re-insert the process into RQ if it's not fully executed and new processes have arrived
        moveArrivedProcessesToReadyQueue(processes, readyQueue, jobQueue, curr_time);
        if (processes[current_process].burst > 0) {
            readyQueue.push_back(current_process);
        } else {
            // Replace the 0 burst value with the original burst value
            processes[current_process].burst = original_bursts[current_process];
        }
    }
}
}
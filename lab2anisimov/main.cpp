#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <random>
#include <chrono>


const std::string BLUE = "\033[1;34m";
const std::string RESET = "\033[1;37m";

struct Process {
    int id;
    int arrivalTime;
    int burstTime;
    int remainingTime;
    int priority;
    int waitingTime;
    int completionTime;

    Process(int _id, int _arrival, int _burst, int _priority)
        : id(_id), arrivalTime(_arrival), burstTime(_burst),
        remainingTime(_burst), priority(_priority),
        waitingTime(0), completionTime(0) {
    }
};

class ProcessScheduler {
private:
    std::vector<Process> processes;
    std::mt19937 gen;

public:
    ProcessScheduler() : gen(std::chrono::steady_clock::now().time_since_epoch().count()) {}

    void generateRandomProcesses(int count) {
        std::uniform_int_distribution<> arrivalDist(0, 10);
        std::uniform_int_distribution<> burstDist(1, 20);
        std::uniform_int_distribution<> priorityDist(1, 10);

        processes.clear();
        for (int i = 0; i < count; ++i) {
            processes.emplace_back(
                i + 1,
                arrivalDist(gen),
                burstDist(gen),
                priorityDist(gen)
            );
        }

        std::sort(processes.begin(), processes.end(),
            [](const Process& a, const Process& b) {
                return a.arrivalTime < b.arrivalTime;
            });
    }

    void simulateFCFS() {
        std::cout << "\n*---- FCFS Scheduling ----*" << std::endl;

        std::vector<Process> fcfsProcesses = processes;
        int currentTime = 0;
        double totalWaitingTime = 0;
        double totalTurnaroundTime = 0;

        for (auto& process : fcfsProcesses) {

            if (currentTime < process.arrivalTime) {
                currentTime = process.arrivalTime;
            }

            process.waitingTime = currentTime - process.arrivalTime;
            totalWaitingTime += process.waitingTime;

            currentTime += process.burstTime;
            process.completionTime = currentTime;

            int turnaroundTime = process.completionTime - process.arrivalTime;
            totalTurnaroundTime += turnaroundTime;

            std::cout << "Process " << process.id
                << ": Arrival = " << process.arrivalTime
                << ", Burst = " << process.burstTime
                << ", Waiting Time = " << process.waitingTime
                << ", Completion Time = " << process.completionTime
                << std::endl;
        }

        std::cout << "\nFCFS Average Waiting Time: "
            << totalWaitingTime / fcfsProcesses.size()
            << std::endl;
        std::cout << "FCFS Average Turnaround Time: "
            << totalTurnaroundTime / fcfsProcesses.size()
            << std::endl;
    }

    void simulateRoundRobin(int timeQuantum = 4) {
        std::cout << "\n*---- Round Robin Scheduling ----*" << std::endl;

        std::vector<Process> rrProcesses = processes;
        std::queue<Process*> readyQueue;
        int currentTime = 0;
        double totalWaitingTime = 0;
        int processesCompleted = 0;

        for (auto& process : rrProcesses) {
            if (process.arrivalTime == 0) {
                readyQueue.push(&process);
            }
        }

        while (!readyQueue.empty() || processesCompleted < rrProcesses.size()) {
            for (auto& process : rrProcesses) {
                if (process.arrivalTime == currentTime) {
                    readyQueue.push(&process);
                }
            }

            if (!readyQueue.empty()) {
                Process* currentProcess = readyQueue.front();
                readyQueue.pop();

                int executionTime = std::min(timeQuantum, currentProcess->remainingTime);
                currentProcess->remainingTime -= executionTime;
                currentTime += executionTime;

                std::cout << "Process " << currentProcess->id
                    << ": Time " << currentTime
                    << ", Remaining Time = " << currentProcess->remainingTime
                    << std::endl;

                for (auto& process : rrProcesses) {
                    if (process.arrivalTime <= currentTime &&
                        process.remainingTime > 0 &&
                        &process != currentProcess) {
                        readyQueue.push(&process);
                    }
                }

                if (currentProcess->remainingTime <= 0) {
                    currentProcess->completionTime = currentTime;
                    processesCompleted++;
                }
                else {
                    readyQueue.push(currentProcess);
                }
            }
            else {
                currentTime++;
            }
        }
    }

    void simulatePriorityScheduling() {
        std::cout << "\n*---- Priority Scheduling ----*" << std::endl;

        std::vector<Process> priorityProcesses = processes;
        int currentTime = 0;
        double totalWaitingTime = 0;
        double totalTurnaroundTime = 0;

        std::sort(priorityProcesses.begin(), priorityProcesses.end(),
            [](const Process& a, const Process& b) {
                return a.priority < b.priority;
            });

        for (auto& process : priorityProcesses) {
            if (currentTime < process.arrivalTime) {
                currentTime = process.arrivalTime;
            }

            process.waitingTime = currentTime - process.arrivalTime;
            totalWaitingTime += process.waitingTime;

            currentTime += process.burstTime;
            process.completionTime = currentTime;

            int turnaroundTime = process.completionTime - process.arrivalTime;
            totalTurnaroundTime += turnaroundTime;

            std::cout << "Process " << process.id
                << ": Priority = " << process.priority
                << ", Burst Time = " << process.burstTime
                << ", Waiting Time = " << process.waitingTime
                << ", Completion Time = " << process.completionTime
                << std::endl;
        }

        std::cout << BLUE <<"\nPriority Scheduling Average Waiting Time: "
            << totalWaitingTime / priorityProcesses.size()
            << std::endl;
        std::cout << "Priority Scheduling Average Turnaround Time: "
            << totalTurnaroundTime / priorityProcesses.size()
            << RESET <<std::endl;
    }

    void printProcesses() {
        std::cout << "\n*---- Generated Processes ----*" << std::endl;
        for (const auto& process : processes) {
            std::cout << "Process " << process.id
                << ": Arrival = " << process.arrivalTime
                << ", Burst = " << process.burstTime
                << ", Priority = " << process.priority
                << std::endl;
        }
    }
};

int main() {
    ProcessScheduler scheduler;

    // генерація випадкових процесів, в моєму випадку 7 випадкових процесів
    scheduler.generateRandomProcesses(7);

    scheduler.printProcesses();

    scheduler.simulateFCFS();
    scheduler.simulateRoundRobin();
    scheduler.simulatePriorityScheduling();

    return 0;
}
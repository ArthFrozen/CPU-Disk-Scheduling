/* * * * * * * * * * * * * * * * *
* Process-Scheduling Simulator *
* Copyright (c) 2008 *
* Kevin Gisi *
* MIT License *
* * * * * * * * * * * * * * * * */
/* Abstract
* This script provides a menu for determining which
* scheduling algorithm to use, and then prompts the
* user for an input file of processes. Statistics are
* then generated about the effectiveness of the
* scheduling algorithm on the provided data.
*
* This script makes the following assumptions:
* - Context-switching and interrupts are ignored
* - Deterministic time quantum
* - Simultaneous events (events happening on the same cycle)
* give priority to arriving processes
* - Process burst times are known in advance, and will
* be provided during the demo
*/
#include <iostream>
#include <string>
#include <queue>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iterator>

using namespace std;

bool cheating;

// Define Process class
class Process{
protected:

	friend ostream &operator<<(ostream &, const Process &);



public:

  Process(){};

  Process(int pidVal, int arrivalVal, int burstVal, int priorityVal){
		pid = pidVal;
		arrival = arrivalVal;
		burst = burstVal;
		priority = priorityVal;
		completedCycles = 0;
		wait = 0;
	};
	int pid;
	int arrival;
	int burst;
	int priority;
	int wait;
	int turnaround;
	int completedCycles;
	bool operator< (const Process &aProcess) const{
	  return arrival > aProcess.arrival;
	};

  int remainingCycles(){
	  return burst - completedCycles;
	};

  int priorityWithWindchill(){
    return priority - (UseAging()?(wait/5):0);
	};
  static void setUseAging(bool yesNo){ cheating = yesNo; };
  static bool UseAging(){ return cheating; };
};

// Output for Process class
ostream& operator<<(ostream& os, const Process &aProcess){
  os << aProcess.pid << "\t" << aProcess.arrival << "\t" << aProcess.burst << "\t" << aProcess.priority << endl;
  return os;
};

// Helper methods for ProcessQueue
void incrementWait(Process& aProcess){
  aProcess.wait++;
}
bool lessRemainingTime(Process a, Process b){
  return a.remainingCycles() < b.remainingCycles();
}

bool lessPriority(Process a, Process b){
  return a.priorityWithWindchill() < b.priorityWithWindchill();
}

// ProcessQueue extends Queue to allow access to private iterator
class ProcessQueue : public queue<Process> {
public:
  void incrementWaits(ProcessQueue& aProcessQueue){
    for_each(aProcessQueue.c.begin(), aProcessQueue.c.end(), incrementWait);
  };

  void sortByRemainingTime(ProcessQueue& aProcessQueue){
    sort(aProcessQueue.c.begin(), aProcessQueue.c.end(),lessRemainingTime);
  };

  void sortByPriority(ProcessQueue& aProcessQueue){
    sort(aProcessQueue.c.begin(), aProcessQueue.c.end(),lessPriority);
  };
};

// Helper method
template <class T>
inline string to_string (const T& t){
  stringstream ss;
  ss << t;
  return ss.str();
}

// Helper method splits on whitespace
vector<string> getTokens(string str){
  string buf; // Have a buffer string
  stringstream ss(str); // Insert the string into a stream
  vector<string> tokens; // Create vector to hold our words
  while (ss >> buf)
  tokens.push_back(buf);
  return tokens;
}

void printResults(vector<Process> processes, ostream& os){
  os << "Performance Results" << endl;
  os << "PID\tWait\tTurnaround" << endl;
  int waitTotal=0;
  int turnaroundTotal=0;
  /* cast to int */
  for(int i=0; i < processes.size(); i++){
    os << "P" << processes[i].pid << "\t" << processes[i].wait << "\t" << processes[i].wait + processes[i].completedCycles << endl;
    waitTotal += processes[i].wait;
    turnaroundTotal += processes[i].wait + processes[i].completedCycles;
  }
  os << "Average Wait Time: " << waitTotal / processes.size() << endl;
  os << "Average Turnarount Time: " << turnaroundTotal / processes.size() << endl;
}

void scheduleFCFS(priority_queue<Process> processes, ostream& os){
  /* cast to int */
  int processCount = processes.size();
  os << "Running the FCFS scheduler..." << endl;
  ProcessQueue waiting;
  vector<Process> completed;
  vector<Process> cpu;
  int time = 0;
  os << "Time\tEvent" << endl;
  while(completed.size() != processCount){
    string event = "";
    // Waiting processes wait values incremented
    waiting.incrementWaits(waiting);
    // Processes arrive
    /* added empty check */
    while(!processes.empty() && processes.top().arrival == time){
      event = event + "P" + to_string(processes.top().pid) + " arrives; ";
      waiting.push(processes.top());
      processes.pop();
      if(processes.empty()){
        break;
      }
    }
    if(cpu.empty()){
    // New process put on cpu
      if(!waiting.empty()){
        event = event + "P" + to_string(waiting.front().pid) + " put on CPU; ";
        cpu.push_back(waiting.front());
        waiting.pop();
      }
    }else{
      /* cast to int */
      for(int i=0; i < (int)cpu.size(); i++){
      // Cpu processes are incremented
        cpu[i].completedCycles++;
        // If current process completed
        if(cpu[i].completedCycles == cpu[i].burst){
          event = event + "P" + to_string(cpu[i].pid) + " completed; ";
          completed.push_back(cpu[i]);
          cpu.erase(cpu.begin() + i);
          // New process put on cpu
          if(!waiting.empty()){
            event = event + "P" + to_string(waiting.front().pid) + " put on CPU; ";
            cpu.push_back(waiting.front());
            waiting.pop();
          }
        }
      }
    }
    // Output events for clock cycle
    os << time << "\t" << event << endl;
    // New clock cycle
    time++;
  }
  os << "******************** End simulation ************************" << endl;
  printResults(completed, os);
}

void scheduleSRTF(priority_queue<Process> processes, ostream& os){
  /* cast to int */
  int processCount = (int)processes.size();
  os << "Running the SRTF scheduler..." << endl;
  ProcessQueue waiting;
  vector<Process> completed;
  vector<Process> cpu;
  int time = 0;
  os << "Time\tEvent" << endl;
  while(completed.size() != processCount){
    string event = "";
    // Waiting processes wait value incremented
    waiting.incrementWaits(waiting);
    // Processes arrive
    /* added empty check */
    while(!processes.empty() && processes.top().arrival == time){
      event = event + "P" + to_string(processes.top().pid) + " arrives; ";
      waiting.push(processes.top());
      processes.pop();
    }
    // Sort waiting list by remaining time
    waiting.sortByRemainingTime(waiting);
    if(cpu.empty()){
      // New process put on cpu
      if(!waiting.empty()){
        event = event + "P" + to_string(waiting.front().pid) + " put on CPU; ";
        cpu.push_back(waiting.front());
        waiting.pop();
      }
    }else{
      /* cast to int */
      for(int i=0; i < (int)cpu.size(); i++){
        // Cpu processes are incremented
        cpu[i].completedCycles++;
        if(cpu[i].completedCycles == cpu[i].burst){
          event = event + "P" + to_string(cpu[i].pid) + " completed; ";
          completed.push_back(cpu[i]);
          cpu.erase(cpu.begin()+i);
          // New process put on cpu
          if(!waiting.empty()){
            event = event + "P" + to_string(waiting.front().pid) + " put on CPU; ";
            cpu.push_back(waiting.front());
            waiting.pop();
          }
        }else if(!waiting.empty()){
          if(cpu[i].remainingCycles() > waiting.front().remainingCycles()){
            event = event + "P" + to_string(cpu[i].pid) + " taken off CPU; ";
            event = event + "P" + to_string(waiting.front().pid) + " put on CPU; ";
            waiting.push(cpu[i]);
            cpu.erase(cpu.begin()+i);
            cpu.push_back(waiting.front());
            waiting.pop();
            waiting.sortByRemainingTime(waiting);
          }
        }
      }
    }
    // Output events for clock cycle
    os << time << "\t" << event << endl;
    // New clock cycle
    time++;
  }
  os << "******************** End simulation ************************" << endl;
  printResults(completed, os);
}

void schedulePriority(priority_queue<Process> processes, ostream& os){
  /* cast to int */
  int processCount = (int)processes.size();
  os << "Running the Priority scheduler..." << endl;
  ProcessQueue waiting;
  vector<Process> completed;
  vector<Process> cpu;
  int time = 0;
  os << "Time\tEvent" << endl;
  while(completed.size() != processCount){
    string event = "";
    // Waiting processes wait value incremented
    waiting.incrementWaits(waiting);
    // Processes arrive
    /* added empty check */
    while(!processes.empty() && processes.top().arrival == time){
      event = event + "P" + to_string(processes.top().pid) + " arrives; ";
      waiting.push(processes.top());
      processes.pop();
    }
    // Sort waiting list by priority
    waiting.sortByPriority(waiting);
    if(cpu.empty()){
      // New process put on cpu
      if(!waiting.empty()){
        event = event + "P" + to_string(waiting.front().pid) + " put on CPU; ";
        cpu.push_back(waiting.front());
        waiting.pop();
      }
    }else{
      /* cast to int */
      for(int i=0; i < (int)cpu.size(); i++){
        // Cpu processes are incremented
        cpu[i].completedCycles++;
        if(cpu[i].completedCycles == cpu[i].burst){
          event = event + "P" + to_string(cpu[i].pid) + " completed; ";
          completed.push_back(cpu[i]);
          cpu.erase(cpu.begin()+i);
          // New process put on cpu
          if(!waiting.empty()){
            event = event + "P" + to_string(waiting.front().pid) + " put on CPU; ";
            cpu.push_back(waiting.front());
            waiting.pop();
          }
        }
      }
    }
    // Output events for clock cycle
    os << time << "\t" << event << endl;
    // New clock cycle
    time++;
  }
  os << "******************** End simulation ************************" << endl;
  printResults(completed, os);
}

void schedulePreemptivePriority(priority_queue<Process> processes, ostream& os){
  /* cast to int */
  int processCount = (int)processes.size();
  os << "Running the Preemptive Priority scheduler..." << endl;
  ProcessQueue waiting;
  vector<Process> completed;
  vector<Process> cpu;
  int time = 0;
  os << "Time\tEvent" << endl;
  while(completed.size() != processCount){
    string event = "";
    // Waiting processes wait value incremented
    waiting.incrementWaits(waiting);
    // Processes arrive
    /* added empty check */
    while(!processes.empty() && processes.top().arrival == time){
      event = event + "P" + to_string(processes.top().pid) + " arrives; ";
      waiting.push(processes.top());
      processes.pop();
    }
    // Sort waiting list by priority
    waiting.sortByPriority(waiting);
    if(cpu.empty()){
      // New process put on cpu
      if(!waiting.empty()){
        event = event + "P" + to_string(waiting.front().pid) + " put on CPU; ";
        cpu.push_back(waiting.front());
        waiting.pop();
      }
    }else{
      /* cast to int */
      for(int i=0; i < (int)cpu.size(); i++){
        // Cpu processes are incremented
        cpu[i].completedCycles++;
        if(cpu[i].completedCycles == cpu[i].burst){
          event = event + "P" + to_string(cpu[i].pid) + " completed; ";
          completed.push_back(cpu[i]);
          cpu.erase(cpu.begin()+i);
          // New process put on cpu
          if(!waiting.empty()){
            event = event + "P" + to_string(waiting.front().pid) + " put on CPU; ";
            cpu.push_back(waiting.front());
            waiting.pop();
          }
        }else if(!waiting.empty()){
          if(cpu[i].priorityWithWindchill() > waiting.front().priorityWithWindchill()){
            event = event + "P" + to_string(cpu[i].pid) + " taken off CPU; ";
            event = event + "P" + to_string(waiting.front().pid) + " put on CPU; ";
            waiting.push(cpu[i]);
            cpu.erase(cpu.begin()+i);
            cpu.push_back(waiting.front());
            waiting.pop();
            waiting.sortByPriority(waiting);
          }
        }
      }
    }
    // Output events for clock cycle
    os << time << "\t" << event << endl;
    // New clock cycle
    time++;
  }
  os << "******************** End simulation ************************" << endl;
  printResults(completed, os);
}

void scheduleRR(priority_queue<Process> processes, ostream& os){
  /* cast to int */
  int processCount = (int)processes.size();
  int timeQuantum;
  int contiguousCycles = 0;
  cout << "Time quantum: ";
  cin >> timeQuantum;
  os << "Running the RR scheduler..." << endl;
  ProcessQueue waiting;
  vector<Process> completed;
  vector<Process> cpu;
  int time = 0;
  os << "Time\tEvent" << endl;
  while(completed.size() != processCount){
    string event = "";
    // Waiting processes wait values incremented
    waiting.incrementWaits(waiting);
    // Processes arrive
    /* added empty check */
    while(!processes.empty() && processes.top().arrival == time){
      event = event + "P" + to_string(processes.top().pid) + " arrives; ";
      waiting.push(processes.top());
      processes.pop();
    }
    if(cpu.empty()){
      // New process put on cpu
      if(!waiting.empty()){
        event = event + "P" + to_string(waiting.front().pid) + " put on CPU; ";
        cpu.push_back(waiting.front());
        waiting.pop();
        contiguousCycles++;
      }
    }else{
      /* cast to int */
      for(int i=0; i < (int)cpu.size(); i++){
        // Cpu processes are incremented
        cpu[i].completedCycles++;
        contiguousCycles++;
        // If current process completed
        if(cpu[i].completedCycles == cpu[i].burst){
          event = event + "P" + to_string(cpu[i].pid) + " completed; ";
          completed.push_back(cpu[i]);
          cpu.erase(cpu.begin() + i);
          // New process put on cpu
          if(!waiting.empty()){
            event = event + "P" + to_string(waiting.front().pid) + " put on CPU; ";
            cpu.push_back(waiting.front());
            waiting.pop();
            contiguousCycles = 0;
          }
        }else if(contiguousCycles >= timeQuantum && !waiting.empty()){
          event = event + "P" + to_string(cpu[i].pid) + " taken off CPU; ";
          event = event + "P" + to_string(waiting.front().pid) + " put on CPU; ";
          waiting.push(cpu[i]);
          cpu.erase(cpu.begin()+i);
          cpu.push_back(waiting.front());
          waiting.pop();
          waiting.sortByRemainingTime(waiting);
          contiguousCycles = 0;
        }
      }
    }
    // Output events for the clock cycle
    os << time << "\t" << event << endl;
    // New clock cycle
    time++;
  }
  os << "******************** End simulation ************************" << endl;
  printResults(completed, os);
}

int main(){
  /* rearanged things a bit, added looped menu and file change and exit options */
  priority_queue<Process> processes;
  int schedulingType;
  int menuOption = 0;
  string inputFile = "";
  string outName = "output.txt";
  ifstream inFile;
  ofstream outFile;
  cout << "Enter the name of the input file.  : ";
  cin >> inputFile;
  string text;
  string ageString = "on";
  ostream* outChoice;
  outChoice = &cout;
  Process::setUseAging(false);
  inFile.open(inputFile.c_str());
  while(getline(inFile,text)){
    vector<string> tokens = getTokens(text);
    processes.push(Process(atoi(tokens[0].c_str()),atoi(tokens[1].c_str()),atoi(tokens[2].c_str()),atoi(tokens[3].c_str())));
  }
  inFile.close();
  while(menuOption != 10){
    cout << "Choose your scheduling algorithm:" << endl;
    cout << "1) FCFS" << endl;
    cout << "2) SRTF" << endl;
    cout << "3) Priority" << endl;
    cout << "4) Pre-emptive priority" << endl;
    cout << "5) RR" << endl;
    cout << "6) change input file" << endl;
    cout << "7) change output file" << endl;
    cout << "8) output to console" << endl;
    cout << "9) output to file" << endl;
    cout << "0) Turn aging " << ageString << endl;
    cout << "10) exit program" << endl;
    cout << "-> ";
    cin >> menuOption;
    if(menuOption == 0){
      Process::setUseAging(!Process::UseAging());
      ageString = Process::UseAging()?"off":"on";
    }else if(menuOption == 7){
      cout << "Enter the name of the output file. ";
      cin >> outName;
      outFile.open(outName.c_str());
    }else if(menuOption == 8){
      outChoice = &cout;
    }else if(menuOption == 9){
      outChoice = &outFile;
    }else if(menuOption == 6){
      cout << "Enter the name of the input file.  : ";
      cin >> inputFile;
      inFile.open(inputFile.c_str());
      string text;
      while(getline(inFile,text)){
        vector<string> tokens = getTokens(text);
        processes.push(Process(atoi(tokens[0].c_str()),atoi(tokens[1].c_str()),atoi(tokens[2].c_str()),atoi(tokens[3].c_str())));
      }
      inFile.close();
    }else if(menuOption < 6){
      schedulingType = menuOption;
      if(schedulingType == 1){
        scheduleFCFS(processes, *outChoice);
      }else if(schedulingType == 2){
        scheduleSRTF(processes, *outChoice);
      }else if(schedulingType == 3){
        schedulePriority(processes, *outChoice);
      }else if(schedulingType == 4){
        schedulePreemptivePriority(processes, *outChoice);
      }else if(schedulingType == 5){
        scheduleRR(processes, *outChoice);
      }
      outFile.close();
    }
  }
};

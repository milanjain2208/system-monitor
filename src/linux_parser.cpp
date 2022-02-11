#include <dirent.h>
#include <unistd.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string key, value, unit;
  long total_memory, free_memory;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    string line;
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value >> unit;
      if (key == "MemTotal:") {
        total_memory = std::stol(value);
      } else if (key == "MemFree:") {
        free_memory = std::stol(value);
        return (1 - ((float)free_memory) / total_memory);
      }
    }
  }
  return 0.0;
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  string uptime;
  long long_uptime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    long_uptime = std::stol(uptime);
  }
  return long_uptime;
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

// DONE: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    string line;
    std::getline(stream, line);
    int count = 0;
    string value;
    std::istringstream linestream(line);
    long total_time = 0;
    while (count <= 16) {
      linestream >> value;
      if (count >= 13 && count <= 16) {
        total_time += std::stol(value);
      }
      count++;
    }
    return total_time;
  }
  return 0;
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> cpu_times = LinuxParser::CpuUtilization();
  long user_time = std::stol(cpu_times[0]);
  long nice_time = std::stol(cpu_times[1]);
  long system_time = std::stol(cpu_times[2]);
  long irq_time = std::stol(cpu_times[5]);
  long softirq_time = std::stol(cpu_times[6]);
  long steal_time = std::stol(cpu_times[7]);
  return user_time + nice_time + system_time + irq_time + softirq_time +
         steal_time;
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpu_times = LinuxParser::CpuUtilization();
  long idle_time = std::stol(cpu_times[3]);
  long iowait_time = std::stol(cpu_times[4]);
  return idle_time + iowait_time;
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string cpu_key, user, nice, system, idle, iowait, irq, softirq, steal;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    string line;
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu_key >> user >> nice >> system >> idle >> iowait >> irq >>
        softirq >> steal;
    vector<string> cpu_times = {user,   nice, system,  idle,
                                iowait, irq,  softirq, steal};
    return cpu_times;
  }
  return {};
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string value, key;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    string line;
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") {
        return std::stoi(value);
      }
    }
  }
  return 0;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string key, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    string line;
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") {
        return std::stoi(value);
      }
    }
  }
  return 0;
}

// DONE: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    string line;
    std::getline(stream, line);
    return line;
  }
  return string();
}

// DONE: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    string line;
    while (std::getline(stream, line)) {
      string key, value;
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmRSS:") {  // using VmRss instead of VmSize because VmRSS
                              // gives the exact physical memory being used as a
                              // part of Physical RAM.
        int len = value.length();
        string mem;
        if (len == 1) {
          mem = "0.00" + value;
        } else if (len == 2) {
          mem = "0.0" + value;
        } else if (len == 3) {
          mem = "0." + value;
        } else {
          mem = value.substr(0, len - 3) + "." + value.substr(len - 3);
        }
        return mem;
      }
    }
  }
  return "0.0";
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    string line;
    while (std::getline(stream, line)) {
      string key, value;
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:") {
        return value;
      }
    }
  }
  return string();
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string uid = LinuxParser::Uid(pid);
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    string line;
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      string user, x, value;
      std::istringstream linestream(line);
      linestream >> user >> x >> value;
      if (value == uid) {
        return user;
      }
    }
  }
  return string();
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    string line;
    std::getline(stream, line);
    int count = 0;
    string value;
    std::istringstream linestream(line);
    while (count < 22) {
      linestream >> value;
      count++;
    }
    return LinuxParser::UpTime() - std::stol(value) / sysconf(_SC_CLK_TCK);
  }
  return 0;
}

#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include "linux_parser.h"

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

// DONE: Return this process's ID
int Process::Pid() { return this->pid; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() {
  long total_time = LinuxParser::ActiveJiffies(this->pid);
  return ((float)total_time) / (sysconf(_SC_CLK_TCK) * Process::UpTime());
}

// DONE: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(this->pid); }

// DONE: Return this process's memory utilization
string Process::Ram() { return this->ram = LinuxParser::Ram(this->pid); }

// DONE: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(this->pid); }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(this->pid); }

// DONE: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
  return std::stof(this->ram) < std::stof(a.ram) ? true : false;
}
#include "processor.h"
#include <unistd.h>
#include <iostream>
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  long prevtotal = LinuxParser::Jiffies();
  long prevIdle = LinuxParser::IdleJiffies();
  sleep(1);
  long total = LinuxParser::Jiffies();
  long idle = LinuxParser::IdleJiffies();
  long deltaTotal = total - prevtotal;
  long deltaIdle = idle - prevIdle;
  float cpu_percentage = (deltaTotal - deltaIdle) / (float)deltaTotal;
  return cpu_percentage;
}
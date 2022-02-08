#include <string>

#include "format.h"

using std::string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  long hours = (seconds / 3600) % 24;
  int minutes = (seconds / 60) % 60;
  int sec = seconds % 60;
  string time = (hours < 10 ? "0" : "") + std::to_string(hours) + ":" +
                (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" +
                (sec < 10 ? "0" : "") + std::to_string(sec);
  return time;
}
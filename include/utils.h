#ifndef UTILS_H
#define UTILS_H

#include <sys/time.h>
#include <cassert>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

enum LogLevel { VERBOSE, DEBUG, INFO, WARN, ERROR, NONE };

#define __FILENAME__ \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define LOG(LEVEL) \
  Log(LEVEL) << __FUNCTION__ << "@" << __FILENAME__ << ":" << __LINE__ << "] "

// test util
class Test {
  public:
    static void test_passed(const char *test_name) {
      std::cout << "\033[32m[PASSED] " << test_name << "\033[0m\n";
    }
    static void test_failed(const char *test_name) {
      std::cout << "\033[31m[FAILED] " << test_name << "\033[0m\n";
    }
};

// logger class
extern LogLevel log_level;
class Log {
  public:
    explicit Log(LogLevel level = INFO) {
      msglevel = level;
      if (level >= WARN) {
        operator << ("\033[31m");
      }
      operator << (get_level(level) + " " + current_time() + " ");
    }
    ~Log() {
      if (msglevel >= WARN) {
        msg_ << "\033[0m";
      }
      if (opened) {
        msg_ << std::endl;
      }
      std::cout << msg_.str() << std::flush;
    }
    template<class T>
    Log &operator<<(const T &msg) {
      if (msglevel >= log_level) {
        msg_ << msg;
        opened = true;
      }
      return *this;
    }

  private:
    bool opened = false;
    LogLevel msglevel;
    std::ostringstream msg_;
    inline std::string get_level(LogLevel level) {
      std::string label;
      switch (level) {
        case VERBOSE: label = "V"; break;
        case DEBUG:   label = "D"; break;
        case INFO:    label = "I"; break;
        case WARN:    label = "W"; break;
        case ERROR:   label = "E"; break;
        case NONE:    break;
      }
      return label;
    }
    inline std::string current_time() {
      timeval tv;
      gettimeofday(&tv, NULL);
      int milli = tv.tv_usec/1000;

      char buffer[80];
      strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&tv.tv_sec));
      std::stringstream ss;
      ss << buffer << "." << std::setfill('0') << std::setw(3) << milli;
      return ss.str();
    }
};
#endif  // UTILS_H

// vim: ts=2 sts=2 sw=2

#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <stdlib.h>


#ifndef _LOG_TAG_WIDTH
  #define _LOG_TAG_WIDTH 12
#endif

using namespace std;

namespace Log {

  unsigned int INFO  = 1;
  unsigned int DEBUG = 2;
  unsigned int ERROR = 3;

  static void log(string prefix, string str){
    cout << left << setw(_LOG_TAG_WIDTH) << prefix << " " << str << endl;
  }


  void log_info(string message, string filename, unsigned long long line_number) {
    stringstream ss;
    ss << "[INFO " << filename << " " << line_number << "]";
    Log::log(ss.str(), message);
  }


  void log_debug(string message, string filename, unsigned long long line_number) {
    stringstream ss;
    ss << "[DEBUG " << filename << " " << line_number << "]";
    Log::log(ss.str(), message);
  }


  void log_error(string message, string filename, unsigned long long line_number, unsigned long long col_number = 0) {
    stringstream ss;
    ss << "[ERROR " << filename << " " << line_number;
    if (col_number != 0) {
      ss << ":" << col_number;
    }
    ss << "]";
    Log::log(ss.str(), message);
  }

}

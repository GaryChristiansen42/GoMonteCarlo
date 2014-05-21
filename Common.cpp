#include "Common.h"

#include <fstream>

void Log(const char* message) {
  std::ofstream file("log.txt", std::ios::app);
  file << message << std::endl;
  file.close();
}


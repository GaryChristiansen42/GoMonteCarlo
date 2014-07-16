#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include "../Board.h"
#include "../Patterns.h"

using namespace boost;
using namespace boost::filesystem;

void getFiles(path p, std::vector<std::string>& files) {
  if (!exists(p)) {
    return;
  }
  if (is_directory(p)) {
    std::vector<path> inDirectory;
    std::copy(directory_iterator(p), directory_iterator(), std::back_inserter(inDirectory));
    for (auto x : inDirectory) {
      if (is_directory(x))
        getFiles(x, files);
      else if (is_regular_file(x))
        files.push_back(x.string());
    }
  }
}

std::vector<std::string> getSGFFiles(std::string dirName) {
  std::vector<std::string> sgfFiles;
  getFiles(path(dirName), sgfFiles);
  sgfFiles.erase(std::remove_if(sgfFiles.begin(), sgfFiles.end(), [] (std::string& x) { return x.find(".sgf") == std::string::npos; }), sgfFiles.end());
  return sgfFiles;
}

void readSGF(std::string fileName) {

  std::fstream f;
  f.open(fileName);
  assert(f.is_open());

  std::string contents = "", temp = "";
  while (f >> temp) {
    contents += temp + "\n";
  }
  regex e("EV\\[.*?\\]");
  contents = regex_replace(contents, e, std::string(""));

  std::cout << contents << std::endl;
  
}

int main(int argc, char* argv[]) {

  auto sgfFiles = getSGFFiles(argv[1]);
  for (auto fileName : sgfFiles)
    readSGF(fileName);

}

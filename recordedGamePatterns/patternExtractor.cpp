#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>
#include <utility>
#include <string>
#include <thread>
#include <vector>

#include <boost/filesystem.hpp>

#include "../Board.h"
#include "../Patterns.h"

using namespace boost::filesystem;

unsigned int failed = 0;
unsigned int moveFailures = 0;
unsigned int skippedOpenParen = 0;
unsigned int total = 0;

Patterns patterns;

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
  ++total;

  std::fstream f;
  f.open(fileName);
  assert(f.is_open());

  std::string contents = "", temp = "";
  while (f >> temp) {
    contents += temp;
  }
  f.close();
  // std::cout << contents << std::endl;

  std::vector<std::string> tokens;
  temp = "";
  unsigned int countOpenParen = 0;
  for (auto c : contents) {
    if (c == ';' || c == '\n') {
      tokens.push_back(temp);
      temp = "";
    } else {
      if (c == '(')
        ++countOpenParen;
      if (countOpenParen > 1) {
        ++skippedOpenParen;
        return;
      }
      temp += c;
    }
  }

  std::vector<std::pair<Player, std::pair<char, char> > > moves;
  for (auto s : tokens) {

    if (s.size() >= 5 && s[0] == 'B' && s[1] == '[') {
      moves.push_back(std::make_pair(Black, std::make_pair(s[2], s[3])));
      std::string temp2(s.begin()+5, s.end());
      if (temp2.size() > 0) {
        s = temp2;
        goto elseBeg;
        std::cout << temp2 << std::endl;
      }
    } else if (s.size() >= 5 && s[0] == 'W' && s[1] == '[') {
      moves.push_back(std::make_pair(White, std::make_pair(s[2], s[3])));
    } else if (s.size() == 0) {
      continue;
    } else if (s.size() == 1 && s[0] == '(') {
      continue;
    } else {
    elseBeg:
      unsigned int pos = 0;
      while (pos != s.size()) {
        if (s[pos+0] == 'F' && s[pos+1] == 'F'    // SGF Version
            || s[pos+0] == 'E' && s[pos+1] == 'V' // Event Name
            || s[pos+0] == 'P' && s[pos+1] == 'B' // Player Black
            || s[pos+0] == 'P' && s[pos+1] == 'W' // Player White
            || s[pos+0] == 'B' && s[pos+1] == 'R' // Black Rank
            || s[pos+0] == 'W' && s[pos+1] == 'R' // White Rank
            || s[pos+0] == 'K' && s[pos+1] == 'M' // Komi
            || s[pos+0] == 'R' && s[pos+1] == 'E' // Result
            || s[pos+0] == 'D' && s[pos+1] == 'T' // Date
            || s[pos+0] == 'U' && s[pos+1] == 'S' // Person where made .sgf file
            || s[pos+0] == 'P' && s[pos+1] == 'C' // Place where game was played
            || s[pos+0] == 'T' && s[pos+1] == 'M' // Time Limit
            || s[pos+0] == 'S' && s[pos+1] == 'O' // Source of SGF File
            || s[pos+0] == 'C' && s[pos+1] == '[' // Comment
            || s[pos+0] == 'C' && s[pos+1] == 'P' // Copyright info
            || s[pos+0] == 'B' && s[pos+1] == 'L' // 
            || s[pos+0] == 'W' && s[pos+1] == 'L' // 
            || s[pos+0] == 'G' && s[pos+1] == 'C' // 
            || s[pos+0] == 'O' && s[pos+1] == 'H' // 
            || s[pos+0] == 'R' && s[pos+1] == 'U' // Rule Set
            || s[pos+0] == 'W' && s[pos+1] == 'T' // White Team
            || s[pos+0] == 'B' && s[pos+1] == 'T' // Black Team
            || s[pos+0] == 'G' && s[pos+1] == 'M' // Game Type
            || s[pos+0] == 'O' && s[pos+1] == 'T' // Overtime
            || s[pos+0] == 'I' && s[pos+1] == 'D' // Game Identifier
            || s[pos+0] == 'H' && s[pos+1] == 'A' // Handicap amount
            || s[pos+0] == 'R' && s[pos+1] == 'O') { // Round #
          ++pos;
          while (s[pos-1] != ']' && pos <= s.size())
            ++pos;
        } else if (s[pos+0] == 'S' && s[pos+1] == 'Z') { // Size 
          return; 
        } else if (s[pos+0] == 'A' && s[pos+1] == 'B') { // Add Black Stone
          pos += 2;
          while (true) {
            if (s[pos] == '[' && s[pos+3] == ']') {
              moves.push_back(std::make_pair(Black, std::make_pair(s[1], s[2])));
              pos += 4;
            } else {
              break;
            }
          }
        } else {
          break;
        }
      }
      try {
        std::string s2(s.begin()+pos, s.end());
        if (s2.size() > 0) {
          // std::cout << "Failed: " << fileName << " " << s2 << std::endl;
          ++failed;
        }
      } catch (... ) {
        // std::cout << "Failed Badly: " << fileName << std::endl;
        ++failed;
      }
    }
  }

  Board b;
  b.init();
  for (auto& x : moves) {
    if (x.second.first < 'a' || x.second.second < 'a'
        || x.second.first >= 't' || x.second.second >= 't') {
      ++moveFailures;
      return;
    }

    Point p((int)x.second.first - 'a', (int)x.second.second - 'a');

    if (b.lastMove != NULL) {
      Pattern3x3 pat3x3(b.lastMove);
      auto hash = pat3x3.hash;
      pat3x3 = patterns.hashTable3x3[pat3x3.hash];
      pat3x3.hash = hash;
      bool found = false;
      for (auto& y : pat3x3.goodMoves)
        if (y.first == p.row && y.second == p.column)
          found = true;
      if (!found) {
        auto pairDiff = std::make_pair(p.row - b.lastMove->row, p.column - b.lastMove->column);
        if (pairDiff.first < 2 && pairDiff.first > -2
          && pairDiff.second < 2 && pairDiff.second > -2)
          pat3x3.goodMoves.push_back(pairDiff); // should be 0 in center not 0,0 on top left
      }
      patterns.addPattern(pat3x3);
      assert(pat3x3.goodMoves.size() > 0);
      b.show();
      std::cout << (int)p.row << " " << (int)p.column << std::endl;
      std::cout << (int)b.lastMove->row << " " << (int)b.lastMove->column << std::endl;
      std::cout << pat3x3 << std::endl;

      Pattern5x5 pat5x5(b.lastMove);
      hash = pat5x5.hash;
      pat5x5 = patterns.hashTable5x5[pat5x5.hash];
      pat5x5.hash = hash;
      found = false;
      for (auto& y : pat5x5.goodMoves)
        if (y.first == p.row && y.second == p.column)
          found = true;
      if (!found)
        pat5x5.goodMoves.push_back(std::make_pair(p.row, p.column));
      patterns.addPattern(pat5x5);
    }

    if (b.turn != x.first)
      b.makeMove(*b.pass);
    b.makeMove(p);
  }
}

int main(int argc, char* argv[]) {

  auto sgfFiles = getSGFFiles(argv[1]);
  for (auto& fileName : sgfFiles)
    readSGF(fileName);

  std::cout << "moveFailures: " << moveFailures << " " << total << std::endl;
  std::cout << "skippedOpenParen: " << skippedOpenParen << " " << total << std::endl;
  std::cout << "failed: " << failed << " " << total << std::endl;

  std::cout << patterns.hashTable3x3.size() << " " << patterns.hashTable5x5.size() << std::endl;
  patterns.save("patterns3x3.pat", "patterns5x5.pat");

  std::ofstream myfile;
  myfile.open ("example.txt");
  myfile << "Writing this to a file.\n";
  myfile.close();
}

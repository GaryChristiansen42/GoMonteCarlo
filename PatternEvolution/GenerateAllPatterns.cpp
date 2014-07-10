#include <assert.h>

#include <algorithm>
#include <iostream>
#include <set>
#include <string>

#include "../Patterns.h"


struct pattern_compare {
  bool operator() (const Pattern& lhs, const Pattern& rhs) const{
    return lhs.hash < rhs.hash;
  }
};

int main() {

  std::default_random_engine engine(time(NULL));

  Patterns patterns;

  std::vector<std::string> strings;
  std::string temp = "adddddddd";
  for (temp[0] = 'a' ; temp[0] <= 'd' ; temp[0]++)
    for (temp[1] = 'a' ; temp[1] <= 'd' ; temp[1]++)
      for (temp[2] = 'a' ; temp[2] <= 'd' ; temp[2]++)
        for (temp[3] = 'a' ; temp[3] <= 'd' ; temp[3]++)
          for (temp[4] = 'a' ; temp[4] <= 'd' ; temp[4]++)
            for (temp[5] = 'a' ; temp[5] <= 'd' ; temp[5]++)
              for (temp[6] = 'a' ; temp[6] <= 'd' ; temp[6]++)
                for (temp[7] = 'a' ; temp[7] <= 'd' ; temp[7]++)
                  for (temp[8] = 'a' ; temp[8] <= 'd' ; temp[8]++)
                    strings.push_back(temp);

  for (auto& hash : strings) {
    for (auto& c : hash) {
      if (c == 'a')
        c = 'B';
      else if (c == 'b')
        c = 'W';
      else if (c == 'c')
        c = 'O';
      else if (c == 'd')
        c = '_';
      else
        assert(false);
    }

    Pattern3x3 p;
    p.hash = hash;
    p.determineRandomGoodMoves(engine);

    patterns.hashTable3x3[hash] = p;

  }

  /*for (auto x : patterns.hashTable) {
    std::cout << x.second << std::endl;
  }*/
  std::cout << patterns.hashTable3x3.size() << std::endl;
  std::cout << (patterns.hashTable3x3["BBBBBBBBB"]) << std::endl;

  std::vector<Pattern3x3> legalGeneratedPatterns;
  for (auto it = patterns.hashTable3x3.begin(); it != patterns.hashTable3x3.end(); ++it) {
    if ((*it).second.isLegalPattern()) {
      legalGeneratedPatterns.push_back((*it).second);
    }
  }
  std::cout << legalGeneratedPatterns.size() << std::endl;

  unsigned long int i = 0;
  for (auto x : legalGeneratedPatterns) {
    for (auto c : x.hash) {
      if (c == '_') {
        ++i;
      }
    }
  }
  std::cout << i << std::endl;

  std::set<Pattern3x3, pattern_compare> notDoubles1;
  for (auto x : legalGeneratedPatterns) {
    Pattern3x3 p = x;
    if (notDoubles1.find(p) != notDoubles1.end())
      continue;
    p.rotate90();
    if (notDoubles1.find(p) != notDoubles1.end())
      continue;
    p.rotate90();
    if (notDoubles1.find(p) != notDoubles1.end())
      continue;
    p.rotate90();
    if (notDoubles1.find(p) != notDoubles1.end())
      continue;
    p.invertColor();
    if (notDoubles1.find(p) != notDoubles1.end())
      continue;
    p.rotate90();
    if (notDoubles1.find(p) != notDoubles1.end())
      continue;
    p.rotate90();
    if (notDoubles1.find(p) != notDoubles1.end())
      continue;
    p.rotate90();
    if (notDoubles1.find(p) != notDoubles1.end())
      continue;
    notDoubles1.insert(p);
  }
  std::cout << notDoubles1.size() << std::endl;
  
  i = 0;
  Patterns patterns2;
  for (auto x : notDoubles1) {
    for (auto c : x.hash) {
      if (c == '_') {
        ++i;
      }
    }
    patterns2.hashTable3x3[x.hash] = x;
  }
  std::cout << i << std::endl;

  patterns2.save("generatedPatterns3x3.pat", "generatedPatterns5x5.pat");
  std::cout << patterns2.hashTable3x3.size() << std::endl;

}

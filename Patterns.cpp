#include "Patterns.h"

#include <assert.h>
#include <fstream>
#include <iostream>

#include "Board.h"

Patterns::Patterns() :
  numCalled(0),
  numNotLegal(0),
  numNull(0),
  numNotCalled(0),
  numCalled2(0),
  numNotLegal2(0),
  numNull2(0),
  numNotCalled2(0),
  total(0),
  hashTable3x3(std::unordered_map<std::string, Pattern3x3>()),
  hashTable5x5(std::unordered_map<std::string, Pattern5x5>()),
  initialized(false) {

}


bool Patterns::operator==(const Patterns &p) {
  if (p.hashTable3x3.size() != hashTable3x3.size())
    return false;
  for (auto i : hashTable3x3) {
    bool found = false;
    for (auto j : p.hashTable3x3) {
      if (!i.first.compare(j.first)) {
        found = true;
        break;
      }
    }
    if (!found)
      return false;
  }
  if (p.hashTable5x5.size() != hashTable5x5.size())
    return false;
  for (auto i : hashTable5x5) {
    bool found = false;
    for (auto j : p.hashTable5x5) {
      if (!i.first.compare(j.first)) {
        found = true;
        break;
      }
    }
    if (!found)
      return false;
  }
 
  return true;
}

void Patterns::init(std::string patternsFile3x3, std::string patternsFile5x5) {

  if (patternsFile3x3 == "")
    return;
  
  std::ifstream in;
  in.open(patternsFile3x3);
  if (!in.is_open())
    assert(false);

  initialized = true;

  Pattern3x3 pattern3x3;
  while (in >> pattern3x3) {
    hashTable3x3[pattern3x3.hash] = pattern3x3;

    pattern3x3.rotate90();
    hashTable3x3[pattern3x3.hash] = pattern3x3;

    pattern3x3.rotate90();
    hashTable3x3[pattern3x3.hash] = pattern3x3;

    pattern3x3.rotate90();
    hashTable3x3[pattern3x3.hash] = pattern3x3;

    pattern3x3.invertColor();
    hashTable3x3[pattern3x3.hash] = pattern3x3;

    pattern3x3.rotate90();
    hashTable3x3[pattern3x3.hash] = pattern3x3;

    pattern3x3.rotate90();
    hashTable3x3[pattern3x3.hash] = pattern3x3;

    pattern3x3.rotate90();
    hashTable3x3[pattern3x3.hash] = pattern3x3;
  }

  // for (auto p : hashTable)
    // std::cout << p.second << std::endl; 
  in.close();

  in.open(patternsFile5x5);
  if (!in.is_open())
    assert(false);

  Pattern5x5 pattern5x5;
  while (in >> pattern5x5) {
    hashTable5x5[pattern5x5.hash] = pattern5x5;

    pattern5x5.rotate90();
    hashTable5x5[pattern5x5.hash] = pattern5x5;

    pattern5x5.rotate90();
    hashTable5x5[pattern5x5.hash] = pattern5x5;

    pattern5x5.rotate90();
    hashTable5x5[pattern5x5.hash] = pattern5x5;

    pattern5x5.invertColor();
    hashTable5x5[pattern5x5.hash] = pattern5x5;

    pattern5x5.rotate90();
    hashTable5x5[pattern5x5.hash] = pattern5x5;

    pattern5x5.rotate90();
    hashTable5x5[pattern5x5.hash] = pattern5x5;

    pattern5x5.rotate90();
    hashTable5x5[pattern5x5.hash] = pattern5x5;
  }

  // for (auto p : hashTable)
    // std::cout << p.second << std::endl; 
  in.close();

}

void Patterns::clear() {
  hashTable3x3.clear();
  hashTable5x5.clear();
  initialized = false;
}

void Patterns::save(std::string fileName3x3, std::string fileName5x5) {

  std::cout << hashTable3x3.size() << std::endl;
  std::cout << hashTable5x5.size() << std::endl;
  std::ofstream f;
  f.open(fileName3x3);
  assert(f.is_open());

  for (auto i : hashTable3x3) {
    f << i.second;
  }
  f.flush();
  f.close();

  std::ofstream f2;
  f2.open(fileName5x5);
  assert(f2.is_open());

  for (auto i : hashTable5x5) {
    f2 << i.second;
  }
  f2.flush();
  f2.close();

}

void Patterns::addPattern(Pattern3x3 pattern) {
  hashTable3x3[pattern.hash] = pattern;

  pattern.rotate90();
  hashTable3x3[pattern.hash] = pattern;

  pattern.rotate90();
  hashTable3x3[pattern.hash] = pattern;

  pattern.rotate90();
  hashTable3x3[pattern.hash] = pattern;

  pattern.invertColor();
  hashTable3x3[pattern.hash] = pattern;

  pattern.rotate90();
  hashTable3x3[pattern.hash] = pattern;

  pattern.rotate90();
  hashTable3x3[pattern.hash] = pattern;

  pattern.rotate90();
  hashTable3x3[pattern.hash] = pattern;
}


void Patterns::addPattern(Pattern5x5 pattern) {
  hashTable5x5[pattern.hash] = pattern;

  pattern.rotate90();
  hashTable5x5[pattern.hash] = pattern;

  pattern.rotate90();
  hashTable5x5[pattern.hash] = pattern;

  pattern.rotate90();
  hashTable5x5[pattern.hash] = pattern;

  pattern.invertColor();
  hashTable5x5[pattern.hash] = pattern;

  pattern.rotate90();
  hashTable5x5[pattern.hash] = pattern;

  pattern.rotate90();
  hashTable5x5[pattern.hash] = pattern;

  pattern.rotate90();
  hashTable5x5[pattern.hash] = pattern;
}

/*void Patterns::mutate(std::default_random_engine& engine) {
  std::uniform_int_distribution<> boolDist(0, 1);
  unsigned int choice = boolDist(engine);
  if (choice == 1 && hashTable.size() > 0) { // delete pattern
    std::uniform_int_distribution<> dist(0, (int)hashTable.size()-1);
    unsigned long int numToDelete = dist(engine);
    unsigned long int i = 0;
    for (auto it = hashTable.begin(); it != hashTable.end(); ++i, ++it) {
      if (i == numToDelete) {
        hashTable.erase(it);
        break;
      }
    }
  } else { // add new random pattern
    Pattern randomPattern = Pattern::getRandomPattern(engine);
    addPattern(randomPattern);
    // hashTable.insert(std::make_pair(randomPattern.hash, randomPattern));
  }
}*/

void Patterns::mutatePattern(std::string hash, std::default_random_engine& engine) {
  if (hash.length() == 9) {
    Pattern3x3 mutated = hashTable3x3[hash].getMutated(engine);
    addPattern(mutated);
  } else if (hash.length() == 25) {
    Pattern5x5 mutated;
    if (hashTable5x5.find(hash) == hashTable5x5.end()) {
      mutated.hash = hash;
      // mutated.determineRandomGoodMoves(engine);
      mutated.changeOneGoodMove(engine);
    } else {
      mutated = hashTable5x5[hash].getMutated(engine);
    }
    addPattern(mutated);
  } else {
    assert(false);
  }
}

bool legal(Board* b, Point* goodMove) {
    if (goodMove != NULL) {
      bool isLegal = false;
      Player sameColor = b->turn == Black ? Black : White;
      Player oppositeColor = b->turn == Black ? White : Black;
      int r = goodMove->row, c = goodMove->column;
      if (b->positions[r][c].color == Empty
        && !b->isSuicide(goodMove, sameColor, oppositeColor)
        && goodMove != b->koPoint) {
        isLegal = true;
      }
      return isLegal;
    }
    return false;

}

Point* Patterns::getMove(Board& b, std::default_random_engine& engine) {

  total++;
  if (b.lastMove != NULL) {
    Pattern3x3 lastMove3x3(b, *b.lastMove);
    Point* goodMove = hashTable3x3[lastMove3x3.hash].getRandomGoodMove(&b, *b.lastMove, engine);
    if (legal(&b, goodMove))
      return goodMove;
    numNull++;
  }

  numNotCalled++;

  /*if (b->secondLastMove != NULL) {
    Pattern3x3 lastMove3x3(b->secondLastMove);
    Point* goodMove = hashTable3x3[lastMove3x3.hash].getRandomGoodMove(b, *b->secondLastMove, engine);
    if (goodMove != NULL) {
      bool isLegal = false;
      Player sameColor = b->turn == Black ? Black : White;
      Player oppositeColor = b->turn == Black ? White : Black;
      int r = goodMove->row, c = goodMove->column;
      if (b->positions[r][c].color == Empty
        && !b->isSuicide(goodMove, sameColor, oppositeColor)
        && goodMove != b->koPoint) {
        isLegal = true;
      }
      if (isLegal) {
        numCalled2++;
        return goodMove;
      }
      numNotLegal2++;
    }
    numNull2++;
  }*/


  numNotCalled2++;

  return b.getRandomMove(engine);
}

#ifndef BOARD_H_
#define BOARD_H_

#define RUNNING_TESTS

#include <vector>

#include "Group.h"

class Group;

class Board {
 public:
  Point positions[9][9];

  Point* legalMoves[82];
  unsigned char numLegalMoves;

  std::vector<Group*> blackGroups;
  std::vector<Group*> whiteGroups;

  Point* lastMove;
  Point* secondLastMove;
  Point* koPoint;
  Point* pass;

  Player turn;

  explicit Board();

 private:
  Board(const Board& b);
  Board& operator=(const Board& b);

  void makeMove(Point* move);

 public:
  ~Board();
  void init();

  bool operator==(const Board &b);

  // void initialize(Player computer);
  Board* clone();
  void cloneInto(Board* clone);
  void cloneGroupsInto(Board* clone);

  bool isValidMove(Point move);

  // Fuego
  void getSimpleScore(float* whiteScore, float* blackScore);
  float getTaylorScore(float komi);

  bool isGameOver(GameResult *result);

  void updateStructures(Point* move);
  void removeDeadStones(Player color);

  unsigned long int getHash();

  Point* getRandomMove();
  void makeMove(Point move);

  void makeRandomMove();
  GameResult playRandomGame();

  void show();

  bool isSuicide(Point move, const Player &sameColor, const Player &oppositeColor);
  bool isSuicide(Point* move, const Player &sameColor, const Player &oppositeColor);

  void eliminatePositionalSuperKo(std::list<unsigned long int> previousHashes);
  bool isPositionalSuperKo(Point* p, std::list<unsigned long int> previousHashes);

  // private:
  void getPossibleMoves();
  
  Point* getPoint(Point* p);
};
#endif  // BOARD_H_

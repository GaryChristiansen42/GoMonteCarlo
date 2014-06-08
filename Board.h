#ifndef BOARD_H_
#define BOARD_H_

#define RUNNING_TESTS

#include <list>

#include "Group.h"

class Group;

class Board {
 public:
  int boardSize;
  Point ***positions;
  std::list<Group*> blackGroups;
  std::list<Group*> whiteGroups;
  Player turn;

  Point* lastMove;
  Point* secondLastMove;

  Point* koPoint;

  Point* pass;

  unsigned int capturedBlack;
  unsigned int capturedWhite;

  // std::vector<Point> pointsToCheck;

  std::list<Point*> possibleMoves;

  unsigned long int hash;
  std::list<unsigned long int> previousHashes;

  explicit Board(int newBoardSize);

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

  bool isValidMove(Point move);

  // Fuego
  void getSimpleScore(float* whiteScore, float* blackScore);
  float getTaylorScore(float komi);

  bool isGameOver(GameResult *result);

  void updateStructures(Point* move);
  unsigned int removeDeadStones(Player color, Point* move);

  void calculateHash();

  void makeMove(Point move);

  void makeRandomMove();
  GameResult playRandomGame();

  void show();

  bool isSuicide(Point move);
  bool isSuicide(Point* move);

  bool isPositionalSuperKo(Point* p);

  // private:
  void getPossibleMoves();
  
  Point* getPoint(Point* p);
};
#endif  // BOARD_H_

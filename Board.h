#ifndef BOARD_H_
#define BOARD_H_

#define RUNNING_TESTS

#include <list>

#include "Group.h"

class Group;

class Board {
 public:
  Point ***positions;

  Point* legalMoves[82];
  int numLegalMoves;

  std::list<Group*> blackGroups;
  std::list<Group*> whiteGroups;

  Point* lastMove;
  Point* secondLastMove;
  Point* koPoint;
  Point* pass;

  unsigned int capturedBlack;
  unsigned int capturedWhite;

  int boardSize;
  Player turn;

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

  unsigned long int getHash();

  Point* getRandomMove();
  void makeMove(Point move);

  void makeRandomMove();
  GameResult playRandomGame();

  void show();

  bool isSuicide(Point move);
  bool isSuicide(Point* move);

  void eliminatePositionalSuperKo(std::list<unsigned long int> previousHashes);
  bool isPositionalSuperKo(Point* p, std::list<unsigned long int> previousHashes);

  // private:
  void getPossibleMoves();
  
  Point* getPoint(Point* p);
};
#endif  // BOARD_H_

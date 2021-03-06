#ifndef BOARD_H_
#define BOARD_H_

#define RUNNING_TESTS

#include <list>
#include <random>
#include <vector>

#include "Group.h"
#include "Patterns.h"

class Group;

class Board {
 public:
  Point positions[BOARD_SIZE*BOARD_SIZE];

  std::vector<Point*> emptySpaces;

  std::vector<Group*> blackGroups;
  std::vector<Group*> whiteGroups;

  Point* lastMove;
  Point* secondLastMove;
  Point* koPoint;
  Point* pass;

  Group* outOfBoundsGroup;
  Point* outOfBoundsPoint;

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

  bool isValidMove(Point move,
      std::list<uint64_t> *previousHashes = nullptr);

  // Fuego
  void getSimpleScore(float* whiteScore, float* blackScore);
  float getTaylorScore(float komi);

  bool isGameOver(GameResult *result);

  void updateStructures(Point& move);
  void removeDeadStones(Point& move, Player color);

  uint64_t getHash();

  Point* getRandomMove(std::default_random_engine& engine);
  void makeMove(Point move);

  void makeRandomMove(std::default_random_engine& engine);
  GameResult playRandomGame(std::default_random_engine& engine);
  GameResult playGame(Patterns* patterns, std::default_random_engine& engine);

  void show();

  bool isSuicide(Point move, const Player &sameColor,
      const Player &oppositeColor);
  bool isSuicide(Point* move, const Player &sameColor,
      const Player &oppositeColor);

  void eliminatePositionalSuperKo(std::vector<Point*> &legalMoves,
      std::list<uint64_t> *previousHashes);
  bool isPositionalSuperKo(Point* p,
      std::list<uint64_t> previousHashes);

  // private:
  std::vector<Point*> getPossibleMoves();

  Point* getPoint(Point* p);
};
#endif  // BOARD_H_

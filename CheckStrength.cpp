#include <assert.h>
#include <time.h>

#include <cstdlib>
#include <cstdio>
#include <string>

#include "UCT.h"
#include "Common.h"

const std::string patternsFile3x3 = "patterns3x3.pat";
const std::string patternsFile5x5 = "patterns5x5.pat";

enum PlayerType {
  MyMonteCarlo1HalfSecond,
  MyMonteCarlo1Second,
  MyMonteCarlo5Seconds,
  MyMonteCarlo10Seconds,
  MyMonteCarlo14AndHalfSeconds,
  WithPatterns1HalfSecond,
  WithPatterns1Second,
  WithPatterns14AndHalfSeconds,
  MyMonteCarlo500Sims,
  WithPatterns500Sims,
  MyMonteCarlo1000Sims,
  WithPatterns1000Sims,
  RandomPlayer,
  GnuGo
};

const char * playerTypeStrings[14] = {
  "MyMonteCarlo1HalfSecond",
  "MyMonteCarlo1Second",
  "MyMonteCarlo5Seconds",
  "MyMonteCarlo10Seconds",
  "MyMonteCarlo14AndHalfSeconds",
  "WithPatterns1HalfSecond",
  "WithPatterns1Second",
  "WithPatterns14AndHalfSeconds",
  "MyMonteCarlo500Sims",
  "WithPatterns500Sims",
  "MyMonteCarlo1000Sims",
  "WithPatterns1000Sims",
  "RandomPlayer",
  "GnuGo"
};

int main() {
  printf("Running...\n");

  Patterns patterns;
  // patterns.init(patternsFile3x3, patternsFile5x5);

  int numTrials = 1;
  // int numSimulations = 1000;
  float komi = 0;
  // PlayerType player1 = WithPatterns1000Sims;
  // PlayerType player1 = RandomPlayer;
  PlayerType player1 = MyMonteCarlo14AndHalfSeconds;
  // PlayerType player1 = MyMonteCarlo1Second;
  PlayerType player2 = MyMonteCarlo14AndHalfSeconds;
  // PlayerType player2 = MyMonteCarlo1Second;
  // PlayerType player2 = MyMonteCarlo1000Sims;

  for (int x = 0; x < 2; x++) {
    int player1Wins = 0;
    int player2Wins = 0;

    for (int i = 0; i < numTrials; i++) {
      if (i % 10 == 0 && i != 0) {
        printf("\nTrial Number: %d\n", i);
      }
      auto b = new Board();
      b->init();
      UCTNode::rootState = b;
      auto node = new UCTNode(1, -1, nullptr);

      GameResult r;
      while (!b->isGameOver(&r)) {
        PlayerType currentPlayer = b->turn == Black ? player1 : player2;
        switch (currentPlayer) {
         case RandomPlayer:
          randomMove(&node, b);
          break;
         case MyMonteCarlo1HalfSecond:
          computerMove(&node, b, 0, 50, nullptr);
          break;
         case MyMonteCarlo1Second:
          computerMove(&node, b, 0, 1000, nullptr);
          break;
         case MyMonteCarlo5Seconds:
          computerMove(&node, b, 0, 5000, nullptr);
          break;
         case MyMonteCarlo10Seconds:
          computerMove(&node, b, 0, 10000, nullptr);
          break;
         case MyMonteCarlo14AndHalfSeconds:
          computerMove(&node, b, 0, 14500, nullptr);
          break;
         case WithPatterns1HalfSecond:
          computerMove(&node, b, 0, 50, &patterns);
          break;
         case WithPatterns1Second:
          computerMove(&node, b, 0, 1000, &patterns);
          break;
         case WithPatterns14AndHalfSeconds:
          computerMove(&node, b, 0, 14500, &patterns);
          break;
         case MyMonteCarlo500Sims:
          computerMove(&node, b, 500, 0, nullptr);
          break;
         case WithPatterns500Sims:
          computerMove(&node, b, 500, 0, &patterns);
          break;
         case MyMonteCarlo1000Sims:
          computerMove(&node, b, 1000, 0, nullptr);
          break;
         case WithPatterns1000Sims:
          computerMove(&node, b, 1000, 0, &patterns);
          break;
         case GnuGo:
         default:
          assert(false);
          break;
        }
        b->show();
      }

      if (static_cast<Player>(r) == Black) {
        player1Wins++;
      } else if (static_cast<Player>(r) == White) {
        player2Wins++;
      } else {
        printf("Draw\n");
      }

      float score = 0;
      score = b->getTaylorScore(komi);
      score = score;
      // printf("\nWhite: %f\nBlack: %f\n", whiteScore, blackScore);

      delete b;
      delete node;
    }

    printf("\n%s: %d / %d\n%s: %d / %d\n",
      playerTypeStrings[static_cast<int>(player1)], player1Wins,
      numTrials, playerTypeStrings[static_cast<int>(player2)], player2Wins,
      numTrials);

    PlayerType temp = player1;
    player1 = player2;
    player2 = temp;
  }
}

#include <assert.h>
#include <time.h>

#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <string>

#include "../UCT.h"

const std::string patternsFile = "patterns.pat";

// http://www.ai-junkie.com/ga/intro/gat2.html
Patterns originalPatterns;

int main(int argc, char** argv) {
  if (argc != 5) {
    printf("Expecting <filename3x3> <filename5x5> <numTrials> <datasavefile>\n");
    return 0;
  }

  printf("TestPatterns\n");

  std::default_random_engine engine(time(nullptr));

  originalPatterns.init(argv[1], argv[2]);

  unsigned int numTrials = atoi(argv[3]);
  unsigned int totalWins = 0;
  unsigned int totalMoves = 0;
  Player patternColor = Black;

 start:
  unsigned int numWins = 0;
  unsigned int gamesPlayed = 0;
  for (gamesPlayed = 0; gamesPlayed < numTrials; ++gamesPlayed) {
    Board b;
    b.init();

    GameResult r;
    Player turnColor = Black;
    while (!b.isGameOver(&r)) {
      if (turnColor == patternColor)
        b.makeMove(*originalPatterns.getMove(&b, engine));
      else 
        b.makeRandomMove(engine);
      turnColor = turnColor == Black ? White : Black;

      if (turnColor == patternColor)
        ++totalMoves;
    }
    if ((Player)r == patternColor)
      ++numWins;
  }
  totalWins += numWins;
  printf("Games - %d / %d = %f\n", numWins, gamesPlayed, ((float)numWins)/((float)(gamesPlayed)));

  if (patternColor == Black) {
    patternColor = White;
    goto start;
  }

  printf("Total - %d / %d = %f\n", totalWins, gamesPlayed*2, ((float)totalWins)/((float)(gamesPlayed*2)));
  printf("Moves - %d / %d = %f\n", originalPatterns.numCalled, totalMoves, ((float)(originalPatterns.numCalled)) / (float)totalMoves);
      
  std::ofstream f;
  // char fileName[200];
  // snprintf(fileName, sizeof(fileName), "%dx%dDataPoints.dat", BOARD_SIZE, BOARD_SIZE);
  f.open(argv[4], std::ios::app);
  if (f.is_open())
    f << totalWins << ", " << gamesPlayed*2 << ", " << ((float)totalWins) / ((float)gamesPlayed*2) << ", " << originalPatterns.numCalled << ", " << totalMoves << ", " << ((float)originalPatterns.numCalled) / ((float)totalMoves) << std::endl;  
  f.close();

  return 0;
}

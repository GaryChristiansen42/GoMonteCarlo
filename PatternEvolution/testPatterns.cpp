#include <assert.h>
#include <time.h>

#include <cstdlib>
#include <cstdio>
#include <string>

#include "../UCT.h"

const std::string patternsFile = "patterns.pat";

// http://www.ai-junkie.com/ga/intro/gat2.html
Patterns originalPatterns;

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("Expecting one argument for number of Trials\n");
    return 0;
  }
  printf("TestPatterns\n");

  srand((unsigned int)time(NULL));

  originalPatterns.init("patterns.pat");

  unsigned int numTrials = atoi(argv[1]);
  unsigned int totalWins = 0;
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
        b.makeMove(*originalPatterns.getMove(&b));
      else 
        b.makeRandomMove();
      turnColor = turnColor == Black ? White : Black;
    }
    if ((Player)r == patternColor)
      ++numWins;
  }
  totalWins += numWins;
  printf("%d / %d = %f\n", numWins, gamesPlayed, ((float)numWins)/((float)(gamesPlayed)));

  if (patternColor == Black) {
    patternColor = White;
    goto start;
  }

  printf("%d / %d = %f\n", totalWins, gamesPlayed*2, ((float)totalWins)/((float)(gamesPlayed*2)));
      
  return 0;
}

#include "Common.h"

#include <fstream>

#include "UCTNode.h"

void Log(const char* message, bool newline) {
  std::ofstream file("log.txt", std::ios::app);
  file << message;
  if (newline)
    file << std::endl;
  file.close();
}

void logTabs(int numTabs) {
  for (int i = 0; i < numTabs; i++)
    Log("\t", false);
}

void logTree(UCTNode* node, int numTabs, int maxDepth) {
  if (node == NULL || numTabs >= maxDepth || node->visits < 2)
    return;
  
  char buffer[100];

  logTabs(numTabs);

  Log("Node");

  logTabs(numTabs);
  sprintf(buffer, "\tMove: %d %d", node->move.row, node->move.column);
  Log(buffer);

  logTabs(numTabs);
  sprintf(buffer, "\tRewards/Visits: %f %d", node->totalRewards, node->visits);
  Log(buffer);

  logTabs(numTabs);
  // (*file) << "Wins: " << root->wins
  // << " Losses: " << root->losses << " Visits: " << root->visits << endl;
  logTabs(numTabs);
  Log("Children: ");
  logTree(node->child, numTabs+1, maxDepth);
  logTree(node->sibling, numTabs, maxDepth);
}


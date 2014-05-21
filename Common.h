#ifndef COMMON_H_
#define COMMON_H_

class UCTNode;

void Log(const char* message, bool newline=true);

void logTree(UCTNode* node, int numTabs, int maxDepth=1);

#endif

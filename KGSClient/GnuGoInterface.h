#include <string>

void GnuGoSetup();
void GnuGoCleanup();
void GnuGoSend(std::string message);
std::string GnuGoReceive();
std::string GnuGoReceiveBoard();

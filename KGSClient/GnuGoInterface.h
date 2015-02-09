#ifndef KGSCLIENT_GNUGOINTERFACE_H_
#define KGSCLIENT_GNUGOINTERFACE_H_

#include <string>

void GnuGoSetup();
void GnuGoCleanup();
void GnuGoSend(std::string message);
std::string GnuGoReceive();
std::string GnuGoReceiveBoard();

#endif  // KGSCLIENT_GNUGOINTERFACE_H_

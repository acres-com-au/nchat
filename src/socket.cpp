#include "socket.h"
#include <unistd.h>
#include <cstring>
#include <string>
#include <sstream>

/*
 * Socket initialisation is based on example from man getaddrinfo.
 * The constructor does most of the work in setting up the connection.
 * For the server, it would create a socket, bind it to an interface
 * and prepare for listenning.
 * For the client, it will create a socket and connect it to the server.
 * The server socket is activated by passing an emptry string as the
 * hostName.
 */
Socket::Socket(const std::string& hostName, int port, int bs) : bufSize(bs) {
  struct addrinfo hints;
  std::memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;	    // for allowing IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM;  // socket for TCP connection
  if (hostName == "")
    hints.ai_flags = AI_PASSIVE;    // for wildcard IP address
  else
    hints.ai_flags = 0;
  hints.ai_protocol = 0;	    // any protocol

  // resulting struct addrinfo instance
  struct addrinfo* pResult;
  const char* pHostName = hostName.c_str();
  if (hostName == "")
    pHostName = NULL;
    /* DBG: temporary solution for aidan! */
    //pHostName = "aidan.ncl.ac.uk";
  std::string sPort = std::to_string(port);
  /* DBG: try-catch! */
  getaddrinfo(pHostName, sPort.c_str(), &hints, &pResult);
  for (struct addrinfo* pR = pResult; pR != NULL; pR = pR->ai_next) {
    /* DBG: try-catch! */
    sfd = socket(pR->ai_family, pR->ai_socktype, pR->ai_protocol);
    if (sfd == -1)
      continue;
    if (hostName == "") {
      /* DBG: try-catch! */
      if (bind(sfd, pR->ai_addr, pR->ai_addrlen) == 0) {
	std::memcpy(&networkAddress, pR->ai_addr, sizeof(struct sockaddr));
	break;
      }
    }
    else {
      /* DBG: try-catch! */
      if (connect(sfd, pR->ai_addr, pR->ai_addrlen) != -1) {
	std::memcpy(&networkAddress, pR->ai_addr, sizeof(struct sockaddr));
	break;
      }
    }
  }
  // Release the resulting struct.
  freeaddrinfo(pResult);
  if (hostName == "") {
    // maximum number of pending connections (from man listen)
    int backlog = 100;
    /* DBG: try-catch! */
    listen(sfd, backlog);
  }

  // Allocate buffer memory.
  buffer = new char[bufSize];
}

Socket& Socket::acceptConnection() {
  struct sockaddr clientAddress;
  socklen_t clientAddressSize = sizeof(clientAddress);
  /* DBG: try-catch! */
  int cfd = accept(sfd, &clientAddress, &clientAddressSize);
  Socket* pCommunicationSocket = new Socket();
  pCommunicationSocket->setSfd(cfd);
  pCommunicationSocket->bufSize = bufSize;
  pCommunicationSocket->allocateBuffer();
  pCommunicationSocket->networkAddress = clientAddress;
  return *pCommunicationSocket;
}

void Socket::send(const char* buf) const {
  write(sfd, buf, bufSize);
}

void Socket::send(const std::string& text) const {
  int numBytesWritten = write(sfd, text.c_str(), text.length());
}

void Socket::send(int num) const {
  std::ostringstream sstream;
  sstream << num;
  int numBytesWritten = write(sfd, sstream.str().c_str(), sstream.str().length());
}

void Socket::recv(char* buf) const {
  read(sfd, buf, bufSize);
}

void Socket::recv(std::string& text) const {
  /* DBG: check the appropriate way to complete reading of a long message! */
  /*
  int numBytesRead = 0;
  while ((numBytesRead = read(sfd, buffer, bufSize)) > 0)
    text += buffer;
    */
  int numBytesRead = read(sfd, buffer, bufSize);
  if (numBytesRead < bufSize)
    buffer[numBytesRead] = 0;
  text = buffer;
}

void Socket::recv(int& num) const {
  read(sfd, buffer, bufSize);
  std::istringstream sstream(buffer);
  sstream >> num;
}
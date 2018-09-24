#ifndef SOCKET_H
#define SOCKET_H

#include "message.h"
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>

/*
 * The Socket class is used mostly to wrap system calls
 * for working with internet sockets.  It contains the
 * socket file descriptor, the size of the buffer for
 * exchanging messages and the structure that describes
 * the network address the socket is associated with.
 * For a listening socket, the network address is that
 * of the local host.  For other sockets, it will be
 * the address of the remote host in the connection.
 */
class Socket {
  // socket descriptor
  int sfd;
  // size of buffers for storing sent and received messages
  int bufSize;
  // buffer for storing sent and received messages
  char* buffer;
  // network address associated with the socket
  struct sockaddr networkAddress;
  public:
  // default constructor (set all members to 0)
  Socket() : sfd(0), bufSize(0), buffer(0) {
    std::memset(&networkAddress, 0, sizeof(struct sockaddr));
  }
  /* The server port is arbitrarily set to 10001 by default.
   * The default buffer size is 1024 bytes. */
  Socket(const std::string& hostName, int port = 10001, int bs = 1024);
  // copy-constructor
  Socket(const Socket& socket) {
    sfd = socket.sfd;
    bufSize = socket.bufSize;
    buffer = socket.buffer;
    networkAddress = socket.networkAddress;
  }
  /* DBG: check if virtual is necessary! */
  virtual ~Socket() {
    // Delete the buffer if its memory has been allocated (bufSize > 0).
    if (bufSize > 0)
      delete buffer;
  }
  int getSfd() const { return sfd; }
  void setSfd(int fd) { sfd = fd; }
  virtual void send(const char* buf) const;
  virtual void send(const std::string& text) const;
  virtual void send(int num) const;
  virtual void send(MessageType messageType, const std::vector<std::string>& parts);
  virtual void recv(char* buf) const;
  virtual void recv(std::string& text) const;
  virtual void recv(int& num) const;
  virtual void recv(MessageType& messageType, std::vector<std::string>& parts);
  virtual Socket& acceptConnection();
  void allocateBuffer() { buffer = new char[bufSize]; }
};

#endif	// SOCKET_H

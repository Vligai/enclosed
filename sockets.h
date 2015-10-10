/*
  header file for Sockets
*/
#ifndef _SOCKETS_H_
#define _SOCKETS_H_
#ifndef _MY_H_
#include "my.h"
#endif
#include <signal.h>
#define MAX 1024
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#ifndef _CLIENT_H_
void sfault1(int sig);
void sfault2(int sig);
#endif

#ifndef _SERVER_H_
void notime(int sig);
#endif

#endif

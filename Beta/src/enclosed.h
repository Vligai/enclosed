/*
  header file for enclosed-password manager
*/
#ifndef _ENCLOSED_H_
#define _ENCLOSED_H_
#include <signal.h>
#define MAX 2048
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "openssl/ssl.h"
#include "openssl/err.h"
#include <openssl/blowfish.h>
#include <errno.h>
/*functions in client*/
#ifndef _CLIENT_H_
void sfault1(int sig);
void sfault2(int sig);
#endif
/*functions in server*/
#ifndef _SERVER_H_
void notime(int sig);
void compute_md5(char *str, unsigned char digest[16]);
#endif

#endif

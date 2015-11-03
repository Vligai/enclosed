/*
  header file for enclosed-password manager
*/
#ifndef _ENCLOSED_H_
#define _ENCLOSED_H_
#include <signal.h>
#define MAX 2048
#define MAX_DATA 512
#define MAX_ROWS 100
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "openssl/ssl.h"
#include "openssl/err.h"
#include <openssl/md5.h>
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

struct Users {
  int id;
  int set;
  char username[MAX_DATA];
  char password[MAX_DATA];
};

struct Database {
  struct Users rows[MAX_ROWS];
};

struct Connection {
  FILE *file;
  struct Database *db;
};

void die(const char *message)
{
  if(errno) {
    perror(message);
  } else {
    printf("ERROR: %s\n", message);
  }

  exit(1);
}
void die(const char *message);
void Users_print(struct Users *user);
void Database_load(struct Connection *conn);
struct Connection *Database_open(const char *filename, char mode);
void Database_close(struct Connection *conn);
void Database_write(struct Connection *conn);
void Database_create(struct Connection *conn);
void Database_set(struct Connection *conn, int id, const char *username, const char *password);
void Database_get(struct Connection *conn, int id);
void Database_delete(struct Connection *conn, int id);
void Database_list(struct Connection *conn);
#endif

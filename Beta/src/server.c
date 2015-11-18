/*
  server side of the enclosed-password manager
  uses regular sockets during alpha, will use SSL in Beta version
*/

#define _SERVER_H_
#include "enclosed.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#define MAX_DATA 512
#define MAX_ROWS 100
char nick[MAX];
char password[MAX];

#define SIZE 8
//Passworddatabase

struct Passwords {
        int id;
        int set;
        char account[MAX_DATA];
        char username[MAX_DATA];
        char password[MAX_DATA];
};

struct Databasep {
        struct Passwords rows[MAX_ROWS];
};
struct Connectionp {
        FILE *filep;
        struct Databasep *db;
};
void Passwords_print(struct Passwords *pass)
{
        printf("%d %s %s %s\n",
                pass->id, pass->account,pass->username, pass->password);
}
void die(const char *message)
{
        if(errno) {
                perror(message);
        } else {
                printf("ERROR: %s\n", message);
        }

        exit(1);
}

void Database_loadp(struct Connectionp *connp)
{
        int rc = fread(connp->db, sizeof(struct Databasep), 1, connp->filep);
        if(rc != 1) die("Failed to load database.");
}

struct Connectionp *Database_openp(const char *filenamep, char mode)
{
        struct Connectionp *connp = malloc(sizeof(struct Connectionp));
        if(!connp) die("Memory error");

        connp->db = malloc(sizeof(struct Databasep));
        if(!connp->db) die("Memory error");

        if(mode == 'c') {
                connp->filep = fopen(filenamep, "w");
        }else {
                connp->filep = fopen(filenamep, "r+");

                if(connp->filep) {
                        Database_loadp(connp);
                }
        }

        if(!connp-> filep) die("Failed to open the file");

        return connp;
}

void Database_closep(struct Connectionp *connp)
{
        if(connp) {
                if(connp->filep) fclose(connp->filep);
                if(connp->db) free(connp->db);
                free(connp);
        }
}

void Database_writep(struct Connectionp *connp)
{
        rewind(connp->filep);

        int rc = fwrite(connp->db, sizeof(struct Databasep), 1, connp->filep);
        if(rc != 1) die("failed to write database.");

        rc = fflush(connp->filep);
        if(rc == -1) die("Cannot flush database.");
}

void Database_createp(struct Connectionp *connp)
{
        int i = 0;

        for(i = 0; i < MAX_ROWS; i++) {
                //Initizalize Database
                struct Passwords pass = {.id = i, .set = 0};
                // Assign database
                connp->db->rows[i] = pass;
        }
}
void Database_setp(struct Connectionp *connp, int id, const char *account, const char *username, const char *password)
{

        struct Passwords *pass = &connp->db->rows[id];

      //if(user->set) 
        //{
          //      id++;
        //}
      
        pass->set = 1;
        //we have to find a better way to do this rather than strncpy
        char *res = strncpy(pass->account, account, MAX_DATA);
        if(!res) die("Account copy failed");

        res = strncpy(pass->username, username, MAX_DATA);
        if(!res) die("Username copy failed");

        res = strncpy(pass->password, password, MAX_DATA);
        if(!res) die("Password copy failed");
}
void Database_getp(struct Connectionp *connp, int id)
{
        struct Passwords *pass = &connp->db->rows[id];

        if(pass->set) {
                Passwords_print(pass);
        } else {
                die("ID is not set");
        }
}


void Database_deletep(struct Connectionp *connp, int id)
{
        struct Passwords pass = {.id = id, .set = 0};
        connp->db->rows[id] = pass;
}

void Database_listp(struct Connectionp *connp)
{
        int i = 0;
        struct Databasep *db = connp->db;

        for(i = 0; i < MAX_ROWS; i++) {
                struct Passwords *cur = &db->rows[i];

                if(cur->set) {
                        Passwords_print(cur);
                }
        }
}


//Database_function
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

void Users_print(struct Users *user)
{
        printf("%d %s %s\n",
                user->id, user->username, user->password);
}
void Database_load(struct Connection *conn)
{
        int rc = fread(conn->db, sizeof(struct Database), 1, conn->file);
        if(rc != 1) die("Failed to load database.");
}
struct Connection *Database_open(const char *filename, char mode)
{
        struct Connection *conn = malloc(sizeof(struct Connection));
        if(!conn) die("Memory error");

        conn->db = malloc(sizeof(struct Database));
        if(!conn->db) die("Memory error");

        if(mode == 'c') {
                conn->file = fopen(filename, "w");
        }else {
                conn->file = fopen(filename, "r+");

                if(conn->file) {
                        Database_load(conn);
                }
        }

        if(!conn-> file) die("Failed to open the file");

        return conn;
}
void Database_close(struct Connection *conn)
{
        if(conn) {
                if(conn->file) fclose(conn->file);
                if(conn->db) free(conn->db);
                free(conn);
        }
}

void Database_write(struct Connection *conn)
{
        rewind(conn->file);

        int rc = fwrite(conn->db, sizeof(struct Database), 1, conn->file);
        if(rc != 1) die("failed to write database.");

        rc = fflush(conn->file);
        if(rc == -1) die("Cannot flush database.");
}

void Database_create(struct Connection *conn)
{
        int i = 0;

        for(i = 0; i < MAX_ROWS; i++) {
                //Initizalize Database
                struct Users user = {.id = i, .set = 0};
                // Assign database
                conn->db->rows[i] = user;
        }
}
void Database_set(struct Connection *conn, int id, const char *username, const char *password)
{

        struct Users *user = &conn->db->rows[id];

      /*if(user->set) 
        {
                id++;
        }
*/
        user->set = 1;
        //we have to find a better way to do this rather than strncpy
        char *res = strncpy(user->username, username, MAX_DATA);
        if(!res) die("Username copy failed");

        res = strncpy(user->password, password, MAX_DATA);
        if(!res) die("Password copy failed");
}

void Database_get(struct Connection *conn, int id)
{
        struct Users *user = &conn->db->rows[id];

        if(user->set) {
                Users_print(user);
        } else {
                die("ID is not set");
        }
}
void Database_delete(struct Connection *conn, int id)
{
        struct Users user = {.id = id, .set = 0};
        conn->db->rows[id] = user;
}

void Database_list(struct Connection *conn)
{
        int i = 0;
        struct Database *db = conn->db;

        for(i = 0; i < MAX_ROWS; i++) {
                struct Users *cur = &db->rows[i];

                if(cur->set) {
                        Users_print(cur);
                }
        }
}







void sfault1(int sig)
{
  if(sig == SIGINT)
    {
      usleep(3000);
      puts("\nclosing server");
      exit(0);
    }
}
/*callsed when user disconnects from the server*/
void sfault2(int sig)
{
  if(sig == SIGINT)
    {
      usleep((getpid()%100+50)*10);
      puts("*** ");
      puts(nick);
      puts(" disconnected");
      exit(0);
    }
}
/*
void compute_md5(char *str, unsigned char digest[16]) 
{
  MD5_CTX ctx;
  MD5_Init(&ctx);
  MD5_Update(&ctx, str, strlen(str));
  MD5_Final(digest, &ctx);
}
*/
/*function used to md5 hash password*/

/*char *md5_hash(char *str, unsigned char digest[16])
{
  int i = 0;
  char result[1024];
  MD5((unsigned char *)str,strlen(str),result);
  for(; i < 16; i++)
    {
      sprintf(&result[i*2],"%02x",(unsigned int)result[i]);
    }
  return result;
}
*/
/*main*/
int main(int argc, char** argv)
{
  struct sockaddr_in srv;
  struct sockaddr_in cli;
  
  int sockfd;
  int sockfd2;
  int nport;
  int scli;
  int n;
  int counter; 
  int pid;
  char buff[MAX];
  char buff2[MAX];
  char website[MAX];
  char username[MAX];
  char pass[MAX];
  char command[MAX];
  signal(SIGINT, sfault1);
  //crypto
  unsigned char *enc_pass = calloc(SIZE+1,sizeof(char));
  BF_KEY *key = calloc(1,sizeof(BF_KEY));
  
  //database stuff
char *filename = "Data.db";
char *filenamep = "pass.db";
char action;
struct Connection *conn = Database_open(filename, action);
struct Connectionp *connp  = Database_openp(filenamep, action);
int id = 0;
  /*check numer of arguments when starting up server*/
  if(argc < 2)
    {
      puts("usage: ./server nport");
      exit(0);
    }
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  /*check if socket is valid*/
  if(sockfd < 0)
    {
      puts("Invalid socket");
      exit(0);
    }
  memset(&srv, 0, sizeof(srv));
  /*checking the port number given by user*/
  nport = atoi(argv[1]);
  if(nport <= 0 || nport > 65535)
    {
      puts("Invalid port num");
      exit(0);  
    }
  srv.sin_family = AF_INET;
  srv.sin_port = htons(nport);
  srv.sin_addr.s_addr = INADDR_ANY;
  
  if(bind(sockfd, (struct sockaddr *)&srv, sizeof(srv)) < 0)
    {
      puts("Invalid bind");	
      exit(0);
    }
  listen(sockfd, 5);
  while(1)
    {
      scli = sizeof(cli);
      sockfd2 = accept(sockfd, (struct sockaddr *)&cli, &scli);
      /*check if connection is valid*/
      if(sockfd2  < 0)
	{
	  puts("Invalid connection");      
	  exit(0);
	}
      /*check if fork is valid*/
      if((pid = fork()) < 0)
	{
	  puts("Bad Fork");
	  exit(0);
	}
      if(pid==0)
	{
	  close(sockfd);			
	  n = read(sockfd2, buff, MAX-1);
	  while((strncmp(buff, "create", 6) != 0) && (strncmp(buff,"login",5) != 0))
	    {
	      printf(" [###] ERROR: user has input invalid command: %s \n", buff);
	      n = read(sockfd2, buff, MAX-1);
	    }
	  /*if user choose to create a new user go here*/
	  if (strncmp(buff, "create", 6) == 0)
	    {
	      puts("          [!] Creating new user...");
	      n=read(sockfd2, nick, MAX-1);
	      nick[n-1]='\0';
	      printf("     |~| Username: ");
	      puts(nick);
	      
	      write (sockfd2, "~", 1);
	      n=read(sockfd2, password, MAX-1);
	      password[n-1] = '\0';
	      //	  unsigned char md5_pass[16];
	      printf("     |~| Password: ");
	      /*hashing master password to compare it with password inside the db*/
	      //md5_hash(password, md5_pass);
	      puts(password);
	      write(sockfd2, "~", 1);
	      //database check
	      int i = 0;
	      int j = 0;
	      struct Database *db = conn->db;
	      
	      for(i = 0; i < MAX_ROWS; i++) {
		struct Users *cur = &db->rows[i];
		
		if(cur->set) {
		  j++;
		}
	      }
	      id = j;
	      BF_set_key(key,SIZE,(const unsigned char*)password);
	      Database_set(conn, id, nick, password);
              Database_write(conn);

	    }
	  /*nick - username user is going to log in with
	   check for username in he name of data fiels*/
	  else if (strncmp(buff, "login", 5) == 0)
	    {
	      //	      close(sockfd);			
	      n=read(sockfd2, nick, MAX-1);
	      nick[n-1]='\0';
	      printf("     |~| %s",nick);
	      puts(" has connected to password manager, check for this username in the database");
	      write (sockfd2, "~", 1);
	      
	      /*password - user master password used to log user in*/
	      n=read(sockfd2, password, MAX-1);
	      password[n-1] = '\0';
	      //	  unsigned char md5_pass[16];
	      printf("     |~| Check if password in the database corresponds to this one: ");
	      /*hashing master password to compare it with password inside the db*/
	      //md5_hash(password, md5_pass);
	      puts(password);
	      BF_set_key(key,SIZE,(const unsigned char*)password);
	      write(sockfd2, "~", 1);
	    }
	  while(1)
	    {
	      close(sockfd);
	      signal(SIGINT, sfault2);
	      n=read(sockfd2, buff, MAX-1);
	      buff[n]='\0';
	      write (sockfd2, "~", 1);
	      /*functions available for the user to input
	       just sample commands*/
	      if(strncmp(buff, "/change_mpass", 13) == 0)
		{
		  //puts ("  ~User requested to change master password...");
		  puts("          [!] Asking user for existing master password...");
		  printf("     |~| User entered existing master password: ");

		  n=read(sockfd2, buff2, MAX-1);
		  buff2[n-1]='\0';
		  puts(buff2);
		  write (sockfd2, "~", 1);
		  
		  puts("          [!] Asking user for desired new master password...");
		  printf("     |~| User entered desired new master password: ");
		  n=read(sockfd2, password, MAX-1);
		  password[n-1]='\0';
		  puts(password);
		  write (sockfd2, "~", 1);

		  puts("          [!] Asking user to re-enter desired new master password...");
		  printf("     |~| User entered desired new master password: ");
		  n=read(sockfd2, buff, MAX-1);
		  buff[n-1]='\0';
		  puts(buff);
		  write (sockfd2, "~", 1);
		  
		  puts("          [!] User's master password now changed");

		}

	      if(strncmp(buff, "/add_acc", 8) == 0)
		{
		  puts("          [!] Creating new account...");
		  n=read(sockfd2, website, MAX-1);
		  website[n-1]='\0';
		  printf("     |~| Website: ");
		  puts(website);
		  write (sockfd2, "~", 1);

		  n=read(sockfd2, nick, MAX-1);
		  nick[n-1]='\0';
		  printf("     |~| Username: ");
		  puts(nick);
		  write (sockfd2, "~", 1);

		  n=read(sockfd2, password, MAX-1);
		  password[n-1] = '\0';
		  printf("     |~| Password: ");
		  BF_ecb_encrypt((unsigned char *)password,enc_pass,key,BF_ENCRYPT);
		  puts(enc_pass);
		  write(sockfd2, "~", 1);
		  Database_setp(connp, id, website, nick, password);
		  Database_writep(connp);
		  
		}

	      if(strncmp(buff, "/exit", 5) == 0)
		{
		  printf("%s", "***");
		  printf("%s",nick);
		  puts(" has disconnected");
		  exit(0);
		}
	      if(strncmp(buff, "/help", 5) == 0)
		{
		puts("          [!] user has viewed a help menu");
		}
	      /*add print whole file of the user db*/
	      if(strncmp(buff, "/view_passes", 12) == 0)
		puts("Here are your saved passwords");

	      else if(((strncmp(buff, "/help", 5) != 0) && (strncmp(buff, "/exit", 5) != 0) && (strncmp(buff, "/add_acc", 8) != 0) && (strncmp(buff, "/view_passes", 12) != 0) && (strncmp(buff, "/change_mpass", 13) != 0)))
		{
		  /*if user input unknown command*/
		  printf(" [###] ERROR: user has input unknown command: %s \n", buff);
		  
		}
	    }	  
	}
      else
	close(sockfd2);
    }
    Database_closep(connp);
    Database_close(conn);
  free(buff);
  free(buff2);
  free(nick);
  free(password);
  free(command);
}

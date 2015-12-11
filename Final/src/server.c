/*
server side of the enclosed-password manager

*/
#include <stdbool.h>
#define _SERVER_H_
#include "enclosed.h"
#define MAX_DATA 512
#define MAX_ROWS 1000
char nick[MAX];
char password[MAX];
char vlad[MAX];
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
//load database
void Database_loadp(struct Connectionp *connp)
{
  int rc = fread(connp->db, sizeof(struct Databasep), 1, connp->filep);
  if(rc != 1) die("Failed to load database.");
}
//connrction function for database
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
//function to close the database
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
//creation of the unique database for each user
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

//struct for the main database of all users
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
//login check
void Users_check( struct Connection *conn, const char *username, const char *password)
{
  int i=0;
  struct Database *db = conn->db;

  for(i=0; i < MAX_ROWS; i++) {
    struct Users *cur = &db-> rows[i];

    if(cur->set) {
      const char *find_user = cur->username;
      const char *find_pass = cur->password;
      if((strcmp((const char*)username,(const char*)find_user)==0) && (strcmp((const char*)password,(const char*)find_pass)==0)){
        printf("hello");
      }
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

/*main*/
int main(int argc, char** argv)
{
  struct sockaddr_in srv;
  struct sockaddr_in cli;
  char *check;
  int sockfd;
  int sockfd2;
  int nport;
  int scli;
  int n;
  int counter;
  int pid;
  char buff[MAX];
  char buff2[MAX];
  char secured[MAX]; //output for decrytption change m_pass 1
  char secured2[MAX]; //output for change m_pass2
  char website[MAX];
  char username[MAX];
  char pass[MAX];
  char command[MAX];
  signal(SIGINT, sfault1);

  char *result;
  const char *account;
  //crypto
  unsigned char *enc_pass = calloc(MAX+1,sizeof(char));
  unsigned char *enc_pass2 = calloc(MAX+1,sizeof(char));
  unsigned char *enc_pass3 = calloc(MAX+1,sizeof(char));
  unsigned char *enc_pass4 = calloc(MAX+1,sizeof(char));
  BF_KEY *key = calloc(1,sizeof(BF_KEY));

  const unsigned char keytext[] = "key";
  const unsigned char ivec[] = "12345678";

  BF_KEY bf_key;
  int bf_ivec_pos;
  unsigned char bf_ivec[sizeof(ivec)];

  EVP_CIPHER_CTX evp_ctx;
  unsigned char evp_outbuf[1024];
  int evp_outlen, evp_tmplen, i;

  bf_ivec_pos = 0;
  memcpy(bf_ivec, ivec, sizeof(ivec));
  BF_set_key(&bf_key, sizeof(keytext), keytext);

  char username_output[MAX];

  //database stuff


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
      int id = 0;
      char *filename = "Data.db";
      char *filenamep = "pass.db";
      char action;
      struct Connection *conn = Database_open(filename, action);
      struct Connectionp *connp  = Database_openp(filenamep, action);
      close(sockfd);
      n = read(sockfd2, buff, MAX-1);
      while((strncmp(buff, "create", 6) != 0) && (strncmp(buff,"login",5) != 0))
      {
        printf(" [###] ERROR: user has input invalid command: %s \n", buff);
        n = read(sockfd2, buff, MAX-1);
      }
      /*if user choose to create a new user go here*/

      //CREATE


      if (strncmp(buff, "create", 6) == 0)
      {
        puts("          [!] Creating new user...");
        n=read(sockfd2, nick, MAX-1);
        nick[n-1]='\0';


        bf_ivec_pos = 0;
        memcpy(bf_ivec, ivec, sizeof(ivec));
        BF_cfb64_encrypt(nick, enc_pass, n, &bf_key, bf_ivec, &bf_ivec_pos, 0);

        // printf("%s\n",(char*)nick);
        strncpy(username_output,enc_pass,n);

        printf("     |~| Username: ");
        puts(enc_pass);
        write (sockfd2, "~", 1);
        n=read(sockfd2, password, MAX-1);
        password[n-1] = '\0';



        //Code to Encrypt
        bf_ivec_pos = 0;
        memcpy(bf_ivec, ivec, sizeof(ivec));

        BF_cfb64_encrypt(password, enc_pass2, n, &bf_key, bf_ivec, &bf_ivec_pos, 0);
        // End



        printf("     |~| Password: ");
        //printf("%s\n",enc_pass2);
        puts("Recieved password");
        write(sockfd2, "~", 1);

        //char *filenamep = strcat(nick, ".db");
        //struct Connectionp *connp  = Database_openp(filenamep, action);
        int i = 0;
        int j = 0;
        struct Database *db = conn->db;

        BF_set_key(key,SIZE,(const unsigned char*)enc_pass2);
        for(i = 0; i < MAX_ROWS; i++) {
          struct Users *cur = &db->rows[i];

          if(cur->set) {
            j++;
          }
        }
        id = j;
        Database_set(conn, id, enc_pass, enc_pass2);
        Database_write(conn);

        char *filenamep = strcat(enc_pass, ".db");
        char action ='c';

        struct Connectionp *connp = Database_openp(filenamep, action);
        Database_createp(connp);
        Database_writep(connp);
        bzero(enc_pass,sizeof(enc_pass));
        bzero(enc_pass2,sizeof(enc_pass2));
      }
      /*nick - username user is going to log in with
      check for username in he name of data fiels*/

      //LOGIN
      else if (strncmp(buff, "login", 5) == 0)
      {
        puts("          [!] Logging In...");
        bzero(nick, MAX);
        bzero(username, MAX);

        bzero(enc_pass2, strlen(enc_pass2));
        bzero(username, MAX);
        n=read(sockfd2, nick, MAX-1);
        nick[n-1]='\0';
        bf_ivec_pos = 0;
        memcpy(bf_ivec, ivec, sizeof(ivec));
        BF_cfb64_encrypt(nick, enc_pass, sizeof(nick), &bf_key, bf_ivec, &bf_ivec_pos, 0);
        strncpy(username_output,enc_pass,n);
        printf("     |~| %s",enc_pass);
        puts(" has connected to password manager, check for this username in the database");
        write (sockfd2, "~", 1);
        bzero(password,MAX);
        /*password - user master password used to log user in*/
        n=read(sockfd2, password, MAX-1);
        password[n-1] = '\0';


        bf_ivec_pos = 0;
        memcpy(bf_ivec, ivec, sizeof(ivec));

        BF_cfb64_encrypt(password, enc_pass2,strlen(password), &bf_key, bf_ivec, &bf_ivec_pos, 0);

        //printf("%s\n",(const char*)enc_pass2);
        BF_set_key(key,SIZE,(const unsigned char*)enc_pass2);
        //	puts(password);
        printf("     |~| Check if password in the database corresponds to this one: ");
        //printf("%s\n",(const char*)enc_pass2);
        puts("Recieved password");
        //printf("let me check that \n");

        //check to find where the account is and send the password
        int i=0;
        struct Database *db = conn->db;
        for(i=0; i < MAX_ROWS; i++) {
          struct Users *cur = &db-> rows[i];

          if(cur->set) {
            const char *find_user = cur->username;
            const char *find_pass = cur->password;
            if((strcmp((const char*)enc_pass,(const char*)find_user)==0) && (strcmp((const char*)enc_pass2,(const char*)find_pass)==0)){
              check = "accept";
            }
          }
        }
      //  puts("Hello");
      //  puts(check);
        write(sockfd2, "~", 1);

        write(sockfd2, check, MAX-1);
        if(strncmp(check, "accept", 6) !=0)
        {
          puts(" ### ERROR: user name and password does not match");
          exit(0);
        }
        char *filenamep = strcat(enc_pass, ".db");
        //free(enc_pass2);
        // bzero(enc_pass,sizeof(enc_pass));
        // bzero(enc_pass2,sizeof(enc_pass2));
        // bzero(check,strlen(check));
      }

      while(1)
      {
        //puts("MENU");
        close(sockfd);
        signal(SIGINT, sfault2);
        n=read(sockfd2, buff, MAX-1);
        buff[n]='\0';
        write (sockfd2, "~", 1);
        /*functions available for the user to input
        just sample commands*/

        //ADD ACCOUNT

        if(strncmp(buff, "/add_acc", 8) == 0)
        {
          puts("          [!] Creating new account...");
          n=read(sockfd2, website, MAX-1);
          website[n-1]='\0';
          printf("     |~| Website: ");
          puts(website);
          write (sockfd2, "~", 1);
          bzero(vlad,strlen(vlad));
          n=read(sockfd2, vlad, MAX-1);
          vlad[n-1]='\0';
          printf("     |~| Username: ");
          puts(vlad);
          write (sockfd2, "~", 1);

          n=read(sockfd2, password, MAX-1);
          password[n-1] = '\0';
          //printf("     |~| Password: ");

          //	puts(password);
          //puts(enc_pass);
          //puts(filenamep);
          write(sockfd2, "~", 1);
          //database check
          //char *filenamep = strcat(nick, ".db");

          //struct Connectionp *connp  = Database_openp(filenamep, action);
          int x = 0;
          int y = 0;
          struct Databasep *db = connp->db;

          for(x = 0; x < MAX_ROWS; x++) {
            struct Passwords *cur = &db->rows[x];

            if(cur->set) {
              y++;
            }
          }
          id = y;
          //char *filenamep = strcat(nick, ".db");

          struct Connectionp *connp  = Database_openp(enc_pass, action);
          Database_setp(connp, id, website, vlad, password);
          Database_writep(connp);

        }

        if(strncmp(buff, "/exit", 5) == 0)
        {
          close(sockfd);
          printf("%s\n", "***");
          // free(key);
          // free(enc_pass);
          bzero(key,sizeof(key));

          printf("%s",(char*)username_output);
          puts(" has disconnected");
          bzero(nick,strlen(nick));
          bzero(enc_pass,sizeof(enc_pass));
          bzero(enc_pass2,sizeof(enc_pass2));

          exit(0);
        }
        if(strncmp(buff, "/help", 5) == 0)
        {
          puts("          [!] user has viewed a help menu");
        }
        /*add print whole file of the user db*/

        if(strncmp(buff, "/view_passes", 12) == 0)
        {
          usleep(3000);
          bzero(website,MAX);
          n=read(sockfd2, website, MAX-1);
          website[n-1]='\0';
          printf("     |~| Account name requested: ");
          puts(website);
          account = website;
          int i = 0;
          struct Connectionp *connp  = Database_openp(enc_pass, action);
          struct Databasep *db = connp->db;
          for(i = 0; i < MAX_ROWS; i++) {
            struct Passwords *cur = &db->rows[i];

            if(cur->set) {
              const char *find = cur->account;
              if (strcmp((const char*)account,(const char*)find)==0){
                //	printf("%s %s\n",cur->username, cur->password);

                result = cur->password;
              }
            }
          }
          // if(strncmp(result,'\0',0) == 0)
          // {
          //   write(sockfd2,"null",4);
          //   puts("No Password Found");
          //   usleep(3000);
          //   write (sockfd2, "~", 1);
          //   continue;
          // }
          // else{
          //  puts(result);
          write(sockfd2, result, MAX-1);

          bzero(result,strlen(result));

          write (sockfd2, "~", 1);
          //}
        }
        //		puts("Here are your saved passwords");

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
  //   Database_closep(connp);
  //   Database_close(conn);
  // free(buff);
  // free(buff2);
  // free(nick);
  // free(password);
  // free(command);
}

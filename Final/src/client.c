
/*
client side of the password manager
regular sockets client
*/
#define _CLIENT_H_
#define SHA256_DIGEST_LENGTH 32
#include "enclosed.h"
#include <Windows.h>
int sockfd;


#define SIZE 8
//Passworddatabase

void SetClipboard(char *output)
{
  const size_t len = strlen(output) + 1;
  HGLOBAL hMem =  GlobalAlloc(GMEM_MOVEABLE, len);
  memcpy(GlobalLock(hMem), output, len);
  GlobalUnlock(hMem);
  OpenClipboard(0);
  EmptyClipboard();
  SetClipboardData(CF_TEXT, hMem);
  CloseClipboard();
}

/*called when user disconnects from server*/
void notime(int sig)
{
  if(sig == SIGINT)
  {
    puts("\ndiconnecting");
    exit(0);
  }
}
/*exiting the server*/
void notime2(int sig)
{
  if(sig == SIGINT)
  {
    SetClipboard("");
    write(sockfd, "/exit", 6);
    puts("\n closing");
    exit(0);
  }
}
/*Get charachter (for password not to be printed on the screen)*/
int getch() {
  struct termios oldt, newt;
  int ch;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  ch = getchar();
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  return ch;
}
/*SHA 256 hashing*/
void sha256(char *string, char outputBuffer[65])
{
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, string, strlen(string));
  SHA256_Final(hash, &sha256);
  int i = 0;
  for(i = 0; i < SHA256_DIGEST_LENGTH; i++)
  {
    sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
  }
  outputBuffer[64] = 0;
}
/*main method*/
int main(int argc, char** argv)
{
  struct sockaddr_in srv;
  struct hostent* bserv;
  int n;
  int m;
  char c;
  int con;
  int nport;
  char buff[MAX];
  char buff2[MAX];
  char buff_view[MAX];
  char nick[MAX];
  char username_output[MAX];
  char decrypted_password[MAX];
  char password[MAX];
  char hash_pass[MAX];
  //BF Password to use as a key
  //Crypto stuff
  unsigned char *enc_pass = calloc(MAX+1,sizeof(char));//use this buffer for the encryption while bzero it to clear it.
  unsigned char *enc_pass2 = calloc(MAX+1,sizeof(char));//use this buffer for the encryption while bzero it to clear it.

  BF_KEY* key = calloc(1,sizeof(BF_KEY));

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
  signal(SIGINT, notime);
  /*check for number of arguments when starting a client*/
  if(argc < 3)
  {
    puts("Use: [host] [nport]");
    exit(0);
  }
  nport = atoi(argv[2]);
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  /*check is socket is valid*/
  if (sockfd < 0)
  {
    puts("invalid socket");
    exit(0);
  }
  bserv = gethostbyname(argv[1]);
  if (bserv == NULL)
  {
    puts("no host");
    exit(0);
  }
  /*sockets for client*/
  memset(&srv, 0, sizeof(srv));
  srv.sin_family = AF_INET;
  srv.sin_port = htons(nport);
  memcpy(&srv.sin_addr.s_addr, bserv->h_addr, bserv->h_length);
  /*check if connection is successful*/
  con = connect(sockfd,  (struct sockaddr *)&srv, sizeof(srv));
  if(con<0)
  {
    printf("%s", "Could not connect to");
    puts(argv[1]);
    exit(0);
  }
  /*authentication process*/
  puts("                    WELCOME TO ENCLOSED");
  puts(" |~| Would you like to 'create' an account or 'login' using existing account");
  n = read(0, buff, MAX-6);
  n = write(sockfd, buff, MAX-6);
  signal(SIGINT, notime2);
  usleep(3000);
  /*while user doesn't input create or login display this error message*/
  while((strncmp(buff, "create", 6) != 0) && (strncmp(buff,"login",5) != 0))
  {
    puts(" [###] ERROR: Incorrect command, use: create or login");
    n = read(0, buff, MAX-6);
    n = write(sockfd, buff, MAX-6);
    signal(SIGINT, notime2);
    usleep(3000);
  }
  /*if user enters create go here*/
  //CREATE
  if(strncmp(buff, "create", 6) == 0)
  {
    puts(" |~| enter username: ");
    n = read(0, nick, MAX-6);
    if(n < 0)
    puts("Invalid socket");
    nick[n-1] = '\0';
    strncpy(username_output,nick,n);
    /*If the name entered is less than 3 ask user to re-enter it*/
    while(n < 3)
    {
      puts(" [###] ERROR: Invalid username, must be at least 3 letters long\n");
      puts(" |~| enter username: ");
      n = read(0, nick, MAX-6);
      if(n < 0)
      puts("Invalid socket");
      nick[n-1] = '\0';
    }

    bf_ivec_pos = 0;
    memcpy(bf_ivec, ivec, sizeof(ivec));
    /*encrypting information that we send*/
    BF_cfb64_encrypt(nick, enc_pass, n, &bf_key, bf_ivec, &bf_ivec_pos, 1);
    n = write(sockfd, enc_pass, MAX-6);
    signal(SIGINT, notime2);
    usleep(3000);
    n = read(sockfd, nick, MAX-6);

    if(nick[0] != '~')
    {
      puts("server down\n");
      exit(0);
    }

    /*check for password*/
    puts(" |~| enter password: ");

    int i = 0;
    for (;;) {
      c = getch();
      password[i] = c;
      i++;
      if(c == '\n')
      {
        //i=0;
        break;
      }
    }
    /*while password is less than5 charachters ask user to re-enter the password*/
    while(i<5)
    {
      puts(" [###] ERROR: Invalid password, must be atleast 5 charachters long");
      puts(" |~| enter password: ");
      i = 0;
      /*So that password doesn't show on screen*/
      for (;;) {
        c = getch();
        password[i] = c;
        i++;
        if(c == '\n')
        {
          //i=0;
          break;
        }
      }
    }
    password[i+1] = '\0';
    sha256(password ,hash_pass);
    BF_set_key(key,SIZE,(const unsigned char*)hash_pass);

    bf_ivec_pos = 0;
    memcpy(bf_ivec, ivec, sizeof(ivec));
    BF_cfb64_encrypt(hash_pass, enc_pass2, sizeof(hash_pass), &bf_key, bf_ivec, &bf_ivec_pos, 1);

    BF_set_key(key,SIZE,(const unsigned char*)hash_pass);   //Set key here
    n = write(sockfd, enc_pass2, MAX-6);
    signal(SIGINT, notime2);
    usleep(3000);
    n = read(sockfd, password, MAX-6);
    puts("          [!] Creating new account...");
    usleep(500000);
    puts(" |~| Account has been successfuly created");
    /*cleaning out some buffers*/
    bzero(enc_pass,sizeof(enc_pass));
    bzero(enc_pass2,sizeof(enc_pass2));
  }

  //LOGIN
  else if(strncmp(buff, "login", 5) == 0)
  {

    puts(" |~| enter username: ");
    /*cleaning out some buffers so that there is no problem*/
    bzero(nick,strlen(nick));
    bzero(enc_pass,sizeof(enc_pass));
    bzero(enc_pass2,sizeof(enc_pass2));
    n = read(0, nick, MAX-6);
    if(n < 0)
    puts("Invalid socket");
    nick[n-1] = '\0';
    strncpy(username_output,nick,n);

    bf_ivec_pos = 0;
    memcpy(bf_ivec, ivec, sizeof(ivec));
    BF_cfb64_encrypt(nick, enc_pass, sizeof(nick), &bf_key, bf_ivec, &bf_ivec_pos, 1);

    n = write(sockfd, enc_pass, MAX-6);
    signal(SIGINT, notime2);
    usleep(3000);
    n = read(sockfd, nick, MAX-6);
    /*checking for signal from the server*/
    if(nick[0] != '~')
    {
      puts("server down\n");
      exit(0);
    }
    bzero(password, MAX);
    /*check for password*/
    puts(" |~| enter password: ");
    int i = 0;
    for (;;) {
      c = getch();
      password[i] = c;
      i++;
      if(c == '\n')
      {
        //i=0;
        break;
      }
    }
    if(n < 0)
    puts("Invalid socket");


    //bzero(hash_pass, MAX);
    sha256(password ,hash_pass);
    BF_set_key(key,SIZE,(const unsigned char*)hash_pass);   //Set key here

    bf_ivec_pos = 0;
    memcpy(bf_ivec, ivec, sizeof(ivec));
    bzero(enc_pass2,sizeof(enc_pass2));
    BF_cfb64_encrypt(hash_pass, enc_pass2, sizeof(hash_pass), &bf_key, bf_ivec, &bf_ivec_pos, 1);

    n = write(sockfd, enc_pass2, MAX-6);
    bzero(enc_pass2,sizeof(enc_pass2));
    signal(SIGINT, notime2);
    usleep(3000);

    bzero(hash_pass,MAX);
    n = read(sockfd, hash_pass, MAX-6);
    if (hash_pass[0] == '~')
    memmove(hash_pass, hash_pass+1, strlen(hash_pass));
    puts("          [!] Logging in");
    //usleep(500000);
    bzero(password,strlen(password));
    n = read(sockfd, password, MAX-1);
    puts(hash_pass);
    puts(password);
    /*checking if server sent a positive signal so user can log in the system*/
    /*for now if user entered wrong command exit system*/
    if (strncmp(hash_pass, "accept", 5) != 0)
    {
      puts(" ### ERROR: user name and password does not match");
      exit(1);
    }
    /*cleaning up*/
    bzero(enc_pass,sizeof(enc_pass));
    bzero(enc_pass,sizeof(enc_pass));
  }
  /*introductory message*/
  printf("                    You are now logged in as %s \n", username_output);
  puts("          [!] What would you like to do?");
  puts("     |~| Your options include: ");
  puts("     |~| /help: to view this message again");
  puts("     |~| /exit: to quit the program");
  puts("     |~| /view_passes: to view all saved passwords");
  puts("     |~| /add_acc: add new account to your password database");
  puts("  |~|~adding more later on!");
  while(1)
  {
    n = read(0, buff, MAX-6);
    buff[n-1] = '\0';

    if(n <= 0)
    puts("Invalid read");

    /*check commands just simple commands in alpha*/

    write(sockfd,buff,MAX-6);

    //ADD_ACC
    /*sent to server so that it prepares the database*/
    if(strncmp(buff, "/add_acc", 8) == 0)
    {
      /*asking user for basic information to create an account*/
      puts(" |~| please input website for this account: ");
      bzero(buff2,MAX);
      n = read(0, buff2, MAX);
      if(n < 0)
      puts("Invalid socket");
      buff2[n-1] = '\0';

      n = write(sockfd, buff2, MAX-6);
      signal(SIGINT, notime2);
      usleep(3000);

      n = read(sockfd, buff2, MAX-6);
      bzero(buff2,MAX);
      puts(" |~| please input your username for that account: ");
      n = read(0, buff2, MAX-6);
      if(n < 0)
      puts("Invalid socket");
      buff2[n-1] = '\0';

      n = write(sockfd, buff2, MAX-6);
      signal(SIGINT, notime2);
      usleep(3000);
      n = read(sockfd, buff2, MAX-6);

      puts(" |~| please input your password for that account: ");
      bzero(buff2,strlen(buff2));
      //n = read(0, buff2, MAX-6);
      int i = 0;
      for (;;) {
        c = getch();
        buff2[i] = c;
        i++;
        if(c == '\n')
        {
          //i=0;
          break;
        }
      }
      if(n < 0)
      puts("Invalid socket");
      //buff2[n-1] = '\0';
      //Encryption for Password

      bf_ivec_pos = 0;
      memcpy(bf_ivec, ivec, sizeof(ivec));
      bzero(enc_pass2,sizeof(enc_pass2));
      BF_cfb64_encrypt(buff2, enc_pass2, sizeof(buff2), key, bf_ivec, &bf_ivec_pos, 1);

      //
      n = write(sockfd, enc_pass2, MAX-6);
      signal(SIGINT, notime2);
      usleep(50000);
      n = read(sockfd, buff2, MAX-6);
      puts("          [!] Account has been added");
      bzero(buff2,MAX);
      bzero(enc_pass2,sizeof(enc_pass2));
    }
    //EXIT
    /*when exiting the system we free buffers*/
    if(strncmp(buff, "/exit", 5) == 0)
    {
      //fflush((char*)buff);
      SetClipboard("");
      free(enc_pass);
      free(enc_pass2);
      free(key);
      bzero(key, MAX);
      puts("exiting server");
      //free(buff);
      //fflush(buff);
      exit(0);
    }

    //VIEW_PASSES

    if(strncmp(buff, "/view_passes", 12) == 0)
    {
      n = read(sockfd,buff2,MAX);
      //puts("Here are your saved passwords");
      puts("          [!] Accessing password database...");
      //usleep(500000);
      puts(" |~| Which passwords would you like to view?");
      puts(" |~| Enter the account name you want");
      bzero(buff2,strlen(buff2));
      n = read(0, buff2, MAX-6);
      if(n < 0)
      puts("Invalid socket");
      buff2[n-1] = '\0';
      n = write(sockfd, buff2, MAX-6);
      signal(SIGINT, notime2);
      usleep(3000);
      bzero(buff2,MAX);
      bzero(buff_view,strlen(buff_view));
      n = read(sockfd, buff_view, MAX-1);
      if(n < 0)
      puts("Invalid socket");
      buff_view[n-1] = '\0';

      /*remove the first charachter if its '~' due to signals that server sents
      to client*/
      if (buff_view[0] == '~')
      memmove(buff_view, buff_view+1, strlen(buff_view));


      bf_ivec_pos = 0;
      memcpy(bf_ivec, ivec, sizeof(ivec));
      //puts(buff_view);
      BF_cfb64_encrypt(buff_view, enc_pass2,strlen(buff_view), key, bf_ivec, &bf_ivec_pos, 0);

      //  puts(enc_pass2);
      /*Password is found in the database*/
      puts("PassWord Avaliable");
      //Decrypted Pass ^

      bzero(buff_view,MAX);
      //	  n = read(sockfd, buff, MAX-6);
      //puts(buff);

      //More Stuff
      /*setting clipboard to be unencrypted password*/
      SetClipboard(enc_pass2);
      bzero(enc_pass2,strlen(enc_pass2));
      n = read(sockfd, buff2, MAX-6);

    }
    /*help function*/
    if(strncmp(buff, "/help", 5) == 0)
    {
      puts("     |~| Your options include: ");
      puts("     |~| /help: to view this message again");
      puts("     |~| /exit: to quit the program");
      puts("     |~| /view_passes: to view all saved passwords");
      puts("     |~| /add_acc: add new account to your password database");
      puts("          |!| adding more later on!");

    }

    /*
    if(buff[0] != '~')
    {
    puts("server down");
    exit(0);
  }
  */
  /*if user inputs non of the above commands*/
  else if(((strncmp(buff, "/help", 5) != 0) && (strncmp(buff, "/exit", 5) != 0) && (strncmp(buff, "/add_acc", 8) != 0) && (strncmp(buff, "/view_passes", 12) != 0) && (strncmp(buff, "/change_mpass", 13) != 0)))
  {
    bzero(buff,MAX);
    bzero(buff2, MAX);
    //n = write(sockfd, buff, MAX-6);
    printf(" [###] ERROR: unknown command %s, type: help to view available actions \n", buff);
    //usleep(3000);
    //n = read(sockfd, buff, MAX-6);
  }
}
/*freeing buffer, password and nickname*/
free(buff);
free(password);
free(hash_pass);
free(nick);
free(buff2);
}

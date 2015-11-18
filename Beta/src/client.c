
/*
  client side of the password manager
  regular sockets client, upgrade to SSL in Beta
*/
#include "enclosed.h"
#define _CLIENT_H_
#define SHA256_DIGEST_LENGTH 32
#include <termios.h>
#include <unistd.h>
int sockfd;
/*called when user disconnects from server*/
void notime(int sig)
{
  if(sig == SIGINT)
    {
      puts("\ndiconnecting");
      exit(0);
    } 
}
void notime2(int sig)
{
  if(sig == SIGINT)
    {
      write(sockfd, "/exit", 6);
      puts("\n closing");
      exit(0);
    }
}
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
/*main*/
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
  char nick[MAX];
  char password[MAX];
  char hash_pass[MAX];
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
  if(strncmp(buff, "create", 6) == 0)
    {
      puts(" |~| enter username: ");
      n = read(0, nick, MAX-6);
      if(n < 0)
	puts("Invalid socket");
      nick[n-1] = '\0';
      while(n < 3)
	{
	  puts(" [###] ERROR: Invalid username, must be at least 3 letters long\n");
	  puts(" |~| enter username: ");
	  n = read(0, nick, MAX-6);
	  if(n < 0)
	    puts("Invalid socket");
	  nick[n-1] = '\0';
	}
      n = write(sockfd, nick, MAX-6);
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

      
      //n = read(0, password, MAX-6);
      if(n < 0)
	puts("Invalid socket");
      
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
      
      //password[n-1] = '\0';
      
      //      puts(password);
      while(i<5)
	{
	  puts(" [###] ERROR: Invalid password, must be atleast 5 charachters long");
	  puts(" |~| enter password: ");
	  i = 0;
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
      
      sha256(password ,hash_pass);
      n = write(sockfd, hash_pass, MAX-6);
      signal(SIGINT, notime2);
      usleep(3000);
      n = read(sockfd, password, MAX-6);
      puts("          [!] Creating new account...");
      usleep(500000);
      puts(" |~| Account has been successfuly created");
    }
  /*if user enters login go here*/
  else if(strncmp(buff, "login", 5) == 0)
    {
      puts(" |~| enter username: ");
      n = read(0, nick, MAX-6);
      if(n < 0)
	puts("Invalid socket");
      nick[n-1] = '\0';
     
      n = write(sockfd, nick, MAX-6);
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
      if(n < 0)
	puts("Invalid socket");
      /*
      while (password != "test123")
	{
	  puts(" ### ERROR: user name and password does not match");
	  puts(" ~ enter password: ");
	  //	  puts(password);
	  n = read(0, password, MAX-6);
	  password[n-1] = '\0';
	}
      */
      sha256(password ,hash_pass);
      n = write(sockfd, hash_pass, MAX-6);
      signal(SIGINT, notime2);
      usleep(3000);
      n = read(sockfd, password, MAX-6);
      puts("          [!] Logging in");
      usleep(500000);
      
    }
  /*introductory message*/
  printf("                    You are now logged in as %s \n", nick);
  puts("          [!] What would you like to do?");
  puts("     |~| Your options include: ");
  puts("     |~| /help: to view this message again");
  puts("     |~| /exit: to quit the program");
  puts("     |~| /view_passes: to view all saved passwords");
  puts("     |~| /change_mpass: to change master password");
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
      if(strncmp(buff, "/change_mpass", 13) == 0)
	{
	  puts("          [!] Accessing user database to change master password...");
	  usleep(500000);
	  puts(" |~| please input current master password: ");
	  
	  n = read(0, buff2, MAX-6);
	  if(n < 0)
	    puts("Invalid socket");
	  buff2[n] = '\0';
	  
	  n = write(sockfd, buff2, MAX-6);
	  signal(SIGINT, notime2);
	  usleep(300);
	  //n = read(sockfd, buff2, MAX-6);
	  puts(" |~| please input new master password: ");
	  n = read(0, buff2, MAX-6);
	  if(n < 0)
	    puts("Invalid socket");
	  buff2[n-1] = '\0';
	  
	  n = write(sockfd, buff2, MAX-6);
	  signal(SIGINT, notime2);
	  usleep(3000);
	  puts(" |~| please re-enter new master password: ");
	  n = read(0, buff2, MAX-6);
	  if(n < 0)
	    puts("Invalid socket");
	  buff2[n-1] = '\0';
	  
	  n = write(sockfd, buff2, MAX-6);
	  signal(SIGINT, notime2);
	  usleep(3000);
	  puts ("         [!] Changing master password...");
	  usleep(500000);
	  puts(" |~| Your master password has been successfully saved");
	  //n = read(sockfd, buff2, MAX-6);
	 	   bzero(buff2,MAX);
	  //puts("          ! your master password is now changed");
	}

      if(strncmp(buff, "/add_acc", 8) == 0)
	{
	  puts(" |~| please input website for this account: ");
	  
	  n = read(0, buff2, MAX-6);
	  if(n < 0)
	    puts("Invalid socket");
	  buff2[n-1] = '\0';
	  
	  n = write(sockfd, buff2, MAX-6);
	  signal(SIGINT, notime2);
	  usleep(3000);
	  n = read(sockfd, buff2, MAX-6);
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
	  n = read(0, buff2, MAX-6);
	  if(n < 0)
	    puts("Invalid socket");
	  buff2[n-1] = '\0';
	  
	  n = write(sockfd, buff2, MAX-6);
	  signal(SIGINT, notime2);
	  usleep(3000);
	  n = read(sockfd, buff2, MAX-6);
	  bzero(buff2,MAX);
	}

      if(strncmp(buff, "/exit", 5) == 0)
	{
	  //fflush((char*)buff);
	  puts("exiting server");
	  //free(buff);
	  //fflush(buff);
	  exit(0);
	}
      if(strncmp(buff, "/view_passes", 12) == 0)
	{
	  //puts("Here are your saved passwords");
	  puts("          [!] Accessing password database...");
	  usleep(500000);
	  puts(" |~| Which passwords would you like to view?");
	  puts(" |~| You can either view '/all' or individual passwords using website names");
	    n = read(0, buff2, MAX-6);
	  if(n < 0)
	    puts("Invalid socket");
	  buff2[n-1] = '\0';
	  n = write(sockfd, buff2, MAX-6);
	  signal(SIGINT, notime2);
	  usleep(3000);
	  n = read(sockfd, buff2, MAX-6);
	  if (strncmp(buff2, "/all", 4) == 0)
	    {
	      //TO DO
	      puts("print all passwords for this user");
	    }
	  
	  bzero(buff2,MAX);
	}
      if(strncmp(buff, "/help", 5) == 0)
	{
          puts("     |~| Your options include: ");
	  puts("     |~| /help: to view this message again");
	  puts("     |~| /exit: to quit the program");
	  puts("     |~| /view_passes: to view all saved passwords");
	  puts("     |~| /change_mpass: to change master password");
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

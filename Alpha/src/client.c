
/*
  client side of the password manager
  regular sockets client, upgrade to SSL in Beta
*/
#include "enclosed.h"
#define _CLIENT_H_
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
/*main*/
int main(int argc, char** argv)
{
  struct sockaddr_in srv;
  struct hostent* bserv;
  int n;
  int m;
  int con;
  int nport;	
  char buff[MAX];
  char nick[MAX];
  char password[MAX];
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
  printf("enter username: ");
  n = read(0, nick, MAX-6);
  if(n < 0)
      puts("Invalid socket");
  nick[n-1] = '\0';
  if(n <= 3)
      puts("Invalid username, must be at least 3 letters long\n");
     
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
  printf("%s", "enter password: ");
  n = read(0, password, MAX-6);
  if(n < 0)
    puts("Invalid socket");
  password[n-1] = '\0';
  if(n <= 5)
      puts("Invalid password, must be atleast 5 charachters long");
  n = write(sockfd, password, MAX-6);
  signal(SIGINT, notime2);
  usleep(3000);
  n = read(sockfd, password, MAX-6);

  /*introductory message*/
  puts("  ~~Welcome to Enclosed");
  puts("  ~~What would you like to do?");
  puts("  ~~Your options include: ");
  puts("  ~~/help -> to view this message again");
  puts("  ~~/exit -> to quit the program");
  puts("  ~~/view_passes -> to view all saved passwords");
  puts("  ~~/change_mpassword -> to change master password");
  puts("  ~~adding more later on");
  while(1)
    {
      n = read(0, buff, MAX-6);
      buff[n-1] = '\0';
      
      if(n <= 0)
	  puts("Invalid read");
      /*check commands just simple commands in alpha*/
      if(strncmp(buff, "/exit", 5) == 0)
	{
	  puts("exiting server");
	  //free(buff);
	  exit(0);
	}
      if(strncmp(buff, "/view_passes", 12) == 0)
	puts("Here are your saved passwords");
      if(strncmp(buff, "/help", 5) == 0)
	{
	  puts("Welcome to Enclosed");
	  puts("What would you like to do?");
	  puts("Your options include: ");
	  puts("help -> to view this message again");
	  puts("/exit -> to exit manager");
	  puts("adding more later on");
	}
      if(strncmp(buff, "/change_mpassword", 16) == 0)
	{
	  printf("please input current master password: ");
	  printf("please input new master password: ");
	}
      
      /*
      if(buff[0] != '~')
	{
	  puts("server down");
	  exit(0);
	}
      */
      /*if user inputs non of the above commands*/
      else
	{
	  n = write(sockfd, buff, MAX-6);
	  puts("unknown command, type: help to view available actions");
	  usleep(3000);
	  n = read(sockfd, buff, MAX-6);
	}
    }
  /*freeing buffer, password and nickname*/
  free(buff);
  free(password);
  free(nick);
  
}  


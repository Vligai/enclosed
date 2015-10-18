
/*
  pre cond: name of the machine server is running on, port number, message
  post cond: sends message to the server, where it is displayed
*/
#include "sockets.h"
#define _CLIENT_H_
int sockfd;
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
  if(argc < 3)
    {
      puts("Use: [host] [nport]");
      exit(0);
    }
  nport = atoi(argv[2]);
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
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
  con = connect(sockfd,  (struct sockaddr *)&srv, sizeof(srv));
  if(con<0)
    {
      printf("%s", "Could not connect to");
      puts(argv[1]);
      exit(0);
    }
  printf("enter username: ");
  n = read(0, nick, MAX-6);
  if(n < 0)
      puts("Invalid socket");
  nick[n-1] = '\0';
  if(n <= 0)
    {
      puts("Invalid nick\n");
      exit(0);
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
  printf("%s", "enter password: ");
  n = read(0, password, MAX-6);
  if(n < 0)
    puts("Invalid socket");
  password[n-1] = '\0';
  if(n <= 0)
    puts("Invalid password");
  n = write(sockfd, password, MAX-6);
  signal(SIGINT, notime2);
  usleep(3000);
  n = read(sockfd, password, MAX-6);


  puts("~Welcome to Enclosed");
  puts("~What would you like to do?");
  puts("~Your options include: ");
  puts("~help -> to view this message again");
  puts("~adding more later on");
  while(1)
    {
      n = read(0, buff, MAX-6);
      buff[n-1] = '\0';
      
      if(n <= 0)
	  puts("Invalid read");

      if(strncmp(buff, "view_passwords", 14) == 0)
	puts("Here is the password for");
      if(strncmp(buff, "help", 4) == 0)
	{
	  puts("Welcome to Enclosed");
	  puts("What would you like to do?");
	  puts("Your options include: ");
	  puts("help -> to view this message again");
	  puts("/exit -> to exit manager");
	  puts("adding more later on");
	}
      n = write(sockfd, buff, MAX-6);
      usleep(3000);
      n = read(sockfd, buff, MAX-6);
      if(buff[0] != '~')
	{
	  puts("server down");
	  exit(0);
	}
    }
    }  


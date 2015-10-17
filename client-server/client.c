/*
  pre cond: name of the machine server is running on, port number, message Vlad was here
  post cond: sends message to the server, where it is displayed
*/
#include "sockets.h"
#define _CLIENT_H_
int sockfd;
void notime(int sig)
{
  if(sig == SIGINT)
    my_panic("\ndiconnecting\n",1);
}
void notime2(int sig)
{
  if(sig == SIGINT)
    {
      write(sockfd, "/exit", 6);
      my_panic("\n closing \n", 1);
    }
}
int main(int argc, char** argv)
{
  struct sockaddr_in srv;
  struct hostent* bserv;
  int n;
  int con;
  int nport;	
  char buff[MAX];
  char nick[MAX];
  signal(SIGINT, notime);
  if(argc < 3)
    my_panic("Use: [host] [nport]\n", 1);
  nport = my_atoi(argv[2]);
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) 
    my_str("invalid socket\n");
  bserv = gethostbyname(argv[1]);
  if (bserv == NULL) 
    my_panic("no host\n", 1);
  memset(&srv, 0, sizeof(srv));
  srv.sin_family = AF_INET;
  srv.sin_port = htons(nport);
  memcpy(&srv.sin_addr.s_addr, bserv->h_addr, bserv->h_length);
  con = connect(sockfd,  (struct sockaddr *)&srv, sizeof(srv));
  if(con<0)
    {
      my_str("Could not connect to ");
      my_str(argv[1]);
      my_char('\n');
      exit(0);
    }
  my_str("Nick: ");
  n = read(0, nick, MAX-6);
  if(n < 0)
    my_str("Invalid socket\n");
  nick[n-1] = '\0';
  if(n <= 0)
    my_str("Invalid nick\n");
  n = write(sockfd, nick, MAX-6);
  signal(SIGINT, notime2);
  usleep(3000);
  n = read(sockfd, nick, MAX-6);
  if(nick[0] != '~')
    my_panic("server down\n", 1);
  while(1)
    {
      my_str("----> ");
      n = read(0, buff, MAX-6);
      buff[n-1] = '\0';
      if(n <= 0)
	  my_str("Invalid read\n");
      if(my_strncmp(buff, "/exit", 5) == 0)
	{
	my_str("exiting\n");
	write(sockfd, "/exit", 6);
	exit(0);
	}
      n = write(sockfd, buff, MAX-6);
      usleep(3000);
      n = read(sockfd, buff, MAX-6);
      if(buff[0] != '~')
	my_panic("server down\n", 1);
    }
}

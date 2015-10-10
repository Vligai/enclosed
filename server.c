/*
  pre cond: choose port number, which is more than 0 and less than 2^16-1
  post cond: prints string recied from the client
*/
#define _SERVER_H_
#include "sockets.h"
char nick[MAX];
void sfault1(int sig)
{
  if(sig == SIGINT)
    {
      usleep(3000);
      my_panic("\nclosing server\n", 0);
    }
}
void sfault2(int sig)
{
  if(sig == SIGINT)
    {
      usleep((getpid()%100+50)*10);
      my_str("*** ");
      my_str(nick);
      my_panic(" disconnected \n", 1);
    }
}
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
  signal(SIGINT, sfault1);
  if(argc < 2)
    my_panic("Use: nport\n", 0);
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0)
    my_panic("Invalid socket\n", 0);
  memset(&srv, 0, sizeof(srv));
  nport = my_atoi(argv[1]);
  if(nport <= 0 || nport > 65535)
    my_panic("Invalid port num\n", 0);
  srv.sin_family = AF_INET;
  srv.sin_port = htons(nport);
  srv.sin_addr.s_addr = INADDR_ANY;
  if(bind(sockfd, (struct sockaddr *)&srv, sizeof(srv)) < 0)
    my_panic("Invalid bind\n", 0);	
  listen(sockfd, 5);
  while(1)
    {
      scli = sizeof(cli);
      sockfd2 = accept(sockfd, (struct sockaddr *)&cli, &scli);
      if(sockfd2  < 0)
	my_panic("Invalid connection\n", 0);      
      if((pid = fork()) < 0)
	my_panic("Bad Fork\n", 0);
      if(pid==0)
	{
	  close(sockfd);			
	  n=read(sockfd2, nick, MAX-1);
	  nick[n-1]='\0';
	  my_str("*** ");
	  my_str(nick);
	  my_str(" is connected\n");
	  write (sockfd2, "~", 1);
	  while(1)
	    {
	      signal(SIGINT, sfault2);
	      n=read(sockfd2, buff, MAX-1);
	      buff[n]='\0';
	      write (sockfd2, "~", 1);
	      if(my_strncmp(buff, "/exit", 5) == 0)
		{
		  my_str("***");
		  my_str(nick);
		  my_str(" has disconnected\n");
		  exit(0);
		}
	      else if(my_strncmp(buff, "/me", 3) == 0)
		{
		  my_str("***");
		  my_str(nick);
		  my_char(' ');
		  my_str(&buff[4]);
		  my_char('\n');
		}
	      else if(my_strncmp(buff, "/nick", 5) == 0)
		{
		  my_str("***");
		  my_str(nick);
		  my_str(" was changed to ");
		  counter = 0;
		  while(counter <= n-6)
		    {
		      nick[counter] = buff[counter+6];
		      counter++;
		    }
		  my_str(nick);
		  my_char('\n');
		}
	      else
		{
		  my_str(nick);
		  my_str(": ");
		  my_str(buff);
		  my_char('\n');
		}
	    }
	}
      else
	close(sockfd2);
    }
}

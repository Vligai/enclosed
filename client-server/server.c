/*
  pre cond: choose port number, which is more than 0 and less than 2^16-1
  post cond: prints string recied from the client
*/
#define _SERVER_H_
#include "sockets.h"
char nick[MAX];
char password[MAX];
void sfault1(int sig)
{
  if(sig == SIGINT)
    {
      usleep(3000);
      puts("\nclosing server");
      exit(0);
    }
}
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
void compute_md5(char *str, unsigned char digest[16]) 
{
  MD5_CTX ctx;
  MD5_Init(&ctx);
  MD5_Update(&ctx, str, strlen(str));
  MD5_Final(digest, &ctx);
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
    {
      puts("Use: nport");
      exit(0);
    }
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0)
    {
      puts("Invalid socket");
      exit(0);
    }
  memset(&srv, 0, sizeof(srv));
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
      if(sockfd2  < 0)
	{
	  puts("Invalid connection");      
	  exit(0);
	}
      if((pid = fork()) < 0)
	{
	  puts("Bad Fork");
	  exit(0);
	}
      if(pid==0)
	{
	  close(sockfd);			
	  n=read(sockfd2, nick, MAX-1);
	  nick[n-1]='\0';
	  printf("%s",nick);
	  puts(" has connected to password manager, check for this username in the database");
	  write (sockfd2, "~", 1);
	  
	  n=read(sockfd2, password, MAX-1);
	  password[n-1] = '\0';
	  unsigned char md5_pass[16];
	  printf("%s", "Check if password in the database corresponds to this one: ");
	  compute_md5(password, md5_pass);
	  puts(md5_pass);
	  write(sockfd2, "~", 1);
	  
	  while(1)
	    {
	      signal(SIGINT, sfault2);
	      n=read(sockfd2, buff, MAX-1);
	      buff[n]='\0';
	      write (sockfd2, "~", 1);
	      if(strncmp(buff, "/exit", 5) == 0)
		{
		  printf("%s", "***");
		  printf("%s",nick);
		  puts(" has disconnected");
		  exit(0);
		}
	      else if(strncmp(buff, "/me", 4) == 0)
		{
		  printf("%s", "***");
		  printf("%s",nick);
		  printf("%s", " ");
		  puts(&buff[4]);
		}
	      else if(strncmp(buff, "/username", 9) == 0)
		{
		  printf("***");
		  printf(nick);
		  printf("%s", " has changed user name to ");
		  counter = 0;
		  while(counter <= n-6)
		    {
		      nick[counter] = buff[counter+6];
		      counter++;
		    }
		  puts(nick);
		}
	      else
		{
		  printf("%s",nick);
		  printf("%s", ": ");
		  puts(buff);
		}
	  
	    }
	  
	}
      else
	close(sockfd2);
    }
}

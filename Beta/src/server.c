/*
  server side of the enclosed-password manager
  uses regular sockets during alpha, will use SSL in Beta version
*/
#define _SERVER_H_
#include "enclosed.h"
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
char *md5_hash(char *str, unsigned char digest[16])
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
  char command[MAX];
  signal(SIGINT, sfault1);
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
	  n = read(sockfd2, command, MAX-1);
	  while((strncmp(command, "create", 6) != 0) && (strncmp(command,"login",5) != 0))
	    {
	      printf("user has input invalid command: %s/n", command);
	      n = read(sockfd2, command, MAX-1);
	    }
	  /*if user choose to create a new user go here*/
	  if (strncmp(command, "create", 6) == 0)
	    {
	      puts("Creating new user...");
	      n=read(sockfd2, nick, MAX-1);
	      nick[n-1]='\0';
	      printf("Username: ");
	      puts(nick);
	      
	      write (sockfd2, "~", 1);
	      n=read(sockfd2, password, MAX-1);
	      password[n-1] = '\0';
	      //	  unsigned char md5_pass[16];
	      printf("Password: ");
	      /*hashing master password to compare it with password inside the db*/
	      //md5_hash(password, md5_pass);
	      puts(password);
	      write(sockfd2, "~", 1);
	    }
	  /*nick - username user is going to log in with
	   check for username in he name of data fiels*/
	  else if (strncmp(command, "login", 5) == 0)
	    {
	      close(sockfd);			
	      n=read(sockfd2, nick, MAX-1);
	      nick[n-1]='\0';
	      printf("%s",nick);
	      puts(" has connected to password manager, check for this username in the database");
	      write (sockfd2, "~", 1);
	      
	      /*password - user master password used to log user in*/
	      n=read(sockfd2, password, MAX-1);
	      password[n-1] = '\0';
	      //	  unsigned char md5_pass[16];
	      printf("%s", "Check if password in the database corresponds to this one: ");
	      /*hashing master password to compare it with password inside the db*/
	      //md5_hash(password, md5_pass);
	      puts(password);
	      write(sockfd2, "~", 1);
	    }
	  while(1)
	    {
	      signal(SIGINT, sfault2);
	      n=read(sockfd2, buff, MAX-1);
	      buff[n]='\0';
	      write (sockfd2, "~", 1);
	      /*functions available for the user to input
	       just sample commands*/
	      if(strncmp(buff, "/change_mpassword", 16) == 0)
		{
		  puts ("  ~User requested to change master password...");

		  m=read(sockfd2, command, MAX-1);
		  command[n-1]='\0';
		  puts("Asking user for existing master password...");
		  printf("User entered existing master password: ");
		  puts(command);
		  write (sockfd2, "~", 1);
		  
		  m=read(sockfd2, command, MAX-1);
		  command[n-1]='\0';
		  puts("Asking user for desired new master password");
		  printf("User entered desired new  master password: ");
		  puts(command);
		  write (sockfd2, "~", 1);
		  
		  puts("#     User's master password now changed");

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
		puts("user has viewed a help menu");
		}
	      /*add print whole file of the user db*/
	      if(strncmp(buff, "/view_passes", 12) == 0)
		puts("Here are your saved passwords");

	      else
		{
		  /*if user input unknown command*/
		  puts("user has input unknown command");
		}
	    }	  
	}
      else
	close(sockfd2);
    }
}

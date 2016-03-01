# include "socket.h"

void traitement_signal(int sig)
{	
	int status;
	switch(sig){
		case(SIGCHLD):
		waitpid((pid_t)(-1), &status , WNOHANG |  WUNTRACED);	
		//printf("I killed my child :'(\n");
		//fflush(stdout);
		break;
	}
}

void initialiser_signaux(void){
	
	struct sigaction sa;	
	sa.sa_handler = traitement_signal;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if(sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		perror("sigaction(SIGCHLD)");
	}
	
	/*if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
	{
		perror("signal(SIGPIPE, SIG_IGN)");
	}*/
}


int main( int argc , char ** argv )
{
	int error_sig;
	int socket_fd ;
	char buffer[10];
	int socket_client ;
	char discut[1024];
	char * temp;
	temp = malloc(sizeof(char)*32);
	const char * message_bienvenue = " Bonjour , bienvenue sur mon serveur \n  " ;
	int pid;	
	initialiser_signaux();
	FILE *socket_file;
	if( (socket_fd = creer_serveur(8080)) != -1)
	{			
		while(1)
		{
			error_sig = 0; 
			socket_client = accept ( socket_fd , NULL , NULL ) ;
			socket_file = fdopen(socket_client,"w+");
			pid = fork();
			if(pid == -1)
			{
				perror("can't fork");
			}
			if(pid == 0)
			{
				if ( socket_client == -1 || socket_file == NULL)
				{
					perror ( "can't accept " );	
					error_sig == 1;
					return -1;
					/* traitement d ' erreur */
				}
				bzero(discut,1024);
				if(fgets(discut,1024,socket_file) != NULL)
				{	
					if(strstr(strncpy(temp,discut,3),"GET") > 0 && discut[3] == ' '){
						int valid = 0; int cpt = -1; int http = 0;
						while(valid < 3){
							cpt++;
							if(discut[cpt] == ' '){
								valid++;
							}else if(discut[cpt] == '\n' && valid == 2){
								valid++;
							}
						}
						if(valid == 3 && discut[cpt] != '\n'){			
							printf("It's a trap!\n");
							fflush(stdout);								
						}else{			
							temp = malloc(sizeof(char)*cpt);
							strncpy(temp,discut,cpt);
							int err;
   							regex_t preg;
   							const char *str_regex = "HTTP/[1].[0-1]";
   							err = regcomp (&preg, str_regex, REG_NOSUB | REG_EXTENDED);
   							if (err == 0)
   							{	
   								int match;	
   								match = regexec (&preg, temp, 0, NULL, 0);
      							regfree (&preg);
      							if(match == 0){
									printf("Message %s",discut);
									fflush(stdout);	
      							}else{
									printf("It's a trap!\n");
									fflush(stdout);
      							}
   							}
						}
					}else{			
						printf("It's a trap!\n");
						fflush(stdout);
					}
				}else{
					perror("fgets ");
				}
				exit(0);
			}
			close(socket_file);
			close(socket_client);
		}
	}
	close(socket_fd);
	return 0;
}

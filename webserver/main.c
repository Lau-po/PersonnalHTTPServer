# include <stdio.h>
# include <string.h>
# include <signal.h>
# include "socket.h"

void traitement_signal(int sig)
{
	fprintf(stdout,"catch %d",sig);
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
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
	{
		perror("signal(SIGPIPE, SIG_IGN)");
	}
}


int main( int argc , char ** argv )
{
	int error_sig;
	initialiser_signaux();
	int socket_fd ;
	char buffer[10];
	int socket_client ;
	char discut[256];	
	const char * message_bienvenue = " Bonjour , bienvenue sur mon serveur \n  " ;
	int pid;
	if( (socket_fd = creer_serveur(8080)) != -1)
	{	
		while(1)
		{
			error_sig = 0; 
			socket_client = accept ( socket_fd , NULL , NULL ) ;
			pid = fork();
			if(pid == -1)
			{
				perror("can't fork");
			}
			if(pid == 0)
			{
				if ( socket_client == -1)
				{
					perror ( "can't accept " );	
					close(socket_fd);
					return -1;
					/* traitement d ' erreur */
				}
				/* On peut maintenant dialoguer avec le client */
				write ( socket_client , message_bienvenue , strlen ( message_bienvenue ));
				while(error_sig == 0)
				{
					bzero(discut,256);
					read(socket_client,discut,sizeof(discut));
					if(write(socket_client,discut,sizeof(discut)) == -1)
					{
						error_sig = -1;
					}
				}
			}
			close(socket_client);
		}
	}
	close(socket_fd);
	return 0;
}

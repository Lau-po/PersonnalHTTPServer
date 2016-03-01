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
				//printf("Nouvelle connection\n");
				fflush(stdout);
				if ( socket_client == -1 || socket_file == NULL)
				{
					perror ( "can't accept " );	
					error_sig == 1;
					return -1;
					/* traitement d ' erreur */
				}else{
				/* On peut maintenant dialoguer avec le client */
				//write ( socket_client , message_bienvenue , strlen ( message_bienvenue ));
				fprintf(socket_file,message_bienvenue);
				fflush(socket_file);
				}
				while(error_sig == 0)
				{
					bzero(discut,1024);
					if(fgets(discut,1024,socket_file) != NULL)
					{	
						//printf("Passage à l'envoi\n %s \n",discut);fflush(stdout);
						if(fprintf(socket_file,"<Pawnee> %s", discut) < 0)
						{
							fflush(socket_file);
							error_sig = -1;
						}
					}else{
						error_sig = -1;
					}
					//printf("Tour de boucle\n");fflush(stdout);
				}
				//printf("Sortie de boucle\n");fflush(stdout);
				exit(0);
			}
			//printf("Fausse couche\n");fflush(stdout);
			close(socket_file);
			close(socket_client);
		}
	}
	close(socket_fd);
	return 0;
}

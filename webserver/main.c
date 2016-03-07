# include "socket.h"

void traitement_signal(int sig)
{	
	int status;
	switch(sig){
		case(SIGCHLD):
		waitpid((pid_t)(-1), &status , WNOHANG |  WUNTRACED);	
		printf("I killed my child :'(\n");
		fflush(stdout);
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

int test_request(FILE * file){
	int its_ok = 0; 
	int err;
	int match;
	char * discut;
	discut = malloc(sizeof(char)*1024);
	bzero(discut,1024);
	const char *get_regex = "GET /(\\S*) HTTP/1.[0-1]\r";	
	const char *host_regex = "Host: (\\S*):[0-9]{1,5}\r";
	regex_t get_reg;
	regex_t host_reg;

	err = regcomp (&get_reg, get_regex, REG_NOSUB | REG_EXTENDED);
	if (err != 0)
	{
		perror("regex1");
		return 500;
	}
	err = regcomp (&host_reg, host_regex, REG_NOSUB | REG_EXTENDED);
	if (err != 0)
	{
		perror("regex2");
		return 500;
	}
	if(fgets(discut,1024,file) !=NULL){
		match = regexec (&get_reg, discut, 0, NULL, 0);
		regfree (&get_reg);
		if(match == 0){
			bzero(discut,1024);
			while(fgets(discut,1024,file) != NULL && its_ok != 200){
				match = regexec (&host_reg, discut, 0, NULL, 0);
				regfree (&host_reg);
				if(match == 0){
					its_ok = 200;
				}
				bzero(discut,1024);
			}
			if(its_ok == 0){
				its_ok = 400;
			}
		}
	}else{
		its_ok = 400;
	}

	return its_ok;
}


int main( int argc , char ** argv )
{
	int error_sig;
	int socket_fd ;
	char buffer[10];
	int socket_client ;
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
				int test = test_request(socket_file);
				if(test == 200){
					fprintf(socket_file,"HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Length: 8\r\n\r\n200 OK");
					fflush(socket_file);
				}
				if(test == 400){
					fprintf(socket_file,"HTTP/1.1 400 Bad Request\r\nConnection: close\r\nContent-Length: 14\r\n\r\n400 Bad request");
					fflush(socket_file);
				}
				if(test == 500){
					printf("REGEX error\n");
					fflush(stdout);
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

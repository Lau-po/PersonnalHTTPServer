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
	int err, helper=1;
	int matchget=1,matchhost=1;
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
	while(helper > 0){
		fgets(discut,1024,file);
		if(matchget != 0){	
			matchget = regexec (&get_reg, discut, 0, NULL, 0);		
		}else{
			regfree (&get_reg);
		}
		if(matchhost != 0){	
			matchhost = regexec (&host_reg, discut, 0, NULL, 0);
		}else{			
			regfree (&host_reg);
		}
		printf("[%d] %s",helper,discut); helper ++;
		if (strcmp(discut,"\r\n") == 0 ){	
			helper = -1;	
		}
		bzero(discut,1024);
	}
	if(matchhost == 0 && matchget == 0){
		its_ok = 200;
	}else{
		its_ok = 400;
	}		
	printf("%d\n", its_ok);
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
				printf("return %d\n", test);
				if(test == 200){
					fprintf(socket_file,"HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Length: 8\r\n\r\n200 OK\r\n");
					fflush(socket_file);
				}
				if(test == 400){
					fprintf(socket_file,"HTTP/1.1 400 Bad Request\r\nConnection: close\r\nContent-Length: 17\r\n\r\n400 Bad request\r\n");
					fflush(socket_file);
				}
				if(test == 500){
					fprintf(socket_file,"HTTP/1.1 500 Internal Server Error\r\nConnection: close\r\nContent-Length: 27\r\n\r\n500 Internal Server Error\r\n");
					fflush(socket_file);
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

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
	int err, helper=1,dontpass = 0;
	int matchget=1,matchget2=1,matchhost=1;
	char * discut;
	discut = malloc(sizeof(char)*1024);
	bzero(discut,1024);
	const char *get404_regex = "GET /(\\S*) HTTP/1.[0-1]\r";
	const char *get200_regex = "GET / HTTP/1.[0-1]\r";	
	const char *host_regex = "Host: (\\S*):[0-9]{1,5}\r";	
	regex_t get1_reg;
	regex_t get2_reg;
	regex_t host_reg;

	err = regcomp (&get1_reg, get404_regex, REG_NOSUB | REG_EXTENDED);
	if (err != 0)
	{
		perror("get404_regex");
		return 500;
	}
	err = regcomp (&get2_reg, get200_regex, REG_NOSUB | REG_EXTENDED);
	if (err != 0)
	{
		perror("get200_regex");
		return 500;
	}
	err = regcomp (&host_reg, host_regex, REG_NOSUB | REG_EXTENDED);
	if (err != 0)
	{
		perror("host_regex");
		return 500;
	}
	while(helper > 0){
		fgets(discut,1024,file);
		if(matchget != 0){	
			matchget = regexec (&get1_reg, discut, 0, NULL, 0);		
		}
		if(matchget == 0 && dontpass == 0){
			dontpass = 1;
			matchget2 = regexec (&get2_reg, discut, 0, NULL, 0);
		}
		if(matchhost != 0){	
			matchhost = regexec (&host_reg, discut, 0, NULL, 0);
		}
		printf("[%d] %s",helper,discut); helper ++;
		if (strcmp(discut,"\r\n") == 0 || strcmp(discut,"\n") == 0 ){	
			helper = -1;	
		}
		bzero(discut,1024);
	}
	if(matchhost == 0 && matchget == 0){
		if(matchget2 == 0){
			its_ok = 200;
		}else{
			its_ok = 404;
		}
	}else{
		its_ok = 400;
	}		
	printf("hst %d - get1 %d - get %d - its_ok %d\n",matchhost,matchget,matchget2, its_ok);
	return its_ok;
}


int main( int argc , char ** argv )
{
	int error_sig;
	int socket_fd ;
	char buffer[10];
	int socket_client ;
	const char * message_bienvenue = "Bonjour petit pawnee, nous sommes heureux de te rencontrer.\nRetrouve nous sur https://github.com/Poulin-L/PersonnalHTTPServer\n  " ;
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
					fprintf(socket_file,"HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Length: %d\r\n\r\n%s\n",strlen(message_bienvenue),message_bienvenue);
					fflush(socket_file);
				}
				if(test == 404){
					fprintf(socket_file,"HTTP/1.1 404 Not Found\r\nConnection: close\r\nContent-Length: 15\r\n\r\n404 Not Found\r\n");
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

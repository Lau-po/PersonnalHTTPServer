//---- Include du fichier de dépendances
# include "socket.h"


//---- Definition des variables globales et repetitives

#define MAX_REQUESTS 5
#define MAX_BUFF_SIZE 1024
#define NOT_FOUND 404
#define BAD_REQUEST 400
#define OK 200
#define INTERNAL_SERVER_ERROR 500
#define SERVER_PORT 8080
#define ERR -1

//---- Recuperation du SIGCHILD si il apparait


void traitement_signal(int sig)
{	
	int status;
	switch(sig){
		case(SIGCHLD):
		waitpid((pid_t)(ERR), &status , WNOHANG |  WUNTRACED);	
		//printf("I killed my child :'(\n");
		//fflush(stdout);
		break;
	}
}

//---- Affectation d'une action personnalisée a SIGCHLD

void initialiser_signaux(void){
	
	struct sigaction sa;	
	sa.sa_handler = traitement_signal;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if(sigaction(SIGCHLD, &sa, NULL) == ERR)
	{
		perror("sigaction(SIGCHLD)");
	}
}

//---- Test de la request du client
// retourne le code HTTP

int test_request(FILE * file){
	int err, helper=1,dontpass = 0, timeout = 0,matchget=1,matchget2=1,matchhost=1,its_ok = 0;
	char * discut;
	char * entry;
	discut = malloc(sizeof(char)*MAX_BUFF_SIZE);
	entry = malloc(sizeof(char)*MAX_BUFF_SIZE);
	bzero(discut,MAX_BUFF_SIZE);
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
		return INTERNAL_SERVER_ERROR;
	}
	err = regcomp (&get2_reg, get200_regex, REG_NOSUB | REG_EXTENDED);
	if (err != 0)
	{
		perror("get200_regex");
		return INTERNAL_SERVER_ERROR;
	}
	err = regcomp (&host_reg, host_regex, REG_NOSUB | REG_EXTENDED);
	if (err != 0)
	{
		perror("host_regex");
		return INTERNAL_SERVER_ERROR;
	}
	while(helper > 0 && timeout < MAX_REQUESTS){
		entry = fgets(discut,MAX_BUFF_SIZE,file);
		printf("%s", discut);
		if(entry == NULL){
			timeout++;
		}
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
		if (strcmp(discut,"\r\n") == 0 || strcmp(discut,"\n") == 0 ){	
			helper = ERR;	
		}
		bzero(discut,MAX_BUFF_SIZE);
	}
	if(timeout > MAX_REQUESTS){
		exit(1);
	}
	if(matchhost == 0 && matchget == 0){
		if(matchget2 == 0){
			its_ok = OK;
		}else{
			its_ok = NOT_FOUND;
		}
	}else{
		its_ok = BAD_REQUEST;
	}		
	printf("Réponse : %d\n", its_ok);
	return its_ok;
}

void send_response(FILE * socket,int status){
	const char * message_bienvenue = "<html><body>Bonjour petit pawnee, nous sommes heureux de te rencontrer.<br/>Retrouve nous sur <a href=\"https://github.com/Poulin-L/PersonnalHTTPServer\">GitHub</a></body></html>\n  " ;
	if(status == OK){
		fprintf(socket,"HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Length: %d\r\n\r\n%s\n",strlen(message_bienvenue),message_bienvenue);
		fflush(socket);
	}
	if(status == NOT_FOUND){
		fprintf(socket,"HTTP/1.1 404 Not Found\r\nConnection: close\r\nContent-Length: 15\r\n\r\n404 Not Found\r\n");
		fflush(socket);
	}
	if(status == BAD_REQUEST){
		fprintf(socket,"HTTP/1.1 400 Bad Request\r\nConnection: close\r\nContent-Length: 17\r\n\r\n400 Bad request\r\n");
		fflush(socket);
	}
	if(status == INTERNAL_SERVER_ERROR){
		fprintf(socket,"HTTP/1.1 500 Internal Server Error\r\nConnection: close\r\nContent-Length: 27\r\n\r\n500 Internal Server Error\r\n");
		fflush(socket);
	}
}

//---- MAIN

int main( int argc , char ** argv )
{
	int error_sig, status, socket_fd, socket_client, pid;
	char buffer[10];FILE *socket_file;

	initialiser_signaux();

	if( (socket_fd = creer_serveur(SERVER_PORT)) != ERR) // Creation du serveur
	{			
		while(1)
		{
			error_sig = 0; 
			socket_client = accept ( socket_fd , NULL , NULL ) ;
			socket_file = fdopen(socket_client,"w+");
			pid = fork();
			if(pid == ERR)
			{
				perror("can't fork");
			}
			if(pid == 0)
			{
				if ( socket_client == ERR || socket_file == NULL)
				{
					perror ( "can't accept " );	
					error_sig == 1;
					return ERR;
				}
				printf("Nouvelle connnection\n");

				status = test_request(socket_file);

				send_response(socket_file, status);

				printf("Une connection va fermer\n");	

				exit(0);
			}

			close(socket_file);
			close(socket_client);
		}
	}
	close(socket_fd);
	return 0;
}

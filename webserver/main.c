# include <stdio.h>
# include <string.h>
# include "socket.h"


int main( int argc , char ** argv )
{
	int socket_fd = creer_serveur(8080);	
	int socket_client ;
	char discut[256]; 
	socket_client = accept ( socket_fd , NULL , NULL ) ;
	/*while(1){*/
	if ( socket_client == -1)
	{
		perror ( " accept " );	
		close(socket_fd);
		return -1;
		/* traitement d ' erreur */
	}
	/* On peut maintenant dialoguer avec le client */
	const char * message_bienvenue = " Bonjour , bienvenue sur mon serveur \n  " ;
	write ( socket_client , message_bienvenue , strlen ( message_bienvenue ));
	/*}*/
	while(1){
		bzero(discut,256);
		read(socket_client,discut,sizeof(discut));
		write(socket_client,discut,sizeof(discut));
	}
	close(socket_fd);
	return 0;
}

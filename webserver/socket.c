/*
* Authors:
*- Levoye Camille - Camille.Levoye@Etudiant.univ-lille1.fr
*- Poulin Laurent - Laurent.Poulin@Etudiant.univ-lille1.fr
*------
* Creation d'un serveur HTTP M4101
*------
*/

# include <stdio.h>
# include <string.h>
# include "socket.h"

#define ERR -1

int creer_serveur(int port){

	int socket_serveur,optval = 1;

	struct sockaddr_in saddr ;
	saddr.sin_family = AF_INET ; /* Socket ipv4 */
	saddr.sin_port = htons (port); /* Port d ' écoute */
	saddr.sin_addr.s_addr = INADDR_ANY ; /* écoute sur toutes les interfaces */
	socket_serveur = socket ( AF_INET , SOCK_STREAM , 0);

	if ( socket_serveur == ERR)
	{
		perror ( " socket_serveur " );
		return ERR;
		/* traitement de l ' erreur */
	}
	if(setsockopt(socket_serveur, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == ERR){
		perror("Can't set REUSEADDR");
		close(socket_serveur);
		return ERR;
	}
	if ( bind ( socket_serveur , ( struct sockaddr *)& saddr , sizeof ( saddr )) == ERR)
	{
		perror ( " bind socket_serveur " );	
		close(socket_serveur);
		return ERR;
		/* traitement de l ' erreur */
	}
	if ( listen ( socket_serveur , 10) == ERR)
	{
		perror ( " listen socket_serveur " );	
		close(socket_serveur);
		return ERR;
		/* traitement d ' erreur */
	}
	/* Utilisation de la socket serveur */
	return socket_serveur;
}
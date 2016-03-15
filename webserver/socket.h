/*
* Authors:
*- Levoye Camille - Camille.Levoye@Etudiant.univ-lille1.fr
*- Poulin Laurent - Laurent.Poulin@Etudiant.univ-lille1.fr
*------
* Creation d'un serveur HTTP M4101
*------
*/

#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
# include <stdio.h>
# include <string.h>
# include <signal.h>
# include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>

#ifndef __SOCKET_H__
#define __SOCKET_H__


int creer_serveur(int port);
#endif
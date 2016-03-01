#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
# include <stdio.h>
# include <string.h>
# include <signal.h>
# include <sys/wait.h>
#include <stdlib.h>

#ifndef __SOCKET_H__
#define __SOCKET_H__


int creer_serveur(int port);
#endif
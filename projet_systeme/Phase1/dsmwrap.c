#include "common_impl.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SAI struct sockaddr_in
#define SA struct sockaddr
#define IA struct in_addr
#define HE struct hostent

int main(int argc, char **argv)
{
  /* processus intermediaire pour "nettoyer" */
  /* la liste des arguments qu'on va passer */
  /* a la commande a executer vraiment */

  char *hostIP = argv[1];
  char *port_num = argv[2];
  char * ultime[argc - 3]; // arguments finaux pour execvp
  int lenmachmax = 100; // taille max d'un nom, arbitraire
  char machine_name[lenmachmax]; // nom de la machine


  int sockcon; // socket pour se connecter à l'host
  int socklisten; // socket d'écoute

  int test_connect; // essai de connexion
  int name_size = 0; //taille effective du nom
  pid_t PID; // pid du processus
  int port_dsm; // num de port de la socket pour se connecter aux autres machines
  int prop_dsm = 0; // proprietes pour creation socket

  int len_port = 6;
  int len_pid = 6;

  int i;
  for (i = 0; i< argc - 3; i++){
    ultime[i] = argv[3 + i];
  }
  ultime[argc-3] = NULL;



  /* creation d'une socket pour se connecter au */
  /* au lanceur et envoyer/recevoir les infos */
  /* necessaires pour la phase dsm_init */
  //HE* IPstock;
  //IA* addr_stock;
  SAI launcher;

  bzero(&launcher, sizeof(SAI));
  //IPstock = gethostbyname(hostIP);
  //addr_stock = (IA*) IPstock->h_addr_list[0];


  inet_aton(hostIP,&launcher.sin_addr);
  launcher.sin_family = AF_INET;
  launcher.sin_port = htons(atoi(port_num));
  socklen_t len = sizeof(SAI);


  sockcon=socket(AF_INET,SOCK_STREAM,0);
   do{
    test_connect = connect(sockcon, (SA *)&launcher, len);
  }while ( test_connect == -1 );


  /*  Envoi du nom de machine au lanceur */
  if (-1 == gethostname(machine_name, lenmachmax)){
    perror("gethostname");
    exit(EXIT_FAILURE);
  }

  while(machine_name[name_size] != '\0'){
    name_size = name_size + 1;
  }

  //do_write(sockcon, &name_size, sizeof(int)); convertir name_size en char
  do_write(sockcon, &machine_name, name_size*sizeof(int));

  /*  Envoi du pid au lanceur */
  PID = getpid();
  char PIDchar[len_pid];
  sprintf(PIDchar, "%d", PID);
  do_write(sockcon, &PIDchar, len_pid*sizeof(char));

  /*  Creation de la socket d'ecoute pour les */
  /* connexions avec les autres processus dsm */

  socklisten = creer_socket(prop_dsm, &port_dsm);

  /* Envoi du numero de port au lanceur */
  /* pour qu'il le propage à tous les autres */
  /* processus dsm */

  char portchar[len_port];
  sprintf(portchar, "%d", port_dsm);
  do_write(sockcon, &portchar, len_port*sizeof(char));

  //fflush(stdout);
  /* on execute le programme voulu */
  execvp(ultime[0],ultime);

  return EXIT_SUCCESS;
}

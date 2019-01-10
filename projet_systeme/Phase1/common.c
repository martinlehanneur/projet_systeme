#include "common_impl.h"
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define SA struct sockaddr
#define SAI struct sockaddr_in
#define LEN 256
#define N 2

void error(const char *msg)
{
  perror(msg);
  exit(1);
}


int do_bind(int sock, SAI s_in, int addrlen)
{
  int b= bind(sock,(SA *) &s_in, sizeof(s_in));
  if (b == -1){
    error("bind");
  }
  return b;
}

int do_accept(int sock, SAI* sin_client, int adrlen)
{
  socklen_t addrsize = sizeof(sin_client);
  int a= accept(sock,(SA *) sin_client, &addrsize);
  if (a == -1){
    error("Error accept");
  }
  return a;
}


int do_read(int sock, char* buf,int len)
{
  int r=read(sock,buf,len);
  if (r == -1){
    error("Error read");
  }
  return r;
}

ssize_t do_write(int fd, const void *buf, size_t count)
{
  ssize_t w= write(fd,buf,count);
  if (w == -1){
    error("Error write");
  }
  return w;
}

int creer_socket(int prop, int *port_num)
{
  SAI saddr_in;
  //clean the serv_add structure
  memset( &saddr_in,0,sizeof(saddr_in) );
  //internet family protocol
  saddr_in.sin_family = AF_INET;
  //we bind to any ip form the host
  saddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
  //we bind on the tcp port specified
  saddr_in.sin_port = htons(0);
  //create the socket, check for validity!
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  socklen_t len = sizeof(SA*); //struct sockaddr size


  if (fd == -1)
  {
    error("ERROR: socket unbound.");
    exit(EXIT_FAILURE);
  }
  do_bind(fd, saddr_in , len);
  // if (-1 == bind(fd, (struct sockaddr *)&s_in, sizeof(s_in))){
  //   perror("bind");
  //   exit(EXIT_FAILURE);
  // }

  *port_num = ntohs(saddr_in.sin_port);

  if(getsockname(fd, (struct sockaddr*)(&saddr_in), &len) == -1){
    perror("error with getsockname");
    exit(EXIT_FAILURE);
  }

  /* fonction de creation et d'attachement */
  /* d'une nouvelle socket */
  /* renvoie le numero de descripteur */
  /* et modifie le parametre port_num */

  return fd;
}

/* Vous pouvez ecrire ici toutes les fonctions */
/* qui pourraient etre utilisees par le lanceur */
/* et le processus intermediaire. N'oubliez pas */
/* de declarer le prototype de ces nouvelles */
/* fonctions dans common_impl.h */

int compte_lignes(FILE *fichier){
  rewind(fichier);
  int caractere;
  int n_ligne=0;
  caractere=0;
  while (caractere!=EOF){
    caractere=fgetc(fichier);
    if(caractere=='\n'){
      n_ligne=n_ligne+1;
    }
  }
  return n_ligne;
}

char * tableau_mot(FILE *fichier, int n_ligne){
  rewind(fichier);
  int i;
  int taille_max=50;
  char chaine[taille_max];
  char * tableau[n_ligne];
  for(i=0;i<n_ligne;i++){
    tableau[i] = malloc(sizeof(char)*taille_max);
    bzero(tableau[i],sizeof(char)*taille_max);
    fgets(chaine, taille_max, fichier);
    strcpy(tableau[i], chaine);
  }
  fclose(fichier);
  return tableau;
}

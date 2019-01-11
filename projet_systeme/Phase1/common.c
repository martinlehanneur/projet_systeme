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


int do_bind(int sock,  SAI addr)
{
  socklen_t addrlen = sizeof (SAI);
  int b= bind(sock, (SA *)&addr, addrlen);
  if (b == -1){
    error("bind");
  }
  return b;
}

int do_accept(int sock, SAI addr)
{
  socklen_t addrlen = sizeof(SAI);
  int a = accept(sock,(SA *)&addr, &addrlen);
  if (a == -1){
    error("Error accept");
  }
  return a;
}


void do_read(int sock, char *buf,int len)
{
  ssize_t r = read(sock,buf,len);
  if (r == -1){
    error("Error read");
  }
}

void do_write(int fd, char *buf, int count)
{
  ssize_t w = write(fd,buf,count);
  if (w == -1){
    error("Error write");
  }
}

int creer_socket(int prop, int *port_num)
{
  SAI saddr_in;
  //clean the serv_add structure
  bzero(&saddr_in, sizeof(SAI));
  //internet family protocol
  saddr_in.sin_family = AF_INET;
  //we bind to any ip form the host
  saddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
  //we bind on the tcp port specified
  saddr_in.sin_port = htons(0);
  //create the socket, check for validity!
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  socklen_t len = sizeof(SA);


  if (fd == -1)
  {
    error("ERROR: socket unbound.");
    exit(EXIT_FAILURE);
  }
  do_bind(fd, saddr_in);


  *port_num = ntohs(saddr_in.sin_port);

  if(getsockname(fd, (SA*)(&saddr_in), &len) == -1){
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

void tableau_mot(FILE *fichier, int n_ligne, char **tableau){
  rewind(fichier);
  int i;
  int taille_max=50;
  char chaine[taille_max];
  for(i=0;i<n_ligne;i++){
    tableau[i] = malloc(sizeof(char)*taille_max);
    bzero(tableau[i],sizeof(char)*taille_max);
    fgets(chaine, taille_max, fichier);
    strcpy(tableau[i], chaine);
  }
  fclose(fichier);
}

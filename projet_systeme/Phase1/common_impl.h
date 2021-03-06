#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>


/* autres includes (eventuellement) */

#define ERROR_EXIT(str) {perror(str);exit(EXIT_FAILURE);}

/* definition du type des infos */
/* de connexion des processus dsm */
struct sockaddr;
struct sockaddr_in;
struct dsm_proc_conn  {
   int rank;
   /* a completer */
};
typedef struct dsm_proc_conn dsm_proc_conn_t;

/* definition du type des infos */
/* d'identification des processus dsm */
struct dsm_proc {
  pid_t pid;
  dsm_proc_conn_t connect_info;
};
typedef struct dsm_proc dsm_proc_t;
int do_bind(int sock, struct sockaddr_in addr);
int do_accept(int sock, struct sockaddr_in addr);
void do_read(int sock, char* buf,int len);
void do_write(int fd, char *buf, int count);

int creer_socket(int type, int *port_num);

int compte_lignes(FILE *fichier);
void tableau_mot(FILE *fichier, int n_ligne, char **tableau);

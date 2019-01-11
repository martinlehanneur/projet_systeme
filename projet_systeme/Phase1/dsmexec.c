#include "common_impl.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

//       pid = fork();
//       if(pid == -1) ERROR_EXIT("fork");
//
//       if (pid == 0) { /* fils */
//         HE* stock;
//         IA* addr_stock;
//         /* redirection stdout */
//         close(pipeout[0]);
//         dup2(pipeout[1], STDOUT_FILENO);//pas indispensable
//         /* redirection stderr */
//         close(pipeerr[0]);
//         dup2(pipeerr[1], STDOUT_FILENO);//pas indispensable
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SA struct sockaddr
#define IA struct in_addr
#define SAI struct sockaddr_in
#define HE struct hostent
#define INFO struct info




/* variables globales */

/* un tableau gérant les infos d'identification */

struct info{
  char name[20];
  char PID[6];
  char port[6];
  int rank;
};
/* des processus dsm */
dsm_proc_t *proc_array = NULL;

/* le nombre de processus effectivement crees */
volatile int num_procs_creat = 0;

void usage(void)
{
  fprintf(stdout,"Usage : dsmexec machine_file executable arg1 arg2 ...\n");
  fflush(stdout);
  exit(EXIT_FAILURE);
}

void sigchld_handler(int sig)
{
  waitpid(-1, NULL, WNOHANG);
  num_procs_creat--;
  /* on traite les fils qui se terminent */
  /* pour eviter les zombies */

}


int main(int argc, char *argv[])
{
  if (argc < 3){
    usage();
  }
  else{

    //initialisation de toutes les variables



    pid_t pid;  //pid du processus
    int num_procs = 0; //nb de machines à utiliser = nb de lignes dans machine_file
    // int i;
    FILE *fichier=NULL;
    int socklisten; //socket d'écoute
    int numport; //numéro du port de la socket d'écoute
    int name_len_max = 64; //taille maximale du nom de la machine
    char machine_name[name_len_max];
    int sizemachname;
    int len_port = 6;
    int len_pid = 6;

    struct sigaction act_chld;
    char **argssh = malloc((argc + 5)*sizeof(char *));

    int sockaccept = -1; //détermine si la socket est acceptée
    SAI s_in; //

    // pipe * pipeerr=NULL;
    // pipe * pipeout=NULL;

    bzero(&act_chld, sizeof(struct sigaction));
    act_chld.sa_handler = sigchld_handler;
    sigaction(SIGCHLD, &act_chld, &act_chld); // en troisième argument, stockage de la valeur du signal, ou NULL pour ne rien stocker

    /* lecture du fichier de machines */
    fichier = fopen(argv[1],"r"); //ouverture de machine_file
    /* 1- on recupere le nombre de processus a lancer */
    int nb_processus = compte_lignes(fichier);
    fprintf(stdout, "%d\n",nb_processus );
      int taille_max=50;
    /* 2- on récupère les noms des machines : le nom de la machine est un des elements d'identification */
    char ** tableau_noms= malloc(nb_processus*sizeof(char)*taille_max);

    tableau_noms= tableau_mot(fichier, nb_processus);

    INFO processus[nb_processus]; //stockage des machines
    INFO machine_encours;
    /* creation de la socket d'écoute */
    socklisten = creer_socket(0, &numport);
    /* + écoute effective */
    listen(socklisten, nb_processus);

    /* creation des fils */
    for(int i = 0; i < nb_processus ; i++) {

      /* creation du tube pour rediriger stdout */
      int pipeout[2];
      pipe(pipeout);
      /* creation du tube pour rediriger stderr */

      int pipeerr[2];
      pipe(pipeerr);

      pid = fork();
      if(pid == -1) ERROR_EXIT("fork");

      if (pid == 0) { /* fils */
        HE* stock;
        IA* addr_stock;
        /* redirection stdout */
        close(pipeout[0]);
        dup2(pipeout[1], STDOUT_FILENO);//pas indispensable
        /* redirection stderr */
        close(pipeerr[0]);
        dup2(pipeerr[1], STDOUT_FILENO);//pas indispensable
        /* Creation du tableau d'arguments pour le ssh */
        argssh[0] = "ssh";
        argssh[1] = tableau_noms[i];
        /*argssh[2] = adresse du fichier à exécuter*/

        gethostname(machine_name, name_len_max);
        stock = gethostbyname(machine_name);
        addr_stock = (IA*) stock->h_addr_list[0];

        argssh[3] = malloc(len_pid*sizeof(char));
        sprintf(argssh[3], "%s", inet_ntoa(*addr_stock)); //adresse IP
        argssh[4] = malloc(len_port*sizeof(char));
        sprintf(argssh[4], "%d", numport);

        // /* création de la nouvelle chaine d'argument pour le ssh */
        // for (k = 1; k < argc - 1; k++){
        //   argssh [4+k] = argv[k+1];
        // }
        // argssh[argc+k] = NULL;

        /* jump to new prog : */
        execvp("ssh", argssh);

      } else  if(pid > 0) { /* pere */
        /* fermeture des extrêmités des tubes non utiles */
        num_procs_creat++;
        close(pipeout[1]);
        close(pipeerr[1]);
      }
    }

    for (int j=0 ; j<num_procs ; j++){

      bzero(&machine_encours, sizeof(INFO));
      bzero(&s_in, sizeof(SAI));
      socklen_t size = sizeof(s_in);

      while (sockaccept == -1){
        sockaccept = do_accept(socklisten, (SA*)&s_in, &size);
      }

      /* envoi du nombre de processus aux processus dsm*/

      do_read(sockaccept, &sizemachname, sizeof(int));
      do_read(sockaccept, machine_encours.name, sizemachname*sizeof(char));
      do_read(sockaccept, machine_encours.PID, len_pid*sizeof(char));
      do_read(sockaccept, machine_encours.port, len_port*sizeof(char));

    /* envoi des rangs aux processus dsm */

    for (int p = 0; p < nb_processus; p++){
      if (0 == strncmp(machine_encours.name, tableau_noms[p], strlen(machine_encours.name))) {
        machine_encours.rank = p;
      }
      processus[machine_encours.rank] = machine_encours;
    }

    /* envoi des infos de connexion aux processus */
    for (int p = 0; p < nb_processus; p++){
      printf("Rank: %i | Name: %s | PID: %s | Port: %s\n", processus[p].rank, processus[p].name, processus[p].PID, processus[p].port);
}
}
    /* gestion des E/S : on recupere les caracteres */
    /* sur les tubes de redirection de stdout/stderr */
    /* while(1)
    {
    je recupere les infos sur les tubes de redirection
    jusqu'à ce qu'ils soient inactifs (ie fermes par les
    processus dsm ecrivains de l'autre cote ...)

  };
  */

  /* on attend les processus fils */

  /* on ferme les descripteurs proprement */

  /* on ferme la socket d'ecoute */
}
exit(EXIT_SUCCESS);
}

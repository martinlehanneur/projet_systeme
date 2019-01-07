#include "common_impl.h"




/* variables globales */

/* un tableau gérant les infos d'identification */

typedef struct info{
  char name[20];
  char IP[20];
  char date[20];
  int num_sock;
}info;
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
  /* on traite les fils qui se terminent */
  /* pour eviter les zombies */
  waitpid(-1, NULL, WNOHANG);
  num_procs_creat--;
  /* on traite les fils qui se terminent */
  /* pour eviter les zombies */
  waitpid(-1,NULL,WNOHANG);
  num_procs_creat=num_procs_creat-1;

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
    int i;
    FILE *fichier=NULL;
    fichier= fopen(argv[1],"r"); //ouverture de machine_file
    //pipe_t * pipe_stderr=NULL;
    //pipe_t * pipe_stdout=NULL;

    /* Mise en place d'un traitant pour recuperer les fils zombies*/
    /* XXX.sa_handler = sigchld_handler; */

    /* Mise en place d'un traitant pour recuperer les fils zombies*/
    /* XXX.sa_handler = sigchld_handler; */

    /* lecture du fichier de machines */
    fichier = fopen(argv[1],"r");
    /* 1- on recupere le nombre de processus a lancer */
    int nb_processus= compte_lignes(fichier);
    fprintf(stdout, "%d\n",nb_processus );
    /* 2- on recupere les noms des machines : le nom de */
    /* la machine est un des elements d'identification */


    /* creation de la socket d'ecoute */
    /* + ecoute effective */

    /* creation des fils */
    for(i = 0; i < num_procs ; i++) {

      /* creation du tube pour rediriger stdout */
      int pipeout[2];
      pipe(pipeout);
      /* creation du tube pour rediriger stderr */
      int pipeerr[2];
      pipe(pipeerr);

      pid = fork();
      if(pid == -1) ERROR_EXIT("fork");

      if (pid == 0) { /* fils */

        /* redirection stdout */
        close(pipeout[0]);
        dup2(pipeout[1], STDOUT_FILENO);//pas indispensable
        /* redirection stderr */
        close(pipeerr[0]);
        dup2(pipeerr[1], STDOUT_FILENO);//pas indispensable
        /* Creation du tableau d'arguments pour le ssh */

        /* jump to new prog : */
        /* execvp("ssh",newargv); */

      } else  if(pid > 0) { /* pere */
        /* fermeture des extremites des tubes non utiles */
        num_procs_creat++;
      }
    }


    for(i = 0; i < num_procs ; i++){

      /* on accepte les connexions des processus dsm */

      /*  On recupere le nom de la machine distante */
      /* 1- d'abord la taille de la chaine */
      /* 2- puis la chaine elle-meme */

      /* On recupere le pid du processus distant  */

      /* On recupere le numero de port de la socket */
      /* d'ecoute des processus distants */
    }

    /* envoi du nombre de processus aux processus dsm*/

    /* envoi des rangs aux processus dsm */

    /* envoi des infos de connexion aux processus */

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

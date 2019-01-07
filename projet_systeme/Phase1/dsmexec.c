#include "common_impl.h"

/* variables globales */

/* un tableau gerant les infos d'identification */

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
  waitpid(-1,NULL,WNOHANG);
  num_procs_creat=num_procs_creat-1;

}
typedef struct info_des_machine { // stocke les infos des differentes machines
  char nom_machine[256];
  char pid[6];
  char port[6];
  int rang;
}info_des_machine;

int main(int argc, char *argv[])
{

  if (argc < 3){
    usage();
  }

  else {

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

    /* lecture du fichier de machines */
    num_procs= compte_lignes(fichier);
    /* 1- on recupere le nombre de processus a lancer */
    printf("%d\n",num_procs );
    /* 2- on recupere les noms des machines : le nom de */
    /* la machine est un des elements d'identification */
    tableau_mot(fichier,num_procs);


    /* creation de la socket d'ecoute */
    /* + ecoute effective */

    /* creation des fils */
    for(i = 0; i < num_procs ; i++) {

      /* creation du tube pour rediriger stdout */
      int pipe_stdout[2];
      pipe(pipe_stdout);
      /* creation du tube pour rediriger stderr */
      int pipe_stderr[2];
      pipe(pipe_stderr);

      /* creation des fils */
      pid = fork();

      if(pid == -1) {
        ERROR_EXIT("problème de fork");
      }

      if (pid == 0) { /* fils */

        /* redirection stdout */
        close(pipe_stdout[0]);
        dup2(pipe_stdout[1],STDOUT_FILENO);
        /* redirection stderr */
        close(pipe_stderr[0]);
        dup2(pipe_stderr[1],STDOUT_FILENO);
        /* Creation du tableau d'arguments pour le ssh */

        /* jump to new prog : */
        /* execvp("ssh",newargv); */

      } else  if(pid > 0) { /* pere */
        /* fermeture des extremites des tubes non utiles */
        close(pipe_stdout[1]);
        close(pipe_stderr[1]);
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

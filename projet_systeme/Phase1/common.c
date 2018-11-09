#include "common_impl.h"

int creer_socket(int prop, int *port_num)
{
   int fd = 0;

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

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<stdlib.h>
#include<assert.h>
#include<unistd.h>
#include<string.h>

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

void tableau_mot(FILE *fichier, int n_ligne){
  rewind(fichier);
  int i;
  int taille_max=500;
  char chaine[taille_max];
  char tableau[n_ligne][taille_max];
  for(i=0;i<n_ligne;i++){
    fgets(chaine, taille_max, fichier);
    strcpy(tableau[i], chaine);
    printf("%s\n",tableau[i]);
  }
  fclose(fichier);
}

// int main(int argc, char **argv)
// {
//   if(argc<2){
//     printf("Il manque un nom de fichier texte en argument.\n");
//   }
//
//   else {
//     FILE *fichier=NULL;
//     fichier= fopen(argv[1],"r");
//     if(fichier==NULL){
//       printf("Le fichier spécifié en argument n'existe pas.\n");
//     }
//     else{
//       int n_lignes=compte_lignes(fichier);
//       printf("Le fichier a %u lignes\n",n_lignes);
//       tableau_mot(fichier,n_lignes);
//       /*fclose(fichier);*/
//     }
//   }
//
//    return 0;
// }

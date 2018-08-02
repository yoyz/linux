#include <cdk.h>
int main() {
    CDKSCREEN    *cdkscreen;
    WINDOW       *cursesWin;
    CDKSCROLL    *liste;
    char *titre="</48>Selectionnez un legume dans la liste";
    char temp[256],*mesg[10];
    int choix;
    char *leglist[] = {"Carotte","Navet","Choux-fleur",
 "Brocoli","Pomme de terre","Courgette","Aubergine",
 "Poivron","Rutabaga"};
/* Initialisation */
   cursesWin = initscr();
   cdkscreen = initCDKScreen (cursesWin);
/* Initialisation des couleurs CDK */
   initCDKColor();
    liste = newCDKScroll(cdkscreen,
 CENTER,     /* coordonnée sur x : colonne de debut*/
 CENTER,     /* coordonnée sur y : ligne de début en partant du haut */
 RIGHT,      /* position de la scrollbar (RIGHT,LEFT,NONE) */
 8,          /* Hauteur de la liste */
 30,         /* Largeur de la liste */
 titre,      /* Titre du widget */
 leglist,    /* La liste des items */
 9,          /* Nombre d'items */
 FALSE,      /* si TRUE liste à puce numérotée */
 A_REVERSE, /* Attribut ncurses de l'item sélectionné */
 TRUE,       /* Dessiner une boite autour? */
 FALSE);     /* Dessiner une ombre autour? */
    /* On affiche */
    refreshCDKScreen (cdkscreen);
while(liste->exitType != vNORMAL) /* On ne sort pas par Echap */
    {
    choix = activateCDKScroll (liste, 0);
    }
    sprintf(temp,"<C>Résultat de votre sélection : %i",choix);
    mesg[0]=copyChar(temp);
    sprintf(temp,"<C>%s",leglist[choix]);
    mesg[1]=copyChar(temp);
    popupLabel(cdkscreen,mesg,2);
    /* on nettoie avant de quitter */
   destroyCDKScroll(liste);
   destroyCDKScreen (cdkscreen);
   endCDK();
}

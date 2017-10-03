#include <cdk/cdk.h>
    int displayCallback (EObjectType cdktype, void *object, void *clientData, chtype input);
    char *menuInfo[4][4];
int main() {
    CDKSCREEN    *cdkscreen;
    WINDOW       *cursesWin;
    CDKMENU      *menu;
    CDKLABEL     *infoBox;
    CDKLABEL     *monlabel;
    int selection;
    char *menulist[MAX_MENU_ITEMS][MAX_SUB_ITEMS];
    int submenusize[4], menuloc[4];
    char *mesg[3],temp[256];
    char *letexte[5];
menulist[0][0]="</40>Menu 1<!40>";
menuInfo[0][0]="";
menulist[1][0]="</40>Menu 2<!40>";
menuInfo[1][0]="";
menulist[2][0]="</40>Menu 3<!40>";
menuInfo[2][0]="";
menulist[3][0]="</40>Menu 4<!40>";
menuInfo[3][0]="";
menulist[0][1]="</56>Menu 1 Item 1<!56>";
menuInfo[0][1]="Information Menu 1 Item 1";
menulist[0][2]="</56>Menu 1 Item 2<!56>";
menuInfo[0][2]="Information Menu 1 Item 2";
menulist[0][3]="</56>Menu 1 Item 3<!56>";
menuInfo[0][3]="Information Menu 1 Item 3";
submenusize[0]=4;
menuloc[0]=LEFT;
menulist[1][1]="</56>Menu 2 Item 1<!56>";
menuInfo[1][1]="Information Menu 2 Item 1";
menulist[1][2]="</56>Menu 2 Item 2<!56>";
menuInfo[1][2]="Information Menu 2 Item 2";
submenusize[1]=3;
menuloc[1]=LEFT;
menulist[2][1]="</56>Menu 3 Item 1<!56>";
menuInfo[2][1]="Information Menu 3 Item 1";
menulist[2][2]="</56>Menu 3 Item 2<!56>";
menuInfo[2][2]="Information Menu 3 Item 2";
menulist[2][3]="</56>Menu 3 Item 3<!56>";
menuInfo[2][3]="Information Menu 3 Item 3";
submenusize[2]=4;
menuloc[2]=LEFT;
menulist[3][1]="</56>Menu 4 Item 1<!56>";
menuInfo[3][1]="Information Menu 4 Item 1";
submenusize[3]=2;
menuloc[3]=RIGHT;
/* Initialisation */
   cursesWin = initscr();
   cdkscreen = initCDKScreen (cursesWin);
   /* Pour désactiver Ctrl-C : raw(); */
   raw();
   /* Initialisation des couleurs CDK */
   initCDKColor();
   /* A l'activation du menu il n'y a pas de callback on initialise
    donc la chaine d'information au premier menu */
   mesg[0]="</48>Information Menu 1 Item 1<!48>";
   infoBox = newCDKLabel (cdkscreen, LEFT, BOTTOM, mesg, 1, FALSE, FALSE);
   menu = newCDKMenu(cdkscreen,
        menulist,       /* tableau de chaines à 2 dimensions contenant
                  les menus */
        4,      /* Nombre de menus */
        submenusize,    /* tableau d'entier contenant le nombre de choix
                  de chaque menu */
        menuloc,        /* tableau d'entier indiquant par les valeurs
                  RIGHT ou LEFT la position du menu */
        TOP,    /* Position des menus TOP ou BOTTOM */
        A_BOLD,         /* Attribut du menu courant */
        A_REVERSE);     /* Attribut du choix courant */
   /* Fonction de callback appelée à chaque modification de l'item courant */
   setCDKMenuPostProcess (menu, displayCallback, infoBox);
    letexte[0]="<C></24>Naviguez dans les menus en utilisant les fleches";
    letexte[1]="<C></24>Sélectionnez avec Entrée";
    letexte[2]="";
    letexte[3]="<C></24>Pour quitter ce programme";
    letexte[4]="<C></24>Utilisez la touche Echap";
/* Définition du label principal */
   monlabel = newCDKLabel(cdkscreen,
    CENTER,     /* coordonnée sur x : colonne de debut*/
    CENTER,     /* coordonnée sur y : ligne de début en partant du haut */
    letexte,    /* le tableau de caractere contenant le label */
    5,          /* le nombre de lignes à afficher */
    TRUE,       /* Dessiner un boite autour? */
    FALSE);     /* Dessiner une ombre? */
    /* On affiche */
    refreshCDKScreen (cdkscreen);
selection=0;
while(menu->exitType != vESCAPE_HIT)
    {
   selection=activateCDKMenu(menu,0);
   if (menu->exitType == vNORMAL)
        {
        sprintf(temp,"</48>Selection menu #%d, submenu #%d<!48>", 1+selection/100, 1+selection%100);
        mesg[0] = copyChar (temp);
        mesg[1] = "";
        mesg[2] = "</48>Appuyez sur une touche.<!48>";
        popupLabel(cdkscreen,mesg,3);
        } else
        {
        /* sortie du menu sans selection (esc) */
        mesg[0] = "</48>Sortie du menu sans selection<!48>";
        mesg[1] = "";
        mesg[2] = "</48>Appuyez sur une touche.<!48>";
        popupLabel(cdkscreen,mesg,3);
        }
    }
   /* on nettoie avant de quitter */
   destroyCDKScreen (cdkscreen);
   endCDK();
   printf("selection %i\n",selection);
}
int displayCallback (EObjectType cdktype, void *object, void *clientData, chtype input)
{
   CDKMENU *menu    = (CDKMENU *)object;
   CDKLABEL *infoBox    = (CDKLABEL *)clientData;
   char *mesg[1];
   char temp[256];
   sprintf (temp,"</48>%s<!48>",menuInfo[menu->currentTitle][menu->currentSubtitle+1]);
   mesg[0] = copyChar (temp);
   /* On change le message d'information */
   setCDKLabel (infoBox, mesg, 1, FALSE);
   drawCDKLabel (infoBox, FALSE);
   /* Nettoyage */
   freeChar (mesg[0]);
   return 0;
}

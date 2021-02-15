/**
 * @file main.c
 * @brief Contient le point d'entr�e du programme.
 * 
 * En principe, ce fichier ne doit pas �tre modifi� directement. Pour modifier
 * le programme du s�quenceur, modifier plut�t les fonctions de user.c, � savoir
 * init_app() et user_task().
 * 
 * @author Gregor Vindry
 */

#include "user.h"
/**
 * @brief Point d'entr�e du programme.
 * 
 * Cette fonction est appel�e automatiquement d�s la mise sous tension du PIC.
 * Elle appelle alors la fonction init_app() une et une seule fois, afin de
 * configurer le PIC; apr�s quoi la fonction user_task() est appel�e en boucle
 * � l'infini. Elle ne retourne donc jamais dans les faits, malgr� la pr�sence
 * du return qui est obligatoire pour que le code compile.
 * 
 * @return Code de retour du programme, en l'occurence toujours 0
 */
int main() {
    init_app();
    
    while(1) {
        user_task();
    }
    
    return 0;
}

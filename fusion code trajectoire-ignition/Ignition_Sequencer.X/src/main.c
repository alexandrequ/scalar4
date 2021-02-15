/**
 * @file main.c
 * @brief Contient le point d'entrée du programme.
 * 
 * En principe, ce fichier ne doit pas être modifié directement. Pour modifier
 * le programme du séquenceur, modifier plutôt les fonctions de user.c, à savoir
 * init_app() et user_task().
 * 
 * @author Gregor Vindry
 */

#include "user.h"
/**
 * @brief Point d'entrée du programme.
 * 
 * Cette fonction est appelée automatiquement dès la mise sous tension du PIC.
 * Elle appelle alors la fonction init_app() une et une seule fois, afin de
 * configurer le PIC; après quoi la fonction user_task() est appelée en boucle
 * à l'infini. Elle ne retourne donc jamais dans les faits, malgré la présence
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

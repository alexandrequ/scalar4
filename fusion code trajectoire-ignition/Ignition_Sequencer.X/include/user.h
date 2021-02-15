/**
 * @file user.h
 */

#ifndef USER_PYRO_H
#define	USER_PYRO_H

/**
 * @brief Fonction d'initialisation de l'application.
 * 
 * Cette fonction sert à configurer le PIC juste après son démarrage : réglage
 * des ports en entrées/sorties, configuration du CAN, etc.
 * 
 * @pre La fonction n'a jamais été appelée.
 * @post Le PIC est initialisé et l'application est prête à s'exécuter.
 */
void init_app();

/**
 * @brief Routine principale de l'application.
 * 
 * Cette fonction est appelée en boucle par le main(), et constitue donc le
 * programme principal.
 * 
 * @pre init_app() a été appelée une et une seule fois.
 */
void user_task();

#endif	/* USER_PYRO_H */


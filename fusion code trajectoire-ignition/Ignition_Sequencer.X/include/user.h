/**
 * @file user.h
 */

#ifndef USER_PYRO_H
#define	USER_PYRO_H

/**
 * @brief Fonction d'initialisation de l'application.
 * 
 * Cette fonction sert � configurer le PIC juste apr�s son d�marrage : r�glage
 * des ports en entr�es/sorties, configuration du CAN, etc.
 * 
 * @pre La fonction n'a jamais �t� appel�e.
 * @post Le PIC est initialis� et l'application est pr�te � s'ex�cuter.
 */
void init_app();

/**
 * @brief Routine principale de l'application.
 * 
 * Cette fonction est appel�e en boucle par le main(), et constitue donc le
 * programme principal.
 * 
 * @pre init_app() a �t� appel�e une et une seule fois.
 */
void user_task();

#endif	/* USER_PYRO_H */


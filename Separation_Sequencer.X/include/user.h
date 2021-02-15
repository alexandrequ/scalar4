/* 
 * File:   user_ssie.h
 * Author: Damien
 *
 * Created on 24 juillet 2018, 18:21
 */

#ifndef USER_SSIE_H
#define	USER_SSIE_H

#include <stdint.h>

void set_servo_pos(uint16_t position);

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

#endif	/* USER_SSIE_H */


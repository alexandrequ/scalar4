/**
 * @file user.h
 * @brief Fonctions de l'application trajectographie proprement dite.
 * @author Ines Arranz, R�my Chatel, Gregor Vindry, Damien Courteville
 */

#ifndef USER_H
#define	USER_H

#include <stdbool.h>
/**
 * @brief Fonction de detection du d�collage.
 *
 * Compare la norme de l'acceleration au seuil de d�tection.
 *
 * @param treshold : seuil de d�tection en m/s^2
 * @return true si l'acceleration d�passe le seuil
 */
bool take_off(float treshold);

/**
 * @brief Application initialization function.
 *
 * Cette fonction sert � configurer le PIC juste apr�s son d�marrage : r�glage
 * des ports en entr�es/sorties, configuration du CAN, etc.
 *
 * The measure state is initialized in CONFIG, and the flight state in GROUND (
 * refer to the documentation of user_task function bellow for a detailed
 * explanation of these states).
 *
 * @pre La fonction n'a jamais �t� appel�e.
 * @post Le PIC est initialis� et l'application est pr�te � s'ex�cuter.
 */
void init_app();

/**
 * @brief Main application routine.
 *
 * This function is called in a loop by the main function, and is thus the main
 * program.
 * It implements a state machine, in which we change states depending on time
 * and sensor inputs. These states are implemented using an indented switch/case
 * structure: the first level contains the measure state:
 *  - Measure state = CONFIG
 *      In this state, we perform the reset and configuration of the Xsens
 *      inertial measurement unit (IMU), by executing a series of functions of
 *      the file xsens_config.c. The program changes measure state as soon as it
 *      finishes the execution of these functions.
 *  - Measure state = INIT
 *      In the initialization measure state, the program performs the setup of
 *      the ground and flight measurement structures (setup done in trajecto.c
 *      via initDelayMovingAverage, from delayed_average.c). These measurement
 *      structures are defined in trajecto.c and trajecto.h (as extern).
 *      The follow-up of this measure state is always the MEASURE state, but the
 *      program checks wether the umbilical cable is still attached to determine
 *      the initial flight state.
 *  - Measure state = MEASURE
 *      In this state, measures are read at every call of user_task, and the
 *      state machine switches between fligt states:
 *      - Flight state = GROUND
 *          The rocket is still in the ground. We wait for a pre-defined time
 *          interval for the umbilical to be disconnected. If after that time
 *          the umbilical hasn't been removed, ...
 *      - Flight state = FLYING
 *          When flying, trajectory integrations are executed and the trajectory
 *          is checked out, in order to allow or not the ignition of the second
 *          stage. There are three levels of trajectory status:
 *          - Trajectory = NOMINAL:
 *              Ignition is allowed. LED2 turned on.
 *          - Trajectory = ANORMAL:
 *              Ignition is not allowed, but it's still considered that the
 *              trajectory can come back to nominal. LED2 turned off.
 *          - Trajectory = CRITICAL:
 *              Ignition not allowed + change of flight state. The rocket works
 *              as a single stage (second stage), because trajectory cannot come
 *              back to its nominal course. Both LEDs blink quickly (100ms).
 *          Propulsion is also checked, and if detection is positive, then there
 *          is a flight state change to SCND_FLYING (second stage flight mode).
 *      - Flight state = SCND_FLYING
 *          The second stage flight state is the last flight stage. The program
 *          only integrates the trajectory until it is recovered (or battery
 *          runs out). The recovery of the rocket in this phase of flight
 *          depends solely in the "recuperation" software, electronics and
 *          mechanics.
 *              `
 * @pre init_app() a �t� appel�e une et une seule fois.
 */
void user_task();

#endif

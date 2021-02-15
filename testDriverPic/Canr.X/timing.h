/**
 * @file timing.h
 * @brief Bibliothèque contenant toutes les fonctions temporelles.
 * 
 * @warning
 * - Afin de permettre à la bibliothèque de fonctionner, il faut définir dans les
 * options du projet une constante préprocesseur CPU_FREQ contenant la fréquence
 * d'horloge du PIC, en Hz.
 * - Pour pouvoir fonctionner, cette bibliothèque nécessite l'installation des
 * [Legacy PIC24 MCU & dsPIC DSC Peripheral Libraries]
 * (http://www.microchip.com/SWLibraryWeb/product.aspx?product=PIC24%20MCU%20dsPIC%20Peripheral%20Lib),
 * version 2.00.
 * 
 * @todo Peut-être se débarrasser de la dépendance envers les peripheral
 * libraries qui ne sont plus activement maintenues par le constructeur et sont
 * amenées à disparaître dans le futur.
 * @todo Insérer exemple d'utilisation de base ici.
 * 
 * @author Ines Arranz, Rémy Chatel, Gregor Vindry
 */

#ifndef TIMING_H
#define TIMING_H

#include <stdint.h> // définit uint16_t
#include <stdbool.h> // définit bool

/**
 * @brief Fonction d'attente bloquante, utilisant le timer 1.
 * 
 * Met en pause le programme principal durant le nombre spécifié de millisecondes,
 * avant de retourner.
 * 
 * @warning Cette fonction monopolise le timer 1, il faut donc s'assurer
 * que ce timer n'est pas déjà utilisé avant d'appeler cette fonction.
 * Par ailleurs, la vocation première de cette fonction est de réaliser des
 * pauses *courtes* dans l'exécution du programme (fonctionnement optimal).
 * Des pauses plus longues sont tolérées (jusqu'à 65535 ms), mais au prix d'une
 * erreur constante par morceaux.
 * 
 * @param ms le nombre entier de millisecondes à attendre
 * @pre Le timer 1 est inutilisé.
 * @pre ms est un entier compris entre 1 et 65535 inclus.
 * @pre Pour un fonctionnement optimal, ms est inférieur à ((65535*512000)/CPU_FREQ) - 1.
 * @post Le timer 1 est arrêté et contient la valeur 0.
 * @todo Quantifier les erreurs.
 * @todo Que se passe-t-il si le timer finit de compter lors d'une interruption ?
 */
void sleep(uint16_t ms);

/**
 * @brief Calcule le nombre d'itérations à passer à precision_sleep().
 * 
 * Calcule le nombre d'itérations à faire effectuer par la fonction
 * precision_sleep() afin d'obtenir une pause pendant le nombre spécifié de
 * microsecondes.
 * 
 * @param us Le nombre de microsecondes à attendre.
 * @param safe_mode Si true, garantit que la pause ne peut pas être de durée
 * inférieure au nombre de microsecondes spécifié.
 * 
 * @pre us est un entier compris entre 0 et 255 inclus.
 * @pre Pour éviter des dépassements trop importants aux faibles fréquences CPU,
 * s'assurer dans la mesure du possible que us > (36e6/CPU_FREQ). Voir feuille
 * de calcul erreur_precision_sleep pour évaluer la précision.
 * 
 * @return Le nombre d'itérations que precision_sleep() doit effectuer
 * 
 * @see precision_sleep() et ses postconditions pour une description de
 * l'influence des différents paramètres.
 */
uint16_t get_precision_sleep_loop_count(uint8_t us, bool safe_mode);

/**
 * @brief Fonction bloquante d'attente de précision.
 * 
 * Met en pause le programme principal en effectuant le nombre d'itérations
 * spécifié, avant de retourner. Ce nombre d'itérations doit être calculé à
 * l'aide de get_precision_sleep_loop_count().
 * 
 * @param loop_count Le nombre d'itérations à effectuer.
 * 
 * @pre loop_count doit être passé en paramètre depuis une variable de type
 * uint16_t, **sans conversion**.
 * @pre loop_count doit être une variable locale ou globale explicitement
 * définie. Autrement dit, la fonction get_precision_sleep_loop_count() doit
 * avoir été appelée à l'avance, et son résultat stocké dans une variable locale
 * ou globale.
 * @post Une pause s'est écoulée, d'une durée *au moins* égale à (us-1), où us
 * est le nombre de microsecondes spécifié à get_precision_sleep_loop_count().
 * @post Si le paramètre safe_mode de get_precision_sleep_loop_count() valait
 * true, la durée est *au moins* égale à us.
 * 
 * Synthèse des bonnes/mauvaises utilisations :
 * @code
 * // Exemple : attente de au moins 1 microseconde
 * // Bonne méthode
 * // On prévoit A L'AVANCE qu'on aura besoin de faire cette pause de 1 microseconde
 * uint16_t loop_count = get_precision_sleep_loop_count(1, true);
 * // ...
 * // Le moment venu, on fait la pause
 * precision_sleep(loop_count);
 * 
 * // Mauvaise méthode 1
 * precision_sleep(get_precision_sleep_loop_count(1, true)); // NON
 * 
 * // Mauvaise méthode 2
 * uint32_t loop_count = get_precision_sleep_loop_count(1, true); // NON : il faut stocker en uint16_t
 * // ...
 * // Le moment venu, on fait la pause
 * precision_sleep(loop_count);
 * @endcode
 * 
 * @note Tous les efforts portés à cette fonction permettent d'assurer une
 * certaine précision au niveau *algorithmique*. En effet, l'hypothèse de ces
 * calculs de précision est une fréquence d'oscillation constante et parfaitement
 * égale à sa valeur théorique, sauf que dans la vraie vie, ces 2 postulats
 * tombent à l'eau. En plus de ça, il peut arriver que certaines instructions
 * prennent plus de temps que prévu (pipeline stall). Toutes ces savantes
 * précisions sont donc à relativiser...
 * 
 * @todo Désactiver les interruptions ?
 */
void precision_sleep(uint16_t loop_count);



typedef void(*Periodic_function)(void);

/**
 * @brief Initialise le timer 2 pour utilisation par start_counting().
 * 
 * Configure le PIC pour que dès l'appel à start_counting(), la fonction
 * periodic_action() soit appelée automatiquement à intervalles fixes. Il suffit
 * d'appeler cette fonction une et une seule fois dans l'initialisation du
 * programme.
 * 
 * @pre Le timer 2 n'est pas en cours d'utilisation.
 * @post Le timer 2 est arrêté et les interruptions configurées pour pouvoir
 * lancer start_counting().
 * @see start_counting()
 */
void init_counting();

/**
 * @brief Fonction non bloquante d'exécution périodique d'une action, 
 * utilisant le timer 2.
 * 
 * Lance une fonction à intervalle fixe. 
 * Contrairement à sleep(), cette fonction retourne immédiatement.
 * 
 * @warning Cette fonction monopolise le timer 2, il faut donc s'assurer
 * que ce timer n'est pas déjà utilisé (autrement que par la fonction actuelle)
 * avant d'appeler cette fonction. 
 * Il ne faut pas non plus changer la configuration du timer avant l'appel à
 * stop_counting().
 * 
 * @param function The static pointer to the function to call periodically. Cette
 * ne doit prendre et retourner aucun argument.
 * @param ms le nombre entier de millisecondes à attendre entre chaque exécution
 * de periodic_action()
 * @pre init_counting() a été appelée.
 * @pre Le timer 2 n'est pas en cours d'utilisation par une fonction autre que
 * la fonction actuelle.
 * @pre ms est un entier compris entre 1 et 65535 inclus.
 * @post Le timer 2 est utilisé pour appeler la fonction
 * periodic_action() à intervalles réguliers.
 * @see init_counting()
 * @see stop_counting()
 * @see periodic_action()
 */
void start_counting(Periodic_function function, uint16_t ms);

/**
 * @brief Arrête l'exécution périodique d'une action lancée par start_counting().
 * 
 * Arrête d'exécuter périodiquement la fonction periodic_action().
 * 
 * @pre Le timer 2 a été lancé par start_counting().
 * @post Le timer 2 est arrêté et contient la valeur 0.
 * @see start_counting()
 * @see Periodic_function
 */
void stop_counting();

/**
 * 
 * @brief Implémente l'action à exécuter périodiquement par start_counting().
 * 
 * Cette fonction n'est pas implémentée dans timing.c, mais doit être définie
 * dans le code client en fonction du besoin.
 * 
 * @warning Cette fonction doit toujours être définie, *même si elle n'est
 * jamais appelée* (si l'on n'utilise pas start_counting()). Dans ce dernier cas,
 * définir simplement une fonction vide :
 * @code
 *     void periodic_action() {}
 * @endcode
 */
//void periodic_action();

/**
 * @brief Initialise le timer 3 pour utilisation par start_counting2().
 * 
 * Configure le PIC pour que dès l'appel à start_counting2(), la fonction
 * periodic_action2() soit appelée automatiquement à intervalles fixes. Il suffit
 * d'appeler cette fonction une et une seule fois dans l'initialisation du
 * programme.
 * 
 * @pre Le timer 3 n'est pas en cours d'utilisation.
 * @post Le timer 3 est arrêté et les interruptions configurées pour pouvoir
 * lancer start_counting2().
 * @see start_counting2()
 */
void init_counting2();

/**
 * @brief Fonction non bloquante d'exécution périodique d'une action, 
 * utilisant le timer 3.
 * 
 * Lance une fonction à intervalle fixe. 
 * Contrairement à sleep(), cette fonction retourne immédiatement.
 * 
 * @warning Cette fonction monopolise le timer 3, il faut donc s'assurer
 * que ce timer n'est pas déjà utilisé (autrement que par la fonction actuelle)
 * avant d'appeler cette fonction. 
 * Il ne faut pas non plus changer la configuration du timer avant l'appel à
 * stop_counting2().
 * 
 * @param function The static pointer to the function to call periodically. Cette
 * ne doit prendre et retourner aucun argument.
 * @param ms le nombre entier de millisecondes à attendre entre chaque exécution
 * de periodic_action2()
 * @pre init_counting2() a été appelée.
 * @pre Le timer 3 n'est pas en cours d'utilisation par une fonction autre que
 * la fonction actuelle.
 * @pre ms est un entier compris entre 1 et 65535 inclus.
 * @post Le timer 3 est utilisé pour appeler la fonction
 * periodic_action2() à intervalles réguliers.
 * @see init_counting2()
 * @see stop_counting2()
 * @see periodic_action2()
 */
void start_counting2(Periodic_function function, uint16_t ms);

/**
 * @brief Arrête l'exécution périodique d'une action lancée par start_counting2().
 * 
 * Arrête d'exécuter périodiquement la fonction periodic_action2().
 * 
 * @pre Le timer 3 a été lancé par start_counting2().
 * @post Le timer 3 est arrêté et contient la valeur 0.
 * @see start_counting2()
 * @see periodic_action2()
 */
void stop_counting2();

/**
 * @brief Implémente l'action à exécuter périodiquement par start_counting2().
 * 
 * Cette fonction n'est pas implémentée dans timing.c, mais doit être définie
 * dans le code client en fonction du besoin.
 * 
 * @warning Cette fonction doit toujours être définie, *même si elle n'est
 * jamais appelée* (si l'on n'utilise pas start_counting2()). Dans ce dernier cas,
 * définir simplement une fonction vide :
 * @code
 *     void periodic_action2() {}
 * @endcode
 */
//void periodic_action2();

/**
 * @brief Initialise la référence de temps du séquenceur.
 * 
 * Lance le comptage du timer 4/5 en 32 bits, permettant plus tard de dater des
 * événements à l'aide de timestamp().
 * 
 * @warning Le comptage reboucle au bout de (2^41)/CPU_FREQ secondes. Avec
 * l'oscillateur RC interne (Fosc=7.37MHz), reboucle au bout de 298374 secondes.
 * 
 * @post Les timers 4 et 5 ont été réinitialisés et comptent le temps.
 */
void zero_hour();

/**
 * @brief Mesure le temps écoulé depuis la référence de temps.
 * 
 * Récupère la valeur du timer 4/5 en 32 bits, et calcule le nombre de millisecondes
 * correspondant.
 * 
 * @warning Le comptage reboucle au bout de (2^41)/CPU_FREQ secondes. Avec
 * l'oscillateur RC interne (Fosc=7.37MHz), reboucle au bout de 298374 secondes.
 * 
 * @pre zero_hour() a été appelée depuis moins de (2^41)/CPU_FREQ secondes.
 * @return le nombre de millisecondes écoulées depuis l'appel à zero_hour()
 * @todo Tester la période de rebouclage (ou la minorer).
 */
long double timestamp();

#endif


/**
 * @file timing.h
 * @brief Biblioth�que contenant toutes les fonctions temporelles.
 * 
 * @warning
 * - Afin de permettre � la biblioth�que de fonctionner, il faut d�finir dans les
 * options du projet une constante pr�processeur CPU_FREQ contenant la fr�quence
 * d'horloge du PIC, en Hz.
 * - Pour pouvoir fonctionner, cette biblioth�que n�cessite l'installation des
 * [Legacy PIC24 MCU & dsPIC DSC Peripheral Libraries]
 * (http://www.microchip.com/SWLibraryWeb/product.aspx?product=PIC24%20MCU%20dsPIC%20Peripheral%20Lib),
 * version 2.00.
 * 
 * @todo Peut-�tre se d�barrasser de la d�pendance envers les peripheral
 * libraries qui ne sont plus activement maintenues par le constructeur et sont
 * amen�es � dispara�tre dans le futur.
 * @todo Ins�rer exemple d'utilisation de base ici.
 * 
 * @author Ines Arranz, R�my Chatel, Gregor Vindry
 */

#ifndef TIMING_H
#define TIMING_H

#include <stdint.h> // d�finit uint16_t
#include <stdbool.h> // d�finit bool

/**
 * @brief Fonction d'attente bloquante, utilisant le timer 1.
 * 
 * Met en pause le programme principal durant le nombre sp�cifi� de millisecondes,
 * avant de retourner.
 * 
 * @warning Cette fonction monopolise le timer 1, il faut donc s'assurer
 * que ce timer n'est pas d�j� utilis� avant d'appeler cette fonction.
 * Par ailleurs, la vocation premi�re de cette fonction est de r�aliser des
 * pauses *courtes* dans l'ex�cution du programme (fonctionnement optimal).
 * Des pauses plus longues sont tol�r�es (jusqu'� 65535 ms), mais au prix d'une
 * erreur constante par morceaux.
 * 
 * @param ms le nombre entier de millisecondes � attendre
 * @pre Le timer 1 est inutilis�.
 * @pre ms est un entier compris entre 1 et 65535 inclus.
 * @pre Pour un fonctionnement optimal, ms est inf�rieur � ((65535*512000)/CPU_FREQ) - 1.
 * @post Le timer 1 est arr�t� et contient la valeur 0.
 * @todo Quantifier les erreurs.
 * @todo Que se passe-t-il si le timer finit de compter lors d'une interruption ?
 */
void sleep(uint16_t ms);

/**
 * @brief Calcule le nombre d'it�rations � passer � precision_sleep().
 * 
 * Calcule le nombre d'it�rations � faire effectuer par la fonction
 * precision_sleep() afin d'obtenir une pause pendant le nombre sp�cifi� de
 * microsecondes.
 * 
 * @param us Le nombre de microsecondes � attendre.
 * @param safe_mode Si true, garantit que la pause ne peut pas �tre de dur�e
 * inf�rieure au nombre de microsecondes sp�cifi�.
 * 
 * @pre us est un entier compris entre 0 et 255 inclus.
 * @pre Pour �viter des d�passements trop importants aux faibles fr�quences CPU,
 * s'assurer dans la mesure du possible que us > (36e6/CPU_FREQ). Voir feuille
 * de calcul erreur_precision_sleep pour �valuer la pr�cision.
 * 
 * @return Le nombre d'it�rations que precision_sleep() doit effectuer
 * 
 * @see precision_sleep() et ses postconditions pour une description de
 * l'influence des diff�rents param�tres.
 */
uint16_t get_precision_sleep_loop_count(uint8_t us, bool safe_mode);

/**
 * @brief Fonction bloquante d'attente de pr�cision.
 * 
 * Met en pause le programme principal en effectuant le nombre d'it�rations
 * sp�cifi�, avant de retourner. Ce nombre d'it�rations doit �tre calcul� �
 * l'aide de get_precision_sleep_loop_count().
 * 
 * @param loop_count Le nombre d'it�rations � effectuer.
 * 
 * @pre loop_count doit �tre pass� en param�tre depuis une variable de type
 * uint16_t, **sans conversion**.
 * @pre loop_count doit �tre une variable locale ou globale explicitement
 * d�finie. Autrement dit, la fonction get_precision_sleep_loop_count() doit
 * avoir �t� appel�e � l'avance, et son r�sultat stock� dans une variable locale
 * ou globale.
 * @post Une pause s'est �coul�e, d'une dur�e *au moins* �gale � (us-1), o� us
 * est le nombre de microsecondes sp�cifi� � get_precision_sleep_loop_count().
 * @post Si le param�tre safe_mode de get_precision_sleep_loop_count() valait
 * true, la dur�e est *au moins* �gale � us.
 * 
 * Synth�se des bonnes/mauvaises utilisations :
 * @code
 * // Exemple : attente de au moins 1 microseconde
 * // Bonne m�thode
 * // On pr�voit A L'AVANCE qu'on aura besoin de faire cette pause de 1 microseconde
 * uint16_t loop_count = get_precision_sleep_loop_count(1, true);
 * // ...
 * // Le moment venu, on fait la pause
 * precision_sleep(loop_count);
 * 
 * // Mauvaise m�thode 1
 * precision_sleep(get_precision_sleep_loop_count(1, true)); // NON
 * 
 * // Mauvaise m�thode 2
 * uint32_t loop_count = get_precision_sleep_loop_count(1, true); // NON : il faut stocker en uint16_t
 * // ...
 * // Le moment venu, on fait la pause
 * precision_sleep(loop_count);
 * @endcode
 * 
 * @note Tous les efforts port�s � cette fonction permettent d'assurer une
 * certaine pr�cision au niveau *algorithmique*. En effet, l'hypoth�se de ces
 * calculs de pr�cision est une fr�quence d'oscillation constante et parfaitement
 * �gale � sa valeur th�orique, sauf que dans la vraie vie, ces 2 postulats
 * tombent � l'eau. En plus de �a, il peut arriver que certaines instructions
 * prennent plus de temps que pr�vu (pipeline stall). Toutes ces savantes
 * pr�cisions sont donc � relativiser...
 * 
 * @todo D�sactiver les interruptions ?
 */
void precision_sleep(uint16_t loop_count);



typedef void(*Periodic_function)(void);

/**
 * @brief Initialise le timer 2 pour utilisation par start_counting().
 * 
 * Configure le PIC pour que d�s l'appel � start_counting(), la fonction
 * periodic_action() soit appel�e automatiquement � intervalles fixes. Il suffit
 * d'appeler cette fonction une et une seule fois dans l'initialisation du
 * programme.
 * 
 * @pre Le timer 2 n'est pas en cours d'utilisation.
 * @post Le timer 2 est arr�t� et les interruptions configur�es pour pouvoir
 * lancer start_counting().
 * @see start_counting()
 */
void init_counting();

/**
 * @brief Fonction non bloquante d'ex�cution p�riodique d'une action, 
 * utilisant le timer 2.
 * 
 * Lance une fonction � intervalle fixe. 
 * Contrairement � sleep(), cette fonction retourne imm�diatement.
 * 
 * @warning Cette fonction monopolise le timer 2, il faut donc s'assurer
 * que ce timer n'est pas d�j� utilis� (autrement que par la fonction actuelle)
 * avant d'appeler cette fonction. 
 * Il ne faut pas non plus changer la configuration du timer avant l'appel �
 * stop_counting().
 * 
 * @param function The static pointer to the function to call periodically. Cette
 * ne doit prendre et retourner aucun argument.
 * @param ms le nombre entier de millisecondes � attendre entre chaque ex�cution
 * de periodic_action()
 * @pre init_counting() a �t� appel�e.
 * @pre Le timer 2 n'est pas en cours d'utilisation par une fonction autre que
 * la fonction actuelle.
 * @pre ms est un entier compris entre 1 et 65535 inclus.
 * @post Le timer 2 est utilis� pour appeler la fonction
 * periodic_action() � intervalles r�guliers.
 * @see init_counting()
 * @see stop_counting()
 * @see periodic_action()
 */
void start_counting(Periodic_function function, uint16_t ms);

/**
 * @brief Arr�te l'ex�cution p�riodique d'une action lanc�e par start_counting().
 * 
 * Arr�te d'ex�cuter p�riodiquement la fonction periodic_action().
 * 
 * @pre Le timer 2 a �t� lanc� par start_counting().
 * @post Le timer 2 est arr�t� et contient la valeur 0.
 * @see start_counting()
 * @see Periodic_function
 */
void stop_counting();

/**
 * 
 * @brief Impl�mente l'action � ex�cuter p�riodiquement par start_counting().
 * 
 * Cette fonction n'est pas impl�ment�e dans timing.c, mais doit �tre d�finie
 * dans le code client en fonction du besoin.
 * 
 * @warning Cette fonction doit toujours �tre d�finie, *m�me si elle n'est
 * jamais appel�e* (si l'on n'utilise pas start_counting()). Dans ce dernier cas,
 * d�finir simplement une fonction vide :
 * @code
 *     void periodic_action() {}
 * @endcode
 */
//void periodic_action();

/**
 * @brief Initialise le timer 3 pour utilisation par start_counting2().
 * 
 * Configure le PIC pour que d�s l'appel � start_counting2(), la fonction
 * periodic_action2() soit appel�e automatiquement � intervalles fixes. Il suffit
 * d'appeler cette fonction une et une seule fois dans l'initialisation du
 * programme.
 * 
 * @pre Le timer 3 n'est pas en cours d'utilisation.
 * @post Le timer 3 est arr�t� et les interruptions configur�es pour pouvoir
 * lancer start_counting2().
 * @see start_counting2()
 */
void init_counting2();

/**
 * @brief Fonction non bloquante d'ex�cution p�riodique d'une action, 
 * utilisant le timer 3.
 * 
 * Lance une fonction � intervalle fixe. 
 * Contrairement � sleep(), cette fonction retourne imm�diatement.
 * 
 * @warning Cette fonction monopolise le timer 3, il faut donc s'assurer
 * que ce timer n'est pas d�j� utilis� (autrement que par la fonction actuelle)
 * avant d'appeler cette fonction. 
 * Il ne faut pas non plus changer la configuration du timer avant l'appel �
 * stop_counting2().
 * 
 * @param function The static pointer to the function to call periodically. Cette
 * ne doit prendre et retourner aucun argument.
 * @param ms le nombre entier de millisecondes � attendre entre chaque ex�cution
 * de periodic_action2()
 * @pre init_counting2() a �t� appel�e.
 * @pre Le timer 3 n'est pas en cours d'utilisation par une fonction autre que
 * la fonction actuelle.
 * @pre ms est un entier compris entre 1 et 65535 inclus.
 * @post Le timer 3 est utilis� pour appeler la fonction
 * periodic_action2() � intervalles r�guliers.
 * @see init_counting2()
 * @see stop_counting2()
 * @see periodic_action2()
 */
void start_counting2(Periodic_function function, uint16_t ms);

/**
 * @brief Arr�te l'ex�cution p�riodique d'une action lanc�e par start_counting2().
 * 
 * Arr�te d'ex�cuter p�riodiquement la fonction periodic_action2().
 * 
 * @pre Le timer 3 a �t� lanc� par start_counting2().
 * @post Le timer 3 est arr�t� et contient la valeur 0.
 * @see start_counting2()
 * @see periodic_action2()
 */
void stop_counting2();

/**
 * @brief Impl�mente l'action � ex�cuter p�riodiquement par start_counting2().
 * 
 * Cette fonction n'est pas impl�ment�e dans timing.c, mais doit �tre d�finie
 * dans le code client en fonction du besoin.
 * 
 * @warning Cette fonction doit toujours �tre d�finie, *m�me si elle n'est
 * jamais appel�e* (si l'on n'utilise pas start_counting2()). Dans ce dernier cas,
 * d�finir simplement une fonction vide :
 * @code
 *     void periodic_action2() {}
 * @endcode
 */
//void periodic_action2();

/**
 * @brief Initialise la r�f�rence de temps du s�quenceur.
 * 
 * Lance le comptage du timer 4/5 en 32 bits, permettant plus tard de dater des
 * �v�nements � l'aide de timestamp().
 * 
 * @warning Le comptage reboucle au bout de (2^41)/CPU_FREQ secondes. Avec
 * l'oscillateur RC interne (Fosc=7.37MHz), reboucle au bout de 298374 secondes.
 * 
 * @post Les timers 4 et 5 ont �t� r�initialis�s et comptent le temps.
 */
void zero_hour();

/**
 * @brief Mesure le temps �coul� depuis la r�f�rence de temps.
 * 
 * R�cup�re la valeur du timer 4/5 en 32 bits, et calcule le nombre de millisecondes
 * correspondant.
 * 
 * @warning Le comptage reboucle au bout de (2^41)/CPU_FREQ secondes. Avec
 * l'oscillateur RC interne (Fosc=7.37MHz), reboucle au bout de 298374 secondes.
 * 
 * @pre zero_hour() a �t� appel�e depuis moins de (2^41)/CPU_FREQ secondes.
 * @return le nombre de millisecondes �coul�es depuis l'appel � zero_hour()
 * @todo Tester la p�riode de rebouclage (ou la minorer).
 */
long double timestamp();

#endif


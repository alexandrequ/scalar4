#include "timing.h"
#include <timer.h>
#include <math.h>
#include <stdbool.h>
#include <xc.h>

// Vérifie que la valeur de TIMER_16_BIT_PRELOAD_1_MS tient bien sur 16 bits
#if CPU_FREQ > 2000*UINT16_MAX
#error "Device frequency is too high"
#endif

// Valeur de preload à rentrer dans n'importe quel timer fonctionnant en 16 bit
// avec un prédiviseur de 1. Cette valeur provoque un débordement toutes les
// millisecondes.
#define TIMER_16_BIT_PRELOAD_1_MS ((uint16_t)(((long double)CPU_FREQ)/2000))

#if CPU_FREQ < 1000000
#error "Device frequency is too low"
#elif CPU_FREQ > 8000*UINT16_MAX
#error "Device frequency is too high"
#endif

// Definition des limites de comptage du timer
#define SLEEP_PS_8_LIMIT ((uint16_t)((((long double)UINT16_MAX)*8*2000) / CPU_FREQ) - 1)
#define SLEEP_PS_64_LIMIT ((uint16_t)((((long double)UINT16_MAX)*64*2000) / CPU_FREQ) - 1)
#define SLEEP_PS_256_LIMIT ((uint16_t)((((long double)UINT16_MAX)*256*2000) / CPU_FREQ) - 1)

// Variables pour le sleep()
volatile uint16_t t1_remaining_ms;
volatile bool sleepComplete;
volatile bool sleepRepeat;

// Variables pour le start_counting()
volatile uint16_t t2_max_ms; // variable donnant le max de ms sur T2
volatile uint16_t t2_current_ms; // variable donnant le nombre de ms écoulées
volatile Periodic_function t2_periodic_action; // Pointer on the function periodically called by T2

// Variables pour le start_counting2()
volatile uint16_t t3_max_ms; // variable donnant le max de ms sur T3
volatile uint16_t t3_current_ms; // variable donnant le nombre de ms écoulées
volatile Periodic_function t3_periodic_action; // Pointer on the function periodically called by T3

void sleep(uint16_t ms) {
#if (CPU_FREQ != 8000000) && (CPU_FREQ != 16000000) && (CPU_FREQ != 50000000)
#warning "sleep() has never been tested at this frequency"
#endif
    uint16_t prescalerFlag;
    uint16_t sleepPrescaler;
    
    // Fermeture preventive du timer puis initialisation des variables
    CloseTimer1();
    sleepRepeat = false;
    sleepComplete = false;
    _T1IF = 0;
    
    // Configuration interruption
    ConfigIntTimer1(T1_INT_PRIOR_4 & T1_INT_ON);
    
    // Suivant le nombre de millisecondes restantes a tester, on preconditionne
    // le timer pour qu'il soit adapte a la duree a mesurer
    if(ms < SLEEP_PS_8_LIMIT) {
        sleepPrescaler = 8;
        prescalerFlag = T1_PS_1_8;
    }
    else if(ms < SLEEP_PS_64_LIMIT) {
        sleepPrescaler = 64;
        prescalerFlag = T1_PS_1_64;
    }
    else {
        sleepPrescaler = 256;
        prescalerFlag = T1_PS_1_256;
        
        // Dans le cas ou la duree est superieur a la duree maximale mesurable
        // par le timer, on programme une repetition de l'interruption
        if(ms >= SLEEP_PS_256_LIMIT) {
            t1_remaining_ms = ms;
            ms = SLEEP_PS_256_LIMIT;
            sleepRepeat = true;
        }
    }
    
    // Mise a zero du timer
    WriteTimer1(0x0000);
    
    // Parametrage et lancement du timer
    const uint16_t preload = (uint16_t)(((((long double)CPU_FREQ)/2000)*ms)/sleepPrescaler);
    OpenTimer1(T1_ON & T1_GATE_OFF & prescalerFlag & T1_SYNC_EXT_OFF & T1_SOURCE_INT,
            preload);

    // Boucle de gel du programme
    while(!sleepComplete) {};
 
    // Fermeture propre du compteur et du flag
    CloseTimer1();
    ConfigIntTimer1(T1_INT_OFF);
    OpenTimer1(T1_OFF, 0);
    WriteTimer1(0x0000);
}

void __attribute__((__interrupt__, __auto_psv__)) _T1Interrupt() {
    _T1IF = 0;
    // Si le timer comptait une duree inferieur a sa valeur max, alors le sleep
    // est fini et on quitte l'interruption
    if(!sleepRepeat) {
        sleepComplete = true;
        return;
    }
    if(t1_remaining_ms <= SLEEP_PS_256_LIMIT) {
        sleepComplete = true;
        return;
    }
    
    // Sinon, on soustrait la valeur max qui vient d'etre mesuree
    t1_remaining_ms -= SLEEP_PS_256_LIMIT;
    
    // Si on est toujours au dessus de la valeur max, on quitte l'interruption
    // et on attend l'interruption suivante
    if(t1_remaining_ms >= SLEEP_PS_256_LIMIT)
        return;
    
    uint16_t prescalerFlag;
    uint16_t sleepPrescaler;
    
    // Sinon, on reparametre le timer pour l'adapter a la nouvelle valeur de
    // duree
    if(t1_remaining_ms < SLEEP_PS_8_LIMIT) {
        sleepPrescaler = 8;
        prescalerFlag = T1_PS_1_8;
    }
    else if(t1_remaining_ms < SLEEP_PS_64_LIMIT) {
        sleepPrescaler = 64;
        prescalerFlag = T1_PS_1_64;
    }
    else {
        sleepPrescaler = 256;
        prescalerFlag = T1_PS_1_256;
    }
    
    // Lancement du timer
    const uint16_t preload = (uint16_t)(((((long double)CPU_FREQ)/2000)*t1_remaining_ms)/sleepPrescaler);
    OpenTimer1(T1_ON & T1_GATE_OFF & prescalerFlag & T1_SYNC_EXT_OFF & T1_SOURCE_INT,
            preload);
}

uint16_t get_precision_sleep_loop_count(uint8_t us, bool safe_mode) {
    
    const double requested_cycles_count = us*((double)CPU_FREQ/2e6);
    
    const uint16_t constant_cycles = 13;
    const uint16_t min_cycles_count = 18;
    
    if(requested_cycles_count <=  min_cycles_count)
        return 0;
    
    const double loop_count_temp = 
            (requested_cycles_count - constant_cycles) / 6;
    
    if(safe_mode)
        return ceil(loop_count_temp);
    else
        return floor(loop_count_temp);
}

void init_counting() {
    // Fermeture preventive du timer
    CloseTimer2();
    // Configuration  interruption
    ConfigIntTimer2(T2_INT_PRIOR_4 & T2_INT_ON);
}

void start_counting(Periodic_function function, uint16_t ms) {
    // Initialisation variables globales pour les interruptions
    t2_max_ms = ms;
    t2_current_ms = 0;
    t2_periodic_action = function;
    // Mise a zero du timer pour s'assurer de sa valeur initiale
    WriteTimer2(0x0000);
    // Lancement du timer
    OpenTimer2(T2_ON & T2_GATE_OFF & T2_PS_1_1 &  T2_SOURCE_INT, 
            TIMER_16_BIT_PRELOAD_1_MS);
}

void stop_counting() {
    // Fermeture du timer
    OpenTimer2(T2_OFF, 0);
}

// Fonction qui gère les interruptions. Dans le cas présent, elle est appelée
//automatiquement toutes les ms.
void __attribute__((__interrupt__, __auto_psv__)) _T2Interrupt() {
    // Incrementation du timer
    t2_current_ms ++;
    // Quand le timer a atteint sa valeur cible, on declenche l'action
    if (t2_current_ms == t2_max_ms) {
        t2_periodic_action();
        t2_current_ms = 0;
    }
    _T2IF = 0;
}

void init_counting2() {
    // Fermeture preventive du timer
    CloseTimer3();
    // Configuration  interruption
    ConfigIntTimer3(T3_INT_PRIOR_4 & T3_INT_ON);
}

void start_counting2(Periodic_function function, uint16_t ms) {
    // Initialisation variables globales pour les interruptions
    t3_max_ms = ms;
    t3_current_ms = 0;
    t3_periodic_action = function;
    // Mise a zero du timer pour s'assurer de sa valeur initiale
    WriteTimer3(0x0000);
    // Lancement du timer
    OpenTimer3(T3_ON & T3_GATE_OFF & T3_PS_1_1 &  T3_SOURCE_INT, 
            TIMER_16_BIT_PRELOAD_1_MS);
}

void stop_counting2() {
    // Fermeture du timer
    OpenTimer3(T2_OFF, 0);
}

// Fonction qui gère les interruptions. Dans le cas présent, elle est appelée
//automatiquement toutes les ms.
void __attribute__((__interrupt__, __auto_psv__)) _T3Interrupt() {
// Incrementation du timer
    t3_current_ms ++;
    // Quand le timer a atteint sa valeur cible, on declenche l'action
    if (t3_current_ms == t3_max_ms) {
        t3_periodic_action();
        t3_current_ms = 0;
    }
    _T3IF = 0;
}

// Fonction qui definit la reference de temps du sequenceur
void zero_hour() {
    // Lancement du timer
    OpenTimer45(T4_ON & T4_GATE_OFF & T4_PS_1_256 & T4_32BIT_MODE_ON & T4_SOURCE_INT, 0xFFFFFFFF);
    // Mise à zéro du timer
    WriteTimer45(0);
}


//Fonction pour obtenir la date en milliseconde depuis l'initialiation du sequenceur
long double timestamp() {
    unsigned long timerbit = ReadTimer45(); //On recupere la valeur du timer
    long double t = 2. * 256. * 1000. * ((long double)timerbit / CPU_FREQ); // On convertit la valeur du timer en une valeur en s
    return t;
}
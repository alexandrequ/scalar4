#include "xc.h"
#include "timer.h"
#include "timing.h"

int Old_main_2(void) {
    
    TRISA = 0x00;                               // port A integralement en sortie
    TRISAbits.TRISA1 = 1;                       // pin RA4 (12) en écoute
    LATAbits.LATA0 = 1;                         // LED Allumee par defaut
    ANSELAbits.ANSA1 = 0;                       // Desactivation du CAN sur RA1
    WriteTimer1(0x0000);                        // Mise a zero de l'horloge
    OpenTimer1(T1_ON & T1_GATE_OFF & T1_PS_1_8 & T1_SYNC_EXT_OFF & T1_SOURCE_INT, 25000);
    short pressed = 0;                          // Bool d'activation du bouton
    
    while(1){
        while(PORTAbits.RA1 == 1){              // On capture l'appui du bouton
            sleep(50);
            if(PORTAbits.RA1 == 1){
                pressed = 1;
            }
        }
        if(pressed == 1){
            LATAbits.LATA0 = ~LATAbits.LATA0;   // On inverse l'etat de la LED
            pressed = 0;                        // on reinitialise le bool
            sleep(8000);
            LATAbits.LATA0 = ~LATAbits.LATA0;
        }
    } 
    return 0;
}
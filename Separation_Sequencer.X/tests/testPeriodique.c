#include "timing.h"
#include <xc.h>

void periodic_action() {
    LATAbits.LATA0 = ~LATAbits.LATA0; // on inverse l'état de la LED
}

void periodic_action2() {}

int Old_main_3() {
    TRISA = 0x00; // port A en sortie
    
    init_counting();
    start_counting(500);
    sleep(5000);
    stop_counting();
    sleep(2000);
    start_counting(500);
    sleep(5000);
    stop_counting(500);
    
    sleep(5000);
    
    LATAbits.LATA0 = 1;
    zero_hour();
    
    while(1) {
        if(timestamp() > 1200){
            LATAbits.LATA0 = 0;
        }
    }
    
    return 0;
}

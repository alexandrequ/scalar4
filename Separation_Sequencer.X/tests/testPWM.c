#include "pwm.h"
#include "timing.h"
//#include <hspwm.h>
//#include <timer.h>
#include <xc.h>

void periodic_action() {
    
}

void periodic_action2() {
    
}

//#define PWM_PERIOD 1000 // période en multiples de 1/CPU_FREQ

int main() {
    //int i = 0;

    _ANSA0 = 0; // RA0 en numérique
    TRISA = 0x00; // port A en sortie
    //TRISB = 0xFFFF;
    /*FCLCON1bits.FLTMOD = 0b11;
    FCLCON2bits.FLTMOD = 0b11;
    FCLCON3bits.FLTMOD = 0b11;*/
    //LATB = 0x0000; // mise à zéro de toutes les pin du port B

    /*// Configuration du module PWM 1 (PWM1H = pin 25 seule activée)
    ConfigHSPWM1(PWM_TB_MODE_PER | PWM_D_CYLE_DC | PWM_DT_POS | PWM_PRI_TB, 
            PWM_H_PIN_EN, 0, 0, 0);
    // Réglage de la durée de l'impulsion haute
    SetHSPWMDutyCycle1((unsigned int)(0.4*PWM_PERIOD));
    // Pas de pause
    SetHSPWMDeadTime1(0,0);
    
    // Réglage de la période et activation du PWM
    OpenHSPWM(PWM_MOD_EN,PWM_INPUT_CLK_DIV0,PWM_PERIOD,0);*/

    init_PWM();
    config_PWM1H_voltage(2, 20000);
    config_PWM2H_voltage(3, 20000);
    config_PWM3H_voltage(3, 10000);
    
    start_PWM();
    LATAbits.LATA0 = 1; // Allumage de la LED (youhou je suis vivant)
    // LATBbits.LATB15 = 1; // On allume la pin RB15=PWM1L (test de son indépendance)
    /*for(i = 0; i < 5; i++) {
        sleep(2000);
        stop_PWM();
        sleep(2000);
        start_PWM();
    }
    stop_PWM();
    clear_config_PWM1H();*/
    
    while(1);
    return 0;
}

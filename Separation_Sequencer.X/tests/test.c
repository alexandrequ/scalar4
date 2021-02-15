#define VCC 5
#include "can.h"
#include "pwm.h"
#include "timing.h"
#include <stdbool.h>
#include <xc.h>

// ------------------ Config test --------------------------------

// Modes de test: timing.c (1), pwm.c (2), can.c (3)
#define TEST_MODE 3

// Definition des etats de la machine a etats.
#define STATE_INIT 0
#define STATE_CONNECTED 1
#define STATE_PERIODIC_SHORT 2
#define STATE_PERIODIC_LONG 3
#define STATE_TIMESTAMP_SHORT 4
#define STATE_TIMESTAMP_LONG 5
#define STATE_BREAK 255             // etat de sortie du programme

// ------------------ Config test timing.c -----------------------

#define LED_BLINK_PERIOD_CONNECTED_MS 2000
#define LED_BLINK_PERIOD_FLYING_MS 500
#define LED_BLINK_PERIOD_PANIC 250

// Definition des durees de test pour les fonctions de timing.c
// min for all duration = 250ms
#define WAIT 400
#define TEST_SLEEP_SHORT 800
#define TEST_SLEEP_LONG 20*1000
#define TEST_TIMESTAMP_SHORT 800
#define TEST_TIMESTAMP_LONG 10000
#define TEST_PER_SHORT 500
#define TEST_PER_LONG 2000

#define WAIT_REBONDS_MS 50

// ------------------ Config test pwm.c -----------------------

void init_app();
void user_task();
void clean_shutdown();

bool connected = false;
unsigned int state;
long double start = 0;
bool started = 0;
int i = 0;
uint16_t can_counter = 0;

void clean_shutdown(){
    start_counting(LED_BLINK_PERIOD_PANIC);
    sleep(LED_BLINK_PERIOD_PANIC*10+WAIT);
    _LATA0 = 0;
    Sleep();
    while(1);
}

void periodic_action() {
    _LATA0 = ~_LATA0;
}

void periodic_action2() {}

void init_app() {
    
    // ****** Association du module CAN aux pins 39 et 40 ******
    __builtin_write_OSCCONL(OSCCONL & 0xbf); // Déverrouillage du PPS
    // Réception CAN sur RP37 (pin n° 14)
    RPINR26bits.C1RXR = 37;
    // Transmission CAN sur RP38 (pin n° 15)
    RPOR2bits.RP38R = 0x000E;
    __builtin_write_OSCCONL(OSCCONL | 0x40); // Verrouillage du PPS

    // ****** Réglage des ports ********************************
    TRISA = 0x00; // port A integralement en sortie
    LATA = 0x00; // port A initialise a 0
    _TRISA1 = 1; // pin RA1 (12) en écoute
    _ANSA1 = 0; // Desactivation du Convertisseur AN sur RA1

    init_counting();
    zero_hour();
    
    // close the interruptor before running
    if (_RA1 == 0) {
        clean_shutdown();
    }
    
    set_can_mask(CAN_MASK0, 0x7FF);
    set_can_mask(CAN_MASK1, 0x7F0);
    enable_can_rx_buffer(CAN_RX8, 0x123, CAN_MASK0);
    enable_can_rx_buffer(CAN_RX9, 0x450, CAN_MASK1);
    config_can(BR_500_KBPS, true, 1, 6, 7);
    
    clear_config_PWM1H();
    init_PWM();
    
    _LATA0 = 1;
    // plug in the Arduino NOW
    sleep(5*WAIT);
    _LATA0 = 0;
    sleep(WAIT);
    state = STATE_INIT;
}

void user_task() {
#if TEST_MODE == 1
    if(state == STATE_INIT){
        sleep(WAIT);
        _LATA0 = 1;
        sleep(TEST_SLEEP_SHORT);
        _LATA0 = 0;
        sleep(WAIT);
        _LATA0 = 1;
        sleep(TEST_SLEEP_LONG);
        _LATA0 = 0;

        state = STATE_PERIODIC_SHORT;
        connected = true;
        sleep(WAIT);
    }
    if(state == STATE_CONNECTED){ // Disconnection detection
        if (connected) {
            if (_RA1 == 0) {
                sleep(WAIT_REBONDS_MS);
                if (_RA1 == 0) {
                    connected = false;
                    state = STATE_PERIODIC_SHORT;
                    _LATA0=1;
                }
        }
    } 
    }
    else if(state == STATE_PERIODIC_SHORT){ // Periodic function test
        start_counting(TEST_PER_SHORT);
        sleep(TEST_PER_SHORT * 8 + 4*WAIT);
        stop_counting();
        _LATA0 = 0;
        sleep(WAIT);
        state = STATE_PERIODIC_LONG;
    }
    else if(state == STATE_PERIODIC_LONG){ // Periodic function test
        start_counting(TEST_PER_LONG);
        sleep(TEST_PER_LONG * 8 + 4*WAIT);
        stop_counting();
        _LATA0 = 0;
        sleep(WAIT);
        state = STATE_TIMESTAMP_SHORT;
    }
    else if(state == STATE_TIMESTAMP_SHORT){
        if(!started){
            start = timestamp();
            _LATA0 = 1;
            started = true;
        }
        if(started && timestamp()-start >= TEST_TIMESTAMP_SHORT){
            _LATA0 = 0;
            started = false;
            sleep(WAIT);
            state = STATE_TIMESTAMP_LONG;
        }
    }
    else if(state == STATE_TIMESTAMP_LONG){
        if(!started){
            start = timestamp();
            _LATA0 = 1;
            started = true;
        }
        if(started && timestamp()-start >= TEST_TIMESTAMP_LONG){
            _LATA0 = 0;
            started = false;
            sleep(WAIT);
            state = STATE_BREAK;
        }
    }
    else{
        clean_shutdown();
    }
#elif TEST_MODE == 2
    config_PWM1H_voltage(0, 20000);            // Go to position 0 degree
    start_PWM();
    sleep(4000);                        // wait 4s to be sure to reach 0
    stop_PWM();
    for(i =1; i<6; i++){
        config_PWM1H_voltage(i*60*VCC/360, 20000);// go to i*60 degree
        start_PWM();
        sleep(2000);                    // wait 2s
        stop_PWM();
    }
#elif TEST_MODE == 3
    uint16_t data[2] = {can_counter, 0xabcd};
    transmit_can((Canframe){0x201, (uint8_t*)data, 3}, true);
    transmit_can((Canframe){0x5A9, (uint8_t*)(&can_counter), 2}, true);
    can_counter++;
    
    Canframe fr = receive_can(CAN_RX8, true);
    fr.id++;
    transmit_can(fr, true);
    
    while(!new_canframe_received(CAN_RX9));
    fr = receive_can(CAN_RX9, false);
    fr.id--;
    transmit_can(fr, true);
#else
    clean_shutdown();
#endif
}

int main(void) {
    init_app();
    while(1) {
        user_task();
    }
    return 0;
}



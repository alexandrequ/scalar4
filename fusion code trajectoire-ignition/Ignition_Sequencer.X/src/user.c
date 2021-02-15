#include "user.h"
#include "config.h"
#include "can_id.h"
#include "can.h"
#include "timing.h"
#include "clock.h"
#include <stdbool.h>
#include <xc.h>

typedef enum { INIT = 0, GROUND = 1, FLYING = 2, WINDOW = 3, INHIBITED = 4, ERROR = 5, GROUND_CONFIG = 6} State;
static State state = INIT;
static long double t0 = 0;
static long double t_adc = 0;
static long double t_sep = 0;
static long double tcan = 0;

#ifdef BUZZER_MODE
    uint8_t nIGNITION;
    uint8_t IO_IGNITION;
#endif

void blink() {
    LED1 = ~LED1;
}

#ifdef BUZZER_MODE
void buzz(){
    nBUZZER = ~nBUZZER;
}
#endif

void send_log() {
#ifdef LOG_CAN_ID
    static uint8_t transmitted_state;
    transmitted_state = (get_clock_status() << 6) 
                        | (FDC_COIFFE << 5)
                        | (state & 0x07);
    transmit_can(LOG_CAN_ID, &transmitted_state, 1, CAN_TX1);
#endif
}

/**
 * @brief Reads inductive sensor tension and check if it detect contact or not.
 * 
 * @warning A reasonable delay must be kept between each call of read_indu() because
 * the ADC sampling condenser needs to recharge between each sampling.
 * 
 * @return 'true' if the sensor detect a contact, 'false' else.
 */
bool read_indu(){
    AD1CON1bits.SAMP = 0;// Start the conversion
    while (!AD1CON1bits.DONE);// Wait for the conversion to complete
    AD1CON1bits.DONE = 0;// Clear conversion done status bit
    uint16_t result = ADC1BUF0;// Read the ADC conversion result
    return (result > INDU_TRESHOLD);
}

void init_app() {
    // Failsafe_clock configured at 7.37/8/8*139 = 16MHz
    config_failsafe_clock(8, 8, 139);
    
    zero_hour();
    
    TRISA = 0x00; // port A intégralement en sortie    
    TRISB = 0x0000; // port B en sortie
    
    // Attribution des pin au module CAN
    __builtin_write_OSCCONL(OSCCONL & 0xbf); // Déverrouillage du PPS
    // Réception CAN sur RP38 (pin n° 15)
    RPINR26bits.C1RXR = 38;
    // Transmission CAN sur RP37 (pin n° 14)
    RPOR1bits.RP37R = 0x000E;
    _TRISB6 = 1;  // RB6 (ie RP38 ie CANRX) en entrée
    _TRISB5 = 0;  // RB5 (ie RP37 ie CANTX) en sortie
    __builtin_write_OSCCONL(OSCCONL | 0x40); // Verrouillage du PPS
    
    IO_UMBILICAL = 1; // RB12 en entrée (ombilical)
    IO_FDC1 = 1;  // RB8  en entrée (fdc1)
    IO_FDC2 = 1;  // RB9  en entrée (fdc2)
    IO_FDC_COIFFE = 1; // RB13 en entrée (fdc coiffe)
    IO_LOG_TRAJ = 1;  // RA4  en entrée (log trajecto)
    IO_IGNITION = 0; // RB15 en sortie (commande mise à feu)
    IO_LED1 = 0; // LED1 en sortie
    IO_LED2 = 0; // LED2 en sortie
            
    LATA = 0x00; // port A initialisé à 0
    LATB = 0x0000; // port B initialisé à 0
    
    nIGNITION = 1; // inhibit ignition
    
    /* Set port configuration */ 
    ANSELA = ANSELB= 0x0000;
    ANSELAbits.ANSA0 = 1;// Ensure AN0 is analog
    ANSELAbits.ANSA1 = 1;// Ensure AN1 is analog
    _TRISA0 = 1; // AN0 input
    _TRISA1 = 1; // AN1 input
    /* Initialize and enable ADC module */
    AD1CON1bits.ASAM = 1; // Activation Auto-sampling
    AD1CON3bits.ADCS = 0x0F; // Tad = 16*Tcy
    AD1CHS0bits.CH0SA = 0; // Channel 0 input = AN0
    AD1CHS0bits.CH0NA = 1; // Channel 0 negative ref = AN1
    AD1CON1bits.ADON = 1; // Enable ADC module
    sleep(10);
    
    LED1 = 0; // LED 1 allumée
    LED2 = 0; // LED 2 éteinte
    init_counting();
#ifdef BUZZER_MODE
    init_counting2();
    nBUZZER = 1;
#endif
    
    set_can_mask(CAN_MASK0, 0x7FF); // Preparation of CAN mask
#ifdef BUZZER_MODE
    // We enable reception of log CAN message from all other sequencers
    enable_can_rx_buffer(RECU_BUFFER, SEQ_RECU_LOG_CAN_ID, CAN_MASK0);
    enable_can_rx_buffer(CHUT_BUFFER, SEQ_PYRO_LOG_CAN_ID, CAN_MASK0);
    enable_can_rx_buffer(TRAJ_BUFFER, TRAJ_LOG_CAN_ID, CAN_MASK0);
#endif
    config_can(BR_500_KBPS, true, 1, 3, 4);
    send_log();
    
    t_adc = timestamp();
    t_sep = timestamp();
    tcan = timestamp();
}

void user_task() {
    static bool indu;
    static bool separated = true;

#ifdef BUZZER_MODE
    if (state < INHIBITED){
        // If we receive a log from recuperation or cansat sequencer indicating that
        // one of the 2 has detected takeoff, we make the buzzer ring continuously
        // to warn people around that something is going to be ejected.
        if (new_canframe_received(RECU_BUFFER)) {
                const Canframe fr = receive_can(RECU_BUFFER, false);
                const uint8_t can_byte = *(fr.databytes);
                if(_get_log_state(can_byte) >= 2){ // If state > GROUND
                    stop_counting2();
                    nBUZZER=0;
                }
        }
        if (new_canframe_received(CHUT_BUFFER)) {
                const Canframe fr = receive_can(CHUT_BUFFER, false);
                const uint8_t can_byte = *(fr.databytes);
                if(_get_log_state(can_byte) >= 2){ // If state > GROUND
                    stop_counting2();
                    nBUZZER=0;
                }
        }
    }
#endif
    
    if(timestamp()-tcan >= LOG_PERIOD){
        tcan = timestamp();
        send_log();
    }
    
    // There must be a delay between separation and ignition, we update t_sep to
    // current timestamp while the rocket is not separated.
    if(!separated) t_sep = timestamp();
    
    // We read inductive sensor every ADC_PERIOD_MS and check if the rocket is separated
    if(timestamp()-t_adc > ADC_PERIOD_MS){
        t_adc = timestamp();
        indu = read_indu();
        // FDC1, FDC2 or indu are at 0 when they detect separation
        // At least 2 out of 3 sensor must be at 0 to consider the rocket is separated
        separated = ((!FDC1) && (!FDC2)) || ((!FDC1) && (!indu)) || ((!FDC2) && (!indu));
    }
    
    switch(state) {
        case INIT:
            // At startup, we check if the umbilical is plugged
            if(UMBILICAL == 1) {
                // If umbilical is unplugged, we go to configuration mode
                t0 = timestamp();
                LED1 = 1;
                LED2 = 1;
                state = GROUND_CONFIG;
                send_log();
            }
            else {
                // If umbilical is plugged, we lock the parachute/cansat and we
                // go to the nominal ground mode
                start_counting(blink,LED_BLINK_PERIOD_GROUND_MS); //debug
#ifdef BUZZER_MODE
                start_counting2(buzz,LED_BLINK_PERIOD_GROUND_MS);
#endif
                LED2 = 1;
                state = GROUND;
                send_log();
            }
            
            break;
            
        case GROUND:
            LED2 = separated; // LED2 indicates the separation state
            
            // In ground mode, we wait for umbilical to unplugged (which means takeoff).
            if (UMBILICAL == 1) {
                t0 = timestamp(); // We store the date of takeoff
                // Debouncing : We verify that the pin remains at 1 for WAIT_REBONDS_MS
                // to be sure that this is not some random noise
                bool takeoff_validated = true;
                while(timestamp()-t0<WAIT_REBONDS_MS)
                    if (UMBILICAL == 0){
                        takeoff_validated = false;
                        break;
                    }
                // If the debouncing is ok, we go to flight mode
                if(takeoff_validated){
                    // Monostage rocket: no further verification is needed
                    state = FLYING;
                    start_counting(blink, LED_BLINK_PERIOD_FLYING_MS);
                    #ifdef BUZZER_MODE
                        stop_counting2();
                        nBUZZER=0;
                    #endif

                    /* This block of code is for a two stages rocket.
                    if(!separated){
                        // If the rocket is not separated at takeoff, everything is ok
                        // and we can go to FLYING mode
                        state = FLYING;
                        start_counting(blink, LED_BLINK_PERIOD_FLYING_MS);
                    }
                    else{
                        // If the rocket is separated at takeoff there is a problem
                        // and ingnition is permanently inhibited.
                        stop_counting();
                        LED1 = 0;
                        LED2 = 0;
                        state = ERROR;
                    }*/
                    send_log();
                }
            }
            break;
            
        case FLYING:
            // In FLYING mode, we wait for WAIT_IGNITING_MS and we go to WINDOW mode.
            // LED2 indicates if all conditions exept the time window are ok.
            LED2 = !(separated && !FDC_COIFFE && LOG_TRAJ);
            if(timestamp()-t0 > WAIT_IGNITING_MS) {
                state++;
                send_log();
                stop_counting();
                LED1 = 0;
     
            }
            break;
            
        case WINDOW:
            // In WINDOW mode, we send ignition signal while all conditions are met:
            // - Rocket has been separated for at least SEP_IGNITION_DELAY
            // - FDC_COIFFE = 0, i.e. the nose has not been ejected
            // - Trajecto Sequencer indicates the trajecto is ok
            nIGNITION = !((timestamp()-t_sep > SEP_IGNITION_DELAY_MS) && !FDC_COIFFE && LOG_TRAJ);
            LED2 = !(separated && !FDC_COIFFE && LOG_TRAJ);
            
            // When the end of the time windows is reached, ignition signal is inhibited
            // and we go to INHIBITED mode
            if(timestamp()-t0 > WAIT_IGNITING_MS + IGNITION_WINDOW_MS) {
                nIGNITION = 1;
#ifdef BUZZER_MODE
                nBUZZER = 1;
#endif
                LED1 = 1;
                LED2 = 1;
                state++;
                send_log();
            }
            break;
            
        case INHIBITED:
        case ERROR:
            nIGNITION = 1;
            
            // If SLEEP_DATE_MS has elapsed since takeoff we put
            // the microcontroller into sleep mode to save power
            if(timestamp()-t0 > SLEEP_DATE_MS){
                LED1 = 1; // We also turn off the LEDs to save more power
                LED2 = 1;
                send_log();
                Sleep();
            }
            break;
            
        case GROUND_CONFIG:
            LED1 = FDC_COIFFE;
            LED2 = separated;
            break;
    }
}
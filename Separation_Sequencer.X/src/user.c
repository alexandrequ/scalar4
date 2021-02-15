#include "config.h"
#include "user.h"
#include "serial.h"
#include "herkulex_controller.h"
#include "timing.h"
#include "clock.h"
#include <stdbool.h>
#include <xc.h>

typedef enum { INIT = 0, GROUND = 1, FLYING = 2, SEPARATED = 3, OPEN = 4, ERROR = 6, GROUND_CONFIG = 7} State;

static const uint8_t separation_servo_id[] = {SEP_SERVOS_ID};
static const uint8_t recuperation_servo_id[] = {RECUP_SERVOS_ID};


static State state = INIT;
static long double t0 = 0;


void set_servo_pos(uint16_t position){
	uint16_t posList[]={position};
	set_servos_pos(separation_servo_id, 1, posList, RECUP_SERVOS_OPEN_ROT_TIME,false, true, false);
}

void blink_red() {
    LED_RED = ~LED_RED;
}

void blink_green() {
    LED_GREEN = ~LED_GREEN;
}

void blink_all() {
    LED_RED = ~LED_RED;
    LED_GREEN = ~LED_GREEN;
}

void init_app() {
    
    // Configuration of the clock PLL
    // Frequency = Oscillator_freq * (_PLLDIV+2)/((_PLLPRE+2)*2(_PLLPOST+1) = 128Mhz
    // note: Il faut mettre #pragma config FNOSC = PRIPLL dans config_vits.c pour pouvoir utiliser le PPL
    // note: The overclocking is necessary to generate a precise baudrate for UART communication with herkulex.
    // TODO: We can use config_clock_PLL(8,2,128) instead
    _PLLPRE = 6;
    _PLLPOST = 0;
    _PLLDIV = 126;
    
    // Failsafe_clock configured at 7.37/4/2*139 = 128.05MHz
    config_failsafe_clock(4, 2, 139);
    
    zero_hour(); // Initialization of time driver
    
    TRISA = 0x00; // port A intégralement en sortie    
    TRISB = 0x0000; // port B en sortie
    
    // Attribution des pin au module UART
    __builtin_write_OSCCONL(OSCCONL & 0xbf); // Déverrouillage du PPS
    // Réception UART sur RP43 (pin n° 22)
    RPINR18bits.U1RXR = 36;
    _TRISB4 = 1; // UART rx en entrée
    // Transmission UART sur RP42 (pin n° 21)
    RPOR0bits.RP35R = 0x0001;
    _TRISB3 = 0; // UART tx
    __builtin_write_OSCCONL(OSCCONL | 0x40); // Verrouillage du PPS
    
    IO_UMBILICAL = 1; // Umbilical as input 
    IO_SEP_BUTTON = 1; // Separation button as input
    IO_RECUP_BUTTON = 1 ; // Recup button as input
    IO_BUZZER = 0;
    
    LATA = 0x00; // port A initialisé à 0
    LATB = 0x0000; // port B initialisé à 0
    LED_RED=1;
    LED_GREEN=1;
    BUZZER = 1;
    
    init_servos(); // Initialization of herkulex drivers
    // Configure maximum torque of each servo.
    set_servo_max_torque(separation_servo_id, 1, SEP_SERVO_TORQUE);
    set_servo_max_torque(recuperation_servo_id, 1, RECUP_SERVO_TORQUE);
    
    init_counting(); // Initialize periodic function
}

void user_task() {
    static bool open_all_button_pushed = false;
    static bool sep_button_pushed = false;
    static bool sep_open = false;
    static bool all_open = false;
    static bool servo_ok = true;
    
    switch(state) {
        case INIT:
            // At startup, we check if the umbilical is plugged
            if(UMBILICAL == 0) {
                // If umbilical is plugged, we lock the parachute and we open the
                // separation, so the pyro technician can couple the two stages.
                set_servo_pos(CLOSED_POSITION);
                start_counting(blink_red,LED_BLINK_PERIOD);
                LED_GREEN=1;
				sep_open = false;
                // Then we go to ground mode
                state = GROUND;
            }
            else {
                // If umbilical is unplugged, we go to configuration mode
                set_servo_pos(CLOSED_POSITION);
                LED_GREEN = 1;
                LED_RED = 0;
                state = GROUND_CONFIG;
            }
            break;
            
        case GROUND:
            // If servo_ok has been set to false, it means that we started a separation servo
            // rotation. Is this case, we check if the servo has reached its position
            // at each loop.
            if(!servo_ok)
                servo_ok = servo_pos_reached(separation_servo_id, 1, false);
            // The red LED is ON is the separation servo has not reached its position.
            // If the red LED remains ON, it means that the mecanism is stuck.
            
            // We check for umbilical to be unplugged (which means takeoff).
            if (UMBILICAL == 1) {
                t0 = timestamp(); // We store the date of takeoff
                // Debouncing : We verify that the pin remains at 1 for WAIT_REBONDS_MS
                // to be sure that this is not some random noise.
                // This debouncing is bad because if ckeck the pin only 2 times
                // TODO : Use the same decouncing method than in recup/chut sequencer
                sleep(WAIT_REBONDS_MS);
                if (UMBILICAL == 1) {
                    // If the debouncing is ok, check the separation servo position
                    if(sep_open || !servo_ok){
                        // If separation is open or stuck, there is a problem.
                        // In that case, we do nothing and go to ERROR mode
                        stop_counting();
                        LED_RED=0;
                        LED_GREEN=0;
                        state = ERROR;
                    }
                    else{
                        // If separation is closed, we go to flight mode
                        state++;
                        stop_counting();
                        LED_RED=1;
                        LED_GREEN=1;
                    }
                }
            }
            
            // In ground mode, pushing the separation button will open/close the separation
            if(!sep_button_pushed && SEP_BUTTON == 0){
                // Debouncing the button signal
                sleep(10);
                if(SEP_BUTTON == 0){
                    sep_button_pushed = true;
                    if(sep_open){ // If it's open, we close
                        set_servo_pos(CLOSED_POSITION);
						sep_open = false;
						all_open = false;
                        stop_counting();
                        start_counting(blink_red,LED_BLINK_PERIOD);
                        LED_GREEN=1;
                    }   
                    else{ // If it's closed, we open
                        set_servo_pos(OPEN_SEP_POSITION);
                        sep_open = true;
						all_open = false;
                        stop_counting();
                        start_counting(blink_green,LED_BLINK_PERIOD);
                        LED_RED=1;
                    }
                    // We set servo_ok to false to indicate that the servo is rotating.
                    servo_ok = false;
                }
            }
            // Realeasing the button
            if(sep_button_pushed && SEP_BUTTON == 1)
                sep_button_pushed = false;
            break;
            
        case FLYING:
            // In FLYING mode, we wait until the separation date
            if(timestamp()-t0 > WAIT_SEPARATION_MS) {
                // Then we open the separation.
                set_servo_pos(OPEN_SEP_POSITION);
                t0 = timestamp(); // We save the new reference date
                // TODO : use only the takeoff date as reference
                state++;
            }
            break;
            
        case SEPARATED:
            // In FLYING mode, we wait until the parachute ejection date
            if(timestamp()-t0 > WAIT_RECUPERATION_MS) {
                // Then we eject parachute
                set_servo_pos(OPEN_ALL_POSITION);
                state++;
                stop_counting();
                LED_RED = 1;
                LED_GREEN = 1;
            }
            break;
            
        case OPEN:
            while(true){
                BUZZER=0;
                sleep(1000);
                BUZZER=1;
                sleep(5000);
            }
        case ERROR:
            break;
            
        case GROUND_CONFIG:
            // In GROUND_CONFIG mode, pushing the separation or recuperation buttons
            // will open/close the separation or recuperation servo.
            stop_counting();
            sleep(10);
            if(!open_all_button_pushed && ALL_OPEN_BUTTON == 0){
                sleep(10);
                if(ALL_OPEN_BUTTON == 0){
                    open_all_button_pushed = true;
                    if(sep_open){
                        LED_RED = 0;
                        LED_GREEN = 1;
						set_servo_pos(CLOSED_POSITION);
                        sep_open = false;
						all_open = false;
                    }   
                    else{
                        LED_RED = 1;
                        LED_GREEN = 0;
                        set_servo_pos(OPEN_ALL_POSITION);
                        sep_open = true;
						all_open = true;
                    }
                }
            }
            
            if(open_all_button_pushed && ALL_OPEN_BUTTON == 1)
                open_all_button_pushed = false;
            

            break;
    }
}
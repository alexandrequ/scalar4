#include "user.h"
#include "can.h"
#include "timing.h"
#include "clock.h"
#include "xbustypes.h"
#include "xbus.h"
#include "serial.h"
#include "data_processing.h"
#include "trajecto.h"
#include "xsens_config.h"
#include "canbus.h"
#include <stdbool.h>
#include <stddef.h>
#include <xc.h>
#include "config.h"

typedef enum { CONFIG = 0, INIT = 1, MEASURE = 2} Measure_state;
typedef enum { GROUND = 0, FLYING = 1, SCND_FLYING = 2, INHIBITED = 3 , GROUND_CONFIG = 4} Flight_state;

static long double t0 = 0;
static long double tcan = 0;

static Measure_state measure_state;
static Flight_state flight_state;
static bool ignition_detected;

void blink() {
    LED1 = ~LED1;
}

void send_log(){
    static uint8_t transmitted_state;
    transmitted_state = (get_clock_status() << 6)
                        | ((measure_state != CONFIG) << 5)
                        | (ENABLE_IGNITION << 4)
                        | (flight_state & 0x07);
    transmit_canframe((Canframe){LOG_CAN_ID, &transmitted_state, 1}, CAN_TX1);
}

// Sends the last Xsens message through CAN
void send_xsens_data(){
    // We send a header message to announce that the following data is a Xsens message
    uint8_t header[2] = {0x01,xbus_receive_buf->dataSize};
    sendLongCanMessage((Canframe){HEADER_CAN_ID,header ,2});
    // We send the Xsens message
    sendLongCanMessage((Canframe){DATA_CAN_ID,xbus_receive_buf->data, xbus_receive_buf->dataSize});
}

void send_traj_data(){ 
    uint8_t *t,*x,*y,*z,*w,*a,*b,*c;
    uint8_t frame[TRAJ_DATA_LEN];
    int i;
    
    // We send a header message to announce that the following data is trajecto data
    uint8_t header[2] = {0x02, TRAJ_DATA_LEN};
    sendLongCanMessage((Canframe){HEADER_CAN_ID, header, 2});

    float date = acquisition_data.sample_10kHz_date*0.0001;
    // We convert the floats into 4-bytes arrays
    t = (uint8_t*)(&date);
    x = (uint8_t*)(&(trajecto_data.pos[0]));
    y = (uint8_t*)(&(trajecto_data.pos[1]));
    z = (uint8_t*)(&(trajecto_data.pos[2]));
    w = (uint8_t*)(&(trajecto_data.quat[0]));
    a = (uint8_t*)(&(trajecto_data.quat[1]));
    b = (uint8_t*)(&(trajecto_data.quat[2]));
    c = (uint8_t*)(&(trajecto_data.quat[3]));
    
    // We prepare the frame data
    for(i=0; i<4; i++){
        frame[i]    = t[i];
        frame[4+i]  = x[i];
        frame[8+i]  = y[i];
        frame[12+i] = z[i];
        frame[16+i] = w[i];
        frame[20+i] = a[i];
        frame[24+i] = b[i];
        frame[28+i] = c[i];
    }
    sendLongCanMessage((Canframe){DATA_CAN_ID, frame, TRAJ_DATA_LEN});
}

/**
 * This function read a CAN buffer check the value of a specific bit. This function
 * was used to read another microcontroler log and check if a flag bit is set to 1
 * This function is now unsused, but I have left it here as an example of CAN reading.
 * 
 * @param buffer The CAN buffer to check
 * @param bit_num The bit to be checked (0-7)
 * @return The value of the bit
 */
bool check_signal(Canrxbuf buffer,int bit_num){
    if(new_canframe_received(buffer)){
        const Canframe fr = receive_can(buffer, false);
        const uint8_t data = *(fr.databytes);
        return (data>>bit_num)& 0b1;
    }
    else{
        return false;
    }
}

void init_app() {
    // Configuration of the clock PLL
    // Frequency = Oscillator_freq * (_PLLDIV+2)/((_PLLPRE+2)*2(_PLLPOST+1) = 128Mhz
    // note: Il faut mettre #pragma config FNOSC = PRIPLL dans config_vits.c pour pouvoir utiliser le PPL
    // note: The overclocking is necessary for faster computation.
    // TODO: We can use config_clock_PLL(8,2,128) instead
    _PLLPRE = 6;
    _PLLPOST = 0;
    _PLLDIV = 126;
    
    // Failsafe_clock configured at 7.37/4/2*139 = 128.05MHz
    config_failsafe_clock(4, 2, 139);
    
    TRISA = 0x00;
    TRISB = 0x00;
    
    zero_hour(); // We initialize timing drivers

    __builtin_write_OSCCONL(OSCCONL & 0xbf); // D�verrouillage du PPS

    // R�ception UART sur RP39 (pin n� 16)
    RPINR18bits.U1RXR = 39;
    // Transmission UART sur RP40 (pin n� 17)
    RPOR3bits.RP40R = 0x0001;
    _TRISB7 = 1; // Pin 16 (UART Rx) en entree
    _TRISB8 = 0; // Pin 17 (UART Tx) en sortie

    // R�ception CAN sur RP38 (pin n� 15)
    RPINR26bits.C1RXR = 38;
    // Transmission CAN sur RP37 (pin n� 14)
    RPOR1bits.RP37R = 0x000E;
    _TRISB6 = 1; // CAN Rx en entr�e
    _TRISB5 = 0; // CAN TX en sortie
    __builtin_write_OSCCONL(OSCCONL | 0x40); // Verrouillage du PPS


    IO_LED1 = 0; // Led 1
    IO_LED2 = 0; // Led 2
    IO_CTS = 0; // CTS en sortie
    IO_nRST = 0; // nRST en sortie
    IO_ENABLE_IGNITION = 0; // ENABLE_IGNITION en sortie
    IO_UMBILICAL = 1; // UMBILICAL en entree

    ANSELBbits.ANSB8 = 0;

    LATB = 0x0000; // port B initialis� � 0
    LED1 = 0; // LED 1 allum�e
    LED2 = 1; // LED 2 allum�e

    init_counting(); // Initialize the periodic functions

    config_uart1(115200, UART_CLOSEST); // Initialize UART driver

    config_can(BR_500_KBPS, true, 1, 3, 4); // Initialize CAN driver
    initExtendedCanDriver(); // Initialize high level CAN driver (the function sendLongCanMessage())

    nRST = 1;
    CTS = 0;
    ENABLE_IGNITION = 0;
    measure_state = CONFIG;
    flight_state = GROUND;
    ignition_detected = false;
    send_log();
    tcan = timestamp();
}

void user_task() {
    
    // We send a CAN log every LOG_PERIOD
    if(timestamp()-tcan >= LOG_PERIOD){
        tcan = timestamp();
        send_log();
    }

    bool data_received = false;
    switch (measure_state) {

        case CONFIG:

            // the Xsens is reset
            nRST = 0;
            sleep(500);
            nRST = 1; //FIXME open-drain instead
            // Checking wake-up message
            if (!receive_xbus_buf(2000) || xbus_receive_buf->MID != 0x3E) // WakeUp
                break;

            // Transmission of the CoToConfig message
            if (!goToConfig())
                break;

            // Setting Xsens option
            if (!configOptions((XSensOption[]) {EnableInRunCompassCalibration}, 1))
                break;

            // Configuration of the content and frequency of Xsens messages
            if (!configOutputs(100, (XSensOutput[]) {
                    XDI_SampleTimeFine, XDI_RateOfTurn, XDI_Acceleration}, 3))
                break;

            // Transmission of the CoToMeasurment message
            if (!goToMeasure())
                break;

            // The first measure is always erroneous, so I dump it
            while (receive_xbus_buf(XSENS_RX_TIMEOUT) && xbus_receive_buf->MID != 0x36);
            //start_counting(blink,500);
            measure_state++;    // Change state to INIT
            send_log();
            break;

        case INIT:
            // We stay in INIT mode until we receive a measure message from Xsens (Message ID = 0x36)
            if ((receive_xbus_buf(XSENS_RX_TIMEOUT) && xbus_receive_buf->MID == 0x36)) {
                read_xbus_data(); // We decode the data in the message
                
                // We init the ground moving averages (while the rocket is static):
                // - The average rotation speed will be computed to determine gyroscopes bias
                // - The average acceleration (= -gravity) will be computed to determine the
                //   initial orientation of the rocket
                initGroundAveraging(GROUND_AVERAGING_SIZE, GROUND_AVERAGING_DELAY);
                // We init the measure of acceleration. This calculation will be done
                // during all the flight to detect brutal acceleration from rocket engines
                initAccelerationAveraging(ACC_AVERAGING_SIZE);
                // Now we can start the trajecto recording
                measure_state = MEASURE;
                
                // We check if the umbilical is plugged
                if(UMBILICAL == 1){
                    // If not, we go into configuration mode
                    flight_state=GROUND_CONFIG;
                    stop_counting();
                    LED1 = 0;
                    LED2 = 0;
                    send_log();
                }
                else{
                    // If it is plugged, we go into nominal ground mode
                    flight_state=GROUND;
                    start_counting(blink,LED_BLINK_PERIOD_GROUND_MS);
                    LED2 = 0;
                    send_log();
                }

            }
            break;

        case MEASURE:

            // We check if a new measure message from Xsens has been received
            if ((receive_xbus_buf(XSENS_RX_TIMEOUT) && xbus_receive_buf->MID == 0x36)) {
                send_xsens_data(); // We transmit the raw xsens message through CAN
                read_xbus_data(); // We decode the data in the message
                
                data_received = true;
            }

            switch (flight_state) {

                case GROUND:
                    // In ground mode, the rocket is static on the launchpad.
                    
                    if (data_received) {
                        // In ground mode, we use the measures from Xsens to update
                        // the moving average of angular speed and acceleration
                        updateGroundAveraging(IDLE_ACCELERATION_TRESHOLD);
                    }
                    if (UMBILICAL == 1) { // We check the umilical to detect takeoff
                        t0 = timestamp(); // We store the date of takeoff
                        // Debouncing : We verify that the pin remains at 1 for WAIT_REBONDS_MS
                        // to be sure that this is not some random noise
                        while (UMBILICAL == 1 && timestamp() - t0 < UMBILICAL_DEBOUNCING_DELAY);
                        if (UMBILICAL == 1) {
                            // Takeoff is confirmed
                            // We also save the takeoff date for the 'trajecto' file
                            save_takeoff_date();
                            // We stop ground computations and we store the deduced
                            // gyro bias and initial rocket orientation
                            stopGroundAveraging();
                            // We go into flight mode
                            flight_state++;
                            start_counting(blink, LED_BLINK_PERIOD_FLYING_MS);
                            send_log();
                            
                        }
                    }
                    break;

                case FLYING:

                    if (data_received) {
                        // In flight mode, we integrate the rotation speed and acceleration
                        // from Xsens to compute rocket orientation and position
                        integration();
                        send_traj_data(); // We send some decoded data through CAN
                        
                        // We check if the rocket orientation is inside the nominal cone
                        // and security cone
                        switch (check_trajecto()) {
                            case NOMINAL:
                                // If orientation is inside the nominal cone, we
                                // enable motor ignition
                                ENABLE_IGNITION = 1;
                                LED2 = 0;
                                break;

                            case NOT_NOMINAL:
                                // If orientation is outside the nominal cone but
                                // inside security cone, we forbid motor ignition
                                ENABLE_IGNITION = 0;
                                LED2 = 1;
                                break;

                            case HAZARDOUS:
                                // If orientation is outside the security cone, we
                                // permanently forbid motor ignition and go to
                                // inhibited mode.
                                ENABLE_IGNITION = 0;
                                LED2 = 1;
                                stop_counting();
                                LED1 = 1;
                                flight_state = INHIBITED;
                                //start_counting(blink, 100);
                                send_log();
                                break;
                        }

                        /* This block has been commented because it is irrelevant for a 1 stage rocket
                        // We check the average acceleration of the rocket to detect
                        // the ignition of the second motor
                        if(check_propulsion(FREE_FALL_ACC_TRESH,PROPULSION_ACC_TRESH)){
                            // If we detect 2nd engine ignition, we go to SCND_FLYING
                            // mode and and we stop checking the orientation
                            ENABLE_IGNITION = 0; // We forbid ignition in case of false detection
                            ignition_detected = true;
                            flight_state = SCND_FLYING;
                            stop_counting();
                            LED1 = 0;
                            LED2 = 0;
                        }*/
                        
                        // If SLEEP_DATE_MS has elapsed since takeoff we put
                        // the microcontroller into sleep mode to save power
                        if (timestamp() - t0 > SLEEP_DATE_MS) {
                            LED1 = 1; // We also turn off the LEDs to save more power
                            LED2 = 1;
                            nRST = 0;
                            send_log();
                            Sleep();
                        }
                    }

                    break;

                case SCND_FLYING:
                case INHIBITED:
                        if (data_received){
                            // We continue to integrate Xsens measure
                            integration();
                            send_traj_data(); // We send some decoded data through CAN
                        }
                        
                        // If SLEEP_DATE_MS has elapsed since takeoff we put
                        // the microcontroller into sleep mode to save power
                        if (timestamp() - t0 > SLEEP_DATE_MS) {
                            LED1 = 1; // We also turn off the LEDs to save more power
                            LED2 = 1;
                            nRST = 0;
                            send_log();
                            Sleep();
                        }
                    break;

                case GROUND_CONFIG:
                    LED2=0;
                    if (data_received) {
                        updateGroundAveraging(IDLE_ACCELERATION_TRESHOLD);
                    }
                    break;
            }
            break;
    }
}

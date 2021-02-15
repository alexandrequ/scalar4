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
#include "can_id.h"

typedef enum { CONFIG = 0, INIT = 1, MEASURE = 2} Measure_state;
typedef enum { GROUND = 0, FLYING = 1, SCND_FLYING = 2, INHIBITED = 3 , GROUND_CONFIG = 4, WINDOW = 5} Flight_state;

static long double t0 = 0;
static long double tcan = 0;

static Measure_state measure_state;
static Flight_state flight_state;
static bool ignition_detected;

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

void send_log1(){
    static uint8_t transmitted_state;
    transmitted_state = (get_clock_status() << 6)
                        | ((measure_state != CONFIG) << 5)
                        | (ENABLE_IGNITION << 4)
                        | (flight_state & 0x07);
    transmit_canframe((Canframe){LOG_CAN_ID, &transmitted_state, 1}, CAN_TX1);
}

void send_log2() {
#ifdef LOG_CAN_ID
    static uint8_t transmitted_state;
    transmitted_state = (get_clock_status() << 6) 
                        | (FDC_COIFFE << 5)
                        | (state & 0x07);
    transmit_can(LOG_CAN_ID, &transmitted_state, 1, CAN_TX1);
#endif
}

bool read_indu(){
    AD1CON1bits.SAMP = 0;// Start the conversion
    while (!AD1CON1bits.DONE);// Wait for the conversion to complete
    AD1CON1bits.DONE = 0;// Clear conversion done status bit
    uint16_t result = ADC1BUF0;// Read the ADC conversion result
    return (result > INDU_TRESHOLD);
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

void init_app(){
	zero_hour();

	TRISA = 0x00;
	TRISB = 0x0000;

	
    _PLLPRE = 6;
    _PLLPOST = 0;
    _PLLDIV = 126;

    //configuration du can et de l'uart
    __builtin_write_OSCCONL(OSCCONL & 0xbf);
    // R�ception UART sur RP39 (pin n� 16)
    RPINR18bits.U1RXR = 39;
    // Transmission UART sur RP40 (pin n� 17)
    RPOR3bits.RP40R = 0x0001;
    _TRISB7 = 1; // Pin 16 (UART Rx) en entree
    _TRISB8 = 0; // Pin 17 (UART Tx) en sortie

    RPINR26bits.C1RXR = 38;
    // Transmission CAN sur RP37 (pin n° 14)
    RPOR1bits.RP37R = 0x000E;
    _TRISB6 = 1;  // RB6 (ie RP38 ie CANRX) en entrée
    _TRISB5 = 0;  // RB5 (ie RP37 ie CANTX) en sortie
    __builtin_write_OSCCONL(OSCCONL | 0x40);

    set_can_mask(CAN_MASK0, 0x7FF);
    enable_can_rx_buffer(RECU_BUFFER, SEQ_RECU_LOG_CAN_ID, CAN_MASK0);
    enable_can_rx_buffer(CHUT_BUFFER, SEQ_PYRO_LOG_CAN_ID, CAN_MASK0);
    config_can(BR_500_KBPS, true, 1, 3, 4);

    config_uart1(115200, UART_CLOSEST); // Initialize UART driver
    
    //configuration des entrees/sorties
    	//attitude
    IO_UMBILICAL = 1; // RB12 en entrée (ombilical)
    IO_FDC1 = 1;  // RB8  en entrée (fdc1)
    IO_FDC2 = 1;  // RB9  en entrée (fdc2)
    IO_FDC_COIFFE = 1; // RB13 en entrée (fdc coiffe)
    IO_LOG_TRAJ = 1;  // RA4  en entrée (log trajecto)
    IO_IGNITION = 0; // RB15 en sortie (commande mise à feu)
    IO_LED1 = 0; // LED1 en sortie
    IO_LED2 = 0; // LED2 en sortie
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
    	//mise a feu
    IO_CTS = 0; // CTS en sortie
    IO_nRST = 0; // nRST en sortie (noRESET)
    IO_ENABLE_IGNITION = 0; // ENABLE_IGNITION en sortie

    //valeurs des pins au départ
    LATA = 0x00; // port A initialisé à 0
    LATB = 0x0000; // port B initialisé à 0
    nIGNITION = 1;

//j'ai retiré un anselb8

}


void user_task(){
    
    if(timestamp()-t_adc > ADC_PERIOD_MS){
        t_adc = timestamp();
        indu = read_indu();
        // FDC1, FDC2 or indu are at 0 when they detect separation
        // At least 2 out of 3 sensor must be at 0 to consider the rocket is separated
        separated = ((!FDC1) && (!FDC2)) || ((!FDC1) && (!indu)) || ((!FDC2) && (!indu));
    }

    if(!separated) t_sep = timestamp();

    if(init_xsens){
	    // We check if a new measure message from Xsens has been received
	    if ((receive_xbus_buf(XSENS_RX_TIMEOUT) && xbus_receive_buf->MID == 0x36)) {
	        send_xsens_data(); // We transmit the raw xsens message through CAN
	    	read_xbus_data(); // We decode the data in the message
	                
	    	data_received = true;
	    }
	}

	switch(state){

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

            state = INIT;
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
                init_xsens = true;
	            if(UMBILICAL == 1) {
	                // If umbilical is unplugged, we go to configuration mode
	                t0 = timestamp();
	                state = GROUND_CONFIG;
	            }
	            else{
	            	state = GROUND;
	            }
	        }
			break;

		case GROUND:
            if (data_received) {
                // In ground mode, we use the measures from Xsens to update
                // the moving average of angular speed and acceleration
                updateGroundAveraging(IDLE_ACCELERATION_TRESHOLD);
            }

			if(UMBILICAL == 1){
				t0 = timestamp();
				takeoff_validated = true;
				while(timestamp()-t0<WAIT_REBONDS_MS){
					if(UMBILICAL == 0){
						takeoff_validated = false;
						break;
					}
				}

				if(takeoff_validated == true && separated == false){
					state = FLYING;
				}

				if(takeoff_validated == true && separated == true){
					state = INHIBITED;
				}
				stopGroundAveraging();

			}
			break;

		case FLYING:
			if(data_received){
				integration();
				send_traj_data();
			}
			break;

		case WINDOW:
			break;

		case INHIBITED:
			break;

		case GROUND_CONFIG:
			break;
	}
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
    // We send a CAN log every LOG_PERIOD
    if(timestamp()-tcan >= LOG_PERIOD){
        tcan = timestamp();
        send_log1();
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
            send_log1();
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
                init_xsens = true;
                
                // We check if the umbilical is plugged
                if(UMBILICAL == 1){
                    // If not, we go into configuration mode
                    flight_state=GROUND_CONFIG;
                    stop_counting();
                    LED1 = 0;
                    LED2 = 0;
                    send_log1();
                }
                else{
                    // If it is plugged, we go into nominal ground mode
                    flight_state=GROUND;
                    start_counting(blink,LED_BLINK_PERIOD_GROUND_MS);
                    LED2 = 0;
                    send_log1();
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
                            send_log1();
                            
                        }
                    }
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
                    send_log2();
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
                                send_log1();
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
                            send_log1();
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
                            send_log1();
                            Sleep();
                        }
                    break;

                case GROUND_CONFIG:
                    LED2=0;
                    if (data_received) {
                        updateGroundAveraging(IDLE_ACCELERATION_TRESHOLD);
                    }
                    LED1 = FDC_COIFFE;
                    LED2 = separated;
                    break;
            }
            break;
    }
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
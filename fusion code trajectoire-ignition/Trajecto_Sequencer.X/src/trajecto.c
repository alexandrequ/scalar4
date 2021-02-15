#include "config.h"
#include "trajecto.h"
#include "quaternion.h"
#include "timing.h"
#include "can.h"
#include "delayed_average.h"
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

Acquisition_Data acquisition_data;
Trajecto_Data trajecto_data;

float gravity[3];
float gyroBias[3];
float take_off_date = -1;

// These two averages are computed while the rocket is on the lanchpad
DelayedMovingAverage meanGravity; // Used to determine rocket orientation on the lauchpad
DelayedMovingAverage meanGyroBias; // Used to estimate the gyroscopes bias
// This average is computed on the ground and during the flight
DelayedMovingAverage meanAcceleration; // Used to detect brutal acceleration from engines

void initGroundAveraging(int size, int delay){
    initDelayMovingAverage(&meanGravity,size,delay);
    initDelayMovingAverage(&meanGyroBias,size,delay);
}

void initAccelerationAveraging(int size){
    initDelayMovingAverage(&meanAcceleration,size,0);
}

void updateGroundAveraging(float takeOffTreshold){
    // Update of the acceleration average for later purpose
    updateAverage(&meanAcceleration, acquisition_data.acc);

    // TODO temporarily stop averaging after an abberant value
    //static int lastSkippingDate;
    float *g = acquisition_data.acc;
    // If the mesured acceleration is higher that a treshold, it is dumped beacause
    // this value is not representative of the gravity vector, and rotation are integrated instead.
    if(g[0]*g[0]+g[1]*g[1]+g[2]*g[2] < takeOffTreshold*takeOffTreshold){
        updateAverage(&meanGyroBias, acquisition_data.gyr);
        updateAverage(&meanGravity, acquisition_data.acc);

        init_trajecto();
    }
    else{
        // TODO : j'ai oublié de prendre en compte les biais de gyro en mode sol.
        integration();
    }
}

void stopGroundAveraging(){
    // The results of the delayed moving averages are copied in global variables
    // x axis is the vertical
    float quat[4];
    initial_quaternion(quat,meanGravity.delayedAverage);
    normalize(quat);
    rotate_vect_with_quat(gravity, meanGravity.delayedAverage, quat);
    
    gyroBias[0] = meanGyroBias.delayedAverage[0];
    gyroBias[1] = meanGyroBias.delayedAverage[1];
    gyroBias[2] = meanGyroBias.delayedAverage[2];
    // The delayed moving average on Gyro bias is freed

}

void save_takeoff_date(){
    take_off_date = trajecto_data.date;
}

void init_trajecto(){
    int i;

    trajecto_data.date=acquisition_data.sample_10kHz_date*0.0001;
    for(i = 0; i<3; i++){
        trajecto_data.pos[i]=0;
        trajecto_data.vel[i]=0;
        trajecto_data.gyr[i]=0;
    }
    trajecto_data.imp=0;

    initial_quaternion(trajecto_data.quat,meanGravity.delayedAverage);
    normalize(trajecto_data.quat);

}

#define CORRECT_BIAS 1
#define COMPUTE_POS 1
void integration(){
    // The date of the acquisition computed from the Xsens timestamp
    float date = acquisition_data.sample_10kHz_date*0.0001;
    long double delta_t = (date - trajecto_data.date);
    float delta_rot_quat[4]; // Quaternion associï¿½ ï¿½ la rotation pendant delta_t
    int i;
    
#if CORRECT_BIAS
    // The measured angular rate is corrected from the measured gyro bias
    for(i=0; i<3; i++)
        trajecto_data.gyr[i]=acquisition_data.gyr[i]-gyroBias[i];
#else
    for(i=0; i<3; i++)
        trajecto_data.gyr[i]=acquisition_data.gyr[i];
#endif

    // Calcul de delta_rot_quat
    angle_to_quaternion(delta_rot_quat, trajecto_data.gyr[0]*delta_t, trajecto_data.gyr[1]*delta_t, trajecto_data.gyr[2]*delta_t);

    // Mise ï¿½ jour de trajecto_data.quat = rotation par rapport au "haut" dans le repï¿½re terrestre
    quaternion_prod(trajecto_data.quat, trajecto_data.quat, delta_rot_quat);

    // Calcul de l'impulsion totale
    //trajecto_data.imp += acquisition_data.acc[2];

#if COMPUTE_POS
    // Changement de base du vecteur acceleration du repere fusï¿½e au repï¿½re terrestre
    rotate_vect_with_quat(trajecto_data.acc, acquisition_data.acc, trajecto_data.quat);

    // acceleration integration
    for(i=0; i<3; i++){
        trajecto_data.vel[i] += delta_t * (trajecto_data.acc[i] - gravity[i]);
        trajecto_data.pos[i] += delta_t *  trajecto_data.vel[i];
    }
#endif
    trajecto_data.date=date;

}

/**
 * This function update the rocket quaternion using the quaternionic increment.
 * Howerver, the Xsens-mti1 used in 2019 does not provide quaternionic increment,
 * so this function is unused and integration() is used instead.
 */
void incrementation(){
    float date = acquisition_data.sample_10kHz_date*0.0001;
    //float dT = date - trajecto_data.date;
    trajecto_data.date = date;

    //int i;
    //float dV_terrestre[3];

    // Remarque : corriger betement la dï¿½rive ï¿½ partir du quaternion mesurï¿½ au repos ne fonctionne pas.
    // Il faut passer par les vitesses angulaires.
    // Mise ï¿½ jour de trajecto_data.quat = rotation par rapport au "haut" dans le repï¿½re terrestre
    quaternion_prod(trajecto_data.quat, trajecto_data.quat , acquisition_data.deltaQ);
    normalize(trajecto_data.quat);
}

#define A0 (1.4267)
#define A1 (-0.0802)
#define A2 (0.0161)
#define A3 (-0.0015)
#define PHI_RAMPE (1.396263402)

Nominality check_trajecto(){
    float ang[2];
    // Computation of heading and elevation from rocket's quaternion
    quat_to_euler(ang , trajecto_data.quat);

    // We check if the angles are outside the security cone
    if(ang[0] > MAX_CRIT_HEADING_RAD || ang[0] < MIN_CRIT_HEADING_RAD)
        return HAZARDOUS;
    if(ang[1] > MAX_CRIT_ELEVATION_RAD || ang[1] < MIN_CRIT_ELEVATION_RAD)
        return HAZARDOUS;
    
    // We check if the angles are outside the nominal cone
    if(ang[0] > MAX_HEADING_RAD || ang[0] < MIN_HEADING_RAD)
        return NOT_NOMINAL;
    if(ang[1] > MAX_ELEVATION_RAD || ang[1] < MIN_ELEVATION_RAD)
        return NOT_NOMINAL;

    return NOMINAL;

}

typedef enum Propulsion_state{FIRST_ACCELERATION, FREE_FLIGHT, SECOND_ACCELERATION} Propulsion_state;

bool check_propulsion(float freeFallTresh, float propulsionTresh){
    // We memorize the state of the function between each call in this variable
    static Propulsion_state state = FIRST_ACCELERATION;
    // Updating the average of the acceleration
    updateAverage(&meanAcceleration, acquisition_data.acc);
    switch(state){
        // This function should be called for the first time just after 
        // the umbillical is unplugged, so the first engine should be thrusting.
        case FIRST_ACCELERATION:
            // First engine is thursting
            if(meanAcceleration.delayedAverage[2] < freeFallTresh)
                // The engine has stopped thrusting
                state++;
            return false;

        case FREE_FLIGHT:
            // No thrusting, only air friction apply to the rocket
            if(meanAcceleration.delayedAverage[2] > propulsionTresh){
                // The second engine has started trusting
                state++;
                return true;
            }
            else
                return false;

        case SECOND_ACCELERATION:
            return true;
    }
    return false;
}

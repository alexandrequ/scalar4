/* 
 * File:   xsens_config.h
 * Author: Damien
 *
 * Created on 4 novembre 2018, 13:43
 */

#ifndef XSENS_CONFIG_H
#define	XSENS_CONFIG_H

#include <stdint.h> // définit uint8_t, uint16_t et uint32_t
#include <stdbool.h> // définit bool

typedef enum XSensOption {
    // When set, the configuration changes will not be saved in the flash memory
    // and will be lost on reset
    DisableAutoStore = 0x01,
            
    // When set, the MTi will not automatically go into measurement mode after 500ms
    DisableAutoMeasurement = 0x02,
            
    // Enables Beidou, disables GLONASS (MTi-G-710)
    EnableBeidou = 0x04,
            
    /* 
     * When set to 1, the MTi will have Active Heading Stabilization (AHS) enabled.
     * AHS overrides magnetic reference, so heading output will be heading tracking
     * instead of referenced heading.
     */
    EnableAHS = 0x10,
            
    /*
     * When set to, the MTi will have In-run Compass Calibration (ICC) enabled. 
     * ICC compensates for magnetic disturbances that move with the object.
     */
    EnableInRunCompassCalibration = 0x80
} XSensOption;

typedef enum XSensOutput{
    /// Timestamp of the MTi in 10000Hz ticks
    XDI_SampleTimeFine = 0x1060,
    /// Speed Increment in m/s
    XDI_DeltaV         = 0x4010,
    /// Acceleration in  m/s^2
    XDI_Acceleration   = 0x4020,
    /// Rotation speed around the 3 axes in rad/s
    XDI_RateOfTurn     = 0x8020,
    /// Quaternion Increment (see quaternion.h)
    XDI_DeltaQ         = 0x8030,
    /// Magnetic field in contructor's arbitrary unit (see on XBASE)
    XDI_MagneticField  = 0xC020,
    /// A byte partially representing the status of the MTi
    XDI_StatusByte     = 0xE010
} XSensOutput;
          
/**
 * @brief Set the maximum time to wait for the XSens acknowlegment message.
 * 
 * @param timeout The time to wait in ms.
 */
void setAcknowledgeTimeout(uint16_t timeout);

/**
 * @brief Activate (set) all Xsens options passed as parameter, and deactivate (clear) all other options
 * 
 * See the XSensOption enumeration for more details the option.
 * Passing an empty list will deactivate all options.
 * 
 * @warning Not all option are avaible for all MTi model.
 * @warning If the 'DisableAutoStore' is not set, these modification will be kept afet MTi reset.
 * 
 * @param options A list of the option you want to be set. All others will be cleared.
 * @param nb Number of option in the list
 * @return 'true' if the acknowlegment message from Xsens have been received
 */
bool configOptions(XSensOption options[], int nb);

/**
 * @brief Configure the measure output of the Xsens
 * 
 * The measures will be sent in the Xsens MTData2 message (0x36) in the same order
 * than in the XSensOutput list passed in parameter.
 * 
 * @param frequency Frequency (Hz) at witch measures will be outputed.
 * @param output An ordered list of the measure to be output by the XSens.
 * @param nb Number of parameters in the list.
 * @return 'true' if the acknowlegment message from Xsens have been received
 */
bool configOutputs(uint8_t frequency, XSensOutput output[], int nb);

/**
 * @brief Switch the Xsens into Config State
 * 
 * @return 'true' if the acknowlegment message from Xsens have been received
 */
bool goToConfig();

/**
 * @brief Switch the Xsens into Measurement State
 * 
 * @return 'true' if the acknowlegment message from Xsens have been received
 */
bool goToMeasure();

/**
 * @brief Start the MTi's No Rotation Update procedure.
 * 
 * @Warning Not supported by the MTi-1 !
 * 
 * @param timeout The duration of the No Rotation Update procedure in s.
 * @return 
 */
bool setNoRotation(uint16_t timeout);

#endif	/* XSENS_CONFIG_H */


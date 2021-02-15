/******************************************************************************
 *
 *                Microchip Memory Disk Drive File System
 *
 ******************************************************************************
 * FileName:        FSconfig.h
 * Processor:       PIC18/PIC24/dsPIC30/dsPIC33/PIC32
 * Dependencies:    None
 * Compiler:        C18/C30/C32
 * Company:         Microchip Technology, Inc.
 * Version:         1.3.0
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the �Company�) for its PICmicro� Microcontroller is intended and
 * supplied to you, the Company�s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN �AS IS� CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
*****************************************************************************/

#ifndef _FS_DEF_
#define _FS_DEF_

#include "fileio_lfn.h"

// Macro indicating how many drives can be mounted simultaneously.
#define FILEIO_CONFIG_MAX_DRIVES        1

// Delimiter for directories.
#define FILEIO_CONFIG_DELIMITER '/'

// Macro defining the maximum supported sector size for the FILEIO module.  This value should always be 512 , 1024, 2048, or 4096 bytes.
// Most media uses 512-byte sector sizes.
#define FILEIO_CONFIG_MEDIA_SECTOR_SIZE 		512

/* *******************************************************************************************************/
/************** Compiler options to enable/Disable Features based on user's application ******************/
/* *******************************************************************************************************/

// Uncomment FILEIO_CONFIG_FUNCTION_SEARCH to disable the functions used to search for files.
//#define FILEIO_CONFIG_SEARCH_DISABLE

// Uncomment FILEIO_CONFIG_FUNCTION_WRITE to disable the functions that write to a drive.  Disabling this feature will
// force the file system into read-only mode.
//#define FILEIO_CONFIG_WRITE_DISABLE

// Uncomment FILEIO_CONFIG_FUNCTION_FORMAT to disable the function used to format drives.
#define FILEIO_CONFIG_FORMAT_DISABLE

// Uncomment FILEIO_CONFIG_FUNCTION_DIRECTORY to disable use of directories on your drive.  Disabling this feature will
// limit you to performing all file operations in the root directory.
//#define FILEIO_CONFIG_DIRECTORY_DISABLE

// Uncomment FILEIO_CONFIG_FUNCTION_PROGRAM_MEMORY_STRINGS to disable functions that accept ROM string arguments.
// This is only necessary on PIC18 parts.
#define FILEIO_CONFIG_PROGRAM_MEMORY_STRINGS_DISABLE

// Uncomment FILEIO_CONFIG_FUNCTION_DRIVE_PROPERTIES to disable the FILEIO_DrivePropertiesGet function.  This function
// will determine the properties of your device, including unused memory.
//#define FILEIO_CONFIG_DRIVE_PROPERTIES_DISABLE

// Uncomment FILEIO_CONFIG_MULTIPLE_BUFFER_MODE_DISABLE to disable multiple buffer mode.  This will force the library to
// use a single instance of the FAT and Data buffer.  Otherwise, it will use one FAT buffer and one data buffer per drive
// (defined by FILEIO_CONFIG_MAX_DRIVES).  If you are only using one drive in your application, this option has no effect.
//#define FILEIO_CONFIG_MULTIPLE_BUFFER_MODE_DISABLE

//-------------Function name redirects------------------------------------------
// During the media initialization sequence for SD cards, it is
// necessary to clock the media at a frequency between 100 kHz and 400 kHz,
// since some media types power up in open drain output mode and cannot run
// fast initially.
// On PIC18 devices, when the CPU is running at full frequency, the standard SPI
// prescalars cannot reach a low enough SPI frequency.  Therefore, we provide these
// configuration options to allow the user to remap the SPI functions called during
// the "slow" part of the initialization to user-implemented functions that can
// provide the correct functionality.  For example, a bit-banged SPI module could
// be implemented to provide a clock between 100 and 400 kHz.

// For this demo, the slow functions are mapped to the fast SPI driver functions.
// The slower clock frequency will be achieved by disabling the PLL during the
// SD Card initialization.  Note that the SYS_CLK_FrequencySystemGet function was implemented
// to return the correct clock frequency in using a PLL-enabled or PLL-disabled clock.




#endif

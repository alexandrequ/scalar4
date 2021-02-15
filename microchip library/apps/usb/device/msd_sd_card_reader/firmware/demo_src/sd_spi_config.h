/*******************************************************************************
Copyright 2016 Microchip Technology Inc. (www.microchip.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

To request to license the code under the MLA license (www.microchip.com/mla_license), 
please contact mla_licensing@microchip.com
*******************************************************************************/

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

#if defined (__XC8)
    // Define the function to initialize the SPI module for operation at a slow clock rate
    #define FILEIO_SD_SPIInitialize_Slow    InitSPISlowMode
    // Define the function to send a media command at a slow clock rate
    #define FILEIO_SD_SendMediaCmd_Slow     FILEIO_SD_SendCmdSlow
    // Define the function to write an SPI byte at a slow clock rate
    #define FILEIO_SD_SPI_Put_Slow          WriteSPIManual
    // Define the function to read an SPI byte at a slow clock rate
    #define FILEIO_SD_SPI_Get_Slow          ReadMediaManual
#else
    // Define the function to initialize the SPI module for operation at a slow clock rate
    #define FILEIO_SD_SPIInitialize_Slow    FILEIO_SD_SPISlowInitialize
    // Define the function to send a media command at a slow clock rate
    #define FILEIO_SD_SendMediaCmd_Slow     FILEIO_SD_SendCmd
    // Define the function to write an SPI byte at a slow clock rate
    #define FILEIO_SD_SPI_Put_Slow          DRV_SPI_Put
    // Define the function to read an SPI byte at a slow clock rate
    #define FILEIO_SD_SPI_Get_Slow          DRV_SPI_Get
#endif



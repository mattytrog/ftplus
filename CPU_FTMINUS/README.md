## Welcome to FTMinus!
This is a special small version of FTPlus, intended for small PIC chips, or as an alternative to FTPlus.

## Features
This has only STOCK YAESU FT757GX features, with the addition of the following:

 - On large PIC chips (18F452 etc) bootloader is included and supported, should one wish to go back to full FTPlus, or upgrade as one wishes.
 - Fine Tuning
 - FT757GX2 Emulation - Duplex CAT facility. All GX2 CAT features are implemented
 - CAT TX is available, using the later Yaesu 0F CAT opcode (eg 000000010F = TX active). This works well in TRX Manager
 - Baud Change is available for apps that support a higher baud rate, accessible on opcode FD. 
 - 00000001FD = 4800
 - 00000002FD = 19200
 - 00000003FD = 57600
 - 00000004FD = 115200
 
## Caveats
 - FT757GX2 support in FLRig is broken. Not my fault!
 - DX Labs Commander and TRX-Manager have been tested and all features work.
 - Operating mode (SSB/AM/FM etc) are assigned to a dummy value, saved to EEPROM. There is no way of switching modulation modes automatically on the GX2.
 - Kenwood emulation is NOT SUPPORTED in FTMinus.
 - Accelerated dial, frequency correction, manual button tuning and enhanced PMS are NOT SUPPORTED in FTMinus.
 - A more basic PMS scanner is provided.
 - FTPlus configurator is NOT SUPPORTED.
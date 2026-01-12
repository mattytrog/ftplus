## Welcome to FTPlus!
This is a custom suite of projects, firmware and software intended for the Yaesu FT757GX transceiver

## Replacement CPU and firmware

This is a replacement CPU board, using a PIC controller and custom firmware to add some useful new features to the old FT757. These features are outlined below:

 - Additional memories (14 usable MR memories, together with 20 additional stored VFO "bands")
 - Enhanced duplex CAT system, supporting tuning, memory management and TX activation
 - Off-frequency correction in software
 - Accelerated dial / fuzzy logic VFO wheel tuning
 - Fine-tuning and showing of hidden digit when tuning
 - Full button-based fast manual tuning for usits without VFO wheel
 - Enhanced scanner-like PMS scan system, which supports custom scans, custom bands and more
 - CB Transceiver mode, supporting all FCC / UK bands, with full clarifier and fine tuning support
 - Bootloader support, with FTPlus Updater app to enable firmware upgrades without a Pickit
 - Full app support and programming and experimentation with the FTPlus Configurator app
 - Full documentation with a Yaesu-styled 30 page PDF file, called "SUPPLEMENT.PDF"
 - Fully open-source

## Replacement VFD display CPU
Based on the excellent work of Rudolf Kauls, this is a suite of board design files (gerbers) to enable successful repair of a faulty display.

 - Easy-fit board designs (I have boards for sale, for minimal cost, either populated or blank)
 - Variety of PIC chip designs with smaller footprints
 - Original plastic 500kc oscillator is NOT REQUIRED with this

## Replacement Dial unit
Complete replacement dial unit, using a small PIC16 chip, enabling the use of any quadrature encoder with the FT757.

 - Bourns encoders with ball-bearings supported
 - Nidec Copal encoders supported
 - Any resolution encoder
 - Original Yaesu beep is implemented, using either a passive piezo speaker, or an "active" beeper. If using a passive unit, change volume, pitch or duration of beep
 - Custom increment options via solder jumpers
 - Quick diagnostic / self-test routine (by spinning VFO and powering on while spinning)

## Replacement signal meter light board
Using 4 x LEDS, a custom board, replacing the bulbs, with easy to connect JST connector if required. Light guides are removed and replaced with this maintenance-free board. 12v and 5v designs.

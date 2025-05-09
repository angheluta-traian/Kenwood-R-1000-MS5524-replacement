Kenwood R-1000 Frequency and Clock Display Replacement
This project replaces the original VFD display system of the Kenwood R-1000 shortwave receiver with a modern 6-digit 7-segment LED display, using an ATmega328P (DIP-28) microcontroller and an RTC DS3231 module. It preserves the original hardware functionality, including the front panel buttons.
Background
The original Kenwood R-1000 was equipped with a VFD display driven by an OKI MSM5525 chip, which is now obsolete and difficult to replace. This project provides a non-invasive solution that restores full frequency and clock display functionality.
Features
* Real-time clock display (hours and minutes) using DS3231
* Frequency counter with IF offset correction (455 kHz)
* Original buttons preserved:
o FREQ/CLOCK: Switch between clock and frequency display
o SET HOUR / SET MIN: Adjust the clock
o Brightness Control: Holding both SET buttons lowers brightness and shows brt
* All code written in Arduino C++ using the SevSeg library
* Easy to reproduce with minimal components
Hardware Used
* ATmega328P DIP-28 (standalone or on breadboard)
* 6-digit common-anode 7-segment display
* DS3231 RTC module
* Frequency counter input on digital pin 5
* Original panel switches wired to A1, D2, and D8
* Pull-up resistors and basic transistor interface for logic levels
Files Included
* Kenwood_R1000_Display.ino – main Arduino sketch
* Kenwood_R1000_Schematic.pdf – wiring and schematic diagram
* Kenwood_R1000_Documentation.pdf – printable user manual


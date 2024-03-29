/* 
	Editor: http://www.visualmicro.com
	        visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
	        the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
	        all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
	        note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: Bare Conductive Touch Board, Platform=avr, Package=Bare_Conductive_Arduino
*/

#define __AVR_ATmega32u4__
#define __AVR_ATmega32U4__
#define ARDUINO 101
#define ARDUINO_MAIN
#define F_CPU 16000000L
#define __AVR__
#define __cplusplus
extern "C" void __cxa_pure_virtual() {;}

//
void updateFlet();
uint8_t getMaximumFlet();
//
void noteOn(uint8_t channel, uint8_t note, uint8_t attack_velocity);
void noteOff(uint8_t channel, uint8_t note, uint8_t release_velocity);
void midiWrite(uint8_t cmd, uint8_t data1, uint8_t data2);
void midiSetup(uint8_t inst);

#include "C:\Program Files (x86)\Arduino\hardware\arduino\avr\variants\leonardo\pins_arduino.h" 
#include "C:\Program Files (x86)\Arduino\hardware\arduino\avr\cores\arduino\arduino.h"
#include "C:\Users\kamiya\repos\broom_guitar\broom_guitar\broom_guitar.ino"
#include "C:\Users\kamiya\repos\broom_guitar\broom_guitar\MPR121.cpp"
#include "C:\Users\kamiya\repos\broom_guitar\broom_guitar\MPR121.h"
#include "C:\Users\kamiya\repos\broom_guitar\broom_guitar\MPR121_defs.h"

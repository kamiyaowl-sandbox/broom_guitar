/*
* broom_guitar.ino
*
* Created: 10/26/2014 11:49:27 AM
* Author: kamiya
*/

#include "MPR121.h"
#include "Wire.h"
#include "SoftwareSerial.h"

#define PC_SERIAL_BAUD 57600
#define TRUE 1
#define FALSE 0

SoftwareSerial midi(12, 10); //Soft TX on 10, we don't use RX in this code

//Touch Board Setup variables
#define FIRST_PIN 0
#define LAST_PIN 11

const uint8_t stroke_size = 4;
const uint8_t stroke_pins[] = {3,2,1,0};
const uint8_t flet_size = 8;
const uint8_t flet_pins[] = {11,10,9,8,7,6,5,4};
const uint8_t guitarNotes[4][9] = {//4��8�t���b�g+�J��
	{77, 78,79,80,81,82, 83,84,85},
	{72, 73,74,75,76,77, 78,79,80},
	{67, 68,69,70,71,72, 73,74,75},
	{62, 63,64,65,66,67, 68,69,70},
};

uint8_t default_inst[] = {30, 27,28,29,31, 35,37,39,69};
//���b�`�����^�b�`�X�e�[�^�X
uint8_t flet_status[8] = {};
//�Đ����̌���index�A�l�͖炵�Ă�m�[�g�i���o�[
uint8_t stroke_enable[4] = {};

const uint8_t resetMIDI = 8;
const uint8_t ledPin = 13;

void setup(){
	Serial.begin(PC_SERIAL_BAUD);
	pinMode(ledPin,OUTPUT);
	//Setup soft serial for MIDI control
	midi.begin(31250);

	// 0x5C is the MPR121 I2C address on the Bare Touch Board
	Wire.begin();
	if(!MPR121.begin(0x5C)){
		Serial.println("error setting up MPR121");
		while(1){
			digitalWrite(ledPin,HIGH);
			delay(200);
			digitalWrite(ledPin,LOW);
			delay(200);
		}
	}

	
	// pin 4 is the MPR121 interrupt on the Bare Touch Board
	MPR121.setInterruptPin(4);
	MPR121.updateTouchData();

	//Reset the VS1053
	pinMode(resetMIDI, OUTPUT);
	digitalWrite(resetMIDI, LOW);
	delay(100);
	digitalWrite(resetMIDI, HIGH);
	delay(100);
	
	//�N�����I�v�V�����ŉ����ύX
	for(uint8_t i = 0 ; i < 10 ; ++i){
		digitalWrite(ledPin,HIGH);
		delay(50);
		digitalWrite(ledPin,LOW);
		delay(50);
	}
	MPR121.updateAll();
	updateFlet();
	//initialise MIDI
	uint8_t index = getMaximumFlet();
	midiSetup(default_inst[index]);

	for (uint8_t i = 0 ; i < stroke_size ; ++i){
		noteOn(0,guitarNotes[i][0],60);
		delay(300);
	}
	delay(500);
	for (uint8_t i = 0 ; i < stroke_size ; ++i){
		noteOff(0,guitarNotes[i][0],60);
	}
}

void updateFlet()
{
	for(uint8_t i = 0 ; i < sizeof(flet_pins) ; ++i){
		if(MPR121.isNewTouch(flet_pins[i])){
			flet_status[i] = TRUE;
			} else if(MPR121.isNewRelease(flet_pins[i])){
			flet_status[i] = FALSE;
		}
	}
}
//������Ă����ԍ������̂Ȃ�t���b�g��Ԃ�
//������Ă��Ȃ��ꍇ��0��1�t���b�g����ł��邱�Ƃɒ��� 0 ~ 8
uint8_t getMaximumFlet(){
	for(int8_t i = flet_size - 1; i >= 0 ; --i){
		if(flet_status[i]) return i + 1;
	}
	return 0;
}
void loop(){
	if(MPR121.touchStatusChanged()){
		MPR121.updateAll();
		updateFlet();
		for(uint8_t i = 0 ; i < stroke_size ; ++i){
			if(MPR121.isNewTouch(stroke_pins[i])){
				//�t���b�g����ǂݎ���Ė炷
				uint8_t flet = getMaximumFlet();
				stroke_enable[i] = guitarNotes[i][flet];
				noteOn(0, stroke_enable[i],60);
			} else if(MPR121.isNewRelease(stroke_pins[i])){
				//�Đ����̉����擾���ă����[�X
				noteOff(0,stroke_enable[i],60);
				stroke_enable[i] = 0x0;
			}

		}
	}
}


void noteOn(uint8_t channel, uint8_t note, uint8_t attack_velocity) {
	midiWrite( (0x90 | channel), note, attack_velocity);
}

void noteOff(uint8_t channel, uint8_t note, uint8_t release_velocity) {
	midiWrite( (0x80 | channel), note, release_velocity);
}

void midiWrite(uint8_t cmd, uint8_t data1, uint8_t data2) {
	digitalWrite(ledPin, HIGH);
	midi.write(cmd);
	midi.write(data1);

	if( (cmd & 0xF0) <= 0xB0)
	midi.write(data2);

	digitalWrite(ledPin, LOW);
}




/*SETTING UP THE INSTRUMENT:
The below function "setupMidi()" is where the instrument bank is defined. Use the VS1053 instrument library
below to aid you in selecting your desire instrument from within the respective instrument bank
*/

void midiSetup(uint8_t inst){
	
	//Volume
	midiWrite(0xB0, 0x07, 127); //0xB0 is channel message, set channel volume to near max (127)
	
	//Melodic Instruments GM1
	//To Play "Electric Piano" (5):
	midiWrite(0xB0, 0, 0x00); //Default bank GM1
	//We change the instrument by changin the middle number in  the brackets
	//talkMIDI(0xC0, number, 0); "number" can be any number from the melodic table below
	midiWrite(0xC0, inst, 0); //Set instrument number. 0xC0 is a 1 data uint8_t command(55,0)
	
	//Percussion Instruments (GM1 + GM2) uncomment the code below to use
	// To play "Sticks" (31):
	//talkMIDI(0xB0, 0, 0x78); //Bank select: drums
	//talkMIDI(0xC0, 5, 0); //Set instrument number
	//Play note on channel 1 (0x90), some note value (note), middle velocity (60):
	//noteOn(0, 31, 60);
	//NOTE: need to figure out how to map this... or is it the same as white keys?
	
}
/*MIDI INSTRUMENT LIBRARY:

MELODIC INSTRUMENTS (GM1)
When using the Melodic bank (0x79 - same as default), open chooses an instrument and the octave to map
To use these instruments below change "number" in talkMIDI(0xC0, number, 0) in setupMidi()


1   Acoustic Grand Piano       33  Acoustic Bass             65  Soprano Sax           97   Rain (FX 1)
2   Bright Acoustic Piano      34  Electric Bass (finger)    66  Alto Sax              98   Sound Track (FX 2)
3   Electric Grand Piano       35  Electric Bass (pick)      67  Tenor Sax             99   Crystal (FX 3)
4   Honky-tonk Piano           36  Fretless Bass             68  Baritone Sax          100  Atmosphere (FX 4)
5   Electric Piano 1           37  Slap Bass 1               69  Oboe                  101  Brigthness (FX 5)
6   Electric Piano 2           38  Slap Bass 2               70  English Horn          102  Goblins (FX 6)
7   Harpsichord                39  Synth Bass 1              71  Bassoon               103  Echoes (FX 7)
8   Clavi                      40  Synth Bass 2              72  Clarinet              104  Sci-fi (FX 8)
9   Celesta                    41  Violin                    73  Piccolo               105  Sitar
10  Glockenspiel               42  Viola                     74  Flute                 106  Banjo
11  Music Box                  43  Cello                     75  Recorder              107  Shamisen
12  Vibraphone                 44  Contrabass                76  Pan Flute             108  Koto
13  Marimba                    45  Tremolo Strings           77  Blown Bottle          109  Kalimba
14  Xylophone                  46  Pizzicato Strings         78  Shakuhachi            110  Bag Pipe
15  Tubular Bells              47  Orchestral Harp           79  Whistle               111  Fiddle
16  Dulcimer                   48  Trimpani                  80  Ocarina               112  Shanai
17  Drawbar Organ              49  String Ensembles 1        81  Square Lead (Lead 1)  113  Tinkle Bell
18  Percussive Organ           50  String Ensembles 2        82  Saw Lead (Lead)       114  Agogo
19  Rock Organ                 51  Synth Strings 1           83  Calliope (Lead 3)     115  Pitched Percussion
20  Church Organ               52  Synth Strings 2           84  Chiff Lead (Lead 4)   116  Woodblock
21  Reed Organ                 53  Choir Aahs                85  Charang Lead (Lead 5) 117  Taiko
22  Accordion                  54  Voice oohs                86  Voice Lead (Lead)     118  Melodic Tom
23  Harmonica                  55  Synth Voice               87  Fifths Lead (Lead 7)  119  Synth Drum
24  Tango Accordion            56  Orchestra Hit             88  Bass + Lead (Lead 8)  120  Reverse Cymbal
25  Acoustic Guitar (nylon)    57  Trumpet                   89  New Age (Pad 1)       121  Guitar Fret Noise
26  Acoutstic Guitar (steel)   58  Trombone                  90  Warm Pad (Pad 2)      122  Breath Noise
27  Electric Guitar (jazz)     59  Tuba                      91  Polysynth (Pad 3)     123  Seashore
28  Electric Guitar (clean)    60  Muted Trumpet             92  Choir (Pad 4)         124  Bird Tweet
29  Electric Guitar (muted)    61  French Horn               93  Bowed (Pad 5)         125  Telephone Ring
30  Overdriven Guitar          62  Brass Section             94  Metallic (Pad 6)      126  Helicopter
31  Distortion Guitar          63  Synth Brass 1             95  Halo (Pad 7)          127  Applause
32  Guitar Harmonics           64  Synth Brass 2             96  Sweep (Pad 8)         128  Gunshot

PERCUSSION INSTRUMENTS (GM1 + GM2)

When in the drum bank (0x78), there are not different instruments, only different notes.
To play the different sounds, select an instrument # like 5, then play notes 27 to 87.

27  High Q                     43  High Floor Tom            59  Ride Cymbal 2         75  Claves
28  Slap                       44  Pedal Hi-hat [EXC 1]      60  High Bongo            76  Hi Wood Block
29  Scratch Push [EXC 7]       45  Low Tom                   61  Low Bongo             77  Low Wood Block
30  Srcatch Pull [EXC 7]       46  Open Hi-hat [EXC 1]       62  Mute Hi Conga         78  Mute Cuica [EXC 4]
31  Sticks                     47  Low-Mid Tom               63  Open Hi Conga         79  Open Cuica [EXC 4]
32  Square Click               48  High Mid Tom              64  Low Conga             80  Mute Triangle [EXC 5]
33  Metronome Click            49  Crash Cymbal 1            65  High Timbale          81  Open Triangle [EXC 5]
34  Metronome Bell             50  High Tom                  66  Low Timbale           82  Shaker
35  Acoustic Bass Drum         51  Ride Cymbal 1             67  High Agogo            83 Jingle bell
36  Bass Drum 1                52  Chinese Cymbal            68  Low Agogo             84  Bell tree
37  Side Stick                 53  Ride Bell                 69  Casbasa               85  Castanets
38  Acoustic Snare             54  Tambourine                70  Maracas               86  Mute Surdo [EXC 6]
39  Hand Clap                  55  Splash Cymbal             71  Short Whistle [EXC 2] 87  Open Surdo [EXC 6]
40  Electric Snare             56  Cow bell                  72  Long Whistle [EXC 2]
41  Low Floor Tom              57  Crash Cymbal 2            73  Short Guiro [EXC 3]
42  Closed Hi-hat [EXC 1]      58  Vibra-slap                74  Long Guiro [EXC 3]

*/



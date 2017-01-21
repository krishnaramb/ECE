/*It uses the capacitiveSensor.h library file.*/
#include <CapacitiveSensor.h>
#include "pitches.h"
/*
*CapitiveSense Library Demo Sketch
*One small sheet of aluminium foil is used as a one side of a capacitor plate
*Other one side would be our hand
*Uses a high value resistor e.g. 1 megohm between send pin and receive pin
*Pin4(PD4) is used as the Sendpin.1M ohm resistor is connected via this pin to one end of
*Pin3 is connected from other other end of the foil to ground
*Pin8 is used for the buzzer
*/
CapacitiveSensor
cs_4_2 = CapacitiveSensor(4,2);
// 1 megohm resistor between pins 4 &
2, pin 2 is sensor pin, add wire, foil
int threshold = 300;
void setup()
{
cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);
// turn off autocalibrate on channel 1
Serial.begin(9600);
}
void playtone()
{
// notes in the melody:
int melody[] = {
NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
4, 8, 8, 4, 4, 4, 4, 4
};
// iterate over the notes of the melody:
for (int thisNote = 0; thisNote < 8; thisNote++) {
// to calculate the note duration, take one second
// divided by the note type.
//e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
int noteDuration = 1000 / noteDurations[thisNote];
tone(8, melody[thisNote], noteDuration);
// to distinguish the notes, set a minimum time between them.
// the note's duration + 30% seems to work well:
int pauseBetweenNotes = noteDuration * 1.30;
delay(pauseBetweenNotes);
// stop the tone playing:
noTone(8);
}
}
void loop()
{
long start = millis();
long total1 = cs_4_2.capacitiveSensor(30);Serial.print(millis() - start);
Serial.print("\t");
Serial.println(total1);
if(total1 > 300)
{
playtone();
}
delay(10);
}
// check on performance in milliseconds
// tab character for debug window spacing
// print sensor output 1
// arbitrary delay to limit data to serial port

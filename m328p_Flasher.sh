#! /bin/bash
#script for flashing program to the m328P microcontroller
#give the name of the script followed by the c file


fileName=$1
echo "-----------------------------------"
echo "-----------------------------------"
echo "Script is going to Flash the file: $fileName"
fileName=${fileName%.*}
echo "-----------------------------------"

if [ -e $fileName.hex ]
then
     echo "Deleting: $fileName.hex"
     rm -rf $fileName.hex
fi

if [ -e $fileName.o ]
then
     echo "Deleting: $fileName.o"
     rm -rf $fileName.o
fi



echo "compiling the program"
echo "-----------------------------------"
avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o $fileName.o $fileName.c
if [ ! -e $fileName.o ]
then
     echo "Error is generating obj file, exiting...."
     exit
fi


echo "Linking the program"
avr-gcc -mmcu=atmega328p $fileName.o USART.o 25LC256.o -o $fileName
echo "-----------------------------------"


echo "Converting the file into Hex format"
avr-objcopy -O ihex -R .eeprom $fileName $fileName.hex
echo "-----------------------------------"

if [ ! -e $fileName.hex ]
then
     echo "Error is generating hex file, exiting...."
     exit
fi


echo "Checking if the port is detected or not"
COMPORT="$(ls /dev|grep ttyACM)"
echo "-----------------------------------"

if [[ -z "$COMPORT" ]];then
    echo "*************No Device is Connected !!! Please check you connection......"
    exit 
fi
echo "COMPORT detected: $COMPORT"
echo "-----------------------------------"
echo "Flashing the hex file: $fileName.hex"
echo "############################################"
echo "############################################"
avrdude -F -V -c arduino -p ATMEGA328P -P /dev/$COMPORT -b 115200 -U flash:w:$fileName.hex

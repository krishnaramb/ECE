#include<avr/io.h>
#include <Arduino.h>
#include <SPI.h>
#include "CYSPIFRAM.h"


/* FRAM related--------*/
uint16_t START_ADDR;
int addr_increment;

const int TOTAL_4s_COUNT = 15;// 15 *4 = 60 sec
const float REF_VOLT = 1.1;// measured in the AREF pin
//const byte adcPin = 0;  // A0


const int MAX_RESULTS = 600;

//volatile int results [MAX_RESULTS];
volatile int resultNumber = 0;
//volatile int foo=0;

volatile int counter_4S = 0;
//float sensorValue;

// High when a value is ready to be read
volatile bool working;

// Value to store analog result
volatile float TempC;

/*--------------structure---------------*/

typedef struct Node{
	byte data;
	struct Node *next;
}Node;

volatile Node Head;
volatile Node *begin;
volatile Node *end;


/*-------------structure---------------*/





static inline void initADC(void){
	/* for saving the power */
	bitSet (DIDR0, ADC0D);  // disable digital buffer on A0
	bitSet (DIDR0, ADC1D);  // disable digital buffer on A1
	bitSet (DIDR0, ADC2D);  // disable digital buffer on A2
	bitSet (DIDR0, ADC3D);  // disable digital buffer on A3
	bitSet (DIDR0, ADC4D);  // disable digital buffer on A4
	bitSet (DIDR0, ADC5D);  // disable digital buffer on A5
	
	ADMUX  |= (1 << REFS1)|(1 << REFS0);					/*reference voltage set as internal 1.1V */
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  /*ADC prescalar which is 128, so ADC works on (16000KHz/128 = 125KHz) */
	ADCSRA |= (1 << ADEN);									/*enable ADC*/
	ADCSRA |= (1 << ADIE);									/*Activate ADC conversion complete interrupt */
	//ADMUX  |= (1 << ADLAR);									/*right adjusted while reading the ADC value from ADCH and ADCL */
	

	
}

static inline void initTimerCounter(void){
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;  //Timer/Counter1

	TCCR1B = bit (CS12)|bit (CS10)  | bit (WGM12);  // CTC, prescaler of 1024
	TIMSK1 = bit (OCIE1B);  // Timer/Counter1, Output Compare B Match Interrupt Enable is inside of Timer/Counter1 Interrupt Mask Register

	OCR1A = 62500;		//Output Compare Register 1 A
	OCR1B = 62500;   // Output Compare Register 1 B
}




// Interrupt service routine for the ADC completion
ISR(ADC_vect){
	int value = ADC;  // read result
	working = false;   //reset the flag
	float readvolt_val = value * (REF_VOLT / 1024.0);
	//results [resultNumber] = readvolt_val;
	TempC = (readvolt_val - 0.5)*100;
	byte TempByte  = (byte)(TempC*4);
	//foo++;
	//Serial.println (foo);
	Serial.print ("Four fold of Senor value: ");
	Serial.println (TempByte);
	resultNumber++;
	
	/*pushing data into the queue*/
	Node * ptr = (Node*) malloc(sizeof(Node));
	end->next = ptr; //linking the last and second last node
	ptr-> data =TempByte;
	//ptr-> data =foo;
	ptr->next = NULL;
	end = ptr; //Now end is also pointing last node added
	
	if (resultNumber >= MAX_RESULTS){
		ADCSRA = 0;		// turn off ADC
		TCCR1B = 0;		//Timer/Counter Stopped
	}
}


ISR(TIMER1_COMPB_vect){
	
	counter_4S++;
	if (counter_4S == TOTAL_4s_COUNT)
	{
		Serial.println("one minute time passed");
		counter_4S = 0;
		//Enable and Start the conversion
		//ADCSRA |= (1 << ADEN);
		
		bitSet(ADCSRA,ADSC); //start ADC conversion
		
	}
	
}





void setup(){
	Serial.begin(115200);
	START_ADDR =0x0000;  //initialized to the starting of the address
	addr_increment = 0;
	
	Head.next = NULL;
	Head.data = 0;
	end = &Head;  //end is pointing to head
	begin = &Head; //begin is also pointing to the head
	initADC();
	initTimerCounter();
	FRAM_SPI_Init();// initialize F-RAM:
	delay(100); //startup delay
	sei();
}


void loop(){
	//_delay_ms(8000);
	Node *tmp;
	if (begin->next !=NULL){
		byte bytevalue = begin->next->data;
		tmp = begin->next; // save of old next value of head
		begin->next= begin->next->next;//update to the next
		if (begin->next == NULL){ //if second node is the last node, update the end pointer to the head of the node
			end = begin;
			
		}
		FRAM_SPI_Write(START_ADDR + addr_increment, bytevalue);
		Serial.print("Value written to FRAM: ");
		Serial.println(bytevalue);
		free(tmp);
		addr_increment++;
	}
}


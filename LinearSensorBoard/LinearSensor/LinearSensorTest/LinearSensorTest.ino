// Parallel read of the linear sensor array TSL1402R (= the sensor with 256 photodiodes)
//-------------------------------------------------------------------------------------

// Define various ADC prescaler:
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
int CLKpin = 12;    // <-- Arduino pin delivering the clock pulses to pin 3 (CLK) of the TSL1402R 
int SIpin = 11;     // <-- Arduino pin delivering the SI (serial-input) pulse to pin 2 of the TSL1402R 
int AOpin1 = A1;    // <-- Arduino pin connected to pin 4 (analog output 1)of the TSL1402R
int AOpin2 = A2;    // <-- Arduino pin connected to pin 8 (analog output 2)of the TSL1402R
int IntArray[256]; // <-- the array where the readout of the photodiodes is stored, as integers
int counter=0;

void setup() 
{
  // Initialize two Arduino pins as digital output:
  pinMode(CLKpin, OUTPUT); 
  pinMode(SIpin, OUTPUT);  

  // To set up the ADC, first remove bits set by Arduino library, then choose 
  // a prescaler: PS_16, PS_32, PS_64 or PS_128:
  ADCSRA &= ~PS_128;  
  ADCSRA |= PS_32; // <-- Using PS_32 makes a single ADC conversion take ~30 us

  // Next, assert default setting:
  analogReference(DEFAULT);

  // Set all IO pins low:
  for( int i=0; i< 14; i++ )
  {
    digitalWrite(i, LOW);  
  }

  Serial.begin(115200);

}

void loop() 
{  
  counter++;
  resetClockPulse();
  for(int i=0; i < 128; i++)
  {
    IntArray[i] = analogRead(AOpin1);
    IntArray[i+128] = analogRead(AOpin2);
    clockPulse(); 
  }

  if (counter==20) {
    printResult(false);
    counter=0; 
  }


resetClockPulse();
  for(int i = 0; i < 260; i++)
  {
    clockPulse(); 
  }    

/*
  for(int i=0; i < 4192; i++) {
    clockPulse(); 
  }
*/
}

// This function generates an outgoing clock pulse from the Arduino digital pin 'CLKpin'. This clock
// pulse is fed into pin 3 of the linear sensor:
void clockPulse()
{
  delayMicroseconds(5);
  digitalWrite(CLKpin, HIGH);
  delayMicroseconds(5);
  digitalWrite(CLKpin, LOW);
}

void resetClockPulse() {
  delayMicroseconds(5);
  digitalWrite(SIpin, HIGH);
  delayMicroseconds(5);
  digitalWrite(CLKpin, HIGH);
  delayMicroseconds(5);
  digitalWrite(SIpin, LOW);
  delayMicroseconds(5);
  digitalWrite(CLKpin, LOW);
}




void printResult(boolean printId) {
  Serial.println("V1");
  // Next, send the measurement stored in the array to host computer using serial (rs-232).
  // communication. This takes ~80 ms during whick time no clock pulses reaches the sensor. 
  // No integration is taking place during this time from the photodiodes as the integration 
  // begins first after the 18th clock pulse after a SI pulse is inserted:
  for(int i = 0; i < 256; i++)
  {
    if (printId) {
      if (i<10) Serial.print(" ");
      if (i<100) Serial.print(" ");
      Serial.print(i);
      Serial.print(" : ");
    }
    if (IntArray[i]<10) Serial.print(" ");
    if (IntArray[i]<100) Serial.print(" ");
    if (IntArray[i]<1000) Serial.print(" ");
    Serial.print(IntArray[i]);

    if (i%16 == 15) {
      Serial.println("");
    }
    else {
      Serial.print(" - ");
    }
  }
  Serial.println(""); // <-- Send a linebreak to indicate the measurement is transmitted.
}






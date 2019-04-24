/*
What: LEDLightBoxAlnitak - PC controlled lightbox implmented using the 
	Alnitak (Flip-Flat/Flat-Man) command set found here:
	http://www.optecinc.com/astronomy/pdf/Alnitak%20Astrosystems%20GenericCommandsR3.pdf

Who: 
	Created By: Jared Wellman - jared@mainsequencesoftware.com

When: 
	Last modified:  2013/May/05


Typical usage on the command prompt:
Send     : >S000\n      //request state
Recieve  : *S19000\n    //returned state

Send     : >B128\n      //set brightness 128
Recieve  : *B19128\n    //confirming brightness set to 128

Send     : >J000\n      //get brightness
Recieve  : *B19128\n    //brightness value of 128 (assuming as set from above)

Send     : >L000\n      //turn light on (uses set brightness value)
Recieve  : *L19000\n    //confirms light turned on

Send     : >D000\n      //turn light off (brightness value should not be changed)
Recieve  : *D19000\n    //confirms light turned off.
*/

#include <Servo.h>
Servo myservo;

volatile int ledPin   = 6;      // the pin that the LED is attached to, needs to be a PWM pin.
volatile int servoPin = 9;      // the pin that the servo signal is attached to, needs to be a PWM pin.
int brightness = 0;

enum devices
{
	FLAT_MAN_L = 10,
	FLAT_MAN_XL = 15,
	FLAT_MAN = 19,
	FLIP_FLAT = 99
};

enum motorStatuses
{
	STOPPED = 0,
	RUNNING
};

enum lightStatuses
{
	OFF = 0,
	ON
};

enum shutterStatuses
{
	UNKNOWN = 0, // ie not open or closed...could be moving
	CLOSED,
	OPEN
};


int deviceId = FLIP_FLAT;
int motorStatus = STOPPED;
int lightStatus = OFF;
int coverStatus = UNKNOWN;

void setup()
{
  // initialize the serial communication:
  Serial.begin(9600);
  // initialize the ledPin as an output:
  pinMode(ledPin, OUTPUT);
  analogWrite(ledPin, 0);
  myservo.attach(servoPin);
}

void loop() 
{
  handleSerial();
}


void handleSerial()
{
  if( Serial.available() >= 5 )  // all incoming communications are fixed length at 6 bytes including the \n
  {
    char* cmd;
	char* data;
    char temp[10];
    
    int len = 0;

    char str[20];
    memset(str, 0, 20);
    
	// I don't personally like using the \n as a command character for reading.  
	// but that's how the command set is.
    Serial.readBytesUntil('\n', str, 20);

	cmd = str + 1;
	data = str + 2;
	
	// useful for debugging to make sure your commands came through and are parsed correctly.
    if( false )
    {
      sprintf( temp, "cmd = >%s%s;", cmd, data);
      Serial.println(temp);
    } 
    


    switch( *cmd )
    {
	  /*
		Ping device
			Request: >P000\n
			Return : *Pii000\n
				id = deviceId
	  */
      case 'P':
		  sprintf(temp, "*P%d000\n", deviceId);
		  Serial.print(temp);
		  break;

      /*
		Open shutter
			Request: >O000\n
			Return : *Oii000\n
				id = deviceId

			This command is only supported on the Flip-Flat!
	  */
      case 'O':
		  sprintf(temp, "*O%d000\n", deviceId);
		  SetShutter(OPEN);
		  Serial.print(temp);
		  break;


      /*
		Close shutter
			Request: >C000\n
			Return : *Cii000\n
				id = deviceId

			This command is only supported on the Flip-Flat!
	  */
      case 'C':
		  sprintf(temp, "*C%d000\n", deviceId);
		  SetShutter(CLOSED);
		  Serial.print(temp);
		  break;

	  /*
		Turn light on
			Request: >L000\n
			Return : *Lii000\n
				id = deviceId
	  */
      case 'L':
		  sprintf(temp, "*L%d000\n", deviceId);
		  Serial.print(temp);
		  lightStatus = ON;
		  analogWrite(ledPin, brightness);
		  break;

	  /*
		Turn light off
			Request: >D000\n
			Return : *Dii000\n
				id = deviceId
	  */
      case 'D':
		  sprintf(temp, "*D%d000\n", deviceId);
		  Serial.print(temp);
		  lightStatus = OFF;
		  analogWrite(ledPin, 0);
		  break;

	  /*
		Set brightness
			Request: >Bxxx\n
				xxx = brightness value from 000-255
			Return : *Biiyyy\n
				id = deviceId
				yyy = value that brightness was set from 000-255
	  */
      case 'B':
		  brightness = atoi(data);    
		  if( lightStatus == ON ) 
			  analogWrite(ledPin, brightness);   
		  sprintf( temp, "*B%d%03d\n", deviceId, brightness );
		  Serial.print(temp);
        break;

	  /*
		Get brightness
			Request: >J000\n
			Return : *Jiiyyy\n
				id = deviceId
				yyy = current brightness value from 000-255
	  */
      case 'J':
        sprintf( temp, "*J%d%03d\n", deviceId, brightness);
        Serial.print(temp);
        break;
      
	  /*
		Get device status:
			Request: >S000\n
			Return : *SidMLC\n
				id = deviceId
				M  = motor status( 0 stopped, 1 running)
				L  = light status( 0 off, 1 on)
				C  = Cover Status( 0 moving, 1 closed, 2 open)
	  */
      case 'S': 
        sprintf( temp, "*S%d%d%d%d\n",deviceId, motorStatus, lightStatus, coverStatus);
        Serial.print(temp);
        break;

	  /*
		Get firmware version
			Request: >V000\n
			Return : *Vii001\n
				id = deviceId
	  */
      case 'V': // get firmware version
		  sprintf(temp, "*V%d001\n", deviceId);
		  Serial.print(temp);
		  break;
    }    

	while( Serial.available() > 0 )
		Serial.read();

  }
}

void SetShutter(int val)
{
	if( val == OPEN && coverStatus != OPEN )
	{
    for (int angle = 0; angle <= 30; angle+=1) 
    {
      myservo.write (angle);
      delay (70);
    } 
    myservo.write (150);
    for (int angle = 150; angle <= 180; angle+=1) 
    {
      myservo.write (angle);
      delay (70);
    } 
		coverStatus = OPEN;
		// TODO: Implement code to OPEN the shutter.
	}
	else if( val == CLOSED && coverStatus != CLOSED )
	{
    for (int angle = 180; angle > 150; angle-=1) 
    {
      myservo.write (angle);
      delay (70);
    }   
    myservo.write (30);  
    for (int angle = 30; angle > 0; angle-=1) 
    {
      myservo.write (angle);
      delay (70);
    } 
		coverStatus = CLOSED;
		// TODO: Implement code to CLOSE the shutter
	}
	else
	{
		// TODO: Actually handle this case
		coverStatus = val;
	}
	
}

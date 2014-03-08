ArduinoLightbox
===============


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

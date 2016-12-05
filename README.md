# HavenLab
## the LoRa protocol code for the RN2483 on a Marvin board with Sensors connected 

###Summary: This file contains the the code fro a LoRa send protocol for the RN2483 chip on a Marvin.  The code has been commented for allmost every line of code for clarification. 

##How to use?
At first, we include every library which is needed for the Marvin to understand which sensor has been  connected. The use and implementation of the libraries may vary. This is up to you as a programmer to implement and get the data; this is just an example.

At first we start declaring the *sensorData fields* as shown within the following block:
```arduino

//*** SensorData fields
 float compassSensorData;
 float turbulanceSensorData;

// *** end SensorData fields


// *** Sensor data to HEX in string values
// This is needed for the Marvin (the RX RN2483 CHIP) needs this in a HEX value to send it over LoRa
// This is stated in the RN2483 manifest. It's quite a hassle but we'll get to that later on!
String compassSensorDataRoundedToHEX;
String turbulanceSensorDataRoundedToHEX;
//*** end of the HEX string values declarations 

```

Make sure you are declaring the fields, which is required for the stringToHEX conversion later on!

After you have set up all your sensorData fields, it's time for you to get your appskey, nwkskey and devaddr from your endpoint manager. For this project we are using a Kerlink with iTalks.com as a wrapper. iTalks provides us the needed keys so we can attune the Marvin with the Kerlink and set up a communication path. This will be done by *InitializeSerials()* and *initializeRN2483()*. These will initialize the given keys and make the connection. 

In *send_LoRa_data()* the send command will be prompted with the data which will be sent to the Kerlink. Keep in mind that the sent protocol only accepts HEXADECIMAL string values. To tackle this problem whe have thought about a smart and simple solution. Every value from a sensor is by default a float, so a 32,34 value for example. Hexadecimals only have natural numbers. But we WANT to have the two numbers after the comma! **_'But how do we fix this you ask?'_** *easy!* multiply the sensordata by 100, *round()* the outcome and use the *String(int, HEX)* function!

The latter will be done by using the floatToHEX() function with small adjustments for your own ease of use. 

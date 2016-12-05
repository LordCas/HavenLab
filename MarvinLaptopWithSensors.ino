


/*
 This file contains the code for the "Meetwaarden Boei te water" for project "De Groene Poort" 
*/

//includes

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>  
// end of the includes


// set and assign uniquge ID to the compass
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

// Port to assign the type of lora data (any port can be used between 1 and 223)
int     set_port  = 223;

// Some standard ports that depend on the layout of the Marvin
long    defaultBaudRate = 57600;
int     reset_port = 5;
int     RN2483_power_port = 6; //Note that an earlier version of the Marvin doesn't support seperate power to RN2483
int     led_port = 13;


//*** declaring the sensors and their respective pins ---> 
int turbulanceSensorValue = analogRead(A3);// read the input on analog pin 0:

 float compassSensorData;
 float turbulanceSensorData;
 String compassSensorDataRoundedToHEX;
 String turbulanceSensorDataRoundedToHEX;

  
 
//*** <---/declaring pins and sensor positions


//*** Set parameters here BEGIN ---->
String  set_nwkskey = "91FD4A1E6CB22444922C7EEDA7610926";
String  set_appskey = "66A81E21D62E6D9A890CC6DCDEF2C301";
String  set_devaddr = "00236953";
//*** <---- END Set parameters heres
/*
 * Setup() function is called when board is started. Marvin uses a serial connection to talk to your pc and a serial
 * connection to talk to the RN2483, these are both initialized in seperate functions. Also some Arduino port are 
 * initialized and a LED is called to blink when everything is done. 
 */
void setup() {
  Wire.begin();
  InitializeSerials(defaultBaudRate);
  initializeRN2483(RN2483_power_port, reset_port);
  pinMode(led_port, OUTPUT); // Initialize LED port
  blinky();
  readCompassSensor();
}

void loop() {

  send_LoRa_data(set_port, "abc123");// the data that has to be send to the server (iTalks in this case)
  delay(100);
  blinky();
  delay(100);
  read_data_from_LoRa_Mod();
  delay(600);
  readTurbulanceSensor(); //read out the Turbulance Sensor
  
  readCompassSensor();// read the CompasSensor
  
}

void InitializeSerials(int baudrate)
{
  Serial.begin(defaultBaudRate);
  Serial1.begin(defaultBaudRate);
  delay(100);
  print_to_console("Serial ports initialised");
}

void initializeRN2483(int pwr_port, int rst_port)
{
  //Enable power to the RN2483
  pinMode(pwr_port, OUTPUT);
  digitalWrite(pwr_port, HIGH);
  print_to_console("RN2483 Powered up");
  delay(1000);
  
  //Disable reset pin
  pinMode(rst_port, OUTPUT);
  digitalWrite(rst_port, HIGH);

  //Configure LoRa module
  send_LoRa_Command("sys reset");
  read_data_from_LoRa_Mod();

  send_LoRa_Command("radio set crc off");
  delay(1000);
  read_data_from_LoRa_Mod();

  send_LoRa_Command("mac set nwkskey " + set_nwkskey);
  read_data_from_LoRa_Mod();

  send_LoRa_Command("mac set appskey " + set_appskey);
  read_data_from_LoRa_Mod();

  send_LoRa_Command("mac set devaddr " + set_devaddr);
  read_data_from_LoRa_Mod();

  //For this commands some extra delay is needed.
  send_LoRa_Command("mac set adr on");
  delay(1000);
  read_data_from_LoRa_Mod();

  send_LoRa_Command("mac save");
  delay(1000);
  read_data_from_LoRa_Mod();

  send_LoRa_Command("mac join abp");
  delay(1000);
  read_data_from_LoRa_Mod();

}

void print_to_console(String message)
{
  Serial.println(message);
}

void read_data_from_LoRa_Mod()
{
  if (Serial1.available()) {
    String inByte = Serial1.readString();
    Serial.println(inByte);
  }

}

void send_LoRa_Command(String cmd)
{
  print_to_console("Now sending: " + cmd);
  Serial1.println(cmd);
  delay(500);
}

void send_LoRa_data(int tx_port, String rawdata)
{

    floatToHEX();
    print_to_console(compassSensorDataRoundedToHEX + String(" ") + turbulanceSensorDataRoundedToHEX);
    send_LoRa_Command("mac tx cnf " + String(tx_port) + String(" ") +  compassSensorDataRoundedToHEX + String("BAB") + turbulanceSensorDataRoundedToHEX);
  
}


void blinky()
{
  digitalWrite(led_port, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);                     // wait for a second
  //Serial.write("het werkt ");
  digitalWrite(led_port, LOW);    // turn the LED off by making the voltage LOW
  delay(500);                     // wait for a second
  digitalWrite(led_port, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);                     // wait for a second
  digitalWrite(led_port, LOW);    // turn the LED off by making the voltage LOW
  delay(500);                     // wait for a second
}


void readTurbulanceSensor(){
      
  float turbulance = turbulanceSensorValue * (5.0 / 1024.0); // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  turbulanceSensorData = turbulance;
  //Serial.println("Turbulance: \t");
  //Serial.print( turbulance ); // print out the value you read:
  //Serial.println("\t");
  delay(500);
}

void readCompassSensor(){
  if(!mag.begin())
  {
     Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
     while(1);
  }
  
  /* Get a new sensor event */ 
  sensors_event_t event; 
  mag.getEvent(&event);
 
  /* Display the results (magnetic vector values are in micro-Tesla (uT)) */
  //Serial.print("X: "); Serial.print(event.magnetic.x); Serial.print("  ");
  //Serial.print("Y: "); Serial.print(event.magnetic.y); Serial.print("  ");
  //Serial.print("Z: "); Serial.print(event.magnetic.z); Serial.print("  ");Serial.println("uT");

  float heading = atan2(event.magnetic.y, event.magnetic.x);
  
  // Find the 'Declination Angle' here: http://www.magnetic-declination.com/
  // ROTTERDAM RDM location
  // Latitude: 51° 55' 0" N
  // Longitude: 4° 30' 0" E
  // Magnetic declination: +0° 55' 
  // Mine is: +0° 55' Positve East, which is ~0 Degrees, or (which we need) 0.00 radians
  // If you cannot find your Declination, comment out the next two lines, the compass will be slightly off without the declination.
  float declinationAngle = 0.00;
  //the calculation below is only applicable when there is an declination
  heading += declinationAngle;
  
  // Correct for when signs are reversed.
  if(heading < 0)
    heading += 2*PI;
    
  // Check for wrap due to addition of declination.
  if(heading > 2*PI)
    heading -= 2*PI;
   
  // Convert radians to degrees 
  float headingDegrees = heading * 180/M_PI; 
  //set the field equal to the SensorData
  compassSensorData = headingDegrees;
  
  //Serial.print("Heading (degrees): "); Serial.println(headingDegrees);
  
  delay(500);
}

void floatToHEX(){
  float compassSensorDataMultiplied = compassSensorData * 100.00;
  int compassSensorDataRounded = round(compassSensorDataMultiplied);   
  Serial.println(compassSensorDataRounded);
  compassSensorDataRoundedToHEX = String(compassSensorDataRounded,HEX); 

   float turbulanceSensorDataMultiplied = turbulanceSensorData * 100.00;
  int turbulanceSensorDataRounded = round(turbulanceSensorDataMultiplied);   
  Serial.println(turbulanceSensorDataRounded);
  turbulanceSensorDataRoundedToHEX = String(turbulanceSensorDataRounded,HEX); 
}





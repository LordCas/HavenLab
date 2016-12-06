


/*
 This file contains the code for the "Meetwaarden Boei te water" for project "De Groene Poort" 
*/

//includes

#include <Wire.h>
//#include <Adafruit_Sensor.h>
//#include <Adafruit_HMC5883_U.h>
#include <OneWire.h>
#include <DallasTemperature.h>
// end of the includes

#define ONE_WIRE_BUS A5
OneWire oneWire(ONE_WIRE_BUS);



// Port to assign the type of lora data (any port can be used between 1 and 223)
int     set_port  = 223;

// Some standard ports that depend on the layout of the Marvin
long    defaultBaudRate = 57600;
int     reset_port = 5;
int     RN2483_power_port = 6; //Note that an earlier version of the Marvin doesn't support seperate power to RN2483
int     led_port = 13;


//*** declaring the sensors and their respective pins ---> 
int turbulanceSensorValue = analogRead(A3);// read the input on analog pin 0:

 DallasTemperature sensors(&oneWire);
 int selinitySensor;
 float turbulanceSensorData;
 
 
 String turbulanceSensorDataRoundedToHEX;
 String selinitySensorToHEX;
 String temparatuurSensorDataToHEX;
 
 
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

  sensors.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement
}

void loop() {
 
  
  send_LoRa_data(set_port);// the data that has to be send to the server (iTalks in this case)
  delay(100);
  blinky();
  delay(100);
  read_data_from_LoRa_Mod();
  delay(600);
  
   //salt
  selinitySensor = analogRead(A0);
  Serial.print("Weerstand ");
  readTurbulanceSensor(); //read out the Turbulance Sensor
  sensors.requestTemperatures(); // Send the command to get temperatures);
   delay(6000);  
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

void send_LoRa_data(int tx_port)
{

    floatToHEX();
 //   print_to_console(selinitySensorToHEX + String(" ") + turbulanceSensorDataRoundedToHEX); uncomment this line for an extra check before sending
    send_LoRa_Command("mac tx cnf " + String(tx_port) + String(" ") +  selinitySensorToHEX + String("BAB") + turbulanceSensorDataRoundedToHEX + String("BAB") + temparatuurSensorDataToHEX); //BAB is to indicate the sensorAttributes later on the end point

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
  int turbulanceSensorValue = analogRead(A3);    
  float turbulance = turbulanceSensorValue * (5.0 / 1024.0); // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  turbulanceSensorData = turbulance;
 // Serial.println("Turbulance: \t");
  //Serial.print( turbulance ); // print out the value you read:
  //Serial.println("\t");
  delay(500);
}



void floatToHEX(){
  //float compassSensorDataMultiplied = compassSensorData * 100.00;
 
  selinitySensorToHEX = String(selinitySensor,HEX); 

  float turbulanceSensorDataMultiplied = turbulanceSensorData * 100.00;
  int turbulanceSensorDataRounded = round(turbulanceSensorDataMultiplied);   
  Serial.println(turbulanceSensorDataRounded); //this is a check if the sensor is working correctly
  turbulanceSensorDataRoundedToHEX = String(turbulanceSensorDataRounded,HEX); 
  
  float temperatureSensorData = (sensors.getTempCByIndex(0)+127) * 100.00; //adds 127 to always make it a natural number. The lowest the temperatureSensor can get is -127
  int temperatureSensorDataRounded = round(temperatureSensorData);
  temparatureSensorDataToHEX = String(temperatureSensorDataRounded, HEX);
}


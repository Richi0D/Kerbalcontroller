#include <Button.h>
#define SERIAL_RX_BUFFER_SIZE 256

//analog pins
const byte pTHROTTLE = A0; //slide pot
const byte pTX = A2;       //translation x-axis
const byte pTY = A1;       //translation y-axis
const byte pTZ = A3;       //translation z-axis
const byte pRX = A5;       //rotation x-axis
const byte pRY = A4;       //rotation y-axis
const byte pRZ = A6;       //rotation z-axis

//digital pins
const byte dataPin = 2;      //DS - blue
const byte clockPin = 4;     //SH_CP - yellow
const byte latchPin = 3;     //ST_CP - pink
const byte maxthrottle = 5;  //Max Throttle button
const byte minthrottle = 6;  //Min Throttle button
const byte motoron = 8;      //Motor on pin (L293D pin 1)
const byte motorup = 9;      //Motor up pin
const byte motordown = 10;   //Motor down pin 
const byte touchReceive = 11;//Receive pin for Capacitance Sensing Circuit 
const byte touchSend = 12;   //Send pin for Capacitance Sensing Circuit
const byte pMODE = 66;       //mode switch (used for debug mode)
//Display 1 RX1  blue
//Display 1 TX1  yellow
//Display 2 RX2  blue
//Display 2 TX2  yellow
const byte pABORT = 22;      //Abort switch (safety switch, active high)
const byte pARM = 23;        //Arm switch (safety switch, active high)
const byte pSTAGE = 24;      //Stage button
const byte pSAS = 25;        //SAS switch
const byte pRCS = 26;        //RCS switch
const byte pprecision = 27;  //Precision Control Switch
const byte pLIGHTS = 28;     //Lights button
const byte pLIGHTSLED = 29;  //Lights button LED
const byte pBRAKES = 30;     //Brakes button
const byte pBRAKESLED = 31;  //Brakes button LED
const byte pGEARS = 32;      //Gears button
const byte pGEARSLED = 33;   //Gears button LED
const byte pSOLAR = 34;      //Solar button (action group 9)
const byte pSOLARLED = 35;   //Solar button LED
const byte pLADDER = 38;     //Ladder button (action group 7)
const byte pLADDERLED = 39;  //Ladder button LED
const byte pCHUTES = 36;     //Chutes button (action group 8)
const byte pCHUTESLED = 37;  //Chutes button LED
const byte pACTION6 = 40;    //Action Group 6 button
const byte pACTION6LED = 41; //Action Group 6 button LED
const byte pACTION5 = 42;    //Action Group 5 button
const byte pACTION5LED = 43; //Action Group 5 button LED
const byte pACTION4 = 44;    //Action Group 4 button
const byte pACTION4LED = 45; //Action Group 4 button LED
const byte pACTION3 = 46;    //Action Group 3 button
const byte pACTION3LED = 47; //Action Group 3 button LED
const byte pACTION2 = 48;    //Action Group 2 button
const byte pACTION2LED = 49; //Action Group 2 button LED
const byte pACTION1 = 50;    //Action Group 1 button
const byte pACTION1LED = 51; //Action Group 1 button LED
const byte pmodeswitch = 52; //rocket mode or plane mode switch
const byte pswitch1 = 62;    //A8 Switch position 1, display mode orbit
const byte pswitch2 = 63;    //A9 Switch position 2, display mode surface
const byte pswitch3 = 64;    //A10 Switch position 3, display mode target/node
const byte pswitch4 = 65;    //A11 Switch position 4, display mode Body Info

//Enumeration of MainControls
#define SAS       7
#define RCS       6
#define LIGHTS    5
#define GEARS     4
#define BRAKES    3
#define PRECISION 2
#define ABORT     1
#define STAGE     0

//Enumeration of ActionGroups (includes main controls and custom action groups)
#define AGSAS      0
#define AGRCS      1
#define AGLight    2
#define AGGears    3
#define AGBrakes   4
#define AGAbort    5
#define AGCustom01 6
#define AGCustom02 7
#define AGCustom03 8
#define AGCustom04 9
#define AGCustom05 10
#define AGCustom06 11
#define AGCustom07 12
#define AGCustom08 13
#define AGCustom09 14
#define AGCustom10 15

//Enumeration of SAS Modes
#define SMOFF           0
#define SMSAS           1
#define SMPrograde      2
#define SMRetroGrade    3
#define SMNormal        4
#define SMAntinormal    5
#define SMRadialIn      6
#define SMRadialOut     7
#define SMTarget        8
#define SMAntiTarget    9
#define SMManeuverNode  10

//Enumeration of Navball Target Modes
#define NAVBallIGNORE   0
#define NAVBallORBIT    1
#define NAVBallSURFACE  2
#define NAVBallTARGET   3

//macro used to generate the control packet (also used for the handshake packet)
#define details(name) (uint8_t*)&name,sizeof(name)

//variables used in timing
const int IDLETIMER = 10000;        //if no message received from KSP for more than 10s, go idle (default 2000)
const int CONTROLREFRESH = 20;      //send control packet every 10 ms (default 25)
const int DISPLAYREFRESH = 50;      //refresh Display every 40 ms = 25fps
unsigned long deadtime, deadtimeOld, controlTime, controlTimeOld, displayTime, displayTimeOld = 0;
unsigned long debugtime, debugtimeOld =0, atmotime, atmotimeOld =0;
unsigned long now = 0;

//variables used in serial communication
boolean Connected = false;
byte id =0;

//variables used for display status
byte PageDisplay1 = 0;              //Page display1
byte PageDisplay2 = 0;              //Page display2 

//debug variable
bool debug = false;
int chooseled = 0; 

//define the structure of a VesseData packet
struct VesselData
{
    //the following data is available in the packet received from the plugin (defined by the KSPSerialIO plugin)
    byte id;                //1   packet id
    float AP;               //2   apoapsis (m to sea level)
    float PE;               //3   periapsis (m to sea level)
    float SemiMajorAxis;    //4   orbit semi major axis (m)
    float SemiMinorAxis;    //5   orbit semi minor axis (m)
    float VVI;              //6   vertical velocity (m/s)
    float e;                //7   orbital eccentricity (unitless, 0 = circular, > 1 = escape)
    float inc;              //8   orbital inclination (degrees)
    float G;                //9   acceleration (Gees)
    long TAp;               //10  time to AP (seconds)
    long TPe;               //11  time to Pe (seconds)
    float TrueAnomaly;      //12  orbital true anomaly (degrees)
    float Density;          //13  air density (presumably kg/m^3, 1.225 at sea level)
    long period;            //14  orbital period (seconds)
    float RAlt;             //15  radar altitude (m)
    float Alt;              //16  altitude above sea level (m)
    float Vsurf;            //17  surface velocity (m/s)
    float Lat;              //18  Latitude (degree)
    float Lon;              //19  Longitude (degree)
    float LiquidFuelTot;    //20  Liquid Fuel Total
    float LiquidFuel;       //21  Liquid Fuel remaining
    float OxidizerTot;      //22  Oxidizer Total
    float Oxidizer;         //23  Oxidizer remaining
    float EChargeTot;       //24  Electric Charge Total
    float ECharge;          //25  Electric Charge remaining
    float MonoPropTot;      //26  Mono Propellant Total
    float MonoProp;         //27  Mono Propellant remaining
    float IntakeAirTot;     //28  Intake Air Total
    float IntakeAir;        //29  Intake Air remaining
    float SolidFuelTot;     //30  Solid Fuel Total
    float SolidFuel;        //31  Solid Fuel remaining
    float XenonGasTot;      //32  Xenon Gas Total
    float XenonGas;         //33  Xenon Gas remaining
    float LiquidFuelTotS;   //34  Liquid Fuel Total (stage)
    float LiquidFuelS;      //35  Liquid Fuel remaining (stage)
    float OxidizerTotS;     //36  Oxidizer Total (stage)
    float OxidizerS;        //37  Oxidizer remaining (stage)
    uint32_t MissionTime;   //38  Time since launch (s)
    float deltaTime;        //39  Time since last packet (s)
    float VOrbit;           //40  Orbital speed (m/s)
    uint32_t MNTime;        //41  Time to next node (s) [0 when no node]
    float MNDeltaV;         //42  Delta V for next node (m/s) [0 when no node]
    float Pitch;            //43  Vessel Pitch relative to surface (degrees)
    float Roll;             //44  Vessel Roll relative to surface (degrees)
    float Heading;          //45  Vessel Heading relative to surface (degrees)
    uint16_t ActionGroups;  //46  status bit order:SAS, RCS, Light, Gears, Brakes, Abort, Custom01 - 10
    byte SOINumber;         //47  SOI Number (decimal format: sun-planet-moon e.g. 130 = kerbin, 131 = mun)
    byte MaxOverHeat;       //48  Max part overheat (% percent)
    float MachNumber;       //49  Mach number
    float IAS;              //50  Indicated Air Speed
    byte CurrentStage;      //51  Current stage number
    byte TotalStage;        //52  TotalNumber of stages
    float TargetDist;       //53  Distance to targeted vessel (m)
    float TargetV;          //54  Target vessel relative velocity
    byte NavballSASMode;    //55  Combined byte for navball target mode and SAS mode
                                // First four bits indicate AutoPilot mode:
                                // 0 SAS is off  //1 = Regular Stability Assist //2 = Prograde
                                // 3 = RetroGrade //4 = Normal //5 = Antinormal //6 = Radial In
                                // 7 = Radial Out //8 = Target //9 = Anti-Target //10 = Maneuver node
                                // Last 4 bits set navball mode. (0=ignore,1=ORBIT,2=SURFACE,3=TARGET)
};

struct HandShakePacket
{
  byte id;
  byte M1;
  byte M2;
  byte M3;
};

//define the structure of a control packet
struct ControlPacket {
  //the following controls can be sent to the KSPSerialIO plugin (defined by the plugin)
  byte id;
  byte MainControls;                  //SAS RCS Lights Gears Brakes Precision Abort Stage (see enum)
  byte Mode;                          //0 = stage, 1 = docking, 2 = map
  unsigned int ControlGroup;          //action groups 1-10
  byte NavballSASMode;                //AutoPilot mode
  byte AdditionalControlByte1;
  int Pitch;                          //-1000 -> 1000
  int Roll;                           //-1000 -> 1000
  int Yaw;                            //-1000 -> 1000
  int TX;                             //-1000 -> 1000
  int TY;                             //-1000 -> 1000
  int TZ;                             //-1000 -> 1000
  int WheelSteer;                     //-1000 -> 1000
  int Throttle;                       //    0 -> 1000
  int WheelThrottle;                  //    0 -> 1000
};

//variables used for detect bounce buttons
Button debouncerStage(pSTAGE);
Button debouncerLights(pLIGHTS);
Button debouncerBrakes(pBRAKES);
Button debouncerGears(pGEARS);
Button debouncerSolar(pSOLAR);
Button debouncerChutes(pCHUTES);
Button debouncerLadder(pLADDER);
Button debouncerA6(pACTION6);
Button debouncerA5(pACTION5);
Button debouncerA4(pACTION4);
Button debouncerA3(pACTION3);
Button debouncerA2(pACTION2);
Button debouncerA1(pACTION1);

//in-game state used to update button LEDs
bool lights_on = false;
bool ladder_on = false;
bool solar_on = false;
bool gears_on = false;
bool brakes_on = false;
bool chutes_on = false;
bool stage_on = false;
bool action1_on = false;
bool action2_on = false;
bool action3_on = false;
bool action4_on = false;
bool action5_on = false;
bool action6_on = false;

//input value variables
int throttle_value;
int tx_value;
int ty_value;
int tz_value;
int rx_value;
int ry_value;
int rz_value;
int flymode = 0;

//variables used for motorfader
int throttletarget;
int theThreshold = 30;


//Create an instance of a handshake packet
HandShakePacket HPacket;
//create an instance of a VesselData object
VesselData VData;
//Create an instance of a control packet
ControlPacket CPacket;
 
void setup() {

  Serial.begin(115200);  //KSPSerialIO connection
  Serial1.begin(38400);  //Display1 connection
  Serial2.begin(38400);  //Display2 connection
      PageDisplay1 = 0;
      PageDisplay2 = 0; 
      sendToDisplay1(String("page ") + String(PageDisplay1));
      sendToDisplay1(String("boot.txt=\"") + String("booting")+String("\""));
      sendToDisplay2(String("page ") + String(PageDisplay2));
      sendToDisplay2(String("boot.txt=\"") + String("booting")+String("\""));

      

//Initialize
  controlsInit();   //set all pin modes
  testLEDS(50);     //blink every LED once to test (with a delay of xx ms)
  InitTxPackets();  //initialize the serial communication
      PageDisplay1 = 0;
      PageDisplay2 = 0; 
      sendToDisplay1(String("page ") + String(PageDisplay1));
      sendToDisplay1(String("boot.txt=\"") + String("booting complete")+String("\""));
      sendToDisplay2(String("page ") + String(PageDisplay2));
      sendToDisplay2(String("boot.txt=\"") + String("booting complete")+String("\""));

}

void loop() {
  
  //check mode 
  if(!digitalRead(pMODE)){debug = true;} else {debug = false;}

  if(debug){
    now = millis();
    const int debuginterval = 50;
    debugtime = now - debugtimeOld;       
    if (debugtime > debuginterval) {
    debugtimeOld = now; 
    
    //Debug mode does not communicate with KSPSerialIO, but simply displays the button states on the LCD.

    //switch lcd to debug
    if (PageDisplay1 !=6){
            PageDisplay1 = 6;
            sendToDisplay1(String("page ") + String(PageDisplay1));
    }
    if (PageDisplay2 !=3){
      PageDisplay2 = 3; 
      sendToDisplay2(String("page ") + String(PageDisplay2));
    }
    
    //read analog input
    tx_value = analogRead(pTX);
          sendToDisplay2(String("transxV.txt=\"") + String(tx_value)+String("\""));
    ty_value = analogRead(pTY);
          sendToDisplay2(String("transyV.txt=\"") + String(ty_value)+String("\""));
    tz_value = analogRead(pTZ);
          sendToDisplay2(String("trasnzV.txt=\"") + String(tz_value)+String("\""));    
    rx_value = analogRead(pRX);
          sendToDisplay2(String("rotxV.txt=\"") + String(rx_value)+String("\""));      
    ry_value = analogRead(pRY);
          sendToDisplay2(String("rotyV.txt=\"") + String(ry_value)+String("\""));     
    rz_value = analogRead(pRZ);
          sendToDisplay2(String("rotzV.txt=\"") + String(rz_value)+String("\""));
    throttle_value = analogRead(pTHROTTLE);
          sendToDisplay2(String("throttleV.txt=\"") + String(throttle_value)+String("\""));                
                    
    //check rotation switch status
    if (!digitalRead(pswitch1)) {
        sendToDisplay2(String("dispswV.txt=\"") + String("1")+String("\""));
      }
      else if (!digitalRead(pswitch2)) {
        sendToDisplay2(String("dispswV.txt=\"") + String("2")+String("\""));
      }
      else if (!digitalRead(pswitch3)) {
        sendToDisplay2(String("dispswV.txt=\"") + String("3")+String("\""));
      }
      else if (!digitalRead(pswitch4)) {
        sendToDisplay2(String("dispswV.txt=\"") + String("4")+String("\""));
      }
      else{
        sendToDisplay2(String("dispswV.txt=\"") + String("none")+String("\""));
      }

    //Throttle buttons
      if (!digitalRead(maxthrottle)){
        sendToDisplay1(String("maxV.txt=\"") + String("1")+String("\""));  
      }
      else {
        sendToDisplay1(String("maxV.txt=\"") + String("0")+String("\""));
      }
      if (!digitalRead(minthrottle)){
        sendToDisplay1(String("minV.txt=\"") + String("1")+String("\""));  
      }
      else {
        sendToDisplay1(String("minV.txt=\"") + String("0")+String("\""));
      }

    //Action buttons
      if (!digitalRead(pSTAGE)){
        sendToDisplay1(String("stageV.txt=\"") + String("1")+String("\""));  
      }
      else {
        sendToDisplay1(String("stageV.txt=\"") + String("0")+String("\""));
      } 
      if (!digitalRead(pACTION1)){
        sendToDisplay1(String("A1V.txt=\"") + String("1")+String("\""));
        digitalWrite(pACTION1LED, HIGH);  
      }
      else {
        sendToDisplay1(String("A1V.txt=\"") + String("0")+String("\""));
        digitalWrite(pACTION1LED, LOW);
      }
      if (!digitalRead(pACTION2)){
        sendToDisplay1(String("A2V.txt=\"") + String("1")+String("\""));
        digitalWrite(pACTION2LED, HIGH);  
      }
      else {
        sendToDisplay1(String("A2V.txt=\"") + String("0")+String("\""));
        digitalWrite(pACTION2LED, LOW);
      }               
      if (!digitalRead(pACTION3)){
        sendToDisplay1(String("A3V.txt=\"") + String("1")+String("\""));
        digitalWrite(pACTION3LED, HIGH);  
      }
      else {
        sendToDisplay1(String("A3V.txt=\"") + String("0")+String("\""));
        digitalWrite(pACTION3LED, LOW);
      }    
      if (!digitalRead(pACTION4)){
        sendToDisplay1(String("A4V.txt=\"") + String("1")+String("\""));
        digitalWrite(pACTION4LED, HIGH);  
      }
      else {
        sendToDisplay1(String("A4V.txt=\"") + String("0")+String("\""));
        digitalWrite(pACTION4LED, LOW);
      }    
      if (!digitalRead(pACTION5)){
        sendToDisplay1(String("A5V.txt=\"") + String("1")+String("\""));
        digitalWrite(pACTION5LED, HIGH);  
      }
      else {
        sendToDisplay1(String("A5V.txt=\"") + String("0")+String("\""));
        digitalWrite(pACTION5LED, LOW);
      }    
      if (!digitalRead(pACTION6)){
        sendToDisplay1(String("A6V.txt=\"") + String("1")+String("\""));
        digitalWrite(pACTION6LED, HIGH);  
      }
      else {
        sendToDisplay1(String("A6V.txt=\"") + String("0")+String("\""));
        digitalWrite(pACTION6LED, LOW);
      }    
       if (!digitalRead(pCHUTES)){
        sendToDisplay1(String("chutesV.txt=\"") + String("1")+String("\""));
        digitalWrite(pCHUTESLED, HIGH);  
      }
      else {
        sendToDisplay1(String("chutesV.txt=\"") + String("0")+String("\""));
        digitalWrite(pCHUTESLED, LOW);
      }    
       if (!digitalRead(pLADDER)){
        sendToDisplay1(String("ladderV.txt=\"") + String("1")+String("\""));
        digitalWrite(pLADDERLED, HIGH);  
      }
      else {
        sendToDisplay1(String("ladderV.txt=\"") + String("0")+String("\""));
        digitalWrite(pLADDERLED, LOW);
      } 
       if (!digitalRead(pSOLAR)){
        sendToDisplay1(String("solarV.txt=\"") + String("1")+String("\""));
        digitalWrite(pSOLARLED, HIGH);  
      }
      else {
        sendToDisplay1(String("solarV.txt=\"") + String("0")+String("\""));
        digitalWrite(pSOLARLED, LOW);
      } 
       if (!digitalRead(pGEARS)){
        sendToDisplay1(String("gearsV.txt=\"") + String("1")+String("\""));
        digitalWrite(pGEARSLED, HIGH);  
      }
      else {
        sendToDisplay1(String("gearsV.txt=\"") + String("0")+String("\""));
        digitalWrite(pGEARSLED, LOW);
      } 
       if (!digitalRead(pBRAKES)){
        sendToDisplay2(String("brakesV.txt=\"") + String("1")+String("\""));
        digitalWrite(pBRAKESLED, HIGH);  
      }
      else {
        sendToDisplay2(String("brakesV.txt=\"") + String("0")+String("\""));
        digitalWrite(pBRAKESLED, LOW);
      } 
       if (!digitalRead(pLIGHTS)){
        sendToDisplay2(String("lightsV.txt=\"") + String("1")+String("\""));
        digitalWrite(pLIGHTSLED, HIGH);  
      }
      else {
        sendToDisplay2(String("lightsV.txt=\"") + String("0")+String("\""));
        digitalWrite(pLIGHTSLED, LOW);
      }
                                                    
      
    //Switches
      if (!digitalRead(pMODE)){
        sendToDisplay1(String("debugV.txt=\"") + String("1")+String("\""));  
      }
      else {
        sendToDisplay1(String("debugV.txt=\"") + String("0")+String("\""));
      }    
      if (digitalRead(pABORT)){
        sendToDisplay1(String("abortV.txt=\"") + String("1")+String("\""));  
      }
      else {
        sendToDisplay1(String("abortV.txt=\"") + String("0")+String("\""));
      }        
      if (digitalRead(pARM)){
        sendToDisplay1(String("armV.txt=\"") + String("1")+String("\""));  
      }
      else {
        sendToDisplay1(String("armV.txt=\"") + String("0")+String("\""));
      } 
      if (!digitalRead(pSAS)){
        sendToDisplay2(String("sasV.txt=\"") + String("1")+String("\""));  
      }
      else {
        sendToDisplay2(String("sasV.txt=\"") + String("0")+String("\""));
      } 
      if (!digitalRead(pRCS)){
        sendToDisplay2(String("rcsV.txt=\"") + String("1")+String("\""));  
      }
      else {
        sendToDisplay2(String("rcsV.txt=\"") + String("0")+String("\""));
      } 
      if (!digitalRead(pprecision)){
        sendToDisplay2(String("preccoV.txt=\"") + String("1")+String("\""));  
      }
      else {
        sendToDisplay2(String("preccoV.txt=\"") + String("0")+String("\""));
      } 
       if (!digitalRead(pmodeswitch)){
        sendToDisplay2(String("extra1V.txt=\"") + String("1")+String("\""));
        sendToDisplay2(String("extra1T.txt=\"") + String("Flymode")+String("\""));    
      }
      else {
        sendToDisplay2(String("extra1V.txt=\"") + String("0")+String("\""));
        sendToDisplay2(String("extra1T.txt=\"") + String("Flymode")+String("\"")); 
      }            
      
    //Atmospheregauge
    const int ledinterval = 1500; 
    atmotime = now - atmotimeOld;       
    if (atmotime > ledinterval) {
    atmotimeOld = now;    
    chooseled = chooseled+1;    
    if (chooseled==12){
      chooseled = 0;
    }
    switch(chooseled){
    case 0:
      AtmosphereGauge(5);   //LED1 
      break;
    case 1:
      AtmosphereGauge(4);   //LED2   
      break;
    case 2:
      AtmosphereGauge(2.5);   //LED3  
      break;
    case 3:
      AtmosphereGauge(1.5);   //LED4  
      break;
    case 4:
      AtmosphereGauge(1);   //LED5 
      break;
    case 5:
      AtmosphereGauge(0.8); //LED6  
      break;
    case 6:
      AtmosphereGauge(0.5); //LED7 
      break;
    case 7:
      AtmosphereGauge(0.3); //LED8 
      break;
    case 8:
      AtmosphereGauge(0.2); //LED9 
      break;            
     case 9:
      AtmosphereGauge(0.1); //LED10 
      break; 
    case 10:
      AtmosphereGauge(0.05); //LED11
      break;
    case 11:
      AtmosphereGauge(0.02); //LED12  
      break;                                  
    }
  }
  }
  motorfader(); //call motorfader if button pressed
  }  

else{   
    //KSP mode
    //send and receive data
    get_vessel_data();
    send_control_packet();
  }
  }

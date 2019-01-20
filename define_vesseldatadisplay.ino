//grab info from KSP here (VData object) and write out results to the Arduino pins

//connect to the KSPSerialIO plugin and receive data
int get_vessel_data() {
  int returnValue = -1;
  now = millis();

  if (KSPBoardReceiveData()){
    //data is being received
    deadtimeOld = now;
    returnValue = id;
    switch(id) {
    case 0: //First packet is a handshake packet
      Handshake();
      PageDisplay1 = 0;
      PageDisplay2 = 0;
      sendToDisplay1(String("page ") + String(PageDisplay1));
      sendToDisplay1(String("boot.txt=\"") + String("Handshake")+String("\""));
      sendToDisplay2(String("page ") + String(PageDisplay2));
      sendToDisplay2(String("boot.txt=\"") + String("Handshake")+String("\""));
      break;
    case 1:
      //subsequent packets are data from the plugin
      if (!digitalRead(pswitch1) && PageDisplay1 != 2) {
        PageDisplay1 = 2;
        sendToDisplay1(String("page ") + String(PageDisplay1)); 
      }
      else if (!digitalRead(pswitch2) && PageDisplay1 != 3) {
        PageDisplay1 = 3;
        sendToDisplay1(String("page ") + String(PageDisplay1)); 
      }
      else if (!digitalRead(pswitch3) && PageDisplay1 != 4) {
        PageDisplay1 = 4;
        sendToDisplay1(String("page ") + String(PageDisplay1)); 
      }
      else if (!digitalRead(pswitch4) && PageDisplay1 != 5) {
        PageDisplay1 = 5;
        sendToDisplay1(String("page ") + String(PageDisplay1)); 
      }     
      if (PageDisplay2 !=2) {
        PageDisplay2 = 2;
        sendToDisplay2(String("page ") + String(PageDisplay2));   
      }
      displayTime = now - displayTimeOld;
      if (displayTime > DISPLAYREFRESH) {
        displayTimeOld = now;
        define_vessel_data_display();   //define what to do with the data below
      }
      break;
    }
    Connected = true;
  }
  else
  { //if no message received for a while, go idle
    deadtime = now - deadtimeOld; 
    if (deadtime > IDLETIMER)
    {
      deadtimeOld = now;
      Connected = false;
      PageDisplay1 = 1;
      PageDisplay2 = 1;
      sendToDisplay1(String("page ") + String(PageDisplay1));
      sendToDisplay2(String("page ") + String(PageDisplay2));
      AtmosphereGauge(10);   //Atmosphere gauge off
    }    
  }
  return returnValue;
}

//get the current state of main controls and custom action groups using enumeration above, e.g. ControlStatus(AGBrakes);
byte ControlStatus(byte n)
{
  return ((VData.ActionGroups >> n) & 1) == 1;
}

//get the current SAS Mode. Can be compared to enum values, e.g. if(getSASMode() == SMPrograde){//do stuff}
byte getSASMode() {
  return VData.NavballSASMode & B00001111; // leaves alone the lower 4 bits of; all higher bits set to 0.
}

//get the current navball mode. Can be compared to enum values, e.g. if (getNavballMode() == NAVBallTARGET){//do stuff}
byte getNavballMode() {
  return VData.NavballSASMode >> 4; // leaves alone the higher 4 bits of; all lower bits set to 0.
}

//define what to do with the vessel data here, e.g. turn on LED's, display text on the LCD
void define_vessel_data_display() {

AtmosphereGauge(VData.Density);   //call Atmosphere gauge

//Orbit Data Page2
  if (PageDisplay1==2){
//Apoapsis
    String strApo = "";
    float DataAP = VData.AP; 
    if (DataAP < 1000000 && DataAP > -1000000) {
      strApo += String(DataAP,0);
      strApo += "m";
    } else if ((DataAP >= 1000000 && DataAP < 1000000000) || (DataAP <= -1000000 && DataAP > -1000000000)) {
      strApo += String((DataAP / 1000),0);
      strApo += "km";
    } else if ((DataAP >= 1000000000 && DataAP < 1000000000000) || (DataAP <= -1000000000 && DataAP > -1000000000000)) {
      strApo += String((DataAP / 1000000),0);
      strApo += "Mm";
    } else {
      strApo += String((DataAP / 1000000000),0);
      strApo += "Gm";
    }
    sendToDisplay1(String("APHV.txt=\"") + String(strApo)+String("\""));
    
//Periapsis
    String strPeri = "";
    float DataPE = VData.PE;
    if (DataPE < 1000000 && DataPE > -1000000) {
      strPeri += String(DataPE,0);
      strPeri += "m";
    } else if ((DataPE >= 1000000 && DataPE < 1000000000) || (DataPE <= -1000000 && DataPE > -1000000000)) {
      strPeri += String((DataPE / 1000),0);
      strPeri += "km";
    } else if ((DataPE >= 1000000000 && DataPE < 1000000000000) || (DataPE <= -1000000000 && DataPE > -1000000000000)) {
      strPeri += String((DataPE / 1000000),0);
      strPeri += "Mm";
    } else {
      strPeri += String((DataPE / 1000000000),0);
      strPeri += "Gm";
    }
    sendToDisplay1(String("PEHV.txt=\"") + String(strPeri)+String("\""));

// Time to Apoapsis
    String strApotime = "";
    long DataAPTime = VData.TAp;
    long secondsAP=0;
    long minutesAP=0;
    long hoursAP=0;
    if (DataAPTime < 60 && DataAPTime > -60){
      strApotime += String(DataAPTime); 
      strApotime += "s";
    } else if ((DataAPTime >= 60 && DataAPTime < 3600) || (DataAPTime <= -60 && DataAPTime > -3600)) {
      secondsAP = DataAPTime % 60;
      minutesAP = DataAPTime / 60;
      strApotime += String(minutesAP);
      strApotime += "m";
      strApotime += String(secondsAP);
      strApotime += "s";
    } else {
      secondsAP = DataAPTime % 60;
      minutesAP = DataAPTime /60;
      hoursAP = minutesAP / 60;
      minutesAP = minutesAP % 60;
      strApotime += String(hoursAP);
      strApotime += "h";     
      strApotime += String(minutesAP);
      strApotime += "m";
      strApotime += String(secondsAP);
      strApotime += "s";
    }
    sendToDisplay1(String("APTV.txt=\"") + String(strApotime)+String("\""));

// Time to Periapsis
    String strPeritime = "";
    long DataPETime = VData.TPe;
    long secondsPE=0;
    long minutesPE=0;
    long hoursPE=0;
    if (DataPETime < 60 && DataPETime > -60){
      strPeritime += String(DataPETime); 
      strPeritime += "s";
    } else if ((DataPETime >= 60 && DataPETime < 3600) || (DataPETime <= -60 && DataPETime > -3600)) {
      secondsPE = DataPETime % 60;
      minutesPE = DataPETime / 60;
      strPeritime += String(minutesPE);
      strPeritime += "m";
      strPeritime += String(secondsPE);
      strPeritime += "s";
    } else {
      secondsPE = DataPETime % 60;
      minutesPE = DataPETime /60;
      hoursPE = minutesPE / 60;
      minutesPE = minutesPE % 60;
      strPeritime += String(hoursPE);
      strPeritime += "h";     
      strPeritime += String(minutesPE);
      strPeritime += "m";
      strPeritime += String(secondsPE);
      strPeritime += "s";
    }
    sendToDisplay1(String("PETV.txt=\"") + String(strPeritime)+String("\""));    

// Eccentricity
    String streccentricity = "";
    streccentricity += String(VData.e);
    streccentricity += " "; 
    sendToDisplay1(String("ECCV.txt=\"") + String(streccentricity)+String("\"")); 

// Inclination
    String strinclination = "";
    strinclination += String(VData.inc);
    strinclination += char(176); 
    sendToDisplay1(String("INCV.txt=\"") + String(strinclination)+String("\""));

// Orbital Period
    String strperiod = "";
    long DataOPeriod = VData.period;    
    long secondsperiod = 0;
    long minutesperiod = 0;
    long hoursperiod = 0;
    if (DataOPeriod < 60 && DataOPeriod > -60){
      strperiod += String(DataOPeriod); 
      strperiod += "s";
    } else if ((DataOPeriod >= 60 && DataOPeriod < 3600) || (DataOPeriod <= -60 && DataOPeriod > -3600)) {
      secondsperiod = DataOPeriod % 60;
      minutesperiod = DataOPeriod / 60;
      strperiod += String(minutesperiod);
      strperiod += "m";
      strperiod += String(secondsperiod);
      strperiod += "s";
    } else {
      secondsperiod = DataOPeriod % 60;
      minutesperiod = DataOPeriod /60;
      hoursperiod = minutesperiod / 60;
      minutesperiod = minutesperiod % 60;
      strperiod += String(hoursperiod);
      strperiod += "h";     
      strperiod += String(minutesperiod);
      strperiod += "m";
      strperiod += String(secondsperiod);
      strperiod += "s";
    }
    sendToDisplay1(String("ORPV.txt=\"") + String(strperiod)+String("\""));

//Semi Major Axis
    String strMajorAxis = "";
    long DataSMAxis = VData.SemiMajorAxis;  
    if (DataSMAxis < 1000000 && DataSMAxis > -1000000) {
      strMajorAxis += String(DataSMAxis);
      strMajorAxis += "m";
    } else if ((DataSMAxis >= 1000000 && DataSMAxis < 1000000000) || (DataSMAxis <= -1000000 && DataSMAxis > -1000000000)) {
      strMajorAxis += String((DataSMAxis / 1000));
      strMajorAxis += "km";
    } else if ((DataSMAxis >= 1000000000 && DataSMAxis < 1000000000000) || (DataSMAxis <= -1000000000 && DataSMAxis > -1000000000000)) {
      strMajorAxis += String((DataSMAxis / 1000000),0);
      strMajorAxis += "Mm";
    } else {
      strMajorAxis += String((DataSMAxis / 1000000000),0);
      strMajorAxis += "Gm";
    }
    sendToDisplay1(String("SMAV.txt=\"") + String(strMajorAxis)+String("\""));

// Orbital Speed
    String strOrbitalSpeed = "";
    strOrbitalSpeed += String(VData.VOrbit);
    strOrbitalSpeed += "m/s"; 
    sendToDisplay1(String("ORSV.txt=\"") + String(strOrbitalSpeed)+String("\""));
  }
  
//Surface Data Page3
  if (PageDisplay1==3){
//Altidude Sealevel
    String strAltSea = "";
    long DataAlt = VData.Alt; 
    if (DataAlt < 1000000 && DataAlt > -1000000) {
      strAltSea += String(DataAlt);
      strAltSea += "m";
    } else if ((DataAlt >= 1000000 && DataAlt < 1000000000) || (DataAlt <= -1000000 && DataAlt > -1000000000)) {
      strAltSea += String((DataAlt / 1000.0));
      strAltSea += "km";
    } else if ((DataAlt >= 1000000000 && DataAlt < 1000000000000) || (DataAlt <= -1000000000 && DataAlt > -1000000000000)) {
      strAltSea += String((DataAlt / 1000000.0));
      strAltSea += "Mm";
    } else {
      strAltSea += String((DataAlt / 1000000000.0));
      strAltSea += "Gm";
    }
    sendToDisplay1(String("ALSV.txt=\"") + String(strAltSea)+String("\""));

//Altidude Terrain
    String strAltTerrain = "";
    long DataRAlt = VData.RAlt; 
    if (DataRAlt < 1000000 && DataRAlt > -1000000) {
      strAltTerrain += String(DataRAlt);
      strAltTerrain += "m";
    } else if ((DataRAlt >= 1000000 && DataRAlt < 1000000000) || (DataRAlt <= -1000000 && DataRAlt > -1000000000)) {
      strAltTerrain += String((DataRAlt / 1000.0));
      strAltTerrain += "km";
    } else if ((DataRAlt >= 1000000000 && DataRAlt < 1000000000000) || (DataRAlt <= -1000000000 && DataRAlt > -1000000000000)) {
      strAltTerrain += String((DataRAlt / 1000000.0));
      strAltTerrain += "Mm";
    } else {
      strAltTerrain += String((DataRAlt / 1000000000.0));
      strAltTerrain += "Gm";
    }
    sendToDisplay1(String("ALTV.txt=\"") + String(strAltTerrain)+String("\""));
    
// Surface Speed
    String strSurfaceSpeed = "";
    strSurfaceSpeed += String(VData.Vsurf,0);
    strSurfaceSpeed += "m/s"; 
    sendToDisplay1(String("SUSV.txt=\"") + String(strSurfaceSpeed)+String("\"")); 

// Vertical Speed
    String strVerticalSpeed = "";
    strVerticalSpeed += String(VData.VVI,0);
    strVerticalSpeed += "m/s"; 
    sendToDisplay1(String("VESV.txt=\"") + String(strVerticalSpeed)+String("\""));    

// Horizontal Speed
    String strHorizontalSpeed = "";
    long hV = sqrt((pow(VData.Vsurf,2) - pow(VData.VVI,2)));
    strHorizontalSpeed += String(hV);
    strHorizontalSpeed += "m/s"; 
    sendToDisplay1(String("HOSV.txt=\"") + String(strHorizontalSpeed)+String("\"")); 

// Mach Number
    String strMach = "";
    strMach += String(VData.MachNumber);
    strMach += ""; 
    sendToDisplay1(String("MACV.txt=\"") + String(strMach)+String("\"")); 

// Max Part Heat
    String strHeat = "";
    strHeat = VData.MaxOverHeat; 
    strHeat += "%"; 
    sendToDisplay1(String("MPHV.txt=\"") + String(strHeat)+String("\""));

// acceleration
    String strgees = "";
    strgees += String(VData.G);
    strgees += " G"; 
    sendToDisplay1(String("ACCV.txt=\"") + String(strgees)+String("\"")); 

// air density
    String strAirDensity = "";
    strAirDensity += String(VData.Density);
    strAirDensity += "kg/m";
    strAirDensity += char(179);
    sendToDisplay1(String("ADEV.txt=\"") + String(strAirDensity)+String("\""));            
  }
  
//Target/Node Data Page4
  if (PageDisplay1==4){
// Distance to targeted vessel
    String strDistanceVessel = "";
    long DataTargetDist = VData.TargetDist;
    if (DataTargetDist < 1000000 && DataTargetDist > -1000000) {
      strDistanceVessel += String(DataTargetDist);
      strDistanceVessel += "m";
    } else if ((DataTargetDist >= 1000000 && DataTargetDist < 1000000000) || (DataTargetDist <= -1000000 && DataTargetDist > -1000000000)) {
      strDistanceVessel += String((DataTargetDist / 1000.0));
      strDistanceVessel += "km";
    } else if ((DataTargetDist >= 1000000000 && DataTargetDist < 1000000000000) || (DataTargetDist <= -1000000000 && DataTargetDist > -1000000000000)) {
      strDistanceVessel += String((DataTargetDist / 1000000.0));
      strDistanceVessel += "Mm";
    } else {
      strDistanceVessel += String((DataTargetDist / 1000000000.0));
      strDistanceVessel += "Gm";
    }
    sendToDisplay1(String("DITV.txt=\"") + String(strDistanceVessel)+String("\""));

// Target relative Speed
    String strTargetSpeed = "";
    strTargetSpeed += String(VData.TargetV,0);
    strTargetSpeed += "m/s"; 
    sendToDisplay1(String("RESV.txt=\"") + String(strTargetSpeed)+String("\""));

// Time to Node
    String strTimeToNode = "";
    long DataMNTime = VData.MNTime;
    long secondsnode = 0;
    long minutesnode = 0;
    long hoursnode = 0;
      if ((DataMNTime >= 0 && DataMNTime < 60) || (DataMNTime <= 0 && DataMNTime > -3600)) {
      strTimeToNode += String(DataMNTime); 
      strTimeToNode += "s";
    } else if ((DataMNTime >= 60 && DataMNTime < 3600) || (DataMNTime <= -60 && DataMNTime > -3600)) {
      secondsnode = DataMNTime % 60;
      minutesnode = DataMNTime / 60;
      strTimeToNode += String(minutesnode);
      strTimeToNode += "m";
      strTimeToNode += String(secondsnode);
      strTimeToNode += "s";
    } else {
      secondsnode = DataMNTime % 60;
      minutesnode = DataMNTime /60;
      hoursnode = minutesnode / 60;
      minutesnode = minutesnode % 60;
      strTimeToNode += String(hoursnode);
      strTimeToNode += "h";     
      strTimeToNode += String(minutesnode);
      strTimeToNode += "m";
      strTimeToNode += String(secondsnode);
      strTimeToNode += "s";
    } 
    sendToDisplay1(String("TTNV.txt=\"") + String(strTimeToNode)+String("\""));

// DeltaV for next Node
    String strDeltaVNode = "";
    strDeltaVNode += String(VData.MNDeltaV,0);
    strDeltaVNode += "m/s"; 
    sendToDisplay1(String("DVNV.txt=\"") + String(strDeltaVNode)+String("\""));         
  }
  
//Body Data Page5
  if (PageDisplay1==5){
// SOI Number
    //100 = KERBOL
    //110 = MOHO
    //120 = EVE
    //121 = GILLY
    //130 = KERBIN
    //131 = MUN
    //132 = MINMUS
    //140 = DUNA
    //141 = IKE
    //150 = DRES
    //160 = JOOL
    //161 = LAYTHE
    //162 = VALL
    //163 = TYLO
    //164 = BOP
    //165 = POL
    //170 = EELOO
    String strSafeAltidude = "-";
    String strHighAtmosphere = "-";
    String strLowSpace = "-";
    String strHighSpace = "-";
    String strSynchrounousAlt = "-";
    String strSOIAlt = "-";
    String strRotationPeriod = "-";
    String strEscapeVelocity = "-";
    String strBody = "-";  
    String strSOINumber = "";
    strSOINumber = VData.SOINumber;
    if (strSOINumber == "100"){
     strSafeAltidude = "2065Mm";
     strHighAtmosphere = "18000m";
     strLowSpace = "600000m";
     strHighSpace = "1000Mm";
     strSynchrounousAlt = "1508Mm";
     strSOIAlt = "-";
     strRotationPeriod = "20d0h0m0s";
     strEscapeVelocity = "94672m/s";
     strBody = "KERBOL";
    }
    else if (strSOINumber == "110"){
     strSafeAltidude = "8000m";
     strHighAtmosphere = "-";
     strLowSpace = "0m";
     strHighSpace = "80000m";
     strSynchrounousAlt = "18173km";
     strSOIAlt = "9647km";
     strRotationPeriod = "56d0h6m40s";
     strEscapeVelocity = "1161m/s";
     strBody = "MOHO";      
    }
    else if (strSOINumber == "120"){
     strSafeAltidude = "90000m";
     strHighAtmosphere = "22000m";
     strLowSpace = "90000m";
     strHighSpace = "400000m";
     strSynchrounousAlt = "10328km";
     strSOIAlt = "85109km";
     strRotationPeriod = "3d4h21m40s";
     strEscapeVelocity = "4832m/s";
     strBody = "EVE";      
    }
    else if (strSOINumber == "121"){
     strSafeAltidude = "7000m";
     strHighAtmosphere = "-";
     strLowSpace = "0m";
     strHighSpace = "6000m";
     strSynchrounousAlt = "42138m";
     strSOIAlt = "126123m";
     strRotationPeriod = "1d1h50m55s";
     strEscapeVelocity = "36m/s";
     strBody = "GILLY";      
    }
    else if (strSOINumber == "130"){
     strSafeAltidude = "70000m";
     strHighAtmosphere = "18000m";
     strLowSpace = "70000m";
     strHighSpace = "250000m";
     strSynchrounousAlt = "2863km";
     strSOIAlt = "84159km";
     strRotationPeriod = "5h59m9s";
     strEscapeVelocity = "3431m/s";
     strBody = "KERBIN";      
    }
    else if (strSOINumber == "131"){
     strSafeAltidude = "8000m";
     strHighAtmosphere = "-";
     strLowSpace = "0m";
     strHighSpace = "60000m";
     strSynchrounousAlt = "2971km";
     strSOIAlt = "2430km";
     strRotationPeriod = "6d2h36m24s";
     strEscapeVelocity = "807m/s";
     strBody = "MUN";      
    }
    else if (strSOINumber == "132"){
     strSafeAltidude = "6000m";
     strHighAtmosphere = "-";
     strLowSpace = "0m";
     strHighSpace = "30000m";
     strSynchrounousAlt = "357941m";
     strSOIAlt = "2247km";
     strRotationPeriod = "1d5h13m20s";
     strEscapeVelocity = "243m/s";
     strBody = "MINMUS";      
    }
    else if (strSOINumber == "140"){
     strSafeAltidude = "50000m";
     strHighAtmosphere = "12000m";
     strLowSpace = "50000m";
     strHighSpace = "140000m";
     strSynchrounousAlt = "2880km";
     strSOIAlt = "47922km";
     strRotationPeriod = "3d0h11m58s";
     strEscapeVelocity = "1372m/s";
     strBody = "DUNA";      
    }
    else if (strSOINumber == "141"){
     strSafeAltidude = "14000m";
     strHighAtmosphere = "-";
     strLowSpace = "0m";
     strHighSpace = "50000m";
     strSynchrounousAlt = "1134km";
     strSOIAlt = "1050km";
     strRotationPeriod = "3d0h11m58s";
     strEscapeVelocity = "534m/s";
     strBody = "IKE";      
    }
    else if (strSOINumber == "150"){
     strSafeAltidude = "6000m";
     strHighAtmosphere = "-";
     strLowSpace = "0m";
     strHighSpace = "25000m";
     strSynchrounousAlt = "732244m";
     strSOIAlt = "32833km";
     strRotationPeriod = "1d3h40m0s";
     strEscapeVelocity = "558m/s";
     strBody = "DRES";      
    }
    else if (strSOINumber == "160"){
     strSafeAltidude = "200000m";
     strHighAtmosphere = "120000m";
     strLowSpace = "200000m";
     strHighSpace = "4000km";
     strSynchrounousAlt = "15011km";
     strSOIAlt = "2456Mm";
     strRotationPeriod = "1d4h0m0s";
     strEscapeVelocity = "9704m/s";
     strBody = "JOOL";      
    }
    else if (strSOINumber == "161"){
     strSafeAltidude = "50000m";
     strHighAtmosphere = "10000m";
     strLowSpace = "50000m";
     strHighSpace = "200000m";
     strSynchrounousAlt = "4686km";
     strSOIAlt = "3724km";
     strRotationPeriod = "2d2h43m1s";
     strEscapeVelocity = "2801m/s";
     strBody = "LAYTHE";      
    }
    else if (strSOINumber == "162"){
     strSafeAltidude = "9000m";
     strHighAtmosphere = "-";
     strLowSpace = "0m";
     strHighSpace = "90000m";
     strSynchrounousAlt = "3593km";
     strSOIAlt = "2406km";
     strRotationPeriod = "4d5h26m2s";
     strEscapeVelocity = "1176m/s";
     strBody = "VALL";      
    }
    else if (strSOINumber == "163"){
     strSafeAltidude = "12000m";
     strHighAtmosphere = "-";
     strLowSpace = "0m";
     strHighSpace = "250000m";
     strSynchrounousAlt = "14158km";
     strSOIAlt = "10857km";
     strRotationPeriod = "9d4h52m6s";
     strEscapeVelocity = "3069m/s";
     strBody = "TYLO";      
    }
    else if (strSOINumber == "164"){
     strSafeAltidude = "23000m";
     strHighAtmosphere = "-";
     strLowSpace = "0m";
     strHighSpace = "25000m";
     strSynchrounousAlt = "2588km";
     strSOIAlt = "1221km";
     strRotationPeriod = "25d1h15m7s";
     strEscapeVelocity = "246m/s";
     strBody = "BOP";      
    }
    else if (strSOINumber == "165"){
     strSafeAltidude = "6000m";
     strHighAtmosphere = "-";
     strLowSpace = "0m";
     strHighSpace = "22000m";
     strSynchrounousAlt = "2415km";
     strSOIAlt = "1042km";
     strRotationPeriod = "41d4h31m";
     strEscapeVelocity = "181m/s";
     strBody = "POL";      
    }
    else if (strSOINumber == "170"){
     strSafeAltidude = "5000m";
     strHighAtmosphere = "-";
     strLowSpace = "0m";
     strHighSpace = "60000m";
     strSynchrounousAlt = "683691m";
     strSOIAlt = "119083km";
     strRotationPeriod = "5h24m20s";
     strEscapeVelocity = "842m/s";
     strBody = "EELOO";      
    }
    else {
     strSafeAltidude = "-";
     strHighAtmosphere = "-";
     strLowSpace = "-";
     strHighSpace = "-";
     strSynchrounousAlt = "-";
     strSOIAlt = "-";
     strRotationPeriod = "-";
     strEscapeVelocity = "-";
     strBody = "-";      
    }                                           
    sendToDisplay1(String("SAAV.txt=\"") + String(strSafeAltidude)+String("\"")); 
    sendToDisplay1(String("HATV.txt=\"") + String(strHighAtmosphere)+String("\"")); 
    sendToDisplay1(String("LOSV.txt=\"") + String(strLowSpace)+String("\"")); 
    sendToDisplay1(String("HISV.txt=\"") + String(strHighSpace)+String("\"")); 
    sendToDisplay1(String("SYAV.txt=\"") + String(strSynchrounousAlt)+String("\"")); 
    sendToDisplay1(String("SOAV.txt=\"") + String(strSOIAlt)+String("\"")); 
    sendToDisplay1(String("ROPV.txt=\"") + String(strRotationPeriod)+String("\"")); 
    sendToDisplay1(String("ESVV.txt=\"") + String(strEscapeVelocity)+String("\"")); 
    sendToDisplay1(String("BODYV.txt=\"") + String(strBody)+String("\""));      
  }
 
//Ressources Page2 Display2
  if (PageDisplay2==2){
  const unsigned int gaugeColorOK = 29927;           //Color green when full
  const unsigned int gaugeColorAlert = 59169;        //Color yellow when alert
  const unsigned int gaugeColorCritical = 63488;     //Color red when critical
    
  //Battery
  if (VData.EChargeTot <= 0){
     sendToDisplay2(String("jel.val=") + String(100));
  }else{
      //gauges
      byte vEL = 100-(100 * VData.ECharge / VData.EChargeTot);            //percentage of electric charge remaining
      sendToDisplay2(String("jel.val=") + String(vEL));
      //change color in gauges
      if (vEL >=0 && vEL < 60){
      sendToDisplay2(String("jel.bco=") + String(gaugeColorOK));
      }
      else if (vEL >=60 && vEL <= 90){
      sendToDisplay2(String("jel.bco=") + String(gaugeColorAlert));  
       }
      else {
      sendToDisplay2(String("jel.bco=") + String(gaugeColorCritical));  
      }
      //Numbers
      String strbattery = String(VData.ECharge,0);
      strbattery += "/";
      strbattery += String(VData.EChargeTot,0);
      sendToDisplay2(String("tel.txt=\"") + String(strbattery)+String("\""));
  }
  //Monopropellant
  if (VData.MonoPropTot <= 0){
     sendToDisplay2(String("jmp.val=") + String(100));  
  }else{
      //gauges
      byte vMP = 100-(100 * VData.MonoProp / VData.MonoPropTot);          //percentage of monopropellant remaining
      sendToDisplay2(String("jmp.val=") + String(vMP));       
      //change color in gauges  
      if (vMP >=0 && vMP < 60){
      sendToDisplay2(String("jmp.bco=") + String(gaugeColorOK));
      }
      else if (vMP >=60 && vMP <= 90){
      sendToDisplay2(String("jmp.bco=") + String(gaugeColorAlert));  
      }
      else {
      sendToDisplay2(String("jmp.bco=") + String(gaugeColorCritical));  
      }
      //Numbers
      String strMonopropellant = String(VData.MonoProp,0);
      strMonopropellant += "/";
      strMonopropellant += String(VData.MonoPropTot,0);
      sendToDisplay2(String("tmp.txt=\"") + String(strMonopropellant)+String("\""));
  }
  //Liquid Fuel
  if (VData.LiquidFuelTot <= 0){
     sendToDisplay2(String("jlf.val=") + String(100));  
  }else{
      //gauges
      byte vLF = 100-(100 * VData.LiquidFuel / VData.LiquidFuelTot);    //percentage of liquid fuel remaining in current stage
      sendToDisplay2(String("jlf.val=") + String(vLF));
      //change color in gauges
      if (vLF >=0 && vLF < 60){
      sendToDisplay2(String("jlf.bco=") + String(gaugeColorOK));
      }
      else if (vLF >=60 && vLF <= 90){
      sendToDisplay2(String("jlf.bco=") + String(gaugeColorAlert));  
      }
      else {
      sendToDisplay2(String("jlf.bco=") + String(gaugeColorCritical));  
      }
      //Numbers
      String strLiquidFuel = String(VData.LiquidFuel,0);
      strLiquidFuel += "/";
      strLiquidFuel += String(VData.LiquidFuelTot,0);
      sendToDisplay2(String("tlf.txt=\"") + String(strLiquidFuel)+String("\""));
  }
  //Oxidized
  if (VData.OxidizerTot <= 0){
     sendToDisplay2(String("jox.val=") + String(100));
  }else{
      //gauges
      byte vOX = 100-(100 * VData.Oxidizer / VData.OxidizerTot);        //percentage of oxidized remaining in current stage
      sendToDisplay2(String("jox.val=") + String(vOX));

      if (vOX >=0 && vOX < 60){
      sendToDisplay2(String("jox.bco=") + String(gaugeColorOK));
      }
      else if (vOX >=60 && vOX <= 90){
      sendToDisplay2(String("jox.bco=") + String(gaugeColorAlert));  
      }
      else {
      sendToDisplay2(String("jox.bco=") + String(gaugeColorCritical));  
      } 
      //Numbers
      String strOxidized = String(VData.Oxidizer,0);
      strOxidized += "/";
      strOxidized += String(VData.OxidizerTot,0);
      sendToDisplay2(String("tox.txt=\"") + String(strOxidized)+String("\""));              
  }
  //Solid Fuel
  if (VData.SolidFuelTot <= 0){
     sendToDisplay2(String("jsf.val=") + String(100));
  }else{
      //gauges
      byte vSF = 100-(100 * VData.SolidFuel / VData.SolidFuelTot);        //percentage of solid fuel remaining
      sendToDisplay2(String("jsf.val=") + String(vSF));
 
      if (vSF >=0 && vSF < 60){
      sendToDisplay2(String("jsf.bco=") + String(gaugeColorOK));
      }
      else if (vSF >=60 && vSF <= 90){
      sendToDisplay2(String("jsf.bco=") + String(gaugeColorAlert));  
      }
      else {
      sendToDisplay2(String("jsf.bco=") + String(gaugeColorCritical));  
      }  
      //Numbers
      String strSolidFuel = String(VData.SolidFuel,0);
      strSolidFuel += "/";
      strSolidFuel += String(VData.SolidFuelTot,0);
      sendToDisplay2(String("tsf.txt=\"") + String(strSolidFuel)+String("\""));        
  }
  //Xenon Gas
  if (VData.XenonGasTot <= 0){
     sendToDisplay2(String("jxe.val=") + String(100));
  }else{
      //gauges
      byte vXE = 100-(100 * VData.XenonGas / VData.XenonGasTot);          //percentage of xenon gas remaining
      sendToDisplay2(String("jxe.val=") + String(vXE));
      //change color in gauges
      if (vXE >=0 && vXE < 60){
      sendToDisplay2(String("jxe.bco=") + String(gaugeColorOK));
      }
      else if (vXE >=60 && vXE <= 90){
      sendToDisplay2(String("jxe.bco=") + String(gaugeColorAlert));  
      }
      else {
      sendToDisplay2(String("jxe.bco=") + String(gaugeColorCritical));  
      }
      //Numbers
      String strXenonGas = String(VData.XenonGas,0);
      strXenonGas += "/";
      strXenonGas += String(VData.XenonGasTot,0);
      sendToDisplay2(String("txe.txt=\"") + String(strXenonGas)+String("\""));           
  }
  //Intake Air
  if (VData.IntakeAirTot <= 0){
     sendToDisplay2(String("jair.val=") + String(100));
  }else{
      //gauges
      byte vAIR = 100-(100 * VData.IntakeAir / VData.IntakeAirTot);         //percentage of xenon gas remaining
      sendToDisplay2(String("jair.val=") + String(vAIR));  
      //change color in gauges
      if (vAIR >=0 && vAIR < 60){
      sendToDisplay2(String("jair.bco=") + String(gaugeColorOK));
      }
     else if (vAIR >=60 && vAIR <= 90){
      sendToDisplay2(String("jair.bco=") + String(gaugeColorAlert));  
      }
      else {
      sendToDisplay2(String("jair.bco=") + String(gaugeColorCritical));  
      }  
      //Numbers
      String strIntakeAir = String(VData.IntakeAir,0);
      strIntakeAir += "/";
      strIntakeAir += String(VData.IntakeAirTot,0);
      sendToDisplay2(String("tair.txt=\"") + String(strIntakeAir)+String("\""));                   
  }
  }  


 
  //get in-game status for updating the LED statuses on the controller  
  lights_on = ControlStatus(AGLight);
  gears_on = ControlStatus(AGGears);
  brakes_on = ControlStatus(AGBrakes);
  action1_on = ControlStatus(AGCustom01);
  action2_on = ControlStatus(AGCustom02);
  action3_on = ControlStatus(AGCustom03);
  action4_on = ControlStatus(AGCustom04);
  action5_on = ControlStatus(AGCustom05);
  action6_on = ControlStatus(AGCustom06);  
  ladder_on = ControlStatus(AGCustom07);
  chutes_on = ControlStatus(AGCustom08);  
  solar_on = ControlStatus(AGCustom09);


  //update button LEDs based on in-game status
  digitalWrite(pLIGHTSLED, lights_on); 
  digitalWrite(pGEARSLED, gears_on);
  digitalWrite(pBRAKESLED, brakes_on);
  digitalWrite(pACTION1LED, action1_on);
  digitalWrite(pACTION2LED, action2_on);
  digitalWrite(pACTION3LED, action3_on);
  digitalWrite(pACTION4LED, action4_on);
  digitalWrite(pACTION5LED, action5_on);
  digitalWrite(pACTION6LED, action6_on);    
  digitalWrite(pLADDERLED, ladder_on);
  digitalWrite(pSOLARLED, solar_on);
  digitalWrite(pCHUTESLED, chutes_on);


}

void AtmosphereGauge (float DataAtmo){
//Atmosphere gauge

 //prepare the shift register
  digitalWrite(dataPin, LOW);
  digitalWrite(clockPin, LOW);
  digitalWrite(latchPin, LOW); 
  
  int vA = (12 - 6*log10(1 + 99*(DataAtmo / 6)));
  vA = (vA < 12) ? vA : 11;
  int powvA = 0.1+pow(2,vA);
    shiftOut(dataPin, clockPin, LSBFIRST, powvA<<4);
    shiftOut(dataPin, clockPin, LSBFIRST, powvA>>4);

  //latch the values in when done shifting
  digitalWrite(latchPin, HIGH); 
}

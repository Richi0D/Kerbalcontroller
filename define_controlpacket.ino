//Define what controls to send to KSP

//check if it is time to send a control packet
void send_control_packet() {
  now = millis();
  controlTime = now - controlTimeOld;
  if (controlTime > CONTROLREFRESH) {
    controlTimeOld = now;
    define_control_packet();
    motorfader(); //call motorfader if button pressed
  }
}

//Main controls uses enum above, e.g. MainControls(SAS,true);
void MainControls(byte n, boolean s) {
  if (s)
    CPacket.MainControls |= (1 << n);       // forces nth bit of x to be 1.  all other bits left alone.
  else
    CPacket.MainControls &= ~(1 << n);      // forces nth bit of x to be 0.  all other bits left alone.
}

//Control groups (action groups) uses an integer to refer to a custom action group, e.g. ControlGroup(5,true);
void ControlGroups(byte n, boolean s) {
  if (s)
    CPacket.ControlGroup |= (1 << n);       // forces nth bit of x to be 1.  all other bits left alone.
  else
    CPacket.ControlGroup &= ~(1 << n);      // forces nth bit of x to be 0.  all other bits left alone.
}

//SAS mode uses enum above, e.g. setSASMode(SMPrograde);
void setSASMode(byte m) {
  CPacket.NavballSASMode &= B11110000;
  CPacket.NavballSASMode += m;
}

//Navball mode uses enum above, e.g. setNavBallMode(NAVBallSURFACE);
void setNavballMode(byte m) {
  CPacket.NavballSASMode &= B00001111;
  CPacket.NavballSASMode += m << 4;
}

void define_control_packet() {
  if (Connected) {
    //here we define what controls to send when which pins are manipulate
    
    //toggle switches
    if(!digitalRead(pSAS)){MainControls(SAS, true);} else {MainControls(SAS, false);}
    if(!digitalRead(pRCS)){MainControls(RCS, true);} else {MainControls(RCS, false);}
    if(digitalRead(pABORT)){MainControls(ABORT, true);} else {MainControls(ABORT, false);}
  
    //momentary stage button
    if(debouncerStage.pressed() && digitalRead(pARM)){MainControls(STAGE, true);} else {MainControls(STAGE, false);}

    
    //toggle buttons
    if(debouncerLights.pressed()){MainControls(LIGHTS, !lights_on);}
    if(debouncerGears.pressed()){MainControls(GEARS, !gears_on);}
    if(debouncerBrakes.pressed()){MainControls(BRAKES, !brakes_on);}
    if(debouncerA1.pressed()){ControlGroups(1, !action1_on);}
    if(debouncerA2.pressed()){ControlGroups(2, !action2_on);}
    if(debouncerA3.pressed()){ControlGroups(3, !action3_on);}
    if(debouncerA4.pressed()){ControlGroups(4, !action4_on);}
    if(debouncerA5.pressed()){ControlGroups(5, !action5_on);} 
    if(debouncerA6.pressed()){ControlGroups(6, !action6_on);}      
    if(debouncerLadder.pressed()){ControlGroups(7, !ladder_on);}
    if(debouncerChutes.pressed()){ControlGroups(8, !chutes_on);}    
    if(debouncerSolar.pressed()){ControlGroups(9, !solar_on);}


    //check flymode 
     if((digitalRead(pprecision)) and (!digitalRead(pmodeswitch))){
       flymode = 0;} // rocket mode
     else if((digitalRead(pprecision)) and (digitalRead(pmodeswitch))){
       flymode = 1;} // plane mode
     else if((!digitalRead(pprecision)) and (!digitalRead(pmodeswitch))){
       flymode = 2;} // rocket mode + precision      
     else if((!digitalRead(pprecision)) and (digitalRead(pmodeswitch))){
       flymode = 3;} // plane mode + precision
     else {flymode = 0;}  

    int deadzone = 25;
    int deadzonethrottle = 10;
        
    switch(flymode){
    case 0:
      CPacket.Throttle = constrain(map(analogRead(pTHROTTLE),1023-deadzonethrottle,0+deadzonethrottle,0,1000),0,1000);
      CPacket.WheelThrottle = constrain(map(analogRead(pTHROTTLE),1023-deadzonethrottle,0+deadzonethrottle,0,1000),0,1000);
      
      if(analogRead(pRX) >= 512+deadzone){CPacket.Yaw = constrain(map(analogRead(pRX),512+deadzone,1023,0,1000),0,1000);
      CPacket.WheelSteer = constrain(map(analogRead(pRX),1023,512+deadzone,-1000,0),-1000,0);}
      else if(analogRead(pRX) <= 512-deadzone){CPacket.Yaw = constrain(map(analogRead(pRX),0,512-deadzone,-1000,0),-1000,0);
      CPacket.WheelSteer = constrain(map(analogRead(pRX),512-deadzone,0,0,1000),0,1000);}
      else {CPacket.Yaw = 0;
      CPacket.WheelSteer = 0;}
      if(analogRead(pRY) >= 512+deadzone){CPacket.Pitch = constrain(map(analogRead(pRY),512+deadzone,1023,0,1000),0,1000);}
      else if(analogRead(pRY) <= 512-deadzone){CPacket.Pitch = constrain(map(analogRead(pRY),0,512-deadzone,-1000,0),-1000,0);}
      else {CPacket.Pitch = 0;}
      if(analogRead(pRZ) >= 512+deadzone){CPacket.Roll = constrain(map(analogRead(pRZ),512+deadzone,1023,0,1000),0,1000);}
      else if(analogRead(pRZ) <= 512-deadzone){CPacket.Roll = constrain(map(analogRead(pRZ),0,512-deadzone,-1000,0),-1000,0);}
      else {CPacket.Roll = 0;}

      if(analogRead(pTX) >= 512+deadzone){CPacket.TX = constrain(map(analogRead(pTX),1023,512+deadzone,-1000,0),-1000,0);}
      else if(analogRead(pTX) <= 512-deadzone){CPacket.TX = constrain(map(analogRead(pTX),512-deadzone,0,0,1000),0,1000);}
      else {CPacket.TX = 0;}
      if(analogRead(pTY) >= 512+deadzone){CPacket.TY = constrain(map(analogRead(pTY),1023,512+deadzone,-1000,0),-1000,0);}
      else if(analogRead(pTY) <= 512-deadzone){CPacket.TY = constrain(map(analogRead(pTY),512-deadzone,0,0,1000),0,1000);}
      else {CPacket.TY = 0;}
      if(analogRead(pTZ) >= 512+deadzone){CPacket.TZ = constrain(map(analogRead(pTZ),1023,512+deadzone,-1000,0),-1000,0);}
      else if(analogRead(pTZ) <= 512-deadzone){CPacket.TZ = constrain(map(analogRead(pTZ),512-deadzone,0,0,1000),0,1000);}
      else {CPacket.TZ = 0;}

      break;
    case 1:
      CPacket.Throttle = constrain(map(analogRead(pTHROTTLE),1023-deadzonethrottle,0+deadzonethrottle,0,1000),0,1000);
      CPacket.WheelThrottle = constrain(map(analogRead(pTHROTTLE),1023-deadzonethrottle,0+deadzonethrottle,0,1000),0,1000);
      
      if(analogRead(pRX) >= 512+deadzone){CPacket.Roll = constrain(map(analogRead(pRX),512+deadzone,1023,0,1000),0,1000);}
      else if(analogRead(pRX) <= 512-deadzone){CPacket.Roll = constrain(map(analogRead(pRX),0,512-deadzone,-1000,0),-1000,0);}
      else {CPacket.Roll = 0;}
      if(analogRead(pRY) >= 512+deadzone){CPacket.Pitch = constrain(map(analogRead(pRY),512+deadzone,1023,0,1000),0,1000);}
      else if(analogRead(pRY) <= 512-deadzone){CPacket.Pitch = constrain(map(analogRead(pRY),0,512-deadzone,-1000,0),-1000,0);}
      else {CPacket.Pitch = 0;}
      if(analogRead(pRZ) >= 512+deadzone){CPacket.Yaw = constrain(map(analogRead(pRZ),512+deadzone,1023,0,1000),0,1000);
      CPacket.WheelSteer = constrain(map(analogRead(pRZ),1023,512+deadzone,-1000,0),-1000,0);}
      else if(analogRead(pRZ) <= 512-deadzone){CPacket.Yaw = constrain(map(analogRead(pRZ),0,512-deadzone,-1000,0),-1000,0);
      CPacket.WheelSteer = constrain(map(analogRead(pRZ),512-deadzone,0,0,1000),0,1000);}
      else {CPacket.Yaw = 0;
      CPacket.WheelSteer = 0;}

      if(analogRead(pTX) >= 512+deadzone){CPacket.TX = constrain(map(analogRead(pTX),1023,512+deadzone,-1000,0),-1000,0);}
      else if(analogRead(pTX) <= 512-deadzone){CPacket.TX = constrain(map(analogRead(pTX),512-deadzone,0,0,1000),0,1000);}
      else {CPacket.TX = 0;}
      if(analogRead(pTY) >= 512+deadzone){CPacket.TY = constrain(map(analogRead(pTY),1023,512+deadzone,-1000,0),-1000,0);}
      else if(analogRead(pTY) <= 512-deadzone){CPacket.TY = constrain(map(analogRead(pTY),512-deadzone,0,0,1000),0,1000);}
      else {CPacket.TY = 0;}
      if(analogRead(pTZ) >= 512+deadzone){CPacket.TZ = constrain(map(analogRead(pTZ),1023,512+deadzone,-1000,0),-1000,0);}
      else if(analogRead(pTZ) <= 512-deadzone){CPacket.TZ = constrain(map(analogRead(pTZ),512-deadzone,0,0,1000),0,1000);}
      else {CPacket.TZ = 0;}
    
      break;
    case 2:
      CPacket.Throttle = constrain(map(analogRead(pTHROTTLE),1023-deadzonethrottle,0+deadzonethrottle,0,1000),0,1000);
      CPacket.WheelThrottle = constrain(map(analogRead(pTHROTTLE),1023-deadzonethrottle,0+deadzonethrottle,0,1000),0,1000);
      
      if(analogRead(pRX) >= 512+deadzone){CPacket.Yaw = constrain(map(analogRead(pRX),512+deadzone,1023,0,500),0,500);
      CPacket.WheelSteer = constrain(map(analogRead(pRX),1023,512+deadzone,-500,0),-500,0);}
      else if(analogRead(pRX) <= 512-deadzone){CPacket.Yaw = constrain(map(analogRead(pRX),0,512-deadzone,-500,0),-500,0);
      CPacket.WheelSteer = constrain(map(analogRead(pRX),512-deadzone,0,0,500),0,500);}
      else {CPacket.Yaw = 0;
      CPacket.WheelSteer = 0;}
      if(analogRead(pRY) >= 512+deadzone){CPacket.Pitch = constrain(map(analogRead(pRY),512+deadzone,1023,0,500),0,500);}
      else if(analogRead(pRY) <= 512-deadzone){CPacket.Pitch = constrain(map(analogRead(pRY),0,512-deadzone,-500,0),-500,0);}
      else {CPacket.Pitch = 0;}
      if(analogRead(pRZ) >= 512+deadzone){CPacket.Roll = constrain(map(analogRead(pRZ),512+deadzone,1023,0,500),0,500);}
      else if(analogRead(pRZ) <= 512-deadzone){CPacket.Roll = constrain(map(analogRead(pRZ),0,512-deadzone,-500,0),-500,0);}
      else {CPacket.Roll = 0;}

      if(analogRead(pTX) >= 512+deadzone){CPacket.TX = constrain(map(analogRead(pTX),1023,512+deadzone,-500,0),-500,0);}
      else if(analogRead(pTX) <= 512-deadzone){CPacket.TX = constrain(map(analogRead(pTX),512-deadzone,0,0,500),0,500);}
      else {CPacket.TX = 0;}
      if(analogRead(pTY) >= 512+deadzone){CPacket.TY = constrain(map(analogRead(pTY),1023,512+deadzone,-500,0),-500,0);}
      else if(analogRead(pTY) <= 512-deadzone){CPacket.TY = constrain(map(analogRead(pTY),512-deadzone,0,0,500),0,500);}
      else {CPacket.TY = 0;}
      if(analogRead(pTZ) >= 512+deadzone){CPacket.TZ = constrain(map(analogRead(pTZ),1023,512+deadzone,-500,0),-500,0);}
      else if(analogRead(pTZ) <= 512-deadzone){CPacket.TZ = constrain(map(analogRead(pTZ),512-deadzone,0,0,500),0,500);}
      else {CPacket.TZ = 0;}
    
      break;
    case 3:
      CPacket.Throttle = constrain(map(analogRead(pTHROTTLE),1023-deadzonethrottle,0+deadzonethrottle,0,1000),0,1000);
      CPacket.WheelThrottle = constrain(map(analogRead(pTHROTTLE),1023-deadzonethrottle,0+deadzonethrottle,0,1000),0,1000);
      
      if(analogRead(pRX) >= 512+deadzone){CPacket.Roll = constrain(map(analogRead(pRX),512+deadzone,1023,0,500),0,500);}
      else if(analogRead(pRX) <= 512-deadzone){CPacket.Roll = constrain(map(analogRead(pRX),0,512-deadzone,-500,0),-500,0);}
      else {CPacket.Roll = 0;}
      if(analogRead(pRY) >= 512+deadzone){CPacket.Pitch = constrain(map(analogRead(pRY),512+deadzone,1023,0,500),0,500);}
      else if(analogRead(pRY) <= 512-deadzone){CPacket.Pitch = constrain(map(analogRead(pRY),0,512-deadzone,-500,0),-500,0);}
      else {CPacket.Pitch = 0;}
      if(analogRead(pRZ) >= 512+deadzone){CPacket.Yaw = constrain(map(analogRead(pRZ),512+deadzone,1023,0,500),0,500);
      CPacket.WheelSteer = constrain(map(analogRead(pRZ),1023,512+deadzone,-500,0),-500,0);}
      else if(analogRead(pRZ) <= 512-deadzone){CPacket.Yaw = constrain(map(analogRead(pRZ),0,512-deadzone,-500,0),-500,0);
      CPacket.WheelSteer = constrain(map(analogRead(pRZ),512-deadzone,0,0,500),0,500);}
      else {CPacket.Yaw = 0;
      CPacket.WheelSteer = 0;}

      if(analogRead(pTX) >= 512+deadzone){CPacket.TX = constrain(map(analogRead(pTX),1023,512+deadzone,-500,0),-500,0);}
      else if(analogRead(pTX) <= 512-deadzone){CPacket.TX = constrain(map(analogRead(pTX),512-deadzone,0,0,500),0,500);}
      else {CPacket.TX = 0;}
      if(analogRead(pTY) >= 512+deadzone){CPacket.TY = constrain(map(analogRead(pTY),1023,512+deadzone,-500,0),-500,0);}
      else if(analogRead(pTY) <= 512-deadzone){CPacket.TY = constrain(map(analogRead(pTY),512-deadzone,0,0,500),0,500);}
      else {CPacket.TY = 0;}
      if(analogRead(pTZ) >= 512+deadzone){CPacket.TZ = constrain(map(analogRead(pTZ),1023,512+deadzone,-500,0),-500,0);}
      else if(analogRead(pTZ) <= 512-deadzone){CPacket.TZ = constrain(map(analogRead(pTZ),512-deadzone,0,0,500),0,500);}
      else {CPacket.TZ = 0;}
    
      break;
    }
    //send the control packet to the KSPSerialIO plugin
    KSPBoardSendData(details(CPacket)); 
  }
}

void motorfader() {
if(!digitalRead(maxthrottle)){
  throttletarget = 0;
  digitalWrite(motoron, HIGH);
  }
else if(!digitalRead(minthrottle)){
  throttletarget = 1020;
  digitalWrite(motoron, HIGH);
  }
else {
  throttletarget = pTHROTTLE;
  digitalWrite(motoron, LOW);
  }
  
    if (analogRead(pTHROTTLE) > (throttletarget + theThreshold) ) {
      analogWrite(motordown, 0);
      analogWrite(motorup, 255);
    }
    else if (analogRead(pTHROTTLE) < (throttletarget - theThreshold) ) {
      analogWrite(motordown, 255);
      analogWrite(motorup, 0);     
    }
    else {
      analogWrite(motordown, 0);
      analogWrite(motorup, 0);
    }
}
  

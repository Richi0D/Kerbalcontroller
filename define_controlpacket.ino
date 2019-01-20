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
    if(!digitalRead(pSTAGE) && digitalRead(pARM)){MainControls(STAGE, true);} else {MainControls(STAGE, false);}

    
    //toggle buttons
    if(!digitalRead(pLIGHTS)){MainControls(LIGHTS, !lights_on);}
    if(!digitalRead(pGEARS)){MainControls(GEARS, !gears_on);}
    if(!digitalRead(pBRAKES)){MainControls(BRAKES, !brakes_on);}
    if(!digitalRead(pACTION1)){ControlGroups(1, !action1_on);}
    if(!digitalRead(pACTION2)){ControlGroups(2, !action2_on);}
    if(!digitalRead(pACTION3)){ControlGroups(3, !action3_on);}
    if(!digitalRead(pACTION4)){ControlGroups(4, !action4_on);}
    if(!digitalRead(pACTION5)){ControlGroups(5, !action5_on);} 
    if(!digitalRead(pACTION6)){ControlGroups(6, !action6_on);}      
    if(!digitalRead(pLADDER)){ControlGroups(7, !ladder_on);}
    if(!digitalRead(pCHUTES)){ControlGroups(8, !chutes_on);}    
    if(!digitalRead(pSOLAR)){ControlGroups(9, !solar_on);}


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
        
    switch(flymode){
    case 0:
      CPacket.Throttle = constrain(map(analogRead(pTHROTTLE),1000,20,0,1023),0,1000);
      
      if(analogRead(pRX) >= 550){CPacket.Yaw = constrain(map(analogRead(pRX),550,1000,0,1000),0,1000);}
      else if(analogRead(pRX) <= 450){CPacket.Yaw = constrain(map(analogRead(pRX),20,450,-1000,0),-1000,0);}
      else {CPacket.Yaw = 0;}
      if(analogRead(pRY) >= 550){CPacket.Pitch = constrain(map(analogRead(pRY),550,1000,0,1000),0,1000);}
      else if(analogRead(pRY) <= 450){CPacket.Pitch = constrain(map(analogRead(pRY),20,450,-1000,0),-1000,0);}
      else {CPacket.Pitch = 0;}
      if(analogRead(pRZ) >= 550){CPacket.Roll = constrain(map(analogRead(pRZ),550,1000,0,1000),0,1000);}
      else if(analogRead(pRZ) <= 450){CPacket.Roll = constrain(map(analogRead(pRZ),20,450,-1000,0),-1000,0);}
      else {CPacket.Roll = 0;}

      if(analogRead(pTX) >= 550){CPacket.TX = constrain(map(analogRead(pTX),1000,550,-1000,0),-1000,0);}
      else if(analogRead(pTX) <= 450){CPacket.TX = constrain(map(analogRead(pTX),450,20,0,1000),0,1000);}
      else {CPacket.TX = 0;}
      if(analogRead(pTY) >= 550){CPacket.TY = constrain(map(analogRead(pTY),1000,550,-1000,0),-1000,0);}
      else if(analogRead(pTY) <= 450){CPacket.TY = constrain(map(analogRead(pTY),450,20,0,1000),0,1000);}
      else {CPacket.TY = 0;}
      if(analogRead(pTZ) >= 550){CPacket.TZ = constrain(map(analogRead(pTZ),1000,550,-1000,0),-1000,0);}
      else if(analogRead(pTZ) <= 450){CPacket.TZ = constrain(map(analogRead(pTZ),450,20,0,1000),0,1000);}
      else {CPacket.TZ = 0;}

      break;
    case 1:
      CPacket.Throttle = constrain(map(analogRead(pTHROTTLE),1000,20,0,1023),0,1000);
      
      if(analogRead(pRX) >= 550){CPacket.Roll = constrain(map(analogRead(pRX),550,1000,0,1000),0,1000);}
      else if(analogRead(pRX) <= 450){CPacket.Roll = constrain(map(analogRead(pRX),20,450,-1000,0),-1000,0);}
      else {CPacket.Roll = 0;}
      if(analogRead(pRY) >= 550){CPacket.Pitch = constrain(map(analogRead(pRY),550,1000,0,1000),0,1000);}
      else if(analogRead(pRY) <= 450){CPacket.Pitch = constrain(map(analogRead(pRY),20,450,-1000,0),-1000,0);}
      else {CPacket.Pitch = 0;}
      if(analogRead(pRZ) >= 550){CPacket.Yaw = constrain(map(analogRead(pRZ),550,1000,0,1000),0,1000);}
      else if(analogRead(pRZ) <= 450){CPacket.Yaw = constrain(map(analogRead(pRZ),20,450,-1000,0),-1000,0);}
      else {CPacket.Yaw = 0;}

      if(analogRead(pTX) >= 550){CPacket.TX = constrain(map(analogRead(pTX),1000,550,-1000,0),-1000,0);}
      else if(analogRead(pTX) <= 450){CPacket.TX = constrain(map(analogRead(pTX),450,20,0,1000),0,1000);}
      else {CPacket.TX = 0;}
      if(analogRead(pTY) >= 550){CPacket.TY = constrain(map(analogRead(pTY),1000,550,-1000,0),-1000,0);}
      else if(analogRead(pTY) <= 450){CPacket.TY = constrain(map(analogRead(pTY),450,20,0,1000),0,1000);}
      else {CPacket.TY = 0;}
      if(analogRead(pTZ) >= 550){CPacket.TZ = constrain(map(analogRead(pTZ),1000,550,-1000,0),-1000,0);}
      else if(analogRead(pTZ) <= 450){CPacket.TZ = constrain(map(analogRead(pTZ),450,20,0,1000),0,1000);}
      else {CPacket.TZ = 0;}
    
      break;
    case 2:
      CPacket.Throttle = constrain(map(analogRead(pTHROTTLE),1000,20,0,500),0,500);
      
      if(analogRead(pRX) >= 550){CPacket.Yaw = constrain(map(analogRead(pRX),550,1000,0,200),0,200);}
      else if(analogRead(pRX) <= 450){CPacket.Yaw = constrain(map(analogRead(pRX),20,450,-200,0),-200,0);}
      else {CPacket.Yaw = 0;}
      if(analogRead(pRY) >= 550){CPacket.Pitch = constrain(map(analogRead(pRY),550,1000,0,200),0,200);}
      else if(analogRead(pRY) <= 450){CPacket.Pitch = constrain(map(analogRead(pRY),20,450,-200,0),-200,0);}
      else {CPacket.Pitch = 0;}
      if(analogRead(pRZ) >= 550){CPacket.Roll = constrain(map(analogRead(pRZ),550,1000,0,200),0,200);}
      else if(analogRead(pRZ) <= 450){CPacket.Roll = constrain(map(analogRead(pRZ),20,450,-200,0),-200,0);}
      else {CPacket.Roll = 0;}

      if(analogRead(pTX) >= 550){CPacket.TX = constrain(map(analogRead(pTX),1000,550,-200,0),-200,-0);}
      else if(analogRead(pTX) <= 450){CPacket.TX = constrain(map(analogRead(pTX),450,20,0,200),0,200);}
      else {CPacket.TX = 0;}
      if(analogRead(pTY) >= 550){CPacket.TY = constrain(map(analogRead(pTY),1000,550,-200,0),-200,-0);}
      else if(analogRead(pTY) <= 450){CPacket.TY = constrain(map(analogRead(pTY),450,20,0,200),0,200);}
      else {CPacket.TY = 0;}
      if(analogRead(pTZ) >= 550){CPacket.TZ = constrain(map(analogRead(pTZ),1000,550,-200,0),-200,0);}
      else if(analogRead(pTZ) <= 450){CPacket.TZ = constrain(map(analogRead(pTZ),450,20,0,200),0,200);}
      else {CPacket.TZ = 0;}
    
      break;
    case 3:
      CPacket.Throttle = constrain(map(analogRead(pTHROTTLE),1000,20,0,500),0,500);
      
      if(analogRead(pRX) >= 550){CPacket.Roll = constrain(map(analogRead(pRX),550,1000,0,-200),0,-200);}
      else if(analogRead(pRX) <= 450){CPacket.Roll = constrain(map(analogRead(pRX),20,450,200,0),200,0);}
      else {CPacket.Roll = 0;}
      if(analogRead(pRY) >= 550){CPacket.Pitch = constrain(map(analogRead(pRY),550,1000,0,-200),0,-200);}
      else if(analogRead(pRY) <= 450){CPacket.Pitch = constrain(map(analogRead(pRY),20,450,200,0),200,0);}
      else {CPacket.Pitch = 0;}
      if(analogRead(pRZ) >= 550){CPacket.Yaw = constrain(map(analogRead(pRZ),550,1000,0,200),0,200);}
      else if(analogRead(pRZ) <= 450){CPacket.Yaw = constrain(map(analogRead(pRZ),20,450,-200,0),-200,0);}
      else {CPacket.Yaw = 0;}

      if(analogRead(pTX) >= 550){CPacket.TX = constrain(map(analogRead(pTX),1000,550,-200,0),-200,-0);}
      else if(analogRead(pTX) <= 450){CPacket.TX = constrain(map(analogRead(pTX),450,20,0,200),0,200);}
      else {CPacket.TX = 0;}
      if(analogRead(pTY) >= 550){CPacket.TY = constrain(map(analogRead(pTY),1000,550,-200,0),-200,-0);}
      else if(analogRead(pTY) <= 450){CPacket.TY = constrain(map(analogRead(pTY),450,20,0,200),0,200);}
      else {CPacket.TY = 0;}
      if(analogRead(pTZ) >= 550){CPacket.TZ = constrain(map(analogRead(pTZ),1000,550,-200,0),-200,0);}
      else if(analogRead(pTZ) <= 450){CPacket.TZ = constrain(map(analogRead(pTZ),450,20,0,200),0,200);}
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
  
/*

//----------------------TEST--------------------------------

// Variables will change:
byte buttonState;             // the current reading from the input pin
byte lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are unsigned long's because the time, measured
// in miliseconds, will quickly become a bigger number than can be stored
// in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin
                                     // was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase
                                     // if the output flickers






void sendButton()
{
  // Read the state of the switch into a local variable.
  byte reading = digitalRead(pSTAGE);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // If the new button state is HIGH, that means the button
      // has just been pressed.
      if (buttonState == LOW) {
        // Send a message to the plugin activating the Stage
        // action group. The plugin will then activate the
        // next stage.
        if (PageDisplay1>5){ //for testing
          PageDisplay1=-1; 
        }
        //if (PageDisplay2>2){ //for testing
        //PageDisplay2=-1; 
        //}
      PageDisplay1++;
      sendToDisplay1(String("page ") + String(PageDisplay1));
      //PageDisplay2++;
      //sendToDisplay1(String("page ") + String(PageDisplay2));  
      }
    }
  }

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;
}*/

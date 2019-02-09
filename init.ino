void controlsInit() {
  pinMode(pTHROTTLE, INPUT);
  pinMode(pTX, INPUT);
  pinMode(pTY, INPUT);
  pinMode(pTZ, INPUT);
  pinMode(pRX, INPUT);
  pinMode(pRY, INPUT);
  pinMode(pRZ, INPUT);
  
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(maxthrottle, INPUT_PULLUP);
  pinMode(minthrottle, INPUT_PULLUP); 
  pinMode(motoron, OUTPUT);
  pinMode(motorup, OUTPUT);
  pinMode(motordown, OUTPUT);    
   
  pinMode(pMODE, INPUT_PULLUP);
  pinMode(pABORT, INPUT);
  pinMode(pARM, INPUT);
  //pinMode(pSTAGE, INPUT_PULLUP);
  pinMode(pSAS, INPUT_PULLUP);
  pinMode(pRCS, INPUT_PULLUP);
  pinMode(pprecision, INPUT_PULLUP);  
  //pinMode(pLIGHTS, INPUT_PULLUP);
  pinMode(pLIGHTSLED, OUTPUT);
  //pinMode(pBRAKES, INPUT_PULLUP);
  pinMode(pBRAKESLED, OUTPUT);
  //pinMode(pGEARS, INPUT_PULLUP);
  pinMode(pGEARSLED, OUTPUT);
  //pinMode(pSOLAR, INPUT_PULLUP);
  pinMode(pSOLARLED, OUTPUT);      
  //pinMode(pLADDER, INPUT_PULLUP);
  pinMode(pLADDERLED, OUTPUT);
  //pinMode(pCHUTES, INPUT_PULLUP);
  pinMode(pCHUTESLED, OUTPUT);
  //pinMode(pACTION6, INPUT_PULLUP);
  pinMode(pACTION6LED, OUTPUT);
  //pinMode(pACTION5, INPUT_PULLUP);
  pinMode(pACTION5LED, OUTPUT);
  //pinMode(pACTION4, INPUT_PULLUP);
  pinMode(pACTION4LED, OUTPUT);
  //pinMode(pACTION3, INPUT_PULLUP);
  pinMode(pACTION3LED, OUTPUT);
  //pinMode(pACTION2, INPUT_PULLUP);
  pinMode(pACTION2LED, OUTPUT);
  //pinMode(pACTION1, INPUT_PULLUP);
  pinMode(pACTION1LED, OUTPUT);
  pinMode(pswitch1, INPUT_PULLUP);
  pinMode(pswitch2, INPUT_PULLUP);
  pinMode(pswitch3, INPUT_PULLUP);
  pinMode(pswitch4, INPUT_PULLUP);
  pinMode(pmodeswitch, INPUT_PULLUP);
  
//setting up bouncers
  debouncerStage.begin();
  debouncerLights.begin();
  debouncerBrakes.begin();
  debouncerGears.begin();
  debouncerSolar.begin();
  debouncerChutes.begin();
  debouncerLadder.begin();
  debouncerA6.begin();
  debouncerA5.begin();
  debouncerA4.begin();
  debouncerA3.begin();
  debouncerA2.begin();
  debouncerA1.begin(); 
}

void testLEDS(int testdelay){
  digitalWrite(pLIGHTSLED,HIGH);
  digitalWrite(pBRAKESLED,HIGH);
  digitalWrite(pGEARSLED,HIGH);
  AtmosphereGauge(5);   //LED1
  delay(testdelay);
  digitalWrite(pLIGHTSLED,LOW);
  digitalWrite(pBRAKESLED,LOW);
  digitalWrite(pGEARSLED,LOW); 
  digitalWrite(pSOLARLED,HIGH);     
  digitalWrite(pLADDERLED,HIGH);
  digitalWrite(pCHUTESLED,HIGH);
  AtmosphereGauge(4);   //LED2   
  delay(testdelay);
  digitalWrite(pSOLARLED,LOW);  
  digitalWrite(pLADDERLED,LOW);
  digitalWrite(pCHUTESLED,LOW);
  digitalWrite(pACTION6LED,HIGH); 
  digitalWrite(pACTION5LED,HIGH);   
  digitalWrite(pACTION4LED,HIGH);
  AtmosphereGauge(2.5);   //LED3   
  delay(testdelay);
  digitalWrite(pACTION6LED,LOW);
  digitalWrite(pACTION5LED,LOW);
  digitalWrite(pACTION4LED,LOW);
  digitalWrite(pACTION3LED,HIGH);
  digitalWrite(pACTION2LED,HIGH);
  digitalWrite(pACTION1LED,HIGH);
  AtmosphereGauge(1.5);   //LED4      
  delay(testdelay);
  digitalWrite(pACTION3LED,LOW);
  digitalWrite(pACTION2LED,LOW);
  digitalWrite(pACTION1LED,LOW);
  AtmosphereGauge(1);   //LED5
  delay(testdelay);  
  AtmosphereGauge(0.8); //LED6     
  delay(testdelay);  
  AtmosphereGauge(0.5); //LED7 
  delay(testdelay);  
  AtmosphereGauge(0.3); //LED8
  delay(testdelay);  
  AtmosphereGauge(0.2); //LED9
  delay(testdelay);  
  AtmosphereGauge(0.1); //LED10
  delay(testdelay);  
  AtmosphereGauge(0.05); //LED11
  delay(testdelay);  
  AtmosphereGauge(0.02); //LED12
  delay(testdelay);  
  AtmosphereGauge(10); //LED12              
}

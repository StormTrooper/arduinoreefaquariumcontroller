
void printButton(char* text, int x1, int y1, int x2, int y2, boolean fontsize = false)
{
  int stl = strlen(text);
  int fx, fy;
  
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (x1, y1, x2, y2);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
 
  myGLCD.setBackColor(0, 0, 255);
  if (fontsize) {
    myGLCD.setFont(BigFont); 
    fx = x1+(((x2 - x1+1)-(stl*16))/2);
    fy = y1+(((y2 - y1+1)-16)/2);
    myGLCD.print(text, fx, fy);
    }
  else {
    myGLCD.setFont(SmallFont); 
    fx = x1+(((x2 - x1)-(stl*8))/2);
    fy = y1+(((y2 - y1-1)-8)/2);
    myGLCD.print(text, fx, fy);
    }
}

//-------------------------------------------------------------------------------------
//					MENU SCREEN ********** dispScreen = 1 ************************/
//-------------------------------------------------------------------------------------

void menuScreen()
{
  printHeader("Choose Option");
  
  myGLCD.setColor(64, 64, 64);
  myGLCD.drawRect(0, 196, 319, 194);
  printButton("CANCEL", canC[0], canC[1], canC[2], canC[3], SMALL);  
  
  printButton("Time and Date", tanD[0], tanD[1], tanD[2], tanD[3]);
  printButton("H2O Temp Control", temC[0], temC[1], temC[2], temC[3]);
 // printButton("WaveMaker", wave[0], wave[1], wave[2], wave[3]);
  printButton("General Settings", gSet[0], gSet[1], gSet[2], gSet[3]);    
  printButton("LED Testing", tesT[0], tesT[1], tesT[2], tesT[3]);
  printButton("Change LED Values", ledChM[0], ledChM[1], ledChM[2], ledChM[3]);
 // printButton("Automatic Feeder", aFeed[0], aFeed[1], aFeed[2], aFeed[3]);
  printButton("About", about[0], about[1], about[2], about[3]);  
}

//-------------------------------------------------------------------------------------
//			TIME and DATE SCREEN ********** dispScreen = 2 
//-------------------------------------------------------------------------------------

void clockScreen(boolean refreshAll=true) 
{
  if (refreshAll)
  {
   rtcSet = mytime;
   
   // for (int i=0; i<7; i++) {
   //rtcSet[i] = rtc[i];
    //}
    
    printHeader("Time and Date Settings");

    myGLCD.setColor(64, 64, 64);                   //Draw Dividers in Grey
    myGLCD.drawRect(0, 196, 319, 194);             //Bottom Horizontal Divider
    myGLCD.drawLine(0, 104, 319, 104);             //Middle Horizontal Divider
    printButton("<< BACK >>", back[0], back[1], back[2], back[3], SMALL);
    printButton("SAVE", prSAVE[0], prSAVE[1], prSAVE[2], prSAVE[3], SMALL);
    printButton("CANCEL", canC[0], canC[1], canC[2], canC[3], SMALL);

    printButton ("+", houU[0], houU[1], houU[2], houU[3], true);     //hour up
    printButton ("+", minU[0], minU[1], minU[2], minU[3], true);     //min up
    printButton ("-", houD[0], houD[1], houD[2], houD[3], true);     //hour down
    printButton ("-", minD[0], minD[1], minD[2], minD[3], true);     //min down
    if (setTimeFormat==1)
      { printButton ("+", ampmU[0], ampmU[1], ampmU[2], ampmU[3], true); //AM/PM up    
        printButton ("-", ampmD[0], ampmD[1], ampmD[2], ampmD[3], true);}//AM/PM down    
        
    printButton ("+", monU[0], monU[1], monU[2], monU[3], true);     //month up
    printButton ("+", dayU[0], dayU[1], dayU[2], dayU[3], true);     //day up
    printButton ("+", yeaU[0], yeaU[1], yeaU[2], yeaU[3], true);     //year up
    printButton ("-", monD[0], monD[1], monD[2], monD[3], true);     //month down
    printButton ("-", dayD[0], dayD[1], dayD[2], dayD[3], true);     //day down
    printButton ("-", yeaD[0], yeaD[1], yeaD[2], yeaD[3], true);     //year down
  }
  
  ReadFromEEPROM();
  timeDispH=rtcSet.hour; timeDispM=rtcSet.min; 
  xTimeH=107; yTime=52; xColon=xTimeH+42;
  xTimeM10=xTimeH+70; xTimeM1=xTimeH+86; xTimeAMPM=xTimeH+155;
  timeChange();

  setFont(LARGE, 255, 255, 255, 0, 0, 0);    
  myGLCD.print("Date", 20, 142);
  myGLCD.print("/", 149, 142);    
  myGLCD.print("/", 219, 142);
  if (setCalendarFormat==0)                             //DD/MM/YYYY Format
    {
     setFont(SMALL, 255, 255, 255, 0, 0, 0); 
     myGLCD.print("(DD/MM/YYYY)", 5, 160); 
     setFont(LARGE, 255, 255, 255, 0, 0, 0);          
     if ((rtcSet.date>=0) && (rtcSet.date<=9))              //Set DAY
       { myGLCD.print("0", 107, 142);
         myGLCD.printNumI(rtcSet.date, 123, 142);}
     else { myGLCD.printNumI(rtcSet.date, 107, 142);} 
     if ((rtcSet.mon>=0) && (rtcSet.mon<=9))              //Set MONTH
       { myGLCD.print("0", 177, 142);
         myGLCD.printNumI(rtcSet.mon, 193, 142);}
     else { myGLCD.printNumI(rtcSet.mon, 177, 142);} 
    } else
  if (setCalendarFormat==1)                             //MM/DD/YYYY Format
    {
     setFont(SMALL, 255, 255, 255, 0, 0, 0); 
     myGLCD.print("(MM/DD/YYYY)", 5, 160); 
     setFont(LARGE, 255, 255, 255, 0, 0, 0);          
     if ((rtcSet.mon>=0) && (rtcSet.mon<=9))              //Set MONTH
       { myGLCD.print("0", 107, 142);
         myGLCD.printNumI(rtcSet.mon, 123, 142);}
     else { myGLCD.printNumI(rtcSet.mon, 107, 142);} 
     if ((rtcSet.date>=0) && (rtcSet.date<=9))              //Set DAY
       { myGLCD.print("0", 177, 142);
         myGLCD.printNumI(rtcSet.date, 193, 142);}
     else { myGLCD.printNumI(rtcSet.date, 177, 142);} 
    }  
  myGLCD.printNumI(rtcSet.year, 247, 142);                //Set YEAR
}


void waitForIt(int x1, int y1, int x2, int y2)   // Draw a red frame while a button is touched
{
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
  while (myTouch.dataAvailable()) {
    myTouch.read(); 
  }
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
}

//-------------------------------------------------------------------------------------
//			H2O TEMP CONTROL SCREEN ********** dispScreen = 3
//-------------------------------------------------------------------------------------

void tempScreen(boolean refreshAll=false)
{
  String deg;
  if (refreshAll)
    {
     if ((setTempC==0) && (setTempScale==0)) {
       setTempC = 26.1;  }                         //change to 26.1 deg C
     if (((setTempF==0) || (setTempF==setTempC)) && (setTempScale==1)) {
       setTempF = 79.0;  }                         //change to 79.0 deg F
       
     if (setTempScale==1) {
       temp2beS = setTempF;
       temp2beO = offTempF;
       temp2beA = alarmTempF; }
     else {
     temp2beS = setTempC;
     temp2beO = offTempC;
     temp2beA = alarmTempC; }
     
     printHeader("H2O Temperature Control Settings");
     
     myGLCD.setColor(64, 64, 64);                    //Draw Dividers in Grey
     myGLCD.drawRect(0, 196, 319, 194);              //Bottom Horizontal Divider
     printButton("<< BACK >>", back[0], back[1], back[2], back[3], SMALL);
     printButton("SAVE", prSAVE[0], prSAVE[1], prSAVE[2], prSAVE[3], SMALL);
     printButton("CANCEL", canC[0], canC[1], canC[2], canC[3], SMALL);

     if (setTempScale==1) {deg ="F";}                //Print deg C or deg F
       else {deg = "C";}
     degC_F=deg;
     char bufferDeg[2];
     degC_F.toCharArray(bufferDeg,2);

     setFont(SMALL, 255, 255, 255, 0, 0, 0);
     myGLCD.print("Desired Temperature in", 60, 34);
     myGLCD.drawCircle(245, 36, 1);              
     myGLCD.print(bufferDeg, 250, 34);
     myGLCD.print(":",258,34);
     myGLCD.print("Temperature Offset:", CENTER, 84);
     myGLCD.print("Alarm Offset:", CENTER, 134);  
     
     printButton("-", temM[0], temM[1], temM[2], temM[3], true);      //temp minus
     printButton("+", temP[0], temP[1], temP[2], temP[3], true);      //temp plus
     printButton("-", offM[0], offM[1], offM[2], offM[3], true);      //offset minus
     printButton("+", offP[0], offP[1], offP[2], offP[3], true);      //offset plus
     printButton("-", almM[0], almM[1], almM[2], almM[3], true);      //alarm minus
     printButton("+", almP[0], almP[1], almP[2], almP[3], true);      //alarm plus
    }

  setFont(LARGE, 255, 255, 255, 0, 0, 0);
  myGLCD.printNumF(temp2beS, 1, CENTER, 54);
  myGLCD.printNumF(temp2beO, 1, CENTER, 104);
  myGLCD.printNumF(temp2beA, 1, CENTER, 154);
}


void timeChange()
{
  setFont(LARGE, 255, 255, 255, 0, 0, 0); 
  myGLCD.print("Time", 20, yTime);

 
  if (setTimeFormat==0)                                 //24HR Format
    { setFont(SMALL, 255, 255, 255, 0, 0, 0); 
      myGLCD.print("(24HR)", 29, yTime+18);}

     
    if (setTimeFormat==1)                                 //12HR Format
    { setFont(SMALL, 255, 255, 255, 0, 0, 0);  
      myGLCD.print("(12HR)", 29, yTime+18);}

  timeCorrectFormat();
}

void timeCorrectFormat()
{
  setFont(LARGE, 255, 255, 255, 0, 0, 0);   
  myGLCD.print(":",  xColon, yTime);  
  if (setTimeFormat==0)                                 //24HR Format
    {
     setFont(LARGE, 255, 255, 255, 0, 0, 0);  
     if ((timeDispH>=0) && (timeDispH<=9))              //Set HOUR
       { myGLCD.print("0", xTimeH, yTime);
         myGLCD.printNumI(timeDispH, xTimeH+16, yTime);}
     else { myGLCD.printNumI(timeDispH, xTimeH, yTime);} 
    }    
  if (setTimeFormat==1)                                 //12HR Format
    {
     setFont(LARGE, 255, 255, 255, 0, 0, 0);  
     if (timeDispH==0)                                  //Set HOUR
       { myGLCD.print("12", xTimeH, yTime);}
     if ((timeDispH>=1) && (timeDispH<=9))
       { myGLCD.print("0", xTimeH, yTime);
         myGLCD.printNumI(timeDispH, xTimeH+16, yTime);}
     if ((timeDispH>=10) && (timeDispH<=12))
       { myGLCD.printNumI(timeDispH, xTimeH, yTime);}
     if ((timeDispH>=13) && (timeDispH<=21))
       { myGLCD.print("0", xTimeH, yTime);
         myGLCD.printNumI(timeDispH-12, xTimeH+16, yTime);}
     if (timeDispH>=22)
       { myGLCD.printNumI(timeDispH-12, xTimeH, yTime);}
      
     if (AM_PM==1)
       { myGLCD.print("AM", xTimeAMPM, yTime); }
     if (AM_PM==2)
       { myGLCD.print("PM", xTimeAMPM, yTime); }
    }
  if ((timeDispM>=0) && (timeDispM<=9))                 //Set MINUTES
    { myGLCD.print("0", xTimeM10, yTime);
      myGLCD.printNumI(timeDispM, xTimeM1, yTime);}
  else { myGLCD.printNumI(timeDispM, xTimeM10, yTime);} 
}    


//-------------------------------------------------------------------------------------
//					LED TESTING OPTIONS SCREEN dispScreen = 4 
//-------------------------------------------------------------------------------------

void ledTestOptionsScreen()
{
  printHeader("LED Testing Options"); 

  myGLCD.setColor(64, 64, 64);                       //Draw Dividers in Grey
  myGLCD.drawRect(0, 196, 319, 194);                 //Bottom Horizontal Divider   
  printButton("<< BACK >>", back[0], back[1], back[2], back[3], SMALL);
  printButton("CANCEL", canC[0], canC[1], canC[2], canC[3], SMALL);
  
  printButton("Test LED Array Output", tstLA[0], tstLA[1], tstLA[2], tstLA[3]);
  printButton("Control Individual Leds", cntIL[0], cntIL[1], cntIL[2], cntIL[3]);
}

//-------------------------------------------------------------------------------------
//			TEST LED ARRAY SCREEN ************* dispScreen = 5 
//-------------------------------------------------------------------------------------

void testArrayScreen(boolean refreshAll=false)
{    
  if (refreshAll) 
    {  
     printHeader("Test LED Array Output Settings");
     myGLCD.fillRect (1, 15, 318, 37);       //clear "Test in Progress" Banner

     myGLCD.setColor(64, 64, 64);            //Draw Dividers in Grey
     myGLCD.drawRect(0, 196, 319, 194);      //Bottom Horizontal Divider
     printButton("<< BACK >>", back[0], back[1], back[2], back[3], SMALL);
     printButton("CANCEL", canC[0], canC[1], canC[2], canC[3], SMALL);     

     printButton ("", stsT[0], stsT[1], stsT[2], stsT[3], true);      //start/stop
     printButton ("-10s", tenM[0], tenM[1], tenM[2], tenM[3], true);  //-10s
     printButton ("+10s", tenP[0], tenP[1], tenP[2], tenP[3], true);  //+10s
     myGLCD.print("START", stsT[0]+6, stsT[1]+15);
     myGLCD.print("TEST", stsT[0]+15, stsT[1]+40);   
    } 
  else
    {
     min_cnt=0;
     
     myGLCD.setColor(0, 0, 0);
     myGLCD.fillRect (1, 15, 318, 99);       //clear test results if any
     myGLCD.fillRect (1, 187, 318, 227);     //clear the "Back" and "Cancel" Buttons
      
     myGLCD.setColor(0, 0, 255);
     myGLCD.fillRect(stsT[0]+5, stsT[1]+5, stsT[2]-5, stsT[3]-40);    //clear 'start'
     setFont(LARGE, 255, 255, 255, 0, 0, 255);
     myGLCD.print("STOP", stsT[0]+15, stsT[1]+15);
     
     myGLCD.setColor(255, 0, 0);
     myGLCD.fillRect (1, 15, 318, 37);
     myGLCD.drawRoundRect (stsT[0], stsT[1], stsT[2], stsT[3]); //red button during test
     setFont(LARGE, 255, 255, 255, 255, 0, 0);
     myGLCD.print("Test in Progress", CENTER, 16);
     setFont(SMALL, 255, 255, 255, 0, 0, 0);
     myGLCD.print("TIME:", 52, 40);
     myGLCD.print("LIGHT OUTPUT (0--255):", 140, 40);
     
   
     while (LEDtestTick)               //test LED and speed up time
     {
      unsigned long currentMillis = millis();
       
      if (myTouch.dataAvailable()) 
        { processMyTouch();}
     
      if (currentMillis - previousMillisLED > 500)    //change time every 0.5s
        {
         previousMillisLED = currentMillis;

         min_cnt++;
         String oldvalue, twelveHR, hrs, HOURS, hrsPM, mins, Minutes, AMPM;
         int hours = min_cnt/60;
         int minut = min_cnt%60;

         if (hours<12){AMPM="AM";}                              //Adding the AM/PM suffix
           else{AMPM="PM";}         

          Serial.println(hours);
         //check 
         //HOURS=hours;
         //hrsPM=hours-12;
         if (hours==0){hrs="12";}                             
           else { if ((hours>=1)&&(hours<=9)){hrs=" "+HOURS;}      //keep hours in place
             else { if ((hours>=13)&&(hours<=21)){hrs=" " +hrsPM;} //convert to 12HR
               else { if ((hours>=22)&&(hours<24)){hrs=hrsPM;}
                 else { if (hours==24){hrs="12";}   
                   else{hrs=HOURS;}}}}}

         //check 
         //mins=minut;                                            //add zero to minutes
         if ((minut>=0)&&(minut<=9)){Minutes="0"+mins;}
           else {Minutes=mins;}
      
         oldvalue=twelveHR;
         twelveHR=hrs+':'+Minutes;
         if ((oldvalue!=twelveHR)||refreshAll)
           {char bufferCount[9];
            twelveHR.toCharArray(bufferCount,9);
            setFont(LARGE, 255, 255, 255, 0, 0, 0);               
            myGLCD.print(bufferCount, 7, 55);
            char bufferAMPM[3];
            AMPM.toCharArray(bufferAMPM,3);
            myGLCD.print(bufferAMPM, 90, 55);
           }
                  
         setFont(SMALL, 0, 150, 0, 0, 0, 0);
         String sled = "SUMP:  " + String(sled_out) + " " + " ";
         char bufferS[11];
         sled.toCharArray(bufferS, 11);
         myGLCD.print(bufferS, 145, 55);

         setFont(SMALL, 255, 0, 0, 0, 0, 0);
         String rled = "Red:   " + String(rled_out) + "  " + " ";
         char bufferR[11];
         rled.toCharArray(bufferR, 11);
         myGLCD.print(bufferR, 145, 67);
         
         setFont(SMALL, 255, 255, 255, 0, 0, 0);
         String wled = "White: " + String(wled_out) + " " + " ";
         char bufferW[11];
         wled.toCharArray(bufferW, 11);
         myGLCD.print(bufferW, 145, 79);
         
         setFont(SMALL, 9, 184, 255, 0, 0, 0);
         String bled = "Blue:  " + String(bled_out) + " " + " ";      
         char bufferB[11];
         bled.toCharArray(bufferB, 11);
         myGLCD.print(bufferB, 235, 55);      
         
         setFont(SMALL, 58, 95, 205, 0, 0, 0);
         String rbled = "Royal: " + String(rbled_out) + "  " + " ";
         char bufferRB[11];
         rbled.toCharArray(bufferRB, 11);
         myGLCD.print(bufferRB, 235, 67);
        
         setFont(SMALL, 224, 102, 255, 0, 0, 0);
         String uvled = "Ultra: " + String(uvled_out) + "  " + " ";
         char bufferUV[11];
         uvled.toCharArray(bufferUV, 11);
         myGLCD.print(bufferUV, 235, 79);         
        
         LED_levels_output();
         checkTempC();
         TimeDateBar();
         }
      }
    } 
}

//-------------------------------------------------------------------------------------
//			TEST INDIVIDUAL LED(S) SCREEN ****** dispScreen = 6 
//-------------------------------------------------------------------------------------

void testIndLedScreen()
{
  printHeader("Test Individual LED Values (0--255)");
  
  setFont(SMALL, 255, 255, 255, 0, 0, 0);
  myGLCD.print("LED OUTPUT %", 52, 20);    
   
  drawBarGraph();
  myGLCD.setColor(255, 255, 255);   
  myGLCD.drawRect(30, 137, 165, 138);               //x-plane extended to include moon%
  myGLCD.setColor(176, 176, 176);
  myGLCD.drawRect(142, 136, 154, 135);              //MOON %bar place holder     

  myGLCD.setColor(0, 150, 0);
  myGLCD.fillRoundRect(5, 148, 105, 168);
  setFont(SMALL, 255, 255, 255, 0, 150, 0);      
  myGLCD.print("Sump:", 15, 152);                   //display SUMP LEDs output
  myGLCD.print("0", 67, 152);
   
  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRoundRect(5, 174, 105, 194);
  setFont(SMALL, 255, 255, 255, 255, 0, 0);   
  myGLCD.print("Red:", 15, 178);                    //display Red LEDs output   
  myGLCD.print("0", 67, 178);

  myGLCD.setColor(255, 255, 255);
  myGLCD.fillRoundRect(5, 200, 105, 220);
  setFont(SMALL, 0, 0, 0, 255, 255, 255);   
  myGLCD.print("White:", 15, 204);                  //display White LEDs output
  myGLCD.print("0", 67, 204);
   
  myGLCD.setColor(9, 184, 255);
  myGLCD.fillRoundRect(110, 148, 210, 168);
  setFont(SMALL, 255, 255, 255, 9, 184, 255);   
  myGLCD.print("Blue:", 120, 152);                  //display Blue LEDs output   
  myGLCD.print("0", 172, 152);

  myGLCD.setColor(58, 95, 205);
  myGLCD.fillRoundRect(110, 174, 210, 194);
  setFont(SMALL, 255, 255, 255, 58, 95, 205);
  myGLCD.print("Royal:", 120, 178);                 //display Royal Blue LEDs output
  myGLCD.print("0", 172, 178);

  myGLCD.setColor(224, 102, 255);
  myGLCD.fillRoundRect(110, 200, 210, 220);
  setFont(SMALL, 255, 255, 255, 224, 102, 255);   
  myGLCD.print("Ultra:", 120, 204);                 //display UltraViolet LEDs output   
  myGLCD.print("0", 172, 204);   
   
  myGLCD.setColor(176, 176, 176);  
  myGLCD.fillRoundRect(215, 148, 315, 168);
  setFont(SMALL, 0, 0, 0, 176, 176, 176);   
  myGLCD.print("Lunar:", 225, 152);                 //display Lunar LEDs output   
  myGLCD.print("0", 277, 152);
   
  myGLCD.setColor(0, 0, 0);  
  myGLCD.drawRoundRect(215, 174, 315, 194);         //blank button
   
  myGLCD.drawRoundRect(215, 200, 315, 220);
  setFont(SMALL, 255, 255, 255, 0, 0, 0);
  myGLCD.print("<< BACK >>", 227, 204);             //display "BACK" button
     
  myGLCD.setColor(255, 255, 255);                   //white Border Around Color Buttons
  for (int x=0; x<3; x++)
    { for (int y=0; y<3; y++)
        { myGLCD.drawRoundRect((x*105)+5, (y*26)+148, (x*105)+105, (y*26)+168); }
    }
   
  Rgad=0, Ggad=0, Bgad=0;
  Rfont=255, Gfont=255, Bfont=255,
  Rback=0, Gback=0, Bback=0,
  Rline=255, Gline=255, Bline=255;
  COLOR=0;
  ledChangerGadget();
  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect(204, 50, 280, 74);
  setFont(SMALL, 255, 0, 0, 0, 0, 0);
  myGLCD.print("SELECT", 220, 50);
  myGLCD.print("BELOW", 224, 62);
}

//-------------------------------------------------------------------------------------
//			SHOW LED COLOR CHOICES SCREEN ******* dispScreen = 7 
//-------------------------------------------------------------------------------------

void ledColorViewScreen()
{
  printHeader("Individual LED Outputs: Color Choices");

  myGLCD.setColor(64, 64, 64);                      //Draw Dividers in Grey
  myGLCD.drawRect(0, 196, 319, 194);                //Bottom Horizontal Divider
  printButton("<< BACK >>", back[0], back[1], back[2], back[3], SMALL);
  printButton("CANCEL", canC[0], canC[1], canC[2], canC[3], SMALL); 
  
  myGLCD.setColor(255, 255, 255);
  myGLCD.fillRoundRect(10, 20, 150, 50);
  setFont(SMALL, 0, 0, 0, 255, 255, 255);   
  myGLCD.print("White", 60, 29);
  
  myGLCD.setColor(58, 95, 205);
  myGLCD.fillRoundRect(10, 60, 150, 90);
  setFont(SMALL, 255, 255, 255, 58, 95, 205);
  myGLCD.print("Royal Blue", 40, 69);

  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRoundRect(10, 100, 150, 130);
  setFont(SMALL, 255, 255, 255, 255, 0, 0);   
  myGLCD.print("Red", 68, 109);

  myGLCD.setColor(176, 176, 176);  
  myGLCD.fillRoundRect(90, 140, 230, 170);
  setFont(SMALL, 0, 0, 0, 176, 176, 176);   
  myGLCD.print("Lunar", 140, 149);
  
  myGLCD.setColor(0, 150, 0);
  myGLCD.fillRoundRect(170, 100, 310, 130);
  setFont(SMALL, 255, 255, 255, 0, 150, 0);      
  myGLCD.print("Sump", 224, 109);
   
  myGLCD.setColor(224, 102, 255);
  myGLCD.fillRoundRect(170, 60, 310, 90);
  setFont(SMALL, 255, 255, 255, 224, 102, 255);   
  myGLCD.print("Ultraviolet", 196, 69);
   
  myGLCD.setColor(9, 184, 255);
  myGLCD.fillRoundRect(170, 20, 310, 50);
  setFont(SMALL, 255, 255, 255, 9, 184, 255);   
  myGLCD.print("Blue", 224, 29);

  myGLCD.setColor(255, 255, 255);
  for (int x=0; x<2; x++)
  { for (int y=0; y<3; y++)
      { myGLCD.drawRoundRect((x*160)+10, (y*40)+20, (x*160)+150, (y*40)+50); }
  }
  myGLCD.drawRoundRect(90, 140, 230, 170); 
}

//-------------------------------------------------------------------------------------
//			SHOW LED VALUES FOR CHOICE SCREEN ********* dispScreen = 8 
//-------------------------------------------------------------------------------------

void ledValuesScreen()
{
  int a;
  
  if (COLOR==1) {
    for (int i; i<96; i++)
      tled[i] = wled[i];
    printHeader("White LED Output Values");
    }
  if (COLOR==2) {
    for (int i; i<96; i++)
      tled[i] = bled[i];
    printHeader("Blue LED Output Values");
    }
  if (COLOR==3) {
    for (int i; i<96; i++)
      tled[i] = rbled[i];
    printHeader("Roayl Blue LED Output Values");
    }
  if (COLOR==4) {
    for (int i; i<96; i++)
      tled[i] = rled[i];
    printHeader("Red LED Output Values");
    }  
  if (COLOR==5) {
    for (int i; i<96; i++)
      tled[i] = uvled[i];
    printHeader("Ultraviolet LED Output Values");
    }
  if (COLOR==6) {
    for (int i; i<96; i++)
      tled[i] = sled[i];
    printHeader("Sump LED Output Values");
    }
  if (COLOR==7) {
    tMI=MI;
    printHeader("View/Change Moon LED Max Output");
    setFont(LARGE, 255, 255, 255, 0, 0, 0);
    myGLCD.print("Max Illumination", CENTER, 45);
    myGLCD.print("at Full Moon", CENTER, 65);    
    myGLCD.printNumI(tMI, CENTER, 120);
    setFont(SMALL, 255, 255, 255, 0, 0, 0);
    myGLCD.print("(0--255)", CENTER, 90);
    myGLCD.print("-1", 95, 145);        
    myGLCD.print("+5", 210, 145);    
    
    printButton("-", miM[0], miM[1], miM[2], miM[3], true);      //Max Illum. minus
    printButton("+", miP[0], miP[1], miP[2], miP[3], true);      //Max Illum. plus
    myGLCD.setColor(64, 64, 64);
    myGLCD.drawRect(0, 196, 319, 194);
    printButton("MORE COLORS", back[0], back[1], back[2], back[3], SMALL);
    printButton("SAVE", prSAVE[0], prSAVE[1], prSAVE[2], prSAVE[3], SMALL);
    printButton("CANCEL", canC[0], canC[1], canC[2], canC[3], SMALL);    
    }  
    
  if (COLOR!=7) {
    setFont(SMALL, 255, 255, 255, 0, 0, 0);
    for (int i=0; i<12; i++) 
    {
      myGLCD.setColor(0, 255, 255);
      myGLCD.printNumI((i*2), (i*26)+13, 14);
      myGLCD.printNumI(((i*2)+1), (i*26)+13, 24);
       for (int j=0; j<8; j++) 
       {
          a= (i*8)+j;         
          myGLCD.setColor(255, 255, 255);
          myGLCD.printNumI(tled[a], (i*26)+7, (j*18)+39);
          myGLCD.setColor(100, 100, 100);
          myGLCD.drawRect((i*26)+4, (j*18)+35, (i*26)+30, (j*18)+53);
       }
    }
    myGLCD.setColor(64, 64, 64);       
    myGLCD.drawRect(0, 196, 319, 194); 
    printButton("MORE COLORS", back[0], back[1], back[2], back[3], SMALL);   
    printButton("CHANGE", ledChV[0], ledChV[1], ledChV[2], ledChV[3], SMALL);
    printButton("SAVE", eeprom[0], eeprom[1], eeprom[2], eeprom[3]);
  }
}

//-------------------------------------------------------------------------------------
//			CHANGE LED VALUES SCREEN dispScreen = 9
//-------------------------------------------------------------------------------------

void ledChangeScreen()
{
  if (COLOR==1)
    { printHeader("Change White LED Output Values"); }
  if (COLOR==2)
    { printHeader("Change Blue LED Output Values"); }  
  if (COLOR==3)
    { printHeader("Change Royal Blue LED Output Values"); }
  if (COLOR==4)
    { printHeader("Change Red LED Output Values"); }      
  if (COLOR==5)
    { printHeader("Change Ultraviolet LED Output Values"); }  
  if (COLOR==6)
    { printHeader("Change Sump LED Output Values"); }
    
  myGLCD.setColor(64, 64, 64);       
  myGLCD.drawRect(0, 196, 319, 194); 
  printButton("MENU", back[0], back[1], back[2], back[3], SMALL);  
  printButton("OK", prSAVE[0], prSAVE[1], prSAVE[2], prSAVE[3], SMALL);
  printButton("CANCEL", canC[0], canC[1], canC[2], canC[3], SMALL);    

  setFont(SMALL, 0, 255, 255, 0,0,0);
  for (int i=0; i<12; i++) {
    myGLCD.setColor(0, 255, 255);
    myGLCD.printNumI((i*2), (i*26)+10, 22);
    myGLCD.printNumI(((i*2)+1), (i*26)+10, 33);
    myGLCD.setColor(100, 100, 100);
    myGLCD.drawRect((i*26)+4, 20 , (i*26)+30, 45);
    }
  
  for (int i=0; i<8; i++)
    {
     printButton("+", (i*38)+10, 70, (i*38)+35, 95, LARGE);
     printButton("-", (i*38)+10, 125, (i*38)+35, 150, LARGE);
    }
}

//-------------------------------------------------------------------------------------
//				GENERAL SETTINGS SCREEN dispScreen = 12
//-------------------------------------------------------------------------------------

void generalSettingsScreen()
{
  printHeader("View/Change General Settings");  

  myGLCD.setColor(64, 64, 64);
  myGLCD.drawRect(0, 196, 319, 194);
  for (int x=0; x<4; x++)
    { myGLCD.drawLine(0, (x*31)+70, 319, (x*31)+70); }

  printButton("<< BACK >>", back[0], back[1], back[2], back[3], SMALL);
  printButton("SAVE", prSAVE[0], prSAVE[1], prSAVE[2], prSAVE[3], SMALL);
  printButton("CANCEL", canC[0], canC[1], canC[2], canC[3], SMALL);   
    
  setFont(SMALL, 0, 255, 0, 0, 0, 0);
  myGLCD.print("Calendar Format", 25, 36);  
  myGLCD.print("Time Format", 25, 80);    
  myGLCD.print("Temperature Scale", 25, 111);  
  myGLCD.print("Screensaver", 25, 142);    
 // myGLCD.print("Auto-Stop on Feed", 25, 173);    

  genSetSelect();
}



//-------------------------------------------------------------------------------------
//					ABOUT SCREEN dispScreen = 15
//-------------------------------------------------------------------------------------
void AboutScreen()
{ 
  setFont(SMALL, 0, 255, 0, 0, 0 , 0);
  myGLCD.print("Version: 0.1", CENTER, 20);
  myGLCD.print("Written by Greg McCarthy", CENTER, 32);
  
  myGLCD.print("Main code based on Stilo", 5, 52);
  myGLCD.print("http://code.google.com/p/stilo/", 5, 64);
  
  myGLCD.print("LED controlling algorithm based on", 5, 84);
  myGLCD.print("Krusduino by Hugh Dangerfield", 5, 96);
  myGLCD.print("and Dave Rosser", 5, 108);
  myGLCD.print("http://code.google.com/p/dangerduino/", 5, 120);

  myGLCD.setColor(64, 64, 64);


  myGLCD.drawRect(0, 196, 319, 194);
  printButton("<< BACK >>", back[0], back[1], back[2], back[3], SMALL);
  printButton("CANCEL", canC[0], canC[1], canC[2], canC[3], SMALL);  

}




//-------------------------------------------------------------------------------------
//						 TOUCH SCREEN
//-------------------------------------------------------------------------------------

void processMyTouch()
{
  DEBUG_PRINTLN("Process MyTouch");
  myTouch.read();
  x=myTouch.getX();
  y=myTouch.getY();
  
  returnTimer=0;
  screenSaverTimer=0;  
    
  if ((x>=canC[0]) && (x<=canC[2]) && (y>=canC[1]) && (y<=canC[3])  //press cancel 
     && (dispScreen!=0) && (dispScreen!=5) && (dispScreen!=6) && (dispScreen!=8) 
     && (dispScreen!=11))
    {
     waitForIt(canC[0], canC[1], canC[2], canC[3]);
     LEDtestTick = false;
//     waveMakerOff = false;     
     ReadFromEEPROM();     
     dispScreen=0;
     clearScreen();
     mainScreen(true);
    }  else
  if ((x>=back[0]) && (x<=back[2]) && (y>=back[1]) && (y<=back[3])  //press back    
     && (dispScreen!=0) && (dispScreen!=1) && (dispScreen!=5) && (dispScreen!=6)
     && (dispScreen!=8) && (dispScreen!=11) && (dispScreen!=14))
    {
     waitForIt(back[0], back[1], back[2], back[3]);
     LEDtestTick = false;
 //    waveMakerOff = false;          
     ReadFromEEPROM();
     dispScreen=1;
     clearScreen();
     menuScreen();
    }      
    else
  {
  switch (dispScreen) {
  case 0:     //--------------- MAIN SCREEN (Press Any Key) ---------------
    dispScreen=1;
    clearScreen();
    menuScreen();
  break;
      
  case 1:     //--------------------- MENU SCREEN -------------------------
    if ((x>=tanD[0]) && (x<=tanD[2]))                      //first column
      {
       if ((y>=tanD[1]) && (y<=tanD[3]))                   //press Date & Clock Screen
         {
          waitForIt(tanD[0], tanD[1], tanD[2], tanD[3]);
          if ((timeDispH>=0) && (timeDispH<=11)) { AM_PM=1;}
          else { AM_PM=2;}          
          dispScreen=2;
          clearScreen();
          clockScreen();
         } 
       if ((y>=temC[1]) && (y<=temC[3]))                   //press H2O Temp Control
         {
          waitForIt(temC[0], temC[1], temC[2], temC[3]);
          ReadFromEEPROM();
          dispScreen=3;
          clearScreen();
          tempScreen(true); 
         }
//       if ((y>=wave[1]) && (y<=wave[3]))                   //press Wavemaker Screen
//         {
//          waitForIt(wave[0], wave[1], wave[2], wave[3]);
//          dispScreen=10;
//          clearScreen();
//         }
       if ((y>=gSet[1]) && (y<=gSet[3]))                   //press General Settings
         {
          waitForIt(gSet[0], gSet[1], gSet[2], gSet[3]);
          dispScreen=12;
          clearScreen();
          generalSettingsScreen();
         }
      }
    if ((x>=tesT[0]) && (x<=tesT[2]))                      //second column
      {
        if ((y>=tesT[1]) && (y<=tesT[3]))                  //press LED Testing page
         {
          waitForIt(tesT[0], tesT[1], tesT[2], tesT[3]);
          dispScreen=4;
          clearScreen();
          ledTestOptionsScreen();
         }  
        if  ((y>=ledChM[1]) && (y<=ledChM[3]))             //press Change LED values
         {
          waitForIt(ledChM[0], ledChM[1], ledChM[2], ledChM[3]);
          dispScreen=7;
          clearScreen();
          ledColorViewScreen(); 
         } 
//        if  ((y>=aFeed[1]) && (y<=aFeed[3]))               //press Automatic Feeder screen
//         {
//          waitForIt(aFeed[0], aFeed[1], aFeed[2], aFeed[3]);
//          dispScreen=13;
//          clearScreen();
//    //      autoFeederScreen();
//         }
        if  ((y>=about[1]) && (y<=about[3]))               //press About sketch
         {
          waitForIt(about[0], about[1], about[2], about[3]);
          dispScreen=15;
          clearScreen();
          AboutScreen(); 
         }         
      }
  break;
      
  case 2:     //--------------- CLOCK & DATE SETUP SCREEN -----------------
    if ((x>=prSAVE[0]) && (x<=prSAVE[2]) && (y>=prSAVE[1]) && (y<=prSAVE[3])) //press SAVE
      {
       waitForIt(prSAVE[0], prSAVE[1], prSAVE[2], prSAVE[3]);
       if (setTimeFormat==1)
         {
          if ((rtcSet.hour==0) && (AM_PM==2))
           { rtcSet.hour+=12;}
          if (((rtcSet.hour>=1) && (rtcSet.hour<=11)) && (AM_PM==2))
            { rtcSet.hour+=12;}  
          if (((rtcSet.hour>=12) && (rtcSet.hour<=23)) && (AM_PM==1))
            { rtcSet.hour-=12;}           
         }
       SaveRTC();
       dispScreen=0;
       clearScreen();
       mainScreen(true);
      }
    else
     {
      if ((y>=houU[1]) && (y<=houU[3]))                    //FIRST ROW
        {
         if ((x>=houU[0]) && (x<=houU[2]))                 //press hour up
           {
            waitForIt(houU[0], houU[1], houU[2], houU[3]);
            rtcSet.hour++;
            if (rtcSet.hour>=24) 
              { rtcSet.hour=0; }
           }
         if ((x>=minU[0]) && (x<=minU[2]))                 //press min up
           {
            waitForIt(minU[0], minU[1], minU[2], minU[3]);
            rtcSet.min++;
            if (rtcSet.min>=60) {rtcSet.min = 0; }
           }
         if ((x>=ampmU[0]) && (x<=ampmU[2]))               //press AMPM up
           {
            waitForIt(ampmU[0], ampmU[1], ampmU[2], ampmU[3]);
            if (AM_PM==1) {AM_PM=2;}
            else {AM_PM=1;}
           }
        }
      if ((y>=houD[1]) && (y<=houD[3]))                    //SECOND ROW
        {
         if ((x>=houD[0]) && (x<=houD[2]))                 //press hour down
           {
            waitForIt(houD[0], houD[1], houD[2], houD[3]);             
            rtcSet.hour--;
            if (rtcSet.hour<0) 
              { rtcSet.hour=23; }
           }
         if ((x>=minD[0]) && (x<=minD[2]))                 //press min down
           {
            waitForIt(minD[0], minD[1], minD[2], minD[3]);
            rtcSet.min--;
            if (rtcSet.min<0) {rtcSet.min = 59; } 
           }
         if ((x>=ampmD[0]) && (x<=ampmD[2]))               //press AMPM down
           {
            waitForIt(ampmD[0], ampmD[1], ampmD[2], ampmD[3]);
            if (AM_PM==1) {AM_PM=2;}
            else {AM_PM=1;}
           }           
        }
      if ((y>=dayU[1]) && (y<=dayU[3]))                    //THIRD ROW
        {
         if (setCalendarFormat==0)                         //DD/MM/YYYY Format
           {
            if ((x>=dayU[0]) && (x<=dayU[2]))              //press day up
              {
               waitForIt(dayU[0], dayU[1], dayU[2], dayU[3]);
               rtcSet.date++;
               if (rtcSet.date>31) {rtcSet.date = 1; }
              }
            if ((x>=monU[0]) && (x<=monU[2]))              //press month up
              {
               waitForIt(monU[0], monU[1], monU[2], monU[3]);
               rtcSet.mon++;
               if (rtcSet.mon>12) {rtcSet.mon = 1; }
              }
           } else {
         if (setCalendarFormat==1)                         //MM/DD/YYYY Format
           {
            if ((x>=dayU[0]) && (x<=dayU[2]))              //press month up
              {
               waitForIt(dayU[0], dayU[1], dayU[2], dayU[3]);
               rtcSet.mon++;
               if (rtcSet.mon>12) {rtcSet.mon = 1; }
              }
            if ((x>=monU[0]) && (x<=monU[2]))              //press day up
              {
               waitForIt(monU[0], monU[1], monU[2], monU[3]);
               rtcSet.date++;
               if (rtcSet.date>31) {rtcSet.date = 1; }
              }
           }      }
         if ((x>=yeaU[0]) && (x<=yeaU[2]))                 //press year up
           {
            waitForIt(yeaU[0], yeaU[1], yeaU[2], yeaU[3]);
            rtcSet.year++;
            if (rtcSet.year>2100) {rtcSet.year = 2000; }
           }
        }
      if ((y>=dayD[1]) && (y<=dayD[3]))                    //FOURTH ROW
        {
         if (setCalendarFormat==0)                         //DD/MM/YYYY Format
           {
            if ((x>=dayD[0]) && (x<=dayD[2]))              //press day down
              {
               waitForIt(dayD[0], dayD[1], dayD[2], dayD[3]);
               rtcSet.date--;
               if (rtcSet.date<1) {rtcSet.date = 31; }
              }
            if ((x>=monD[0]) && (x<=monD[2]))              //press month down
              {
               waitForIt(monD[0], monD[1], monD[2], monD[3]);
               rtcSet.mon--;
               if (rtcSet.mon<1) {rtcSet.mon = 12; }
              }
           } else {
         if (setCalendarFormat==1)                         //MM/DD/YYYY Format
           {
            if ((x>=dayD[0]) && (x<=dayD[2]))              //press month down
              {
               waitForIt(dayD[0], dayD[1], dayD[2], dayD[3]);
               rtcSet.mon--;
               if (rtcSet.mon<1) {rtcSet.mon = 12; }
              }
            if ((x>=monD[0]) && (x<=monD[2]))              //press day down
              {
               waitForIt(monD[0], monD[1], monD[2], monD[3]);
               rtcSet.date--;
               if (rtcSet.date<1) {rtcSet.date = 31; }
              }
           }      }   
         if ((x>=yeaD[0]) && (x<=yeaD[2]))                 //press year down
           {
            waitForIt(yeaD[0], yeaD[1], yeaD[2], yeaD[3]);
            rtcSet.year--;
            if (rtcSet.year<2000) {rtcSet.year = 2100; }
           }
        }
      clockScreen(false); 
     }       
  break;
  
  case 3:     //------------------ H20 TEMPERATURE CONTROL ---------------
    if ((x>=prSAVE[0]) && (x<=prSAVE[2]) && (y>=prSAVE[1]) && (y<=prSAVE[3])) //press SAVE
      {
       waitForIt(prSAVE[0], prSAVE[1], prSAVE[2], prSAVE[3]);
       setTempC = temp2beS;
       setTempF = temp2beS;
       offTempC = temp2beO;
       offTempF = temp2beO;
       alarmTempC = temp2beA;
       alarmTempF = temp2beA;
       if (setTempScale==0)                      //Celsius to Farenheit (Consistency Conversion)
         {
          setTempF=((1.8*setTempC)+32.05);
          offTempF=((1.8*offTempC)+0.05);
          alarmTempF=((1.8*alarmTempC)+0.05);
         }    
       if (setTempScale==1)                      //Farenheit to Celsius (Consistency Conversion)
         {
          setTempC=((.55556*(setTempF-32))+.05);
          offTempC=(.55556)*offTempF+.05;
          alarmTempC=(.55556)*alarmTempF+.05;
         }
       dispScreen=0;
       SaveTempToEEPROM();
       clearScreen();
       mainScreen(true);
       } else
         setFont(LARGE, 255, 255, 255, 0, 0, 0);
         {
          if ((x>=temM[0]) && (x<=temM[2]))                         //first column
            {
             if ((y>=temM[1]) && (y<=temM[3]))                      //press temp minus
               {
                waitForIt(temM[0], temM[1], temM[2], temM[3]);
                temp2beS -= 0.1;
                if ((setTempScale==1) && (temp2beS <= 50)){
                  temp2beS = 50; }
                if ((setTempScale==0) && (temp2beS <= 10)) {
                  temp2beS = 10; }
                  tempScreen();
               }
             if ((y>=offM[1]) && (y<=offM[3]))                      //press offset minus
               {
                waitForIt(offM[0], offM[1], offM[2], offM[3]);
                temp2beO -= 0.1;
                if (temp2beO < 0.1) {
                  temp2beO = 0.0; }
                  tempScreen();
               }          
             if ((y>=almM[1]) && (y<=almM[3]))                      //press alarm minus
               {
                waitForIt(almM[0], almM[1], almM[2], almM[3]);
                temp2beA -= 0.1;
                if (temp2beA < 0.1) {
                  temp2beA = 0.0;  }
                  tempScreen();
               }
            }
          if ((x>=temP[0]) && (x<=temP[2]))                         //second column
            {
             if ((y>=temP[1]) && (y<=temP[3]))                      //press temp plus
               {
                waitForIt(temP[0], temP[1], temP[2], temP[3]);
                temp2beS += 0.1;
                if ((setTempScale==1) && (temp2beS >= 104)){
                  temp2beS = 104; }
                if ((setTempScale==0) && (temp2beS >= 40)) {
                  temp2beS = 40; }
                  tempScreen();
               }
             if ((y>=offP[1]) && (y<=offP[3]))                      //press offset plus
               {
                waitForIt(offP[0], offP[1], offP[2], offP[3]);
                temp2beO += 0.1;
                if (temp2beO >= 10) {
                  temp2beO = 9.9; }
                 tempScreen();
               }
             if ((y>=almP[1]) && (y<=almP[3]))                      //press alarm plus
               {
                waitForIt(almP[0], almP[1], almP[2], almP[3]);
                temp2beA += 0.1;
                if (temp2beA >= 10) {
                  temp2beA = 9.9;  }
                tempScreen();
               }
            }
         
  break;
  
  case 4:     // -------------- LED TEST OPTIONS SCREEN -----------------    
    if ((x>=tstLA[0]) && (x<=tstLA[2]) && (y>=tstLA[1]) && (y<=tstLA[3]))   //Test LED Array Output
      {
       waitForIt(tstLA[0], tstLA[1], tstLA[2], tstLA[3]); 
       dispScreen=5;
       clearScreen();
       testArrayScreen(true);
      }  
    if ((x>=cntIL[0]) && (x<=cntIL[2]) && (y>=cntIL[1]) && (y<=cntIL[3]))   //Test Individual LEDs
      {
       waitForIt(cntIL[0], cntIL[1], cntIL[2], cntIL[3]); 
       dispScreen=6;
       clearScreen();
       testIndLedScreen();
       colorLEDtest = true;
      }         
  break;
        
  case 5:     //---------------- TEST LED ARRAY SCREEN ------------------
    if ((x>=back[0]) && (x<=back[2]) && (y>=back[1]) && (y<=back[3])   //press back    
       && (LEDtestTick==false))    
      {
       waitForIt(back[0], back[1], back[2], back[3]);
       LEDtestTick = false;
       ReadFromEEPROM();     
       dispScreen=4;
       clearScreen();
       ledTestOptionsScreen();
      }      
    if ((x>=canC[0]) && (x<=canC[2]) && (y>=canC[1]) && (y<=canC[3])   //press CANCEL
       && (LEDtestTick==false))    
      {
       waitForIt(canC[0], canC[1], canC[2], canC[3]);
       LEDtestTick = false;
       ReadFromEEPROM();     
       dispScreen=0;
       clearScreen();
       mainScreen(true);
      }  
    if ((x>=stsT[0]) && (x<=stsT[2]) && (y>=stsT[1]) && (y<=stsT[3]))  //press start/stop test
      {
       waitForIt(stsT[0], stsT[1], stsT[2], stsT[3]); 
           
       if (LEDtestTick) {
         LEDtestTick = false;
         testArrayScreen(true);
         }
         else {
           LEDtestTick = true;             
           testArrayScreen();
         }
      } else
          {
           if ((x>=tenM[0]) && (x<=tenM[2]) && (y>=tenM[1]) && (y<=tenM[3]))   //press -10s
             {
              min_cnt -= 10;
              if (min_cnt<0) {
                min_cnt= 0; }
              delay(50);
             }
           if ((x>=tenP[0]) && (x<=tenP[2]) && (y>=tenP[1]) && (y<=tenP[2]))   //press +10s
             {
              min_cnt += 10;
              if (min_cnt>1440) {
                min_cnt = 1440; }
              delay(50);
             }
          }
  break;
        
  case 6:     // --------------- TEST INDIVIDUAL LED SCREEN --------------    
    int CL_check, CL_check2;
    if ((x>=215) && (x<=315) && (y>=200) && (y<=220))          //press back    
      {
       waitForIt(215, 200, 315, 220);
       LEDtestTick = false;
       dispScreen=4;
       clearScreen();
       ledTestOptionsScreen();
       colorLEDtest = false;
      }
    if ((x>=5) && (x<=105))                                    //First Column of Colors
      {
       if ((y>=148) && (y<=168))                               //Press SUMP
         {
          waitForIt(5, 148, 105, 168);
          COLOR = 0;
          Rgad=0, Ggad=150, Bgad=0,
          Rfont=255, Gfont=255, Bfont=255,
          Rback=0, Gback=150, Bback=0,
          Rline=255, Gline=255, Bline=255;
          xValue=67 , yValue=152, x1Bar=40, x2Bar=x1Bar+12;
          COLOR = SUMP;
          ledChangerGadget();
         }
       if ((y>=174) && (y<=194))                               //press Red
         {
          waitForIt(5, 174, 105, 194);
          COLOR = 0;
          Rgad=255, Ggad=0, Bgad=0,
          Rfont=255, Gfont=255, Bfont=255,
          Rback=255, Gback=0, Bback=0,
          Rline=255, Gline=255, Bline=255;
          xValue=67 , yValue=178, x1Bar=57, x2Bar=x1Bar+12;
          COLOR = RED;
          ledChangerGadget();
         }
       if ((y>=200) && (y<=220))                               //press White
         {
          waitForIt(5, 200, 105, 220);
          COLOR = 0;
          Rgad=255, Ggad=255, Bgad=255,
          Rfont=0, Gfont=0, Bfont=0,
          Rback=255, Gback=255, Bback=255,
          Rline=0, Gline=0, Bline=0;
          xValue=67 , yValue=204, x1Bar=74, x2Bar=x1Bar+12;
          COLOR = WHITE;
          ledChangerGadget();
         }
      }

    if ((x>=110) && (x<=210))                                  //Second Column of Colors
      {
       if ((y>=148) && (y<=168))                               //Press Blue
         {
          waitForIt(110, 148, 210, 168);
          COLOR = 0;
          Rgad=9, Ggad=184, Bgad=255,
          Rfont=255, Gfont=255, Bfont=255,
          Rback=9, Gback=184, Bback=255,
          Rline=255, Gline=255, Bline=255;
          xValue=172 , yValue=152, x1Bar=91, x2Bar=x1Bar+12;
          COLOR = BLUE;
          ledChangerGadget();
         }
       if ((y>=174) && (y<=194))                               //press Royal Blue
         {
          waitForIt(110, 174, 210, 194);
          COLOR = 0;
          Rgad=58, Ggad=95, Bgad=205,
          Rfont=255, Gfont=255, Bfont=255,
          Rback=58, Gback=95, Bback=205,
          Rline=255, Gline=255, Bline=255;
          xValue=172 , yValue=178, x1Bar=108, x2Bar=x1Bar+12;
          COLOR = ROYAL;
          ledChangerGadget();
         }
       if ((y>=200) && (y<=220))                               //press Ultraviolet
         {
          waitForIt(110, 200, 210, 220);
          COLOR = 0;
          Rgad=224, Ggad=102, Bgad=255,
          Rfont=255, Gfont=255, Bfont=255,
          Rback=224, Gback=102, Bback=255,
          Rline=255, Gline=255, Bline=255;
          xValue=172 , yValue=204, x1Bar=125, x2Bar=x1Bar+12;
          COLOR = ULTRA;
          ledChangerGadget();
         }
      }
        
    if ((x>=215) && (x<=315) && (y>=148) && (y<=168))          //Press Moon
      {
       waitForIt(215, 148, 315, 168);
       COLOR = 0;
       Rgad=176, Ggad=176, Bgad=176,
       Rfont=0, Gfont=0, Bfont=0,
       Rback=176, Gback=176, Bback=176,
       Rline=0, Gline=0, Bline=0;
       xValue=277 , yValue=152, x1Bar=142, x2Bar=x1Bar+12;
       COLOR = MOON;
       ledChangerGadget();
      }
      
    if ((y>=78) && (y<=100))                                   //Plus buttons were touched
      {
       for (int i=0; i<3; i++) {                
         if ((x>=(i*27)+204) && (x<=(i*27)+226) && (COLOR!=0)) {
           waitForItSq((i*27)+204, 78, (i*27)+226, 100);
           if (i==0) { cl_100 += 1; }
           if (i==1) { cl_10  += 1; }
           if (i==2) { cl_1   += 1; }
           CL_check = (cl_10*10)+cl_1;
           if ((cl_100>2) && (i==0)) { cl_100=0; }
           if ((cl_100<2) && (cl_10>9) && (i==1)) { cl_10=0; }
           if ((cl_100>=2) && (cl_10>=5) && (i==1)) { cl_10=5; }
           if ((cl_100<2) && (cl_1>9) && (i==2)) { cl_1=0; }
           if ((cl_100>=2) && (cl_10<5) && (cl_1>9) && (i==2)) { cl_1=0; }
           if ((cl_100>=2) && (cl_10>=5) && (i==2)) { cl_1=5; }
           if ((CL_check>=55) && ((i==0) || (i==1)) && (cl_100>1)) 
             { cl_10=5; cl_1=5; 
               setFont(LARGE, 255, 255, 255, 0, 0, 0);
               myGLCD.printNumI(cl_10, 234, 54);
               myGLCD.printNumI(cl_1, 255, 54); }
           setFont(LARGE, 255, 255, 255, 0, 0, 0);
           if (i==0) { myGLCD.printNumI(cl_100, 214, 54);}
           if (i==1) { myGLCD.printNumI(cl_10, 234, 54);}
           if (i==2) { myGLCD.printNumI(cl_1, 255, 54);}
           CL_100 = cl_100*100;
           CL_10  = cl_10*10;
           CL_1   = cl_1;
           for (int c=0; c<3; c++)
             {
              myGLCD.setColor(Rline, Gline, Bline);
              myGLCD.drawRect((c*27)+204, 78, (c*27)+226, 100);
              myGLCD.drawRect((c*27)+204, 105, (c*27)+226, 127);
             }
           drawBarandColorValue();
           LED_levels_output();
           checkTempC();             
           }
         }
      }
    else if ((y>=105) && (y<=127))                             //Minus buttons were touched
      {
       for (int i=0; i<3; i++) {                
         if ((x>=(i*27)+204) && (x<=(i*27)+226) && (COLOR!=0)) {
           waitForItSq((i*27)+204, 105, (i*27)+226, 127);
           if (i==0) { cl_100 -= 1; }
           if (i==1) { cl_10  -= 1; }
           if (i==2) { cl_1   -= 1; }
           CL_check = (cl_10*10)+cl_1;
           CL_check2 = (cl_100*100)+cl_1;
           if ((cl_100<0) && (i==0)) { cl_100=2; }
           if ((cl_10<0) && (i==1) && (cl_100<2)) { cl_10=9; }
           if ((cl_10<0) && (i==1) && (cl_100==2)) { cl_10=5; }
           if ((cl_1<0) && (i==2) && (cl_100<2)) { cl_1=9; }
           if ((cl_1<0) && (i==2) && (cl_100==2) && (cl_10<5)) { cl_1=9; }
           if ((cl_1<0) && (i==2) && (cl_100==2) && (cl_10>=5)) { cl_1=5; }
           if ((CL_check>=55) && ((i==0) || (i==1)) && (cl_100>1)) 
             { cl_10=5; cl_1=5; 
               setFont(LARGE, 255, 255, 255, 0, 0, 0);
               myGLCD.printNumI(cl_10, 234, 54);
               myGLCD.printNumI(cl_1, 255, 54); }
           if ((CL_check2>205) && (i==1) && (cl_10<1)) 
             { cl_10=5; cl_1=5; 
               setFont(LARGE, 255, 255, 255, 0, 0, 0);
               myGLCD.printNumI(cl_10, 234, 54);
               myGLCD.printNumI(cl_1, 255, 54); }
           setFont(LARGE, 255, 255, 255, 0, 0, 0);
           if (i==0) { myGLCD.printNumI(cl_100, 214, 54);}
           if (i==1) { myGLCD.printNumI(cl_10, 234, 54);}
           if (i==2) { myGLCD.printNumI(cl_1, 255, 54);}
           CL_100 = cl_100*100;
           CL_10  = cl_10*10;
           CL_1   = cl_1;
           for (int c=0; c<3; c++)
             {
              myGLCD.setColor(Rline, Gline, Bline);
              myGLCD.drawRect((c*27)+204, 78, (c*27)+226, 100);
              myGLCD.drawRect((c*27)+204, 105, (c*27)+226, 127);
             }
           drawBarandColorValue();
           LED_levels_output();
           checkTempC();
           
           }
         }
      }
  break;
    
  case 7:     // ----------- VIEW INDIVIDUAL LED COLORS SCREEN -----------    
    if ((x>=10) && (x<=150))                                //first column
      {
       if ((y>=20) && (y<=50))                              //View White LEDs Array
         {
          waitForIt(10, 20, 150, 50); 
          dispScreen=8;
          COLOR = WHITE;
          clearScreen();
          ledValuesScreen();
         }  
       if ((y>=60) && (y<=90))                              //View Royal Blue LEDs Array
         {
          waitForIt(10, 60, 150, 90); 
          dispScreen=8;
          COLOR = ROYAL;
          clearScreen();
          ledValuesScreen();
         }
       if ((y>=100) && (y<=130))                            //View Red LEDs Array
         {
          waitForIt(10, 100, 150, 130); 
          dispScreen=8;
          COLOR = RED;
          clearScreen();
          ledValuesScreen();
         }  
      }
    if ((x>=170) && (x<=310))                               //second column
      {
       if ((y>=20) && (y<=50))                              //View Blue LEDs Array
         { 
          waitForIt(170, 20, 310, 50); 
          dispScreen=8;
          COLOR = BLUE;
          clearScreen();
          ledValuesScreen();
         }         
       if ((y>=60) && (y<=90))                              //View Ultra LEDs Array
         {
          waitForIt(170, 60, 310, 90); 
          dispScreen=8;
          COLOR = ULTRA;
          clearScreen();
          ledValuesScreen();
         }  
       if ((y>=100) && (y<=130))                            //View Sump LEDs Array
         {
          waitForIt(170, 100, 310, 130); 
          dispScreen=8;
          COLOR = SUMP;
          clearScreen();
          ledValuesScreen();
         }         
      }
    if ((x>=90) && (x<=230) && (y>=140) && (y<=170))        //View Moon LEDs MI
      {
       waitForIt(90, 140, 230, 170); 
       ReadFromEEPROM();
       dispScreen=8;
       COLOR = MOON;
       clearScreen();
       ledValuesScreen();
      }
  break;    
    
  case 8:     // -------------- SHOW LED VALUES TABLE SCREEN -------------
    if ((x>=back[0]) && (x<=back[2]) && (y>=back[1]) && (y<=back[3]))   //press MORE COLORS
      {
       waitForIt(back[0], back[1], back[2], back[3]);
       ReadFromEEPROM();
       dispScreen=7;
       clearScreen();
       ledColorViewScreen();
      } else 
    if ((x>=ledChV[0]) && (x<=ledChV[2]) && (y>=ledChV[1]) && (y<=ledChV[3]) && (COLOR!=7))   //press CHANGE
      {
       waitForIt(ledChV[0], ledChV[1], ledChV[2], ledChV[3]);
       ReadFromEEPROM();
       dispScreen=9;
       clearScreen(); 
       ledChangeScreen();
      } else 
    if ((x>=eeprom[0]) && (x<=eeprom[2]) && (y>=eeprom[1]) && (y<=eeprom[3]) && (COLOR!=7))  //press SAVE
      {
       waitForIt(eeprom[0], eeprom[1], eeprom[2], eeprom[3]);
       SaveLEDToEEPROM();
       dispScreen=7;
       clearScreen();
       ledColorViewScreen();
      } else 
    if ((x>=prSAVE[0]) && (x<=prSAVE[2]) && (y>=prSAVE[1]) && (y<=prSAVE[3])) //press SAVE
      {
       waitForIt(prSAVE[0], prSAVE[1], prSAVE[2], prSAVE[3]);
       SaveMoonLEDToEEPROM();
       dispScreen=7;
       clearScreen();
       ledColorViewScreen();
      } else             
    if ((x>=canC[0]) && (x<=canC[2]) && (y>=canC[1]) && (y<=canC[3]))         //press CANCEL
      {
       waitForIt(canC[0], canC[1], canC[2], canC[3]);
       ReadFromEEPROM();
       LEDtestTick = false;
       dispScreen=0;
       clearScreen();
       mainScreen(true);
      } else               

    if ((x>=miM[0]) && (x<=miM[2]) && (y>=miM[1]) && (y<=miM[3]))   //press MI minus
      {
       waitForIt(miM[0], miM[1], miM[2], miM[3]);
       tMI -= 1;
       if (tMI <= 0) {
         tMI = 0; }
       MI = tMI;         
       setFont(LARGE, 255, 255, 255, 0, 0, 0);
       myGLCD.print("   ", CENTER, 120);
       myGLCD.printNumI(tMI, CENTER, 120);
      } else      
    if ((x>=miP[0]) && (x<=miP[2]) && (y>=miP[1]) && (y<=miP[3]))   //press MI plus
      {
       waitForIt(miP[0], miP[1], miP[2], miP[3]);
       tMI += 5;
         if (tMI >255) {
       tMI = 255; }
       MI = tMI;         
       setFont(LARGE, 255, 255, 255, 0, 0, 0);
       myGLCD.print("   ", CENTER, 120);         
       myGLCD.printNumI(tMI, CENTER, 120);
      }
  break;
      
  case 9:     //---------------- CHANGE LED VALUES SCREEN ---------------
    if ((x>=prSAVE[0]) && (x<=prSAVE[2]) && (y>=prSAVE[1]) && (y<=prSAVE[3])) //press SAVE
      {
       waitForIt(prSAVE[0], prSAVE[1], prSAVE[2], prSAVE[3]);
       dispScreen=8;
       if (COLOR==1) {
         for (int i; i<96; i++) {
           wled[i]=tled[i]; }
         }
       if (COLOR==2) {
         for (int i; i<96; i++) {
           bled[i]=tled[i]; }
         }
       if (COLOR==3) {
         for (int i; i<96; i++) {
           rbled[i]=tled[i]; }
         }
       if (COLOR==4) {
         for (int i; i<96; i++) {
           rled[i]=tled[i]; }
         }  
       if (COLOR==5) {
         for (int i; i<96; i++) {
           uvled[i]=tled[i]; }
         }
       if (COLOR==6) {
         for (int i; i<96; i++) {
           sled[i]=tled[i]; }
         }
         clearScreen();
         ledValuesScreen();
      }   

       else if ((y>=15) && (y<=40))        //top row with times was touched
         {
          if ((x>=4) && (x<=316))
            {
             int oldLCT = LedChangTime;
             LedChangTime = map(x, 3, 320, 0, 12);                
               
             if (oldLCT != LedChangTime)   //highlight touched time
               {
                myGLCD.setColor(0, 0, 0);
                myGLCD.fillRect((oldLCT*26)+5, 21, (oldLCT*26)+29, 45);
                setFont(SMALL, 0, 255, 255, 0, 0, 0);
                myGLCD.printNumI((oldLCT*2), (oldLCT*26)+10, 22);
                myGLCD.printNumI(((oldLCT*2)+1), (oldLCT*26)+10, 33);
                myGLCD.setColor(255, 0, 0);
                myGLCD.fillRect((LedChangTime*26)+5, 21, (LedChangTime*26)+29, 45);
                setFont(SMALL, 255, 255, 255, 255, 0, 0);
                myGLCD.printNumI((LedChangTime*2), (LedChangTime*26)+10, 22);
                myGLCD.printNumI(((LedChangTime*2)+1), (LedChangTime*26)+10, 33);
                  
                for (int i=0; i<8; i++)    //print led values for highlighted time
                  {
                   int k=(LedChangTime*8)+i;
                   setFont(SMALL, 255, 255, 255, 0, 0, 0);
                   myGLCD.print( "   ", (i*38)+12, 105);
                   myGLCD.printNumI( tled[k], (i*38)+12, 105);
                  }
                }
            }
         } 
       else if ((y>=70) && (y<=95))        //plus buttons were touched
         {
          for (int i=0; i<8; i++) {                
            if ((x>=(i*38)+10) && (x<=(i*38)+35)) {
              int k= (LedChangTime*8)+i;
              tled[k]++;
              if (tled[k]>255) {
                tled[k]=255; }
              myGLCD.printNumI( tled[k], (i*38)+12, 105);
               }
            }
         }
       else if ((y>=125) && (y<=150))      //minus buttons were touched
         {
          for (int i=0; i<8; i++) {                
            if ((x>=(i*38)+10) && (x<=(i*38)+35)) {
              int k= (LedChangTime*8)+i;
              tled[k]--;
              if (tled[k]<0) {
                tled[k]=0; }
              myGLCD.print( "  ", (i*38)+20, 105);
              myGLCD.printNumI( tled[k], (i*38)+12, 105);
               }
            }
         }
  }
    break;
    
       case 12:     //-------------------- GENERAL SETTINGS -------------------
      if ((x>=prSAVE[0]) && (x<=prSAVE[2]) && (y>=prSAVE[1]) && (y<=prSAVE[3])) //press SAVE
        {
         waitForIt(prSAVE[0], prSAVE[1], prSAVE[2], prSAVE[3]);
         SaveGenSetsToEEPROM();
         dispScreen=1;
         clearScreen();
         menuScreen();
        }
      if ((x>=185) && (x<=305) && (y>=19) && (y<=39))   //press DD/MM/YYYY Button
        {
         waitForIt(185, 19, 305, 39);
         setCalendarFormat = 0;
         genSetSelect();
        }

      if ((x>=185) && (x<=305) && (y>=45) && (y<=65))   //press Month DD, YYYY Button
        {
         waitForIt(185, 45, 305, 65);
         setCalendarFormat = 1;
         genSetSelect();
        }

      if ((x>=195) && (x<=235))                         //first column
        {
         if ((y>=76) && (y<=96))                        //press 12HR Button
           { 
            waitForIt(195, 76, 235, 96);
            setTimeFormat = 1;
            genSetSelect();
           }
         if ((y>=107) && (y<=127))                      //press deg C
           { 
            waitForIt(195, 107, 235, 127);
            setTempScale = 0;
            genSetSelect();            
           }
         if ((y>=138) && (y<=158))                      //press Screensaver ON
           { 
            waitForIt(195, 138, 235, 158);
            setScreensaver = 1;
            genSetSelect();                 
           }
         if ((y>=169) && (y<=189))                      //press Auto-Stop on Feed ON
           { 
            waitForIt(195, 169, 235, 189);
            setAutoStop = 1;
            genSetSelect();                       
           }           
        } 
      if ((x>=255) && (x<=295))                         //second column
        {
         if ((y>=76) && (y<=96))                        //press 24HR Button
           { 
            waitForIt(255, 76, 295, 96);
            setTimeFormat = 0;
            genSetSelect();            
           }
         if ((y>=107) && (y<=127))                      //press deg F
           { 
            waitForIt(255, 107, 295, 127);
            setTempScale = 1;
            genSetSelect();               
           }
         if ((y>=138) && (y<=158))                      //press Screensaver OFF
           { 
            waitForIt(255, 138, 295, 158);
            setScreensaver = 2;
            genSetSelect();                      
           }
         if ((y>=169) && (y<=189))                      //press Auto-Stop on Feed OFF
           { 
            waitForIt(255, 169, 295, 189);
            setAutoStop = 2;
            genSetSelect();                      
           }           
        } 

    break;

    case 15:     //------------------------- ABOUT -------------------------
    break;
  }

}


  delay(100);
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------


void changeFont(boolean font, byte cr, byte cg, byte cb, byte br, byte bg, byte bb)
{
  myGLCD.setBackColor(br, bg, bb);    //font background 
  myGLCD.setColor(cr, cg, cb);        //font colour 
  if (font==LARGE)
    myGLCD.setFont(BigFont);          //font size 
  else if (font==SMALL)
    myGLCD.setFont(SmallFont);
}


void printHeader(char* headline)
{
     changeFont(SMALL, 255, 0, 0, 255, 0, 0);
     myGLCD.fillRect (1, 1, 318, 15);
     myGLCD.setColor(255, 255, 255);     
     myGLCD.print(headline, CENTER, 2);   
}

void setFont(boolean font, byte cr, byte cg, byte cb, byte br, byte bg, byte bb)
{
  myGLCD.setBackColor(br, bg, bb);               //font background black
  myGLCD.setColor(cr, cg, cb);                   //font color white
  if (font==LARGE)
    myGLCD.setFont(BigFont);                 //font size LARGE
  else if (font==SMALL)
    myGLCD.setFont(SmallFont);
}

void waitForItSq(int x1, int y1, int x2, int y2) // Draw a red frame while a button is touched
{
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawRect (x1, y1, x2, y2);
  while (myTouch.dataAvailable()) {
    myTouch.read(); 
  }
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRect (x1, y1, x2, y2);
}

void clearScreen()
{
  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect(1, 15, 318, 226);
}



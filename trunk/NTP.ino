// gmtime -- convert calendar time (sec since 1970) into broken down time
// returns something like Fri 2007-10-19 in day and 01:02:21 in clock
// The return values is the minutes as integer. This way you can update
// the entire display when the minutes have changed and otherwise just
// write current time (clock). That way an LCD display needs complete
// re-write only every minute.
uint8_t gmtime(const uint32_t time,char *day, char *clock)
{
  char dstr[4];
  uint8_t i;
  uint32_t dayclock;
  uint16_t dayno;
  uint16_t tm_year = EPOCH_YR;
  uint8_t tm_sec,tm_min,tm_hour,tm_wday,tm_mon;

  dayclock = time % SECS_DAY;
  dayno = time / SECS_DAY;

  tm_sec = dayclock % 60UL;
  tm_min = (dayclock % 3600UL) / 60;
  tm_hour = dayclock / 3600UL;
  
  tm_wday = (dayno + 4) % 7; 	/* day 0 was a thursday */
  while (dayno >= YEARSIZE(tm_year)) {
    dayno -= YEARSIZE(tm_year);
    tm_year++;
  }
  tm_mon = 0;
  while (dayno >= monthlen(LEAPYEAR(tm_year),tm_mon)) {
    dayno -= monthlen(LEAPYEAR(tm_year),tm_mon);
    tm_mon++;
  }

  dstr[3]='\0';
  setTime(tm_hour,tm_min,tm_sec,dayno+1,tm_mon+1,tm_year);
  utc = now();
     
  return(0);
}

//Function to compare NTP time with RTC time and set is time different
void CompareNTPTime(time_t t, char *tz)
{
     
    //Check time against NTP and see if it needs to be corrected
    //Check hour, mins, day, month, year
                   
    if ((mytime.hour != hour(t)) or (mytime.min != minute(t)) or (mytime.date != day(t)) or (mytime.mon != month(t)) or (mytime.year != year(t)) ) 
         {
              DEBUG_PRINTLN("time needs to be corrected");
              rtc.setTime(hour(t), minute(t), second(t));     // Set the time to 12:00:00 (24hr format)
              rtc.setDate(day(t), month(t), year(t));   // Set the date to October 3th, 2010
          } else {
              DEBUG_PRINTLN("Time ok");
         }
}

void getNTP() 
{
    int plen = 0;
    uint16_t dat_p;
    char day[22];
    char clock[22]; 
       
    // time to send request
    lastNTPUpdate = millis();
    
    if (!ether.dnsLookup( (char*)pgm_read_word(&(ntpList[currentTimeserver])) )) {
       DEBUG_PRINTLN( F("NTP DNS failed" ));
    } else {
       ether.printIp("NTP SRV: ", ether.hisip);
       DEBUG_PRINT( F("Send NTP request " ));
       DEBUG_PRINTLN( currentTimeserver);
    
       ether.ntpRequest(ether.hisip, clientPort);
       boolean WaitforResponse = false;
       
       //Loop here until we receive a response from ntp server
       while (WaitforResponse == false)
          {
              plen = ether.packetReceive();
              dat_p=ether.packetLoop(plen);
              if (plen > 0) {
                  timeLong = 0L;
                 
                  
                  if (ether.ntpProcessAnswer(&timeLong,clientPort)) {
                      WaitforResponse = true;
                      if (timeLong) {
                          timeLong -= GETTIMEOFDAY_TO_NTP_OFFSET;
                       gmtime(timeLong,day,clock);                  //Convert Longtime to correct format
                       local = myTZ.toLocal(utc, &tcr);            //Convert using TZ
                       CompareNTPTime(local, tcr -> abbrev);        // Check time against RTC
                      }
                  }
               }
         }
    }
    if( ++currentTimeserver >= NUM_TIMESERVERS )
       currentTimeserver = 0; 
}


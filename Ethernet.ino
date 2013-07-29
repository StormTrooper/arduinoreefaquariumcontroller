void TransmitData()
{
    if (tempW != 0)                      //Values all still zero so Arduino has probably jut restarted. Do not send these values.
    {
      digitalWrite(TX_led, HIGH);
      delay(50);
      digitalWrite(TX_led, LOW);
      
          if (!ether.dnsLookup(website))
        Serial.println("DNS failed");
        ether.printIp("SRV: ", ether.hisip);
        DEBUG_PRINTLN("Ethernet Okay");

      
      DEBUG_PRINTLN("Ethernet routine called");
      
      DEBUG_PRINTLN(pH);
            
      DEBUG_PRINT("Temp:");
      DEBUG_PRINTLN(tempW);
        
      
      DEBUG_PRINTLN();
      DEBUG_PRINT("<<< REQ ");
    
      FillOutBuff (pH, 29, 5, 2);    //pH
      FillOutBuff (Fanspeed, 42, 4, 0);    //Fanspeed
      FillOutBuff (tempW, 54, 5, 2);    //Water
      FillOutBuff (tempH, 67, 5, 2);    //Heatsink
      FillOutBuff (tempS, 80, 5, 2);    //Sump
      
      ether.browseUrl(PSTR("/update/"), cbuff , website, my_callback);
  
      DEBUG_PRINTLN("Send");
      DEBUG_PRINT("Count:");
      DEBUG_PRINTLN(Counter);
      Counter++;
      
      DEBUG_PRINT("Uptime: ");
      DEBUG_PRINT(millis() / 60000);
      DEBUG_PRINTLN(" mins");
    }
}

// called when the client request is complete
static void my_callback (byte status, word off, word len) {
//  DEBUG_PRINTLN(">>>");
  Ethernet::buffer[off+300] = 0;
//  DEBUG_PRINT((const char*) Ethernet::buffer + off);
//  DEBUG_PRINTLN("...");
}

void FillOutBuff (float temp1, int dstart, int dlen, int zeros){
  dtostrf(temp1,dlen,zeros,tbuff);
  for (int i=0+dstart; i <= dstart+dlen-1; i++){
    cbuff[i] = tbuff[i-dstart];
    if(cbuff[i] == ' ') cbuff[i] = '0';//if cbuff[i] = <SPACE> then cbuff[i] = a leading zero
  }
}


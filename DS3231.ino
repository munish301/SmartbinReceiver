/////////////////////RTC DS3231////////////////
//    #include <TimeAlarms.h>
//    
//    tmElements_t getRTCtimeElements() {
//      byte sec;
//      tmElements_t tm;
//    
//      I2C_int.beginTransmission(0x68);
//      I2C_int.write(0);
//      uint8_t err = I2C_int.endTransmission(true);
//    //  if(err!= 0){
//    //    Serial.println("RTCC not present at address 0x68.");
//    //  }
//      I2C_int.requestFrom(0x68,7);  
//      if (I2C_int.available() < 7) return tm;
//      sec = I2C_int.read();
//      tm.Second = bcd2dec(sec & 0x7f);   
//      tm.Minute = bcd2dec(I2C_int.read() );
//      tm.Hour =   bcd2dec(I2C_int.read() & 0x3f);  // mask assumes 24hr clock
//      tm.Wday = bcd2dec(I2C_int.read() );
//      tm.Day = bcd2dec(I2C_int.read() );
//      tm.Month = bcd2dec(I2C_int.read() & 0x7f ); 
//      tm.Year = y2kYearToTm((bcd2dec(I2C_int.read())));
//      return tm; 
//    }
//    
//    time_t getRTCtime() {
//      tmElements_t tm = getRTCtimeElements();
//      return (makeTime(tm)); 
//    }
//    
//    
//    bool setRTCtime(tmElements_t tm){
//        //tmElements_t tm; 
//        //breakTime(intime, tm);
//        if (tm.Year >= 1000)
//            tm.Year = CalendarYrToTm(tm.Year)%100;
//        else    //(tm.Year < 100)
//            tm.Year = y2kYearToTm(tm.Year);
//      // To eliminate any potential race conditions,
//      // stop the clock before writing the values,
//      // then restart it after.
//        I2C_int.beginTransmission(0x68);
//        I2C_int.write((uint8_t)0x00); // reset register pointer  
//    //  I2C_int.write((uint8_t)0x80); // Stop the clock. The seconds will be written last
//        I2C_int.write(dec2bcd(tm.Second));
//        I2C_int.write(dec2bcd(tm.Minute));
//        I2C_int.write(dec2bcd(tm.Hour));      // sets 24 hour format
//        I2C_int.write(dec2bcd(tm.Wday));   
//        I2C_int.write(dec2bcd(tm.Day));
//        I2C_int.write(dec2bcd(tm.Month));   
//        I2C_int.write(dec2bcd(tm.Year)); 
//        if (I2C_int.endTransmission() != 0) {
//    //    exists = false;
//        return false;
//      }
//      return true;
//    }
//    
//    bool setRTCtime2(tmElements_t tm){
//    
//      // To eliminate any potential race conditions,
//      // stop the clock before writing the values,
//      // then restart it after.
//        I2C_int.beginTransmission(0x68);
//        I2C_int.write((uint8_t)0x00); // reset register pointer  
//    //  I2C_int.write((uint8_t)0x80); // Stop the clock. The seconds will be written last
//        I2C_int.write(dec2bcd(tm.Second));
//        I2C_int.write(dec2bcd(tm.Minute));
//        I2C_int.write(dec2bcd(tm.Hour));      // sets 24 hour format
//        I2C_int.write(dec2bcd(tm.Wday));   
//        I2C_int.write(dec2bcd(tm.Day));
//        I2C_int.write(dec2bcd(tm.Month));   
//        I2C_int.write(dec2bcd(tm.Year)); 
//        if (I2C_int.endTransmission() != 0) {
//    //    exists = false;
//        return false;
//      }
//      return true;
//    }
//    
//    byte dec2bcd(byte val) {
//      return ((val/10*16) + (val%10));
//    }
//    byte bcd2dec(byte val) {
//      return ((val/16*10) + (val%16));
//    }

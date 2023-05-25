void read_config() {
  String str;
  unsigned char cnt = 0;
  Serial.println("*******Reading Config file*******");
  File file = SPIFFS.open("/config.csv");
  if (!file) {
    Serial.println("- failed to open file for reading");
    return;
  }
 
  while (file.available()) {
    str = file.readStringUntil('\n');
    str = str.substring(str.indexOf(',', 1) + 1);
    slave_id[cnt] = (uint8_t) str.toInt();
    Serial.print("slave_id[cnt]: ");
    Serial.println(slave_id[cnt], DEC);
    cnt++;
  }
  SLAVE_ID_LENGTH = cnt;
  file.close();
}


/*************************************
 Fxn : unsigned int CRC16(unsigned int crc, unsigned int data)
 Info: CRC16 calculation
 *************************************/
unsigned int CRC16(unsigned int crc, unsigned int data)
{
  const unsigned int Poly16=0xA001;
  unsigned int LSB, i;

  crc = ((crc^data) | 0xFF00) & (crc | 0x00FF);
  for (i=0; i<8; i++) 
  {
    LSB=(crc & 0x0001);
    crc=crc/2;
    if (LSB)
      crc=crc^Poly16;
  }
  return(crc);
}

/*************************************
 Fxn : void calculateCRC16(uint8_t NUMDATA, unsigned char * data, unsigned char * Crc_HByte,unsigned char * Crc_LByte)
 Info: CRC16 calculation
 *************************************/
void calculateCRC16(uint8_t numData, byte * data, byte * Crc_HByte,byte * Crc_LByte)
{

  unsigned int Crc = 0xFFFF;

  for (uint8_t i=0; i<numData; i++) 
  {
    Crc = CRC16 (Crc, data[i] );
  }
  *Crc_LByte = (Crc & 0x00FF);    // Low byte calculation
  *Crc_HByte = (Crc & 0xFF00) / 256;  // High byte calculation
}



unsigned char crc_test_rx(void)
{
  unsigned char i,h = 0;
  unsigned int crc_temp_reg = 0xffff;
  
  for (h = 0;h <(rx_frame_length - 4); h++)
  {
    crc_temp_reg ^= rx_frame_array[h];          // XOR byte into least sig. byte of crc
    
    for(i = 8;i != 0;i--)
    {    // Loop over each bit
      if ((crc_temp_reg & 0x0001) != 0)
      {      // If the LSB is set
        crc_temp_reg >>= 1;                    // Shift right and XOR 0xA001
        crc_temp_reg ^= 0xA001;
      }
      else                            // Else LSB is not set
      crc_temp_reg >>= 1;                    // Just shift right          //plc send lsb then msb
    }
  }
  unsigned int crc_temp_reg1 = ((rx_frame_array[rx_frame_length-3] <<8) | rx_frame_array[rx_frame_length-4]);
  Serial.print("CRC RX: "+crc_temp_reg1);
  if (crc_temp_reg == crc_temp_reg1)
    return 1;
  return 0;
}

uint16_t crc_tx()
{
  unsigned char i,h = 0;
  unsigned int crc_temp_reg = 0xffff;
  
  for (h = 0;h <(rx_frame_length - 2); h++)
  {
    crc_temp_reg ^= rx_frame_array[h];          // XOR byte into least sig. byte of crc
    
    for(i = 8;i != 0;i--)
    {    // Loop over each bit
      if ((crc_temp_reg & 0x0001) != 0)
      {      // If the LSB is set
        crc_temp_reg >>= 1;                    // Shift right and XOR 0xA001
        crc_temp_reg ^= 0xA001;
      }
      else                            // Else LSB is not set
      crc_temp_reg >>= 1;                    // Just shift right          //plc send lsb then msb
    }
  }

    return crc_temp_reg;
}

boolean rx_timeout_flag = false;
int rx_timeout = 0;
boolean packet_received_flag = false;

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
  
void IRAM_ATTR onTimer(){
  portENTER_CRITICAL_ISR(&timerMux);
  if (rx_timeout_flag) {
    rx_timeout++;
    if (rx_timeout >= 2) {  
      rx_timeout = 0;
      rx_timeout_flag=0;
      packet_received_flag=true;

    }
  } 
  portEXIT_CRITICAL_ISR(&timerMux);   
}

void initTimer(){
    // Use 1st timer of 4 
    // 1 tick take 1/(80MHZ/80) = 1us so we set divider 80 and count up 
    timer = timerBegin(0, 80, true);
    
    // Attach onTimer function to our timer 
    timerAttachInterrupt(timer, &onTimer, true);
    
    // Set alarm to call onTimer function every second 1 tick is 1us
    //=> 1 second is 1000000us 
    // Repeat the alarm (third parameter) 
    timerAlarmWrite(timer, 100, true);
    
    //  // Start an alarm 
    // timerAlarmEnable(timer);
} 

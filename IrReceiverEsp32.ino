
#include <SPIFFS.h>
#include <TimeAlarms.h> //needs modification, increase number & return index
//#include <StringTokenizer.h>
//schedule processor
#include <HardwareSerial.h>
#include <Wire.h>
#include "defs.h"

#define SENSOR_VP                   GPIO_NUM_36
#define RS485_BAUD                  115200
/*__________________________________________
UART  | RX IO   | TX IO   | CTS   | RTS
--------------------------------------------
UART0 | GPIO3   | GPIO1   | N/A   | N/A
UART1 | GPIO9   | GPIO10  | GPIO6 | GPIO11
UART2 | GPIO16  | GPIO17  | GPIO8 | GPIO7
___________________________________________*/
HardwareSerial                      SerialIrda(1);

//3 seconds WDT
#define WDT_TIMEOUT                 3 //in seconds

unsigned long hb_currentMillis=0, hb_previousMillis = 0;
unsigned long hb_interval = 500;
int send_counts = 0;
int serial_counts = 0;
char serial_rxd[20];
String irdaRxString = "";

unsigned char slave_counts = 0;
unsigned char slave_addr[4] = {0, 0, 0, 0};
boolean check_slave_flag = true;
unsigned long m_slave_id = 0;

//Project 1
void setup() {

  Serial.begin(115200);
   
  pinMode(LED_HB, OUTPUT);
  pinMode(LED_RF_RX, OUTPUT);
  pinMode(LED_BL_INDI, OUTPUT);
  

  pinMode(UL_IN1, INPUT);
  pinMode(UL_IN2, INPUT);
  pinMode(UL_IN3, INPUT);
  pinMode(UL_IN4, INPUT);
  
  digitalWrite(LED_HB, 0);
  digitalWrite(LED_RF_RX, 0);
  digitalWrite(LED_BL_INDI, 0);
  
  pinMode(IRF_EN, OUTPUT);
  digitalWrite(IRF_EN, 0);
  delay(10);
  digitalWrite(IRF_EN, 1);
  delay(10);
  //////////////////////////////////////////
  SerialIrda.begin(19200, SERIAL_8N1, 15, 13);        //15-RXD, 13-TXD
  //SerialIrda.begin(19200, SERIAL_8N1, 13, 15);        //15-RXD, 13-TXD
  delay(10);
  I2C_int.begin((uint8_t)21, (uint8_t)22, 100000);    //
  delay(10);
  if (!SPIFFS.begin()) { //
    Serial.println("SPIFFS Mount Failed");            //
    //Flash HB LED fast to indicate error
    digitalWrite(LED_HB, 1);
    while (1);                                        //Hang In There
  }
  delay(10);
  read_config();
  Serial.println("Start Receiving: ");
}


boolean checkSerial(){
  boolean response_flag = false;
  if(SerialIrda.available()){
      rx_frame_length = 0; 
      rx_frame_array[rx_frame_length++] = (uint8_t)SerialIrda.read();
      //SerialIrda.flush();
      //Serial.print(rx_frame_array[rx_frame_length-1]);
      if(rx_frame_array[rx_frame_length-1] == START_BYTE){
          digitalWrite(LED_RF_RX, 1);
          response_flag = true;
          slave_addr[3] = 0; slave_addr[2] = 0; slave_addr[1] = 0; slave_addr[0] = 0;
          while(SerialIrda.available()){
            rx_frame_array[rx_frame_length++] = (uint8_t)SerialIrda.read();
            //SLAVE_ID_LENGTH
            if(rx_frame_array[rx_frame_length-1] == END_LF){
              if(rx_frame_array[rx_frame_length-2] == END_CR){
                    response_flag = false;
                    for(int j=0; j<SLAVE_ID_LENGTH; j++){
                      if(rx_frame_array[1] == slave_id[j]){
                        response_flag = true;
                        break;
                      }
                    }
                    break;
              }
            }
          }
          check_slave_flag = true;
          SerialIrda.flush();
      }
  }
  return response_flag;
}


void createResponse(uint8_t slaveid){
  uint8_t crc_H, crc_L;
  frame_length = 0;
  frame_array[frame_length++] = START_BYTE;
  frame_array[frame_length++] = slaveid;
  frame_array[frame_length++] = REQUEST_SERIAL; //fxn_code
   frame_array[frame_length++] = 1;
  //DATA = NONE
  calculateCRC16(frame_length, frame_array, &crc_H, &crc_L);
  frame_array[frame_length++] = crc_L; //crc low byte
  frame_array[frame_length++] = crc_H;   //crc high byte
  frame_array[frame_length++] = END_CR; //crc low byte
  frame_array[frame_length++] = END_LF;   //crc high byte  
}

void sendResponse(){
  for(int i=0; i<frame_length; i++){
    SerialIrda.write(frame_array[i]);
    Serial.print(frame_array[i], DEC);
  }  
  Serial.println();     
}

//void checkData(){
//  if(rx_frame_array[0] == START_BYTE){
//    if(rx_frame_array[1] == SLAVE_ID){
//      if(rx_frame_array[2] == REQUEST_SERIAL){
//         createResponse(); 
//         sendResponse();
//      }
//    }
//  }
//}
void loop() {
  int i;
  //esp_task_wdt_reset();
  hb_currentMillis = millis();
  if (hb_currentMillis - hb_previousMillis >= hb_interval) {
      hb_previousMillis = hb_currentMillis;
      digitalWrite(LED_HB, !digitalRead(LED_HB));
      Serial.println(".");
  } 

  if(checkSerial()){
    Serial.print("Request Received: ");
    for(i=0; i<rx_frame_length; i++){
      Serial.print(rx_frame_array[i], DEC);
    }

    if(crc_test_rx() > 0){
      //Serial.println("CRC matches");
      if(rx_frame_array[2] == REQUEST_SERIAL){
        //digitalWrite(IRF_EN, 1);
        //delay(50); //this delay is required
        Serial.print("SLAVE ");
        Serial.print(rx_frame_array[1]);
        Serial.println(" requested");
        createResponse(rx_frame_array[1]);
        sendResponse();
        //delay(50); //this delay is required
        //digitalWrite(IRF_EN, 0);
      }else{
        Serial.println("Invalid FXN_ID");
      }
    }else{
      Serial.println("Invalid Frame");
    }
    digitalWrite(LED_RF_RX, 0);
  }
//  if(SerialIrda.available()){
//      serial_counts = 0; 
//      irdaRxString += (char)SerialIrda.read();
////      rx_timeout_flag=1; rx_timeout=0;
//      while(SerialIrda.available()){
//        irdaRxString += (char)SerialIrda.read();
//        //rx_timeout_flag=1; rx_timeout=0;
//      }
//      SerialIrda.flush();
//     // Serial.print("Master Request: ");
//      Serial.println(irdaRxString);
//      irdaRxString = "";
//      //SerialIrda.println("Hello from Slave");
//      
//   }
}

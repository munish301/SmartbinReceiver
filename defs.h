
uint8_t slave_id[10] = {0};

//LEDs
const int LED_RF_RX = 33;
const int LED_BL_INDI = 27;
const int LED_HB = 26;

//Wire
TwoWire I2C_int = TwoWire(0);

//ULN2804
const int UL_IN1 = 19;
const int UL_IN2 = 18;
const int UL_IN3 = 5;
const int UL_IN4 = 17;

const int IRF_EN = 16;


uint8_t rx_frame_array[40];
uint8_t rx_frame_length = 0;

uint8_t frame_array[40];
uint8_t frame_length = 0;
#define START_BYTE       0x52
//#define SLAVE_ID         0x01
#define REQUEST_SERIAL   0x10

#define END_CR           0x0D
#define END_LF           0x0A

unsigned char SLAVE_ID_LENGTH = 1;  //bytes

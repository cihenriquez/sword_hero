#include <FastLED.h>
#include <arduino-timer.h>

#include <Adafruit_ADS1X15.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>





#define DEBUG false

#define LED_PIN 14          // Replace with the pin number you used for DATA connection
#define NUM_LEDS 24         // Replace with the number of LEDs in your strip
#define FADE_DELAY 1        // Delay between brightness changes (in milliseconds)
#define MAX_BRIGHTNESS 255  //255 // Maximum brightness value
#define LEDS_PER_DIV 4
#define THRESHOLD 3000
#define PREP_TIME_MS 1000
#define LED_BUILTIN 2


#define PIN_SENSOR_DOWN_RIGHT 39
#define PIN_SENSOR_RIGHT 26
#define PIN_SENSOR_UP_RIGHT 35  
#define PIN_SENSOR_UP 32
#define PIN_SENSOR_UP_LEFT 34
#define PIN_SENSOR_LEFT 25
#define PIN_SENSOR_DOWN_LEFT 36
#define PIN_SENSOR_DOWN 33
#define PIN_SENSOR_CENTER_DOWN 12
#define PIN_SENSOR_CENTER_UP 13

#define DOWN_RIGHT 0
#define RIGHT_DOWN 0
#define RIGHT 1
#define RIGHT_UP 2
#define UP_RIGHT 2
#define UP 3
#define UP_LEFT 4
#define LEFT_UP 4
#define LEFT 5
#define LEFT_DOWN 6
#define DOWN_LEFT 6
#define DOWN 7
#define CENTER_DOWN 8
#define CENTER_UP 9

// Status
#define DEFAULT 0
#define WAITING 1
#define START 2
#define STARTING 3
#define RUNNING 4
#define WAIT_HIT 5
#define RESULTS 6

#define decreasing !increasing

#define DEVICE_NAME "Sword HERO Dummy"
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

#define ADC_1_I2C_ADDR 0x48
#define ADC_2_I2C_ADDR 0x4A

// Check if Bluetooth is available
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Check Serial Port Profile
#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
#endif

CRGB leds[NUM_LEDS];
Adafruit_ADS1115 ads1;  /* Use this for the 16-bit version */
Adafruit_ADS1115 ads2;  /* Use this for the 16-bit version */


int16_t reads[10];

int ledState = LOW;  // variable used to store the last LED status, to toggle the light


const uint8_t down_right_leds[] = {0, 1, 2, 3 };
const uint8_t right_leds[] = {3, 4, 5, 6 };
const uint8_t up_right_leds[] = {6, 7, 8, 9 };
const uint8_t up_leds[] = {9, 10, 11, 12 };
const uint8_t up_left_leds[] = {12, 13, 14, 15 };
const uint8_t left_leds[] = {15, 16, 17, 18};
const uint8_t down_left_leds[] = {18, 19, 20, 21};
const uint8_t down_leds[] = {21, 22, 23, 0};

const uint8_t segments[][LEDS_PER_DIV] = { { 0, 1, 2, 3},
                                           { 3, 4, 5, 6},
                                           { 6, 7, 8, 9 },
                                           { 9, 10, 11, 12},
                                           { 12, 13, 14, 15 },
                                           { 15, 16, 17, 18 },
                                           { 18, 19, 20, 21},
                                           { 21, 22, 23, 0 } };

int state;
int prev_state;
int next_state;

Timer<10, millis> timer;

int countdown_led;
Timer<10, millis>::Task task_countdown;
int brightness = 0;
bool increasing = true;

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;



int actual_pattern_position = 0;
int hit_counter = 0;
int pattern_1[] = { UP_RIGHT, DOWN_LEFT, DOWN_RIGHT, UP_LEFT };
int pattern_2[] = { UP_RIGHT, DOWN_LEFT, DOWN_RIGHT, UP_LEFT, RIGHT, LEFT, UP, DOWN, RIGHT, LEFT };  //Rosa hasta el 10

//Setup callbacks onConnect and onDisconnect
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    deviceConnected = true;
    Serial.println("DEVICE CONNECTED");

    
  };
  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
  }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
    String rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        if (DEBUG){
        Serial.println("*********");
        Serial.print("Received Value: ");
        
        for (int i = 0; i < rxValue.length(); i++)
          Serial.print(rxValue[i]);

        Serial.println();
        Serial.println("*********");
        }
        if (rxValue=="restart"){
          next_state=DEFAULT;
        }
      }
    }
};


void setup() {
  state = 0;
  pinMode(LED_BUILTIN, OUTPUT);  // declare the ledPin as as OUTPUT
  pinMode(15, INPUT_PULLDOWN);



  FastLED.addLeds<WS2811, LED_PIN, RGB>(leds, NUM_LEDS);
  Serial.begin(115200);  // use the serial port
  digitalWrite(LED_BUILTIN, HIGH);



  BLEDevice::init(DEVICE_NAME);
  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
										CHARACTERISTIC_UUID_TX,
										BLECharacteristic::PROPERTY_NOTIFY
									);
                      
  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
											 CHARACTERISTIC_UUID_RX,
											BLECharacteristic::PROPERTY_WRITE
										);

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();

  //ADC 1 
  ads1.setGain(GAIN_ONE);
  ads1.begin(ADC_1_I2C_ADDR);
  ads2.setDataRate(RATE_ADS1115_860SPS);

  //ADC 2 
  ads2.setGain(GAIN_ONE);
  ads2.begin(ADC_2_I2C_ADDR);
  ads2.setDataRate(RATE_ADS1115_860SPS);
  
  //breath();
  timer.every(50, state_machine);
  timer.every(50, b_read_sensors);
  timer.every(30, b_bluetooth_comms);
}

void loop() {
  timer.tick();
  //v_read_sensors();

  //delay(1);  // delay to avoid overloading the serial port buffer
}

bool state_machine(void *) {
  prev_state = state;
  state = next_state;
  switch (state) {
    case DEFAULT:
      if (!breath()) { next_state = START; }
      hit_counter = 0;
      actual_pattern_position = 0;
      break;

    case START:
      countdown_led = NUM_LEDS;
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      task_countdown = timer.every(PREP_TIME_MS / NUM_LEDS, countdown);
      next_state = STARTING;
      break;

    case STARTING:
      if (countdown_led == 0) {
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        next_state = RUNNING;

        break;
      }

      break;
    case RUNNING:
      FastLED.clear();
      for (int i = 0; i < LEDS_PER_DIV; i++) {
        leds[segments[pattern_2[actual_pattern_position]][i]] = CRGB(MAX_BRIGHTNESS, 0, 0);
      }
      FastLED.show();
      delay(2);
      next_state = WAIT_HIT;
      break;
    case WAIT_HIT:
      if (reads[pattern_2[actual_pattern_position]] > THRESHOLD) {
        if (DEBUG) {Serial.println("\n\n\n HIT! \n\n\n");}
        FastLED.clear();
        for (int i = 0; i < LEDS_PER_DIV; i++) {
          leds[segments[pattern_2[actual_pattern_position]][i]] = CRGB(0, MAX_BRIGHTNESS, 0);
        }
        FastLED.show();
        delay(2);
        actual_pattern_position += 1;
        hit_counter+=1;
        if (actual_pattern_position == sizeof(pattern_2) / sizeof(pattern_2[0])) {
          actual_pattern_position = 0;
        }
        next_state = RUNNING;
      }
      break;
    default:
      next_state = DEFAULT;
      break;
  }
  return true;
}



uint16_t interpolate_reads(uint16_t sensor_read_1, uint16_t sensor_read_2) {

  return (1.5 * (sensor_read_1 + sensor_read_2) - 1.2 * (abs(sensor_read_1 - sensor_read_2)));
}

int read_to_brightness(uint16_t sensor_read) {
  if (sensor_read > THRESHOLD) {
    return (int)255 * sensor_read / 1024;
  } else {
    return 0;
  }
}

#include <FastLED.h>
#define LED_PIN 5    // Replace with the pin number you used for DATA connection
#define NUM_LEDS 50  // Replace with the number of LEDs in your strip
#define FADE_DELAY 1   // Delay between brightness changes (in milliseconds)
#define MAX_BRIGHTNESS 255 // Maximum brightness value
#define LEDS_PER_DIV 4 
#define THRESHOLD 300
CRGB leds[NUM_LEDS];



const int knockSensor_up_right = A0; //pulldown -> 1.5M Ohm
const int knockSensor_down_right = A2;
const int knockSensor_up_left = A7;
const int knockSensor_down_left = A1;
const int knockSensor_center_down = A3;
const int knockSensor_center_up = A4;

int up_right_read = 0;      
int down_right_read = 0;
int up_left_read = 0;
int down_left_read = 0; 
int center_down_read = 0;    
int center_up_read = 0;  
int ledState = LOW;         // variable used to store the last LED status, to toggle the light

int down_right_leds[] = {0,1,2,3};
int right_leds[]      = {4,5,6,7};
int up_right_leds[]   = {8,9,10,11};
int up_leds[]         = {12,13,14,15};
int up_left_leds[]    = {16,17,18,19};
int left_leds[]       = {20,21,22,23};
int down_left_leds[]  = {24,25,26,27};
int down_leds[]       = {28,29,30,31};


 
void setup() {
  pinMode(LED_BUILTIN, OUTPUT); // declare the ledPin as as OUTPUT
  FastLED.addLeds<WS2811, LED_PIN, RGB>(leds, NUM_LEDS);
  Serial.begin(115200);       // use the serial port

  v_set_down_right(MAX_BRIGHTNESS);
  v_set_up_right(MAX_BRIGHTNESS);
  v_set_down_left(MAX_BRIGHTNESS);
  v_set_up_left(MAX_BRIGHTNESS);

  v_set_up(MAX_BRIGHTNESS);
  v_set_down(MAX_BRIGHTNESS);
  v_set_left(MAX_BRIGHTNESS);
  v_set_right(MAX_BRIGHTNESS);
  FastLED.show();
  delay(3000);
}

void loop() {
  // read the sensor and store it in the variable sensorReading:
  up_right_read = analogRead(knockSensor_up_right);
  up_left_read = analogRead(knockSensor_up_left);
  down_right_read = analogRead(knockSensor_down_right);
  down_left_read = analogRead(knockSensor_down_left);
  center_down_read = analogRead(knockSensor_center_down);
  center_up_read = analogRead(knockSensor_center_up);
  Serial.print(up_right_read);
  Serial.print(",");
  Serial.print(up_left_read);
  Serial.print(",");
  Serial.print(down_right_read);
  Serial.print(",");
  Serial.print(down_left_read);
  Serial.print(",");
  Serial.print(center_down_read);
  Serial.print(",");
  Serial.print(center_up_read);
  Serial.print("\n");
  

  v_set_down_right(read_to_brightness(down_right_read));
  v_set_up_right(read_to_brightness(up_right_read));
  v_set_down_left(read_to_brightness(down_left_read));
  v_set_up_left(read_to_brightness(up_left_read));

  v_set_up(read_to_brightness(interpolate_reads(up_left_read,up_right_read)));
  v_set_down(read_to_brightness(interpolate_reads(down_left_read,down_right_read)));
  v_set_left(read_to_brightness(interpolate_reads(up_left_read, down_left_read)));
  v_set_right(read_to_brightness(interpolate_reads(up_right_read,down_right_read)));
  FastLED.show();
  delay(10);  // delay to avoid overloading the serial port buffer
}

int interpolate_reads(int sensor_read_1, int sensor_read_2){
  if (abs(sensor_read_1 - sensor_read_2) < 150)
  {
    return (1.5*(sensor_read_1 + sensor_read_2));
  }
  else
  {
    return 0;
  }
}

int read_to_brightness(int sensor_read){
  if (sensor_read > THRESHOLD)
  {
    return (int)255*sensor_read/1024;
  }
  else
  {
    return 0;
  }
}


void v_set_down_right(int brightness) {
  for (int i = 0; i < LEDS_PER_DIV; i++) {
    leds[down_right_leds[i]] = CRGB(brightness, 0, 0);
  }
}

void v_set_up_right(int brightness) {
  for (int i = 0; i < LEDS_PER_DIV; i++) {
    leds[up_right_leds[i]] = CRGB(0, brightness, 0);
  }
}

void v_set_up_left(int brightness) {
  for (int i = 0; i < LEDS_PER_DIV; i++) {
    leds[up_left_leds[i]] = CRGB(0, 0, brightness);
  }
}

void v_set_down_left(int brightness) {
  for (int i = 0; i < LEDS_PER_DIV; i++) {
    leds[down_left_leds[i]] = CRGB(brightness, 0, brightness);
  }
}

void v_set_up(int brightness) {
  for (int i = 0; i < LEDS_PER_DIV; i++) {
    leds[up_leds[i]] = CRGB(brightness, brightness, 0);
  }
}
void v_set_down(int brightness) {
  for (int i = 0; i < LEDS_PER_DIV; i++) {
    leds[down_leds[i]] = CRGB(0, brightness, brightness);
  }
}
void v_set_left(int brightness) {
  for (int i = 0; i < LEDS_PER_DIV; i++) {
    leds[left_leds[i]] = CRGB(brightness/2, brightness/2, brightness);
  }
}
void v_set_right(int brightness) {
  for (int i = 0; i < LEDS_PER_DIV; i++) {
    leds[right_leds[i]] = CRGB(brightness, brightness/2, brightness/2);
  }
}


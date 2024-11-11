#include <FastLED.h>
#include <arduino-timer.h>


#define LED_PIN 14    // Replace with the pin number you used for DATA connection
#define NUM_LEDS 24  // Replace with the number of LEDs in your strip
#define FADE_DELAY 1   // Delay between brightness changes (in milliseconds)
#define MAX_BRIGHTNESS 255 //255 // Maximum brightness value
#define LEDS_PER_DIV 4 
#define THRESHOLD 700
#define LED_BUILTIN 2


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

CRGB leds[NUM_LEDS];

const int knockSensor_up_right = 35; //pulldown -> 1.5M Ohm
const int knockSensor_down_right = 39;
const int knockSensor_up_left = 34;
const int knockSensor_down_left = 36;
const int knockSensor_center_down = 26;
const int knockSensor_center_up = 25;

uint16_t reads[10];

int ledState = LOW;         // variable used to store the last LED status, to toggle the light
 

const uint8_t down_right_leds[] = {1,2,3,4};
const uint8_t right_leds[]      = {4,5,6,7};
const uint8_t up_right_leds[]   = {7,8,9,10};
const uint8_t up_leds[]         = {10,11,12,13};
const uint8_t up_left_leds[]    = {13,14,15,16};
const uint8_t left_leds[]       = {16,17,18,19};
const uint8_t down_left_leds[]  = {19,20,21,22};
const uint8_t down_leds[]       = {22,23,0,1};

const uint8_t segments[][LEDS_PER_DIV] = {{1,2,3,4},
                                          {4,5,6,7},
                                          {7,8,9,10},
                                          {10,11,12,13},
                                          {13,14,15,16},
                                          {16,17,18,19},
                                          {19,20,21,22},
                                          {22,23,0,1}}; 

int state;
int prev_state;
int next_state;

Timer<10, millis> timer; 

int countdown_led;
Timer<10, millis>::Task task_countdown;

int actual_position = 0;
int pattern_1[] = {UP_RIGHT, DOWN_LEFT , DOWN_RIGHT, UP_LEFT};
int pattern_2[] = {UP_RIGHT, DOWN_LEFT , DOWN_RIGHT, UP_LEFT, RIGHT, LEFT, UP, DOWN, RIGHT, LEFT}; //Rosa hasta el 10
 
void setup() {
  state = 0;
  analogReadResolution(12);
  pinMode(LED_BUILTIN, OUTPUT); // declare the ledPin as as OUTPUT
  FastLED.addLeds<WS2811, LED_PIN, RGB>(leds, NUM_LEDS);
  Serial.begin(115200);       // use the serial port
  digitalWrite(LED_BUILTIN, HIGH);



  //breath();
  timer.every(50, state_machine);
  //timer_sensors.every(100, v_read_sensors);
  timer.every(10, publish_data);
}

void loop() {
  timer.tick();
  //v_read_sensors();

  /*
  v_read_sensors();
  


  

  v_set_down_right(read_to_brightness(down_right_read));
  v_set_up_right(read_to_brightness(up_right_read));
  v_set_down_left(read_to_brightness(down_left_read));
  v_set_up_left(read_to_brightness(up_left_read));

  v_set_up(read_to_brightness(interpolate_reads(up_left_read,up_right_read)));
  v_set_down(read_to_brightness(interpolate_reads(down_left_read,down_right_read)));
  v_set_left(read_to_brightness(interpolate_reads(up_left_read, down_left_read)));
  v_set_right(read_to_brightness(interpolate_reads(up_right_read,down_right_read)));
  */

  
  
  //delay(1);  // delay to avoid overloading the serial port buffer
}

bool state_machine(void *)
{
  prev_state = state;
  state = next_state;
  switch (state){
    case DEFAULT:     
        next_state = START;
        break;

    case START:
        countdown_led = NUM_LEDS;
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        task_countdown = timer.every(1000/NUM_LEDS, countdown);
        next_state = STARTING;
        break;

    case STARTING:
        if (countdown_led == 0){
          digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
          next_state = RUNNING;
          break;
        }
        
        break;
    case RUNNING:
        FastLED.clear();
        for (int i=0; i < LEDS_PER_DIV; i++ )
        {
            leds[segments[pattern_2[actual_position]][i]] = CRGB(MAX_BRIGHTNESS, 0, 0);
        }
        FastLED.show();
        delay(2);
        next_state = WAIT_HIT;
        break;
    case WAIT_HIT:
        if (reads[pattern_2[actual_position]] > THRESHOLD)
        {
          Serial.println("\n\n\n HIT! \n\n\n");
          FastLED.clear();
          for (int i=0; i < LEDS_PER_DIV; i++ )
          {
                leds[segments[pattern_2[actual_position]][i]] = CRGB(0, MAX_BRIGHTNESS, 0);
          }
          FastLED.show();
          delay(2);
          actual_position+=1;
          if (actual_position == sizeof(pattern_2)/sizeof(pattern_2[0]))
          {
              actual_position = 0;
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











uint16_t interpolate_reads(uint16_t sensor_read_1, uint16_t sensor_read_2){

  return (1.5*(sensor_read_1 + sensor_read_2)-1.2*(abs(sensor_read_1 - sensor_read_2)));

}

int read_to_brightness(uint16_t sensor_read){
  if (sensor_read > THRESHOLD)
  {
    return (int)255*sensor_read/1024;
  }
  else
  {
    return 0;
  }
}




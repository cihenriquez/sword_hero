bool b_bluetooth_comms(void *){
    if (deviceConnected) {
        pTxCharacteristic->setValue(String(state)+ ","+ String(hit_counter));
        pTxCharacteristic->notify();
		//delay(10); // bluetooth stack will go into congestion, if too many packets are sent
	}

    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
		// do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
    return true;
}



bool publish_data_sensors(void *)
{ 

  delay(1);
  for (int i=0; i<8; i++)
  {
      Serial.print(reads[i]);
      Serial.print(",");
  }

  Serial.print("\n");

  return true;
}

bool breath()
{
  
  for (int i=0; i<NUM_LEDS; i++){ 
      leds[i] = CRGB(brightness, brightness, brightness);
  }
  FastLED.show();
  delay(1);
  for (int i=0; i<8; i++)
  {
      if (reads[i]>THRESHOLD){return false;} 
  }


  if (brightness==int(MAX_BRIGHTNESS*0.2))
  {
    increasing=false;
  }
  if (brightness==5)
  {
    increasing=true;
  }

  if (increasing)
  {
      brightness+=1;
  }
  else
  {
      brightness-=1;
  }

  return true;

}

bool countdown(void *) {
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      if (i < countdown_led){
        leds[i] = CRGB(0, 0 , MAX_BRIGHTNESS);
      }
      else
      {
        leds[i] = CRGB(0, 0, 0);
      }
    }
    FastLED.show();
    if (countdown_led == 0)
    {
      return false;
    }
    countdown_led -= 1;
    return true;
}




bool b_read_sensors(void *)
{
  reads[UP_RIGHT] = analogRead(knockSensor_up_right);
  reads[UP_LEFT] = analogRead(knockSensor_up_left);
  reads[DOWN_RIGHT]  = analogRead(knockSensor_down_right);
  reads[DOWN_LEFT] = analogRead(knockSensor_down_left);
  reads[CENTER_DOWN] = analogRead(knockSensor_center_down);
  reads[CENTER_UP] = analogRead(knockSensor_center_up);
  reads[UP] = interpolate_reads(reads[UP_LEFT], reads[UP_RIGHT]);
  reads[DOWN] = interpolate_reads(reads[DOWN_LEFT],reads[DOWN_RIGHT]);
  reads[LEFT] = interpolate_reads(reads[UP_LEFT], reads[DOWN_LEFT]);
  reads[RIGHT] = interpolate_reads(reads[UP_RIGHT],reads[DOWN_RIGHT]);

  return true;
  
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
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
        if (DEBUG){Serial.println("start advertising");}
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
  int num_sensors = sizeof(reads) / sizeof(reads[0]); // Calcula el número de sensores dinámicamente
  
  for (int i = 0; i < num_sensors; i++)
  {
      Serial.print(reads[i]);
      if (i < num_sensors - 1)  // Evita imprimir la última coma
      {
          Serial.print(",");
      }
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
  reads[DOWN_RIGHT] = ads1.readADC_SingleEnded(0);
  reads[DOWN]       = ads2.readADC_SingleEnded(0); 
  reads[RIGHT]      = ads1.readADC_SingleEnded(1);
  reads[DOWN_LEFT]  = ads2.readADC_SingleEnded(1);
  reads[UP_RIGHT]   = ads1.readADC_SingleEnded(2);
  reads[LEFT]       = ads2.readADC_SingleEnded(2); 
  reads[UP]         = ads1.readADC_SingleEnded(3);
  reads[UP_LEFT]    = ads2.readADC_SingleEnded(3);
  
  
  

  //reads[CENTER_DOWN] = analogRead(PIN_SENSOR_CENTER_DOWN);
  //reads[CENTER_UP] = analogRead(PIN_SENSOR_CENTER_UP);
  
  
  if (DEBUG){publish_data_sensors(nullptr);}
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
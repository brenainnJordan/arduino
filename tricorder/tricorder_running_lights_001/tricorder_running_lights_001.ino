// tlc5940 running lights test

#include "Tlc5940.h"
#define BTN 7
#define BTN_INTERUPT 0

int btn_value = 0;
//volatile boolean state = false;
boolean state = false;

void setup() {
  Tlc.init(0); // Initiates the TLC5940 and set all channels off
  Serial.begin(9600);
  pinMode(BTN, INPUT);
  //attachInterrupt(BTN_INTERUPT, switch_state, FALLING);
}

void switch_state(){
  if(state){
    state = false;
    Serial.println("false");
  }
  else{
    state = true;
    Serial.println("true");
  }
}

void btn_pressed(){
  while (digitalRead(BTN) == HIGH)
    ; // do nothing

  if(state)
    state = false;
  else
    state = true;

  Serial.println(state);
}

void tricorder_loop(float delay_time){
  for (int i=1; i<=7; i++){
      if(digitalRead(BTN) == HIGH){
        btn_pressed();
        return;
      }
      
      Tlc.clear();
      Tlc.set(8-i, 4095);
      Tlc.set(7+i, 4095);
      Tlc.update();
      delay(delay_time);
  }
}

void trail(float slope){
  int leds = 14;
  int start_index = 1;
  int divisions = leds - 1;
  
  //float slope = 3.0;

  // main "travel" loop
  for (int i=0; i<=divisions; i++) {

    float x = (float)i/divisions;
    
    // for each iteration we want y at x to equal 1.0
    // so we need to calculate the offset (aka y-intercept)
    // to give us this value
    float offset = 1.0-(x*slope);

    // calculate values for each led
    for (int j=0; j<=divisions; j++) {
      if(digitalRead(BTN) == HIGH){
        btn_pressed();
        return;
      }
      
      int led = j + start_index;

      float i_x = (float)j/divisions;
      
      float y_value = (i_x*slope) + offset;

      //calculate "spill over"
      if (y_value > 1.0)
        y_value = y_value - slope;

      // clamp values
      if (y_value > 1.0)
        y_value = 1.0;

      if (y_value < 0.0)
        y_value = 0.0;

      // set chip value
      int value = y_value * 4095;
      Tlc.set(led, value);
    }

    Tlc.update();
    delay(50);
  }
}


void sin_wave(float freq, float phase_speed){
  int leds = 14;
  int start_index = 1;
  int divisions = leds - 1;
  
  //float freq = 2.0;
  //float phase_speed = 0.1;

  // phase loop
  for (float phase=0; phase < 2.0; phase += phase_speed) {
    
    // calculate values for each led
    for (int j=0; j<=divisions; j++) {
      
      int led = j + start_index;

      float x = (float)j/divisions;

      float y = sin((PI*x*freq) + (phase*PI));
      // remap y between 0 and 1
      y = (0.5*y)+0.5;
      // change curve
      y = y*y*y;
      
      // set chip value
      int value = y * 4095;
      Tlc.set(led, value);
    }

    Tlc.update();
    delay(50);
  }
}

void sin_wave_mid(){
  int leds = 14;
  int start_index = 1;
  int end_index = 7;
  int divisions = end_index - 1;
  
  float freq = 0.5;
  float phase_speed = 0.05;

  // phase loop
  for (float phase=0; phase < 2.0; phase += phase_speed) {
    
    // calculate values for each led
    for (int j=0; j<=divisions; j++) {
      
      int led = j + start_index;

      float x = (float)j/divisions;

      float y = sin((PI*x*freq) + (phase*PI));
      // remap y between 0 and 1
      y = (1.0+y)*0.5;
      // change curve
      //y = y*y;
      
      // set chip value
      int value = y * 4095;
      Tlc.set(led, value);

      // set opposite led
      Tlc.set(leds-led+1, value);
    }

    Tlc.update();
    delay(50);
  }
}


void sin_wave_mid_sharp(float freq, float phase_speed){
  int leds = 14;
  int start_index = 1;
  int end_index = 7;
  int divisions = end_index - 1;
  
  //float freq = 0.8;
  //float phase_speed = 0.06;

  // phase loop
  for (float phase=0; phase < 2.0; phase += phase_speed) {
    
    // calculate values for each led
    for (int j=0; j<=divisions; j++) {
      
      int led = j + start_index;

      float x = (float)j/divisions;

      float y = sin((PI*x*freq) + (phase*PI));
      if (y > 0.0){
        y = 0.0 - y;
      }
      
      // remap y between 0 and 1
      y += 1.0;
      
      // change curve
      y = y*y;
      
      // set chip value
      int value = y * 4095;
      Tlc.set(led, value);

      // set opposite led
      Tlc.set(leds-led+1, value);
    }

    Tlc.update();
    delay(50);
  }
}

void loop() {
  //btn_value = digitalRead(BTN);

  for(int i = 0; i <= 2; i++)
    sin_wave_mid_sharp(0.8, 0.03);
  
  for(int i = 0; i <= 4; i++)
    sin_wave_mid_sharp(0.9, 0.06);
    
  for(int i = 0; i <= 10; i++)
    tricorder_loop(120);

  for(int i = 0; i <= 5; i++)
    // TODO replace this with sawtooth wave
    trail(1.0);

  for(int i = 0; i <= 10; i++)
    sin_wave(1.0, 0.1);
  
  //if (state)
    //tricorder_loop();
  //else
    //trail();
}

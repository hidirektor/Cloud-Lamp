#include <Wire.h>
#include <FastLED.h>

#define LED_SAYISI 60
#define DATA_PIN 6


enum Mode { CLOUD,ACID,OFF,ON,RED,GREEN,BLUE,FADE};
Mode mode = CLOUD;  
Mode lastMode = CLOUD;

#define MIC_PIN   0
#define DC_OFFSET  0
#define NOISE     10
#define SAMPLES   10
byte
  volCount  = 0;
int
  vol[SAMPLES];
int      n, total = 30;
float average = 0;
  
int fade_h;
int fade_direction = 1;


CRGB leds[LED_SAYISI];

void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, LED_SAYISI); 
  memset(vol, 15, sizeof(vol));
  Serial.begin(115200);
  Wire.begin(9);
  Wire.onReceive(receiveEvent);
}



void receiveEvent(int bytes) {
  while(Wire.available())
   { 
      unsigned int received = Wire.read();
      Serial.print("Receiving IR hex: ");
      Serial.println(received,HEX);
      lastMode = mode;
      switch(received){
        case 0x3F:
          mode = ON; break;
        case 0xBF:
          mode = OFF; break;
        case 0x2F:
          mode = CLOUD; break;
        case 0xF:
          mode = ACID; break;
        case 0x37:
          mode = FADE; break;
        case 0x9F:
          mode = BLUE; break;
        case 0x5F:
          mode = GREEN; break;
        case 0xDF:
          mode = RED; break;
        
      }
   }

}
 
void loop() { 
  switch(mode){
    case CLOUD: detect_thunder();reset();break;
    case ACID: acid_cloud();reset();break;
    case OFF:reset();break;
    case ON: constant_lightning();reset();break;
    case RED: single_colour(0);break;
    case BLUE: single_colour(160);break;
    case GREEN: single_colour(96);break;
    case FADE: colour_fade();break;
    default: detect_thunder(); reset();break; 
  }
  
}

void single_colour(int H){
  for (int i=0;i<LED_SAYISI;i++){
    leds[i] = CHSV( H, 255, 255);
  }
  if(lastMode != mode){
    FastLED.show(); 
    lastMode = mode;
  }
  delay(50);
}

void colour_fade(){
  for (int i=0;i<LED_SAYISI;i++){
    leds[i] = CHSV( fade_h, 255, 255);
  }
  if(fade_h >254){
    fade_direction = -1;
  }
  else if(fade_h < 0){
    fade_direction = 1;
  }
    
  fade_h += fade_direction;
  FastLED.show();
  delay(100);
}



void detect_thunder() {
  n   = analogRead(MIC_PIN);
  n   = abs(n - 512 - DC_OFFSET);
  n   = (n <= NOISE) ? 0 : (n - NOISE);
  vol[volCount] = n;
  if(++volCount >= SAMPLES) volCount = 0;
 
  total = 0;
  for(int i=0; i<SAMPLES; i++) {
    total += vol[i];
  }
  
  // DEBUG BLOCK.
  
  /*
  for(int t=0; t<SAMPLES; t++) {
    Serial.print("Sample item ");
    Serial.print(t);
    Serial.print(":");
    Serial.println(vol[t]);
  }
  Serial.print("total");
  Serial.println(total);
  Serial.print("divided by sample size of ");
  Serial.println(SAMPLES);
  
 
  Serial.print("average:");
  Serial.println(average);
  Serial.print("current:");
  Serial.println(n);
  */
  
  average = (total/SAMPLES)+2;
  if(n>average){
    Serial.println("TRIGGERED");
    reset();
     
    switch(random(1,3)){
       //switch(3){
  
      case 1:
        thunderburst();
        delay(random(10,500));
         Serial.println("Thunderburst");
        break;
       
      case 2:
        rolling();
        Serial.println("Rolling");
        break;
        
      case 3:
        crack();
        delay(random(50,250));
        Serial.println("Crack");
        break;
        
      
    }
  }
}
 
void reset(){
  for (int i=0;i<LED_SAYISI;i++){
    leds[i] = CHSV( 0, 0, 0);
  }
  FastLED.show();
   
}

void acid_cloud(){
    for(int i=0;i<LED_SAYISI;i++){
      if(random(0,100)>90){
        leds[i] = CHSV( random(0,255), 255, 255); 

      }
      else{
        leds[i] = CHSV(0,0,0);
      }
    }
    FastLED.show();
    delay(random(5,100));
    reset();
}

void rolling(){
  for(int r=0;r<random(2,10);r++){
    for(int i=0;i<LED_SAYISI;i++){
      if(random(0,100)>90){
        leds[i] = CHSV( 0, 0, 255); 

      }
      else{
        leds[i] = CHSV(0,0,0);
      }
    }
    FastLED.show();
    delay(random(5,100));
    reset();
    
  }
}

void crack(){
   for(int i=0;i<LED_SAYISI;i++) {
      leds[i] = CHSV( 0, 0, 255);  
   }
   FastLED.show();
   delay(random(10,100));
   reset();
}

void thunderburst(){

  int rs1 = random(0,LED_SAYISI/2);
  int rl1 = random(10,20);
  int rs2 = random(rs1+rl1,LED_SAYISI);
  int rl2 = random(10,20);
  
  for(int r = 0;r<random(3,6);r++){
    
    for(int i=0;i< rl1; i++){
      leds[i+rs1] = CHSV( 0, 0, 255);
    }
    
    if(rs2+rl2 < LED_SAYISI){
      for(int i=0;i< rl2; i++){
        leds[i+rs2] = CHSV( 0, 0, 255);
      }
    }
    
    FastLED.show();
    delay(random(10,50));
    
    reset();
    delay(random(10,50));
  }
  
}
 
void constant_lightning(){
  switch(random(1,10)){
   case 1:
        thunderburst();
        delay(random(10,500));
         Serial.println("Thunderburst");
        break;
       
      case 2:
        rolling();
        Serial.println("Rolling");
        break;
        
      case 3:
        crack();
        delay(random(50,250));
        Serial.println("Crack");
        break;
        
    
  }  
}

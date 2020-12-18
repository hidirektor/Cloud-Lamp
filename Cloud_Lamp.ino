#include <Wire.h>
#include <FastLED.h>

#define LED_SAYISI 60
#define RGB_DATA 6


enum Modlar { 
  BULUT,ACID,OFF,ON,RED,GREEN,BLUE,FADE 
  };
  
Modlar mod = BULUT;  
Modlar sonMod = BULUT;

#define Mikrofon   0
#define DC_OFFSET  0
#define NOISE     10
#define SAMPLES   10
byte volCount  = 0;
int vol[SAMPLES];
int n, toplam = 30;
float ortalama = 0;
  
int fade_h;
int fade_direction = 1;


CRGB leds[LED_SAYISI];

void setup() {
  FastLED.addLeds<WS2812B, RGB_DATA, GRB>(leds, LED_SAYISI); 
  memset(vol, 15, sizeof(vol));
  Serial.begin(115200);
  Wire.begin(9);
  Wire.onReceive(receiveEvent);
}



void receiveEvent(int bytes) {
  while(Wire.available())
   { 
      unsigned int received = Wire.read();
      Serial.print("HEX: ");
      Serial.println(received,HEX);
      sonMod = mod;
      switch(received){
        case 0x3F:
          mod = ON; break;
        case 0xBF:
          mod = OFF; break;
        case 0x2F:
          mod = BULUT; break;
        case 0xF:
          mod = ACID; break;
        case 0x37:
          mod = FADE; break;
        case 0x9F:
          mod = BLUE; break;
        case 0x5F:
          mod = GREEN; break;
        case 0xDF:
          mod = RED; break;
        
      }
   }

}
 
void loop() {
  switch(mod){
    case BULUT:
      simsekAlgilama();
      reset();
      break;
    case ACID:
      bulut();
      reset();
      break;
    case OFF:
      reset();
      break;
    case ON:
      sabitSimsek();
      reset();
      break;
    case RED:
      tekRenk(0);
      break;
    case BLUE:
      tekRenk(160);
      break;
    case GREEN:
      tekRenk(96);
      break;
    case FADE:
      renkSolma();
      break;
    default:
      simsekAlgilama(); 
      reset();
      break; 
  }
}

void tekRenk(int H){
  for (int i=0;i<LED_SAYISI;i++){
    leds[i] = CHSV( H, 255, 255);
  }
  if(sonMod != mod){
    FastLED.show(); 
    sonMod = mod;
  }
  delay(50);
}

void renkSolma(){
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



void simsekAlgilama() {
  n   = analogRead(Mikrofon);
  n   = abs(n - 512 - DC_OFFSET);
  n   = (n <= NOISE) ? 0 : (n - NOISE);
  vol[volCount] = n;
  if(++volCount >= SAMPLES) volCount = 0;
 
  toplam = 0;
  for(int i=0; i<SAMPLES; i++) {
    toplam += vol[i];
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
  
  ortalama = (toplam/SAMPLES)+2;
  if(n>ortalama){
    reset();
    switch(random(1,3)){
      case 1:
        gokGurultusu();
        delay(random(10,500));
        break;
       
      case 2:
        gurleme();
        break;
        
      case 3:
        catlak();
        delay(random(50,250));
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

void bulut(){
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

void gurleme(){
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

void catlak(){
   for(int i=0;i<LED_SAYISI;i++) {
      leds[i] = CHSV( 0, 0, 255);  
   }
   FastLED.show();
   delay(random(10,100));
   reset();
}

void gokGurultusu(){

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
 
void sabitSimsek(){
  switch(random(1,10)){
   case 1:
        gokGurultusu();
        delay(random(10,500));
        break;
       
      case 2:
        gurleme();
        break;
        
      case 3:
        catlak();
        delay(random(50,250));
        break;
        
    
  }  
}

#include <Adafruit_NeoPixel.h>

#define NEO_PIXEL_PIN 3
#define IR_PIN 4

#define MAX_MODE 6
#define WRN_IDLE 5000
#define MAX_IDLE 6000

#define CLICK_DELAY 200

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(6, NEO_PIXEL_PIN, NEO_GRB + NEO_KHZ800);

volatile int mode = 0;
volatile int abortLoop = 0;

int counter = 0;
int irValue = 0;
int status = 0;
unsigned long lastCheck = 0;

void setup() 
{
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  pinMode( IR_PIN, INPUT);
  
  Serial.begin(9600);
  
  status = 2;
  lastCheck = millis();
  setAll(strip.Color(0,255,0));
  
  attachInterrupt(1, unclickButton, FALLING);
}

void loop() 
{
  counter++;

  if(mode == 0)
  {
    irValue = digitalRead(IR_PIN);
    
    if( irValue == HIGH && status != 0)
    {
      if(status != 0 )
        setAll(strip.Color(255,0,0));
        
      status = 0;   
      Serial.print(status, DEC);    
      lastCheck = millis();
    }
    else
    {
      if(millis() - lastCheck > MAX_IDLE && status == 1)
      {
        status = 2;
        setAll(strip.Color(0,255,0));      
        Serial.print(status, DEC);
      }
      else if(millis() - lastCheck > WRN_IDLE && status == 0)
      {
        status = 1;
        setAll(strip.Color(255,200,0));
        Serial.print(status, DEC);
      }
    }
  }
  else if(mode == 1)
  {
    colorWipe(strip.Color(255, 0, 0), 50); // Red
    colorWipe(strip.Color(0, 255, 0), 50); // Green
    colorWipe(strip.Color(0, 0, 255), 50); // Blue
  }
  else if(mode == 2)
  {
    theaterChase(strip.Color(0, 255, 0), 50); // White
    theaterChase(strip.Color(255,   0,   0), 50); // Red
    theaterChase(strip.Color(  0,   0, 255), 50); // Blue
  }
  else if(mode == 3)
  {
    rainbow(20);
  }
  else if(mode == 4)
  {
    rainbowCycle(20);
  }
  else if(mode == 5)
  {
    theaterChaseRainbow(50);
  }
  else if(mode == 6)
  {
    strobe(30);
  }
  
  if(abortLoop > 0)
  {
    abortLoop = 0; 
    //Serial.print("!"); Serial.println(mode, DEC);
  }
}

void unclickButton()
{
  static unsigned long debounceLast = 0;
  
  unsigned long debounceNow = millis();
  
  if(debounceNow - debounceLast > CLICK_DELAY)
  {    
    mode = mode + 1;
    abortLoop = 1;
      
    if(mode > MAX_MODE)
      mode = 0;
  }
    
  debounceLast = debounceNow;
}

void setAll(uint32_t c)
{
  for(uint16_t i=0; i<strip.numPixels(); i++) 
  {
    strip.setPixelColor(i, c);
  }
  
  strip.show();
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) 
{
  for(uint16_t i=0; i<strip.numPixels(); i++) 
  {
      if(abortLoop>0)
        return;
        
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void rainbow(uint8_t wait) 
{
  uint16_t i, j;

  for(j=0; j<256; j++) 
  {
    for(i=0; i<strip.numPixels(); i++) 
    {
      if(abortLoop>0)
        return;
        
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) 
{
  uint16_t i, j;

  for(j=0; j<256*5; j++) 
  { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) 
    {
      if(abortLoop>0)
        return;
        
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) 
{
  for (int j=0; j<10; j++) 
  {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) 
    {
      for (int i=0; i < strip.numPixels(); i=i+3) 
      {
        if(abortLoop>0)
          return;
        
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
     
      delay(wait);
     
      for (int i=0; i < strip.numPixels(); i=i+3) 
      {
        if(abortLoop>0)
          return;
          
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) 
{
  for (int j=0; j < 256; j++) 
  {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) 
    {
        for (int i=0; i < strip.numPixels(); i=i+3) 
        {
          if(abortLoop>0)
            return;
        
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();
       
        delay(wait);
       
        for (int i=0; i < strip.numPixels(); i=i+3) 
        {
          if(abortLoop>0)
            return;
        
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

//Theatre-style crawling lights.
void strobe(uint8_t wait) 
{  
  for (int j=0; j<100; j++) 
  {
    if(abortLoop)
      return;
      
    setAll(strip.Color(255, 255, 255));
    delay(wait);
    setAll(strip.Color(0, 0, 0));
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) 
{
  if(WheelPos < 85) 
  {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if(WheelPos < 170) 
  {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else 
  {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}


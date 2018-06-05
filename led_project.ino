#include <Adafruit_NeoPixel.h>
#include <math.h>

//consts definition for led strip
#define ledsNumber 8
#define ledsDinPin A0
#define ledsVersion NEO_GRB + NEO_KHZ800

//consts definition for brightness levels
#define brightnessMin 1
#define brightnessMax 255
#define minK 2
#define maxK 6

//consts definition for encoder
#define encoderOutputAPin 5
#define encoderOutputBPin 6
#define encoderButtonPin 7
#define bPressed LOW
#define bNotPressed HIGH

#define effectsNumber 9

byte brightnessLevels = 0;
int brightnessChangePerLed = 0;
double currentK = minK;

void changeKParameter();

typedef struct {
  const int debounce = 300;
  const int buttonHoldTime = 1000;
  
  unsigned long holdTime = 0;
  byte clicksCounter = 1;
  bool buttonPrevState = bNotPressed;
  bool buttonCurrentState;

  unsigned long rotationsCounter = 0;
  int aState;
  int aLastState;

  void RotationHandler() {
    aState = digitalRead(encoderOutputAPin);
    if(aState != aLastState) {
      if(digitalRead(encoderOutputBPin) != aState) {
        if(rotationsCounter < (brightnessLevels - ledsNumber))
          ++rotationsCounter;
      }
      else {
        if(rotationsCounter > 0)
          --rotationsCounter;
      }
      aLastState = aState;
    }
  }

  void ButtonHandler() {
    buttonCurrentState = digitalRead(encoderButtonPin);
    if(buttonCurrentState != buttonPrevState) {
      delay(10);
      buttonCurrentState = digitalRead(encoderButtonPin);
      if(buttonCurrentState == bPressed)
        holdTime = millis();
      if(buttonCurrentState == bNotPressed) {
        if(millis() - holdTime >= buttonHoldTime)
            changeKParameter();
        else {
          ++clicksCounter;
          if(clicksCounter > effectsNumber)
            clicksCounter = 1;
        }
      }
      buttonPrevState = buttonCurrentState;
    }
  }
  
} Encoder;

Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(ledsNumber, ledsDinPin, ledsVersion);
Encoder encoder;

void setup() {
  pinMode(encoderOutputAPin, INPUT);
  pinMode(encoderOutputBPin, INPUT);
  pinMode(encoderButtonPin, INPUT);
  
  encoder.aLastState = digitalRead(encoderOutputAPin);
  Serial.begin(9600);
  setBrightnessLevels(currentK);
  ledStrip.begin();
  ledStrip.show();
  ledStrip.clear();
}

void loop() {
  encoder.RotationHandler();
  encoder.ButtonHandler();
  
  switch(encoder.clicksCounter) {
    case 1:
      redEffect();
      break;
     case 2:
      greenEffect();
      break;
     case 3:
      blueEffect();
      break;
     case 4:
      redGreenEffect();
      break;
     case 5:
      redBlueEffect();
      break;
     case 6:
      greenBlueEffect();
      break;
     case 7:
      rainbowEffectWithScrolling();
      break;
     case 8:
      policeEffect();
      break;
     case 9:
      meteorRainEffect(255, 255, 255, 1, 200, 100);
      break;
  }
}

void changeKParameter() {
  byte prevK = currentK;
  encoder.holdTime = 0;
  encoder.buttonPrevState = bNotPressed;
  
  while(true) {
    ledStrip.clear();
    for(int i = 0; i < currentK; ++i)
      ledStrip.setPixelColor(i, ledStrip.Color(255, 255, 255));
    ledStrip.show();
    
    encoder.aState = digitalRead(encoderOutputAPin);
    if(encoder.aState != encoder.aLastState) {
      if(digitalRead(encoderOutputBPin) != encoder.aState) {
        if(currentK < maxK)
          currentK += 0.5;
      }
      else {
        if(currentK > minK)
          currentK -= 0.5;
      }
      encoder.aLastState = encoder.aState;
    }
    
    encoder.buttonCurrentState = digitalRead(encoderButtonPin);
    if(encoder.buttonCurrentState != encoder.buttonPrevState) {
      delay(10);
      encoder.buttonCurrentState = digitalRead(encoderButtonPin);
      if(encoder.buttonCurrentState == bPressed)
        encoder.holdTime = millis();
      if(encoder.buttonCurrentState == bNotPressed) {
        if(millis() - encoder.holdTime >= encoder.buttonHoldTime)
          break;    
        else {
          currentK = prevK;
          break;
        }
      }
      encoder.buttonPrevState = encoder.buttonCurrentState;
    }
  }

  setBrightnessLevels(currentK);
  encoder.rotationsCounter = 0;
}

void setBrightnessLevels(byte K) {
  brightnessLevels = pow(2,K) * 2 + 1; //prevent round-off error, because brightnessLevels is of type double
  Serial.println(brightnessLevels);
  brightnessChangePerLed = (brightnessMax - brightnessMin) / (brightnessLevels - 1);
}

void redEffect() {
  ledStrip.clear();
  
  for(byte i = 0; i < ledsNumber; ++i)
    ledStrip.setPixelColor(i, ledStrip.Color((encoder.rotationsCounter + i) * brightnessChangePerLed + brightnessMin, 0, 0));
  ledStrip.show();
}

void greenEffect() {
  ledStrip.clear();
  
  for(byte i = 0; i < ledsNumber; ++i)
    ledStrip.setPixelColor(i, ledStrip.Color(0, (encoder.rotationsCounter + i) * brightnessChangePerLed + brightnessMin, 0));
  ledStrip.show();
}

void blueEffect() {
  ledStrip.clear();
  
  for(byte i = 0; i < ledsNumber; ++i)
    ledStrip.setPixelColor(i, ledStrip.Color(0, 0, (encoder.rotationsCounter + i) * brightnessChangePerLed + brightnessMin));
  ledStrip.show();
}

void redGreenEffect() {
  ledStrip.clear();
  
  for(byte i = 0; i < ledsNumber; ++i) {
    if(i % 2 == 0)
      ledStrip.setPixelColor(i, ledStrip.Color((encoder.rotationsCounter + i) * brightnessChangePerLed + brightnessMin, 0, 0));
    else 
      ledStrip.setPixelColor(i, ledStrip.Color(0, (encoder.rotationsCounter + i) * brightnessChangePerLed + brightnessMin, 0));
  }
  ledStrip.show();
}

void redBlueEffect() {
  ledStrip.clear();
  
  for(byte i = 0; i < ledsNumber; ++i) {
    if(i % 2 == 0)
      ledStrip.setPixelColor(i, ledStrip.Color((encoder.rotationsCounter + i) * brightnessChangePerLed + brightnessMin, 0, 0));
     else
      ledStrip.setPixelColor(i, ledStrip.Color(0, 0, (encoder.rotationsCounter + i) * brightnessChangePerLed + brightnessMin));
  }
  ledStrip.show();
}

void greenBlueEffect() {
  ledStrip.clear();
  
  for(byte i = 0; i < ledsNumber; ++i) {
    if(i % 2 == 0)
      ledStrip.setPixelColor(i, ledStrip.Color(0, (encoder.rotationsCounter + i) * brightnessChangePerLed + brightnessMin, 0));
     else
      ledStrip.setPixelColor(i, ledStrip.Color(0, 0, (encoder.rotationsCounter + i) * brightnessChangePerLed + brightnessMin));
  }
  ledStrip.show();
}

void policeEffect() {
  ledStrip.clear();
  
  for(byte i = 0; i < ledsNumber; ++i)
    ledStrip.setPixelColor(i, ledStrip.Color(255, 0, 0));
  ledStrip.show();
  delay(100);

  for(byte i = 0; i < ledsNumber; ++i)
    ledStrip.setPixelColor(i, ledStrip.Color(0, 0, 255));
  ledStrip.show();
  delay(100);
}

void rainbowEffectWithScrolling() {
  unsigned int counter = 0;
  byte r = 255, g = 255, b = 255;
  byte prevClicksCounter = encoder.clicksCounter;
  changeLedStripColor(255, 0, 0);
  bool clockwise;
  
  while(true) {
    encoder.ButtonHandler();
    if(encoder.clicksCounter != prevClicksCounter)
      break;
    encoder.aState = digitalRead(encoderOutputAPin);
    if(encoder.aState != encoder.aLastState) {
      if(digitalRead(encoderOutputBPin) != encoder.aState) {
        if(counter < 1530)
          ++counter;
        if(counter >= 1530) {
          counter = 0;
          r = 255;
          g = 255;
          b = 255;
        }
        clockwise = true;
      }
      else {
        if(counter > 0)
          --counter;
        clockwise = false;
      }

      if(counter <= 255)
        changeLedStripColor(255, counter, 0);
      else if(counter <= 510) {
        if(clockwise)
          --r;
        else 
          ++r;
        changeLedStripColor(r, 255, 0);
      }
      else if(counter <= 765)
        changeLedStripColor(0, 255, counter % 255);
      else if(counter <= 1020) {
        if(clockwise)
          --g;
        else
          ++g;
        changeLedStripColor(0, g, 255);
      }
      else if(counter <= 1275)
        changeLedStripColor(counter % 255, 0, 255);
      else {
        if(clockwise)
          --b;
        else
          ++b;
        changeLedStripColor(255, 0, b);
      }
      encoder.aLastState = encoder.aState;
    }
  }
}

void changeLedStripColor(byte r, byte g, byte b) {
  for(byte i = 0; i < ledsNumber; ++i)
    ledStrip.setPixelColor(i, ledStrip.Color(r, g, b));
  ledStrip.show();
}

void meteorRainEffect(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, int speedDelay) {  
  ledStrip.clear();

  byte prevClicksCounter = encoder.clicksCounter;
  
  for(byte i = 0; i < ledsNumber+ledsNumber; ++i) {
    encoder.ButtonHandler();
    if(encoder.clicksCounter != prevClicksCounter)
      break;
    // fade brightness all LEDs one step
    for(byte j = 0; j < ledsNumber; ++j) {
        fadeToBlack(j, meteorTrailDecay);        
    }
    // draw meteor
    for(byte j = 0; j < meteorSize; j++) {
      if( (i - j < ledsNumber) && (i - j >= 0) ) {
        ledStrip.setPixelColor(i - j, ledStrip.Color(red, green, blue));
      } 
    }
   
    ledStrip.show();
    delay(speedDelay);
  }
}

void fadeToBlack(byte ledNo, byte fadeValue) {
  byte oldColor;
  byte r, g, b;
  
  oldColor = ledStrip.getPixelColor(ledNo);
  r = (oldColor & 0x00ff0000UL) >> 16;
  g = (oldColor & 0x0000ff00UL) >> 8;
  b = (oldColor & 0x000000ffUL);

  r=(r<=10)? 0 : (int) r-(r*fadeValue/256);
  g=(g<=10)? 0 : (int) g-(g*fadeValue/256);
  b=(b<=10)? 0 : (int) b-(b*fadeValue/256);
  
  ledStrip.setPixelColor(ledNo, r, g, b);
}

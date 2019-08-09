/*
*  Include libraries needed for the project
*/

#include "FastLED.h"
#include <FastLED.h>
#include <FHT.h> // include the library

/*
*  Constant declarations
*/
#define OCTAVE 1 //   // Group buckets into octaves  (use the log output function LOG_OUT 1)
#define OCT_NORM 0 // Don't normalise octave intensities by number of bins
#define FHT_N 256 // set to 256 point fht

#define LED_PIN     5
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define FRAMES_PER_SECOND  120

#define SWITCH_PIN_RGB_OVERRIDE       10
#define SWITCH_PIN_RED                9
#define SWITCH_PIN_GREEN              8
#define SWITCH_PIN_BLUE               7
#define SWITCH_PIN_RAINBOW_SUITE      6

#define MAX_BRIGHTNESS 150      // Thats full on, watch the power!
#define MIN_BRIGHTNESS 32       // set to a minimum of 25%

// Params for width and height
const uint8_t kMatrixWidth = 11;
const uint8_t kMatrixHeight = 27;
const uint8_t gHue = 0; // rotating "base color" used by many of the patterns

#define NUM_LEDS (kMatrixWidth * kMatrixHeight)

// Param for different pixel layouts
// Set 'kMatrixSerpentineLayout' to false if your pixels are
// laid out all running the same way, like this:

// Set 'kMatrixSerpentineLayout' to true if your pixels are
// laid out back-and-forth, like this:
const bool    kMatrixSerpentineLayout = true;

/*
*  Variables declarations
*/
int noise[] = {204,198,100,85,85,80,80,80};
float noise_fact[] = {15, 7, 1.5, 1, 1.2, 1.4, 1.7,3}; // noise level determined by playing pink noise and seeing levels [trial and error]{204,188,68,73,150,98,88,68}
float noise_fact_adj[] = {15, 7, 1.5, 1, 1.2, 1.4, 1.7,3}; // noise level determined by playing pink noise and seeing levels [trial and error]{204,188,68,73,150,98,88,68}

byte selectedEffect=0;

CRGB leds[NUM_LEDS];

int counter2=0;



/*=======================================================================
*
*     Program setup
*
=======================================================================*/
void setup() {

  //initialize serial output
  Serial.begin(9600);
  delay(1000);

  //FastLED initialization
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(MAX_BRIGHTNESS);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();

  // TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0

  //Switch initialization
  pinMode(SWITCH_PIN_RGB_OVERRIDE, INPUT_PULLUP);
  pinMode(SWITCH_PIN_GREEN, INPUT_PULLUP);
  pinMode(SWITCH_PIN_RED, INPUT_PULLUP);
  pinMode(SWITCH_PIN_BLUE, INPUT_PULLUP);
  pinMode(SWITCH_PIN_RAINBOW_SUITE, INPUT_PULLUP);
}



/*=======================================================================
*
*     Program main function
*
=======================================================================*/
void loop() {


  while (1) { // reduces jitter



    /*
    *    Handling first mode : RGB
    *
    *    This mode displays the color defined by three different switches
    */
    if(!digitalRead (SWITCH_PIN_RGB_OVERRIDE)){

      if(!digitalRead (SWITCH_PIN_RED) && !digitalRead (SWITCH_PIN_GREEN) && !digitalRead (SWITCH_PIN_BLUE)){
        //White
        fill_solid(leds, NUM_LEDS, CRGB::White);
        FastLED.show();
      }
      if(!digitalRead (SWITCH_PIN_RED) && digitalRead (SWITCH_PIN_GREEN) && digitalRead (SWITCH_PIN_BLUE)){
        //Red
        fill_solid(leds, NUM_LEDS, CRGB::Red);
        FastLED.show();
      }
      if(digitalRead (SWITCH_PIN_RED) && !digitalRead (SWITCH_PIN_GREEN) && digitalRead (SWITCH_PIN_BLUE)){
        //Green
        fill_solid(leds, NUM_LEDS, CRGB::Lime);
        FastLED.show();
      }
      if(digitalRead (SWITCH_PIN_RED) && digitalRead (SWITCH_PIN_GREEN) && !digitalRead (SWITCH_PIN_BLUE)){
        //Blue
        fill_solid(leds, NUM_LEDS, CRGB::Blue);
        FastLED.show();
      }
      if(!digitalRead (SWITCH_PIN_RED) && !digitalRead (SWITCH_PIN_GREEN) && digitalRead (SWITCH_PIN_BLUE)){
        //Yellow
        fill_solid(leds, NUM_LEDS, CRGB::Yellow);
        FastLED.show();
      }
      if(digitalRead (SWITCH_PIN_RED) && !digitalRead (SWITCH_PIN_GREEN) && !digitalRead (SWITCH_PIN_BLUE)){
        //Cyan
        fill_solid(leds, NUM_LEDS, CRGB::Cyan);
        FastLED.show();
      }
      if(!digitalRead (SWITCH_PIN_RED) && digitalRead (SWITCH_PIN_GREEN) && !digitalRead (SWITCH_PIN_BLUE)){
        //Magenta
        fill_solid(leds, NUM_LEDS, CRGB::Magenta);
        FastLED.show();
      }
    }

    /**
    *  This condition handles the "override mode". The default mode is RGB,
    *  once you are in override you can choose between the soundsensor function or the loopSuite
    *
    *  This is handled by another switch
    **/
    else{
      if(!digitalRead (SWITCH_PIN_RAINBOW_SUITE)){
        soundsensorfunction();
      }
      else{
        loopsuitefunction();
      }
    }
  } // End of while loop
} //End of void loop


/*=======================================================================
*
*     Loop Suite
*
=======================================================================*/

/**
*  this function defines an effect loop on the strip
**/
void loopsuitefunction() {
  for(i=0; i <= 10; i++){
    sinelon_B();
    FastLED.show();
  }

  for(i=0; i <= 10; i++){
    rainbowWithGlitter();
    FastLED.show();
  }
}

/*=======================================================================
*
*     Effect functions list
*
=======================================================================*/

void rainbow() {
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() {
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter(fract8 chanceOfGlitter) {
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}


void confetti() {
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}


void sinelon_B() {
  // a colored dot sweeping back and forth, with fading trails
  int Hue = 155;
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(40,0,NUM_LEDS);
  leds[pos] += CHSV( Hue, 255, 200);
}



void bpm() {
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}



void juggle_B() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 155;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16(i+7,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
    //dothue += 32;
  }
}



void confetti_GB() {
  // random colored speckles, Green and Blue hues only
  // Green = 96, Blue = 160
  uint8_t p = 60;  // What percentage of the time to make speckles.  [Range 0-100]

  fadeToBlackBy( leds, NUM_LEDS, 10);
  if (random8(100) < p) {
    int pos = random16(NUM_LEDS);
    uint8_t hue = random8(2);  // randomly chooses a 0 or 1
    if (hue == 0) {
      hue = random8(92,111);  // pick a hue somewhere in the green zone
    } else {
      hue = random8(156,165);  // pick a hue somewhere in the blue zone
    }
    leds[pos] += CHSV( hue, random8(200,240), 255);
  }
}


/*=======================================================================
*
*     Sound sensor functions
*
=======================================================================*/

/**
*   This function displays a soun reactive rainbow like effect on the LED strip
**/
void soundsensorfunction() {

  int prev_j[8];
  int beat=0;
  int prev_oct_j;
  int counter=0;
  int prev_beat=0;
  int led_index=0;
  int saturation=0;
  int saturation_prev=0;
  int brightness=0;
  int brightness_prev=0;

  cli();  // UDRE interrupt slows this way down on arduino1.0

  for (int i = 0 ; i < FHT_N ; i++) { // save 256 samples
    while (!(ADCSRA & 0x10)); // wait for adc to be ready
    ADCSRA = 0xf5; // restart adc
    byte m = ADCL; // fetch adc data
    byte j = ADCH;
    int k = (j << 8) | m; // form into an int
    k -= 0x0200; // form into a signed int
    k <<= 6; // form into a 16b signed int
    fht_input[i] = k; // put real data into bins
  }

  fht_window(); // window the data for better frequency response
  fht_reorder(); // reorder the data before doing the fht
  fht_run(); // process the data in the fht
  fht_mag_octave(); // take the output of the fht  fht_mag_log()

  // every 50th loop, adjust the volume accourding to the value on A2 (Pot)
  if (counter >= 50) {
    ADMUX = 0x40 | (1 & 0x07); // set admux to look at Analogpin A1 - Master Volume
    while (!(ADCSRA & 0x10)); // wait for adc to be ready
    ADCSRA = 0xf5; // restart adc

    delay(10);

    while (!(ADCSRA & 0x10)); // wait for adc to be ready

    ADCSRA = 0xf5; // restart adc
    byte m = ADCL; // fetch adc data
    byte j = ADCH;
    int k = (j << 8) | m; // form into an int
    float master_volume=(k+0.1)/1000 +.5;  // so the valu will be between ~0.5 and 1.5
    // Serial.println (master_volume);

    for (int i=1; i<8; i++) {
        noise_fact_adj[i]=noise_fact[i]*master_volume;
    }

    ADMUX = 0x40 | (0 & 0x07); // set admux back to look at A0 analog pin (to read the microphone input
    counter = 0;
  }

  sei();
  counter++;

  // End of Fourier Transform code - output is stored in fht_oct_out[i].
  // i=0-7 frequency (octave) bins (don't use 0 or 1), fht_oct_out[1]= amplitude of frequency for bin 1
  // for loop a) removes background noise average and takes absolute value b) low / high pass filter as still very noisy
  // c) maps amplitude of octave to a colour between blue and red d) sets pixel colour to amplitude of each frequency (octave)

  for (int i = 1; i < 8; i++) {  // goes through each octave. skip the first 1, which is not useful
    int j;
    j = (fht_oct_out[i] - noise[i]); // take the pink noise average level out, take the asbolute value to avoid negative numbers
    if (j<10) {j=0;}
    j= j*noise_fact_adj[i];

    if (j<10) {j=0;}
    else {
      j= j*noise_fact_adj[i];
      if (j>180) {
        if (i>=7) {
          beat+=2;
        }
        else {
          beat+=1;
        }
      }
      j=j/30;
      j=j*30; // (force it to more discrete values)
    }
    prev_j[i]=j;
    //Serial.print(j);
    //Serial.print(" ");
    // this fills in 11 LED's with interpolated values between each of the 8 OCT values
    if (i>=2) {
      led_index=2*i-3;
      prev_oct_j=(j+prev_j[i-1])/2;

      saturation=constrain(j+30, 0,255);
      saturation_prev=constrain(prev_oct_j+30, 0,255);
      brightness=constrain(j, 0,255);
      brightness_prev=constrain(prev_oct_j, 0,255);

      if (brightness==255) {
        saturation=50;
        brightness=200;
      }

      if (brightness_prev==255) {
        saturation_prev=50;
        brightness_prev=200;
      }

      for (uint8_t y=0;y<kMatrixHeight;y++){
        leds[XY(led_index-1,y)] = CHSV(j+y*30,saturation, brightness);
        if (i>2){
          prev_oct_j=(j+prev_j[i-1])/2;
          leds[ XY(led_index-2,y)]=CHSV(prev_oct_j+y*30,saturation_prev, brightness_prev);
        }
      }
    }
  }

  if (beat>=7) {
      fill_solid(leds, NUM_LEDS, CRGB::Gray);
      FastLED.setBrightness(120);
      //    FastLED.setBrightness(200);
  }
  else {
    if (prev_beat!=beat) {
      FastLED.setBrightness(40+beat*beat*5);
      prev_beat=beat;
    }
  }

  FastLED.show();
  if (beat) {
    counter2+=((beat+4)/2-2);
    if (counter2<0) {counter2=1000;}
    if (beat>3 && beat<7) {
       FastLED.delay (20);
    }
    beat=0;
  }
  // Serial.println();
}



/*
*
*/
uint16_t XY( uint8_t x, uint8_t y) {
  uint16_t i;

  if( kMatrixSerpentineLayout == false) {
    i = (y * kMatrixWidth) + x;
  }

  if( kMatrixSerpentineLayout == true) {
    if( y & 0x01) {
      // Odd rows run backwards
      uint8_t reverseX = (kMatrixWidth - 1) - x;
      i = (y * kMatrixWidth) + reverseX;

    } else {
      // Even rows run forwards
      i = (y * kMatrixWidth) + x;

    }
  }

  i=(i+counter2)%NUM_LEDS;
  return i;
}

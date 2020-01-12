#include <FastLED.h>
#include <IRremote.h>


#define REMOTE_TYPE 1  // Samsung
//#define REMOTE_TYPE 2  // IR_remote


#if REMOTE_TYPE == 1
#define BUTT_UP     0xE0E006F9
#define BUTT_DOWN   0xE0E08679
#define BUTT_LEFT   0xE0E0A659
#define BUTT_RIGHT  0xE0E046B9
#define BUTT_OK     0xE0E016E9
#define BUTT_1      0xE0E020DF
#define BUTT_2      0xE0E0A05F
#define BUTT_3      0xE0E0609F
#define BUTT_4      0xE0E010EF
#define BUTT_5      0xE0E0906F
#define BUTT_6      0xE0E050AF
#define BUTT_7      0xE0E030CF
#define BUTT_8      0xE0E0B04F
#define BUTT_9      0xE0E0708F
#define BUTT_0      0xE0E08877
#endif

#if REMOTE_TYPE == 2
#define BUTT_UP     0xFF18E7
#define BUTT_DOWN   0xFF4AB5
#define BUTT_LEFT   0xFF10EF
#define BUTT_RIGHT  0xFF5AA5
#define BUTT_OK     0xFF38C7
#define BUTT_1      0xFFA25D
#define BUTT_2      0xFF629D
#define BUTT_3      0xFFE21D
#define BUTT_4      0xFF22DD
#define BUTT_5      0xFF02FD
#define BUTT_6      0xFFC23D
#define BUTT_7      0xFFE01F
#define BUTT_8      0xFFA857
#define BUTT_9      0xFF906F
#define BUTT_0      0xFF9867
#define BUTT_STAR   0xFF6897
#define BUTT_HASH   0xFFB04F
#endif


FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few
// of the kinds of animation patterns you can quickly and easily
// compose using FastLED.
//
// This example also shows one easy way to define multiple
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    2
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    24
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

IRrecv irrecv(3); // указываем вывод, к которому подключен приемник
decode_results results;

void setup() {
  delay(3000); // 3 second delay for recovery

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  irrecv.enableIRIn(); // запускаем прием
  //  Serial.begin(9600);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void loop()
{
  remoteTick();  // опрос ИК пульта

  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  if (irrecv.isIdle())
  {
    FastLED.show();
    // insert a delay to keep the framerate modest
    //    FastLED.delay(1000/FRAMES_PER_SECOND);
    delay(1000 / FRAMES_PER_SECOND);

    // do some periodic updates
    EVERY_N_MILLISECONDS( 20 ) {
      gHue++;  // slowly cycle the "base color" through the rainbow
    }
    //  EVERY_N_MILLISECONDS( 10 ) {   remoteTick(); }  // опрос ИК пульта
    //  EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
  }
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter)
{
  if ( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS - 1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for ( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for ( int i = 0; i < 8; i++) {
    leds[beatsin16( i + 7, 0, NUM_LEDS - 1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}


void remoteTick() {
  if (irrecv.decode(&results)) { // если данные пришли
    auto data = results.value;
    //    Serial.println(data, HEX);
    switch (data) {
      // режимы
      case BUTT_1: gCurrentPatternNumber = 1;
        break;
      case BUTT_2: gCurrentPatternNumber = 2;
        break;
      case BUTT_3: gCurrentPatternNumber = 3;
        break;
      case BUTT_4: gCurrentPatternNumber = 4;
        break;
      case BUTT_5: gCurrentPatternNumber = 5;
        break;
      case BUTT_6: gCurrentPatternNumber = 6;
        break;
      case BUTT_7: gCurrentPatternNumber = 7;
        break;
      case BUTT_8: gCurrentPatternNumber = 8;
        break;
      case BUTT_9: gCurrentPatternNumber = 9;
        break;
      case BUTT_0: gCurrentPatternNumber = 0;
        break;
    }

    gCurrentPatternNumber %= ARRAY_SIZE(gPatterns);
    //    Serial.println(gCurrentPatternNumber);
    irrecv.resume(); // принимаем следующую команду
  }
}

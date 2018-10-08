#include "FastLED.h"
// How many leds in your strip?
#define NUM_LEDS 150
// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 3
#define ROW_MAX 9
//#define CLOCK_PIN 5
// Define the array of leds
CRGB leds[NUM_LEDS];

void HandleTaster_A(void);
void HandleTaster_B(void);
void HandleSchalter(void);
void set_Demo_night(void);
void set_Demo_day(void);
void setLightWindow(CRGB newLight, int fade);
void setLightKitchen(CRGB newLight, int fade);
void setLightRoom(CRGB newLight, int fade);
void setLightWork(CRGB newLight, int fade);
void setLightWachter(CRGB newLight, int fade);
void setDemoLight( );

enum systemTime_en {
  night,
  day
};

enum lightState_en {
  on,
  off
};

struct lightCharacter {
  CRGB color;
  int fade;
  lightState_en state;
};




struct systemLight_st {
  lightCharacter background;
  lightCharacter outdoor;
};


struct systemInformation_st {
  const int fade_max = 200;
  const systemLight_st dayLight =   (systemLight_st) {
    (lightCharacter) {CRGB::Gold, 0, on } , (lightCharacter) {
      CRGB::MidnightBlue , 55, on
    }
  };
  const systemLight_st nightLight = (systemLight_st) {
    (lightCharacter) {CRGB::Black, 0, on } , (lightCharacter) {
      CRGB::PaleTurquoise , 50, on
    }
  };;
  const lightCharacter roomLight = (lightCharacter) {
    0xFFF4E5, 0, off
  };
  const lightCharacter workLight = (lightCharacter) {
    0xFFF4E5, 0, off
  };
  systemTime_en currentTime = day;
  bool changeDemoSetup = false;
  lightCharacter currentWorkLight = (lightCharacter) {
    CRGB::Black, 0, off
  }  ;
  lightCharacter currentKitchenLight = (lightCharacter) {
    CRGB::Black, 0, off
  } ;
  lightCharacter currentRoomLight = (lightCharacter) {
    CRGB::Black, 0, off
  };
};


static systemInformation_st SystemInformation;

void setup() {

  Serial.begin(38400);

  // Uncomment/edit one of the following lines for your leds arrangement.
  // FastLED.addLeds<TM1803, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<TM1804, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<TM1809, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  // FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  // FastLED.addLeds<APA104, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<UCS1903, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<UCS1903B, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<GW6205, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<GW6205_400, DATA_PIN, RGB>(leds, NUM_LEDS);

  // FastLED.addLeds<WS2801, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<SM16716, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<LPD8806, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<P9813, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<APA102, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<DOTSTAR, RGB>(leds, NUM_LEDS);
  //FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<SM16716, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<LPD8806, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<P9813, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<DOTSTAR, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);

  set_Demo_day();


  pinMode(4, INPUT_PULLUP);  
  pinMode(5, INPUT);  
  pinMode(6, INPUT);  

  pinMode(7, OUTPUT);         
  pinMode(8, OUTPUT);          
  pinMode(9, OUTPUT);          

  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);

  Serial.println(" comands: " );
  Serial.println(" s - read Demo status");
  Serial.println(" n - set Demo to day ");
  Serial.println(" m - set Demo to night ");
  Serial.println(" o - set Demo 230 system DANGERZONE " );
  Serial.println(" p - unset Demo 230 system  ");
  Serial.println(" h - print help ");
  Serial.println(" c - clear consol ");

  return;
}


char rx_byte = 0;

int period = 10;
unsigned long time_now = 0;

     static int fade = 0;
   static int fade_min = 0;
   static int fade_max = 100;

   
void loop() {

  //  ACS712read(1);
  //  Serial.println(ACS712read(100),3);
  //  delay(500);

  HandleSchalter();

  HandleTaster_A();

  HandleTaster_B();

  HandleSerial();


  
  if (true == SystemInformation.changeDemoSetup)
  {
    if ( day ==  SystemInformation.currentTime)
    {
      set_Demo_day();
      Serial.println("Demo set to Day");
    } else {
      set_Demo_night();
      Serial.println("Demo set to Night");
    }
    SystemInformation.changeDemoSetup = false;
    
    fade = SystemInformation.currentKitchenLight.fade;
    
  }

  if ((  off   == SystemInformation.currentWorkLight.state)
  &&  ( night == SystemInformation.currentTime))
  {
    
    SystemInformation.currentWorkLight.color = SystemInformation.currentKitchenLight.color;
    SystemInformation.currentWorkLight.fade = 230;

   
   if(millis() > time_now + period){

      fade = ( fade >  fade_min) ? fade - 5 : fade_max;
      

      //Serial.println(fade);
      
      time_now = millis();

        
    }

  } 
  

  setLightWork(SystemInformation.currentWorkLight.color, SystemInformation.currentWorkLight.fade );
  setLightRoom(SystemInformation.currentRoomLight.color, SystemInformation.currentRoomLight.fade );
  setLightKitchen( SystemInformation.currentKitchenLight.color , fade );


  delay(50);

  /*
    //for(int i=0; i < NUM_LEDS; i++) {
    }




    float ACS712read(int mitteln) {
    // Den ACS712 Stromsensor auslesen
    // Sens ist im Datenblatt auf Seite 2 mit 185 angegeben.
    // Für meinen Sensor habe ich 186 ermittelt bei 5.0V Vcc.
    // Sens nimmt mit ca. 38 pro Volt VCC ab.
    //
    // 3,3V muss zu Analog Eingang 5 gebrückt werden.
    // Der Sensoreingang ist Analog 1
    //
    // Parameter mitteln : die Anzahl der Mittlungen
    //
    // Matthias Busse 9.5.2014 Version 1.0

    float sense=186.0;           // mV/A Datenblatt Seite 2
    float sensdiff=39.0;         // sense nimmt mit ca. 39/V Vcc ab.
    float vcc, vsensor, amp, ampmittel=0;
    int i;

    /*
    for(i=0;i< mitteln;i++) {
    vcc = (float) 3.30 / analogRead(5) * 1023.0;    // Versorgungsspannung ermitteln
    vsensor = (float) analogRead(0) * vcc / 1023.0; // Messwert auslesen
    vsensor = (float) vsensor - (vcc/2);            // Nulldurchgang (vcc/2) abziehen
    sense = (float) 186.0 - ((5.00-vcc)*sensdiff);  // sense für Vcc korrigieren
    amp = (float) vsensor /sense *1000 ;            // Ampere berechnen
    ampmittel += amp;                               // Summieren
    }

    Serial.print(vcc, 3);
    Serial.print(" vcc | ");
    Serial.print(vsensor, 3);
    Serial.print(" vsensor | ");
    Serial.print(sense, 3);
    Serial.print(" sense | ");
    Serial.print(amp, 3);
    Serial.print(" amp | ");
      Serial.print(ampmittel, 3);
    Serial.println(" ampmittel | ");





    Serial.print( analogRead(1));
    Serial.print(" 1 | ");
    Serial.print(analogRead(2));
    Serial.print(" 2 | ");
    Serial.print( analogRead(3));
    Serial.print(" 3 | ");
    Serial.print(analogRead(4));
    Serial.print(" 4 | ");
    Serial.print( analogRead(5));
    Serial.print(" ref | ");
    Serial.print(analogRead(0));
    Serial.print(" vensor | ");
    int diff = analogRead(5) - (2* analogRead(0));
    Serial.print(diff);
    Serial.print(" diff | ");
    float aF = analogRead(5);
    float aZ = analogRead(0);
      vcc = (float) 3.30 / aF * 1023.0 *1000;    // Versorgungsspannung ermitteln
    vsensor = (float) aZ * vcc / 1023.0; // Messwert auslesen
    Serial.print(vcc);
    Serial.print(" vcc | ");
    Serial.print(vsensor);
    Serial.print(" vsensor | ");
     vcc = (float) 3.30 / aF* 1023.0 ;    // Versorgungsspannung ermitteln
    vsensor = (float) aZ * vcc / 1023.0; // Messwert auslesen
    Serial.print(vcc);
    Serial.print(" vcc | ");
    Serial.print(vsensor);
    Serial.println(" vsensor | ");

    return ampmittel/mitteln;

  */
}

void  HandleSerial(void) {
  if (Serial.available() > 0) {    // is a character available?
    rx_byte = Serial.read();       // get the character

    // check if a number was received
    if ((rx_byte >= '0') && (rx_byte <= '9')) {
      Serial.print("Number received: ");
      Serial.println(rx_byte);
    } else if (rx_byte == 'n') {
      SystemInformation.currentTime = day;
      SystemInformation.changeDemoSetup = true;
      Serial.println("Demo set to Day empfangen");
    } else if (rx_byte == 'm') {
      Serial.println("Demo set to night empfangen");
      SystemInformation.currentTime = night;
      SystemInformation.changeDemoSetup = true;
    } else if (rx_byte == 'o') {
      digitalWrite(7, LOW);;
      Serial.println("Demo set to High Volate - DANGERZONE");
    } else if (rx_byte == 'p') {
      digitalWrite(7, HIGH);;
      Serial.println("Demo unset to High Volate");
    } else if (rx_byte == 'c') {
      for (int i = 0 ; i < 100; i++ )
      {
        Serial.println(" ");
      }
    } else if (rx_byte == 'h') {
      Serial.println(" comands: " );
      Serial.println(" s - read Demo status");
      Serial.println(" n - set Demo to day ");
      Serial.println(" m - set Demo to night ");
      Serial.println(" o - set Demo 230 system DANGERZONE " );
      Serial.println(" p - unset Demo 230 system  ");
      Serial.println(" h - print help ");
      Serial.println(" c - clear consol ");
    }

    else {
      Serial.println("Not a number.");
    }


  } // end: if (Serial.available() > 0)

  return;
}

void HandleSchalter(void)
{

  // Licht auf Tisch
  if (digitalRead(4))
  {
    SystemInformation.currentRoomLight.color = SystemInformation.roomLight.color;
    SystemInformation.currentRoomLight.fade = SystemInformation.roomLight.fade;
    SystemInformation.currentRoomLight.state = on;

  }
  else {
    SystemInformation.currentRoomLight.color = CRGB::Black;
    SystemInformation.currentRoomLight.fade = 0;
    SystemInformation.currentRoomLight.state = off;
  }

  //  Serial.print(digitalRead(4));
  //  Serial.println(" schalter | ");

  return;
}


void HandleTaster_A(void)
{
  static bool active = false;
  static bool was_active = false;

  active = (bool) digitalRead(5);

  if (off == SystemInformation.currentWorkLight.state)
  {
    SystemInformation.currentWorkLight.fade  = SystemInformation.fade_max;
  }

  // hat sich der Schalter Position geändert
  if (active == was_active)
  {
    // schalter hat sich nicht geändert


    // was ist der aktuelle Schalter Position
    if (true == active)
    {
      // gedrückt
      if ( 11 > SystemInformation.currentWorkLight.fade)
      {
        SystemInformation.currentWorkLight.fade = SystemInformation.fade_max;
      } else {
        SystemInformation.currentWorkLight.fade = SystemInformation.currentWorkLight.fade - 10;
      }


    } else {

      // nicht gedrückt
    }
  } else {
    // schalter  hat sich  geändert

    // was ist der aktuelle Schalter Position
    if (true == active)
      // gedrückt
    {
      SystemInformation.currentWorkLight.state = (on ==  SystemInformation.currentWorkLight.state) ? off :  on;

    }
  }

  if (on == SystemInformation.currentWorkLight.state)
  {
    SystemInformation.currentWorkLight.color = SystemInformation.workLight.color;
  } else {
    SystemInformation.currentWorkLight.color = CRGB::Black;
    SystemInformation.currentWorkLight.fade = 0;
  }

  //  Serial.print(active);
  //  Serial.print(" active | ");
  //  Serial.print(was_active);
  //  Serial.print(" was_active | ");
  //  Serial.print(SystemInformation.currentWorkLight.state);
  //  Serial.print(" state | ");
  //  Serial.print(SystemInformation.currentWorkLight.fade);
  //  Serial.println(" fade | ");

  was_active = active;
}



void HandleTaster_B(void)
{
  static bool active = false;
  static bool was_active = false;

  static bool state = false;


  active = (bool) digitalRead(6);


  // hat sich der Schalter Position geändert
  if (active == was_active)
  {
    // schalter hat sich nicht geändert


    // was ist der aktuelle Schalter Position
    if (true == active)
    {
      // gedrückt


    } else {

      // nicht gedrückt
    }
  } else {
    // schalter  hat sich  geändert

    // was ist der aktuelle Schalter Position
    if (true == active)
    {
      // gedrückt

      if (state)
      {
        digitalWrite(8, HIGH);;
      } else {
        digitalWrite(8, LOW);;
      }

      state = !state;
    }
  }


  //  Serial.print(active);
  //  Serial.print(" active | ");
  //  Serial.print(was_active);
  //  Serial.print(" was_active | ");
  //  Serial.print(state);
  //  Serial.print(" state | ");
  //  Serial.print(fade);
  //  Serial.println(" fade | ");

  was_active = active;
}


void set_Demo_night(void)
{
  // Ober und Unter licht
  SystemInformation.currentKitchenLight = (lightCharacter) {SystemInformation.dayLight.background.color , SystemInformation.dayLight.background.fade, on};
  
  // Fenster
  setLightWindow( SystemInformation.dayLight.outdoor.color , SystemInformation.dayLight.outdoor.fade);

  FastLED.show();
  return;
}


void set_Demo_day(void)
{
  // Ober und Unter licht
  SystemInformation.currentKitchenLight = (lightCharacter)  {SystemInformation.nightLight.background.color , SystemInformation.nightLight.background.fade, off};

  // Fenster
  setLightWindow( SystemInformation.nightLight.outdoor.color , SystemInformation.nightLight.outdoor.fade);

  FastLED.show();
  return;
}


void setLightWindow(CRGB newLight, int fade)
{
  // Fenster
  leds[23] = newLight  ;
  leds[23].fadeLightBy( fade );
  leds[24] = newLight  ;
  leds[24].fadeLightBy( fade );
  leds[25] = newLight  ;
  leds[25].fadeLightBy( fade );
  leds[26] = newLight   ;
  leds[26].fadeLightBy( fade );

  FastLED.show();
  return;
}


void setLightKitchen(CRGB newLight, int fade)
{
  leds[1] = newLight  ;
  leds[1].fadeLightBy( fade );
  leds[2] = newLight  ;
  leds[2].fadeLightBy( fade );
  leds[3] = newLight   ;
  leds[3].fadeLightBy( fade );
  leds[4] = newLight  ;
  leds[4].fadeLightBy( fade );
  leds[5] = newLight  ;
  leds[5].fadeLightBy( fade );
  leds[6] = newLight  ;
  leds[6].fadeLightBy( fade );


  leds[17] = newLight  ;
  leds[17].fadeLightBy( fade );
  leds[18] = newLight  ;
  leds[18].fadeLightBy( fade );
  leds[19] = newLight  ;
  leds[19].fadeLightBy( fade );
  leds[20] = newLight   ;
  leds[20].fadeLightBy( fade );
  leds[21] = newLight  ;
  leds[21].fadeLightBy( fade );
  leds[22] = newLight  ;
  leds[22].fadeLightBy( fade );

  FastLED.show();
  return;
}


void setLightRoom(CRGB newLight, int fade)
{

  leds[7] = newLight  ;
  leds[7].fadeLightBy( fade );
  leds[8] = newLight  ;
  leds[8].fadeLightBy( fade );

  FastLED.show();
  return;
}

void setLightWork(CRGB newLight, int fade)
{

  leds[10] = newLight  ;
  leds[10].fadeLightBy( fade );
  leds[11] = newLight  ;
  leds[11].fadeLightBy( fade );
  leds[12] = newLight  ;
  leds[12].fadeLightBy( fade );
  leds[13] = newLight   ;
  leds[13].fadeLightBy( fade );
  leds[14] = newLight  ;
  leds[14].fadeLightBy( fade );
  leds[15] = newLight  ;
  leds[15].fadeLightBy( fade );

  FastLED.show();
  return;
}

void setLightWachter(CRGB newLight, int fade)
{

  leds[0] = newLight  ;
  leds[0].fadeLightBy( fade );
  leds[9] = newLight  ;
  leds[9].fadeLightBy( fade );
  leds[16] = newLight  ;
  leds[16].fadeLightBy( fade );

  FastLED.show();
  return;
}


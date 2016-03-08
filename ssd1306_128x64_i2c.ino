/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

This example is for a 128x64 size display using I2C to communicate
3 pins are required to interface (2 I2C and one reset)

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!;

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

unsigned int counter = 0;
unsigned int minutes = 0;
unsigned int hours = 0;

int inPin1 = 3;
int inPin2 = 4;
int state1 = 0;
int state2 = 0;
int led = 13;


void displaySensorDetails(void)
{
  sensor_t sensor;
  bmp.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" hPa");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" hPa");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" hPa");  
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void setup()   {               
  
  Serial.begin(9600);
  pinMode(inPin1, INPUT_PULLUP);
  pinMode(inPin2, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  
    // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done
  
    /* Initialise the sensor */
  if(!bmp.begin())
  {
    /* There was a problem detecting the BMP085 ... check your connections */
    Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  /* Display some basic information on this sensor */
  displaySensorDetails();
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);

  }


void loop() {
  
  sensors_event_t event;
  bmp.getEvent(&event);
  
  if (event.pressure)
  {
    
    state1 = !digitalRead(3);
    state2 = !digitalRead(4);
    
    if(state1) // display 1
    {    
      float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;
      float altitudeMean = 0;
      float valArray[10];
      valArray[0] = altitudeMean;
      
      for(int i=1; i<6; i++)
      {
        valArray[i] = bmp.pressureToAltitude(seaLevelPressure, event.pressure);
        delay(200);
      }
      
      for(int i=0; i<6; i++)
      {
        altitudeMean = altitudeMean + valArray[i];
      }
      
      altitudeMean = altitudeMean /6;
            
      display.clearDisplay();
      display.setTextSize(4);
      display.setTextColor(WHITE);
      display.setCursor(24,20);
      //display.print(bmp.pressureToAltitude(seaLevelPressure, event.pressure),1);
      display.print(altitudeMean,1);
      display.setTextSize(1);
      display.setCursor(24,50);
      display.print("  Altitude(m) ");     
      display.display();
    } 
    else if(state2) //display 2
    {      
     
      float temperature;
      bmp.getTemperature(&temperature);
      display.clearDisplay();
      display.setTextSize(4);
      display.setCursor(24,20);
      display.setTextColor(WHITE);
      display.print(temperature,1);
      display.setTextSize(1);
      display.print("   Temperature(c)");  
      display.drawRect(14, 16, 105, 34, WHITE);
      display.display(); 
    }
    else //display 3
    {
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.print("Pressure: ");
      display.print(event.pressure);
      display.print(" hpa"); 

      float temperature;
      bmp.getTemperature(&temperature);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.print("Temperature: ");
      display.print(temperature);
      display.println(" C");  

      float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;
      display.setTextSize(1);
      display.setTextColor(WHITE);
      //display.setCursor(0,0);
      display.print("Altitude: ");
      display.print(bmp.pressureToAltitude(seaLevelPressure, event.pressure));
      display.println(" m");   
      display.print("H:");
      display.print(hours);
      display.print(" M:");
      display.print(minutes);
      display.print(" S:");
      display.print(counter);
   
      display.display();     
    }
    
    /*
   if(counter<1000)
   {
     counter++;
   }
   else
   {
     counter = 0;
   }*/
   
   if(counter<120)
   {
     counter++;   
     if(counter==120 && minutes<60 )
     {
       minutes++;
       counter = 0;  
       if(minutes==60)
       {
         hours++;
         minutes=0;
       }
     }
   }
   
   
   /*
   else if(counter==120 || minutes<60 )
   {
     minutes++;
     counter = 0;
   }
   else if(minutes==60)
   {
     hours++;
     minutes = 0;
   }*/
   
   
   
   
  }
  else
  {
    display.clearDisplay();
    display.println("Sensor Error");   
    display.display();
   
  }
  
  digitalWrite(led, LOW);   
  delay(500);
  digitalWrite(led, HIGH);    
  
}

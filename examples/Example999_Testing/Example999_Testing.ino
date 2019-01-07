#include "hyperdisplay_custom_conf.h"
#include "HyperDisplay_ILI9163C.h"

//#include "bubb.h"
#include "bubbs.h"

#define RST_PIN 2
#define PWM_PIN 3
#define READ_PIN 4
#define CS_PIN 5
#define DC_PIN 6
#define SPI_PORT SPI
//#define SPI_SPEED 4000000
#define SPI_SPEED 8000000

KWH018ST03 myTFT; // Create an object of the type

  
ILI9163C_color_18_t myColor;

#define SEQ_LEN 5
ILI9163C_color_18_t myColorSequence[SEQ_LEN] = 
{
  {0,0,0},
  {0,127-64,0},
  {0,127,0},
  {0,127+64,0},
  {0,255,0}
};

//#define SIN_LEN 278 // (uno limit!)

#define SIN_LEN 360
//#define SIN_LEN 10
ILI9163C_color_18_t mySinSequence[SIN_LEN];


double triX[3] = {10, 53, 64};
double triY[3] = {80, 20, 60};

uint16_t count = 0;
uint8_t count2 = 0;

wind_info_t myWindow;


sf2drt_coordinate_t rtPolyPoints[5] = 
{
  {23,10},
  {20,0},
  {60,35},
  {10,50},
  {0,40}
};
sf2drt_polygon myRTPolygon(5, rtPolyPoints);


void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
//  while(!Serial){};

//  Serial.println((uint32_t) trippy_pixel_map);
//  Serial.println(sizeof(trippy_pixel_map));

  myTFT.begin(DC_PIN, RST_PIN, CS_PIN, PWM_PIN, READ_PIN, SPI_PORT, SPI_SPEED);


  // Initializet the SIN Sequence
  for(uint16_t indi = 0; indi < SIN_LEN; indi++)
  {
    ILI9163C_color_18_t temp;
    temp.r = 0;
    temp.g = round(127 * sin((((float)indi)/SIN_LEN)*2*3.1415) + 127);
    temp.b = 0;
    mySinSequence[indi] = temp;
  }
  
  myTFT.clearDisplay(  );

  myColor.r = 255;
  myColor.g = 0;
  myColor.b = 0;

  myTFT.setWindowDefaults(&myWindow);

  myWindow.xMin = 0;
  myWindow.yMin = 0;
//  myWindow.xMax = 117;
//  myWindow.yMax = 149;



  for(uint32_t indi = 0; indi < sizeof(bubbers); indi++)
  {
    *(bubbers + indi) *= 3;
  }


  for(uint32_t indi = 0; indi < 128; indi++)
  {
    for(uint8_t indj = 0; indj < 160; indj++)
    {
      *(buberoohs + indj + (160*indi)) = *(bubbers + indi + (128*indj));
    }
  }

  
  




  myTFT.pCurrentWindow = &myWindow;

//  myTFT.fillWindow((color_t)&myColor);

Serial.println("End of setup");
}

void loop() {
  // put your main code here, to run repeatedly:


//  myTFT.pixel(0, 0, (color_t)&mySinSequence, SIN_LEN, count++);
//  myTFT.xline(64,0,47, (color_t)&myColorSequence, SEQ_LEN, count++, false);
//  myTFT.xline(64,0,47, (color_t)&myColorSequence, SEQ_LEN, count++, true);
//  myTFT.yline(0,64,47, (color_t)&myColorSequence, SEQ_LEN, count++, false);
//  myTFT.yline(0,64,47, (color_t)&myColorSequence, SEQ_LEN, count++, true);
//  myTFT.rectangle(0,0,37,37, false, (color_t)&myColorSequence, SEQ_LEN, count++, false, false);
//  myTFT.rectangle(0,0,37,37, false, (color_t)&myColorSequence, SEQ_LEN, count++, true, false);
//  myTFT.rectangle(0,0,37,37, true, (color_t)&myColorSequence, SEQ_LEN, count++, false, false);
//  myTFT.rectangle(0,0,37,37, true, (color_t)&myColorSequence, SEQ_LEN, count++, true, false);
//  myTFT.rectangle(0,0,37,37, true, (color_t)&myColorSequence, SEQ_LEN, count++, false, true);
//  myTFT.rectangle(0,0,37,37, true, (color_t)&myColorSequence, SEQ_LEN, count++, true, true);
//  myTFT.line(90, 10, 5, 20, 1, (color_t)&myColorSequence, SEQ_LEN, count++, false);
//  myTFT.polygon(triX, triY, 3, 1, (color_t)&myColorSequence, SEQ_LEN, count--, true);
//  myTFT.rtPolygon(&myRTPolygon, 1, false, (color_t)&myColor, 1, 0, false);
//  myTFT.rtPolygon(&myRTPolygon, 1, true, (color_t)&myColor, 1, 0, false);
  myTFT.fillFromArray(0, 0, 127, 159, (color_t)&bubbers,  160*128, true);


//  myTFT.fillWindow((color_t)(myColorSequence + count++), 1, 0);
//  if(count >= SEQ_LEN)
//  {
//    count = 0;
//  }


  Serial.print("Loop");
  Serial.println();
  delay(1000);

//  while(1);

}



/* 

Code Boneyard!

  for(uint8_t indi = 0; indi < 80; indi++)
  {
      myTFT.xline(10, indi, 32, (color_t)myColorSequence, SEQ_LEN, 0, false); 
      myTFT.xline(10, indi+80, 32, (color_t)myColorSequence, SEQ_LEN, 0, true); 
  }
  for(uint8_t indi = 0; indi < 64; indi++)
  {
    myTFT.yline(indi, 10, 80, (color_t)myColorSequence, SEQ_LEN, 0, false); 
    myTFT.yline(indi+64, 20, 80, (color_t)myColorSequence, SEQ_LEN, 0, true); 
  }



  //  myTFT.hwrectangle( 10, 10, 100, 100, false, (color_t)&myColor, 1, 0, false, false);

//  delay(6000);

//  myTFT.hwrectangle( 0, 0, 127, 159, true, (color_t)&myColor);
//  myTFT.hwrectangle( 0, 0, 127, 159, true, (color_t)&mySinSequence, SIN_LEN, 0, true, true);

//  myTFT.hwrectangle( 10, 10, 100, 100, false, (color_t)&mySinSequence, SIN_LEN, 0, false, false);





//  for(uint8_t indj = 0; indj < 128; indj++)
//  {
//    for(uint8_t indi = 0; indi < 160; indi++)
//    {
//      myTFT.pixel(indj, indi, (void*)&myColor);
//    }
//  }
//
//  myTFT.clearDisplay(  );

count++;
//Serial.println(count % 5);


//  myTFT.line(137, 10, 17, 100, 10, (color_t)&mySinSequence, SIN_LEN, count, true);

//  myTFT.hwxline(20, 20, 40, (color_t)mySinSequence, SIN_LEN, count++, false); 
//  myTFT.hwrectangle( 10, 10, 100, 100, false, (color_t)&mySinSequence, SIN_LEN, count, true, false);

//  myTFT.polygon(triX, triY, 3, 1, (color_t)&mySinSequence, SIN_LEN, count, false);







*/

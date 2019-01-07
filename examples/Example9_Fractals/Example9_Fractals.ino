#include "HyperDisplay_ILI9163C.h"
#include "BigNumber.h" // https://github.com/nickgammon/BigNumber


#define SERIAL_PORT Serial

#define X_PIX 128
#define Y_PIX 160
#define NUM_PIX ((X_PIX)*(Y_PIX))

#define DEFAULT_SCALE 10

typedef uint16_t f_it_t;
typedef uint16_t f_ext_t;

struct bigComplexNumber {
  // a + bi
  BigNumber a;
  BigNumber b;
};

struct fractalView {
  bigComplexNumber center;
  BigNumber realExt;
  BigNumber imagExt;
};

struct pixelView {
  f_it_t iterations;
  f_it_t* presults;
  f_ext_t xExt;
  f_ext_t yExt;

  f_ext_t zoomCX;
  f_ext_t zoomCY;
  f_ext_t zoomEX;
  f_ext_t zoomEY;

  f_it_t maxValue;
  f_it_t minValue;

  bool* pComputed;
};







f_it_t results[NUM_PIX];
ILI9163C_color_18_t screenMap[NUM_PIX];
bool pixelsComputed[NUM_PIX];


pixelView screen;

bool redraw = false;
bool continueDrawing = true;


#define RST_PIN 2
#define PWM_PIN 3
#define READ_PIN 4
#define CS_PIN 5
#define DC_PIN 6
#define SPI_PORT SPI
//#define SPI_SPEED 4000000
#define SPI_SPEED 8000000

KWH018ST03 myTFT; // Create an object of the type


int scale = 10;

void setup ()
{
  SERIAL_PORT.begin (115200);
//  while (!SERIAL_PORT){};
  delay(500);
  BigNumber::begin (DEFAULT_SCALE);  // initialize library
  Serial.println("Hello");

  // Variables
  fractalView view;


  view.center.a = "-0.5";
  view.center.b = "-0.0";
  view.realExt = "3";
  view.imagExt = "2";

  screen.xExt = X_PIX;
  screen.yExt = Y_PIX;
  screen.presults = results;
  screen.iterations = 100;
  screen.pComputed = pixelsComputed;

  screen.zoomCX = X_PIX/2;
  screen.zoomCY = Y_PIX/2;
  screen.zoomEX = X_PIX/2;
  screen.zoomEY = Y_PIX/2;


  myTFT.begin(DC_PIN, RST_PIN, CS_PIN, PWM_PIN, READ_PIN, SPI_PORT, SPI_SPEED);

  giveMenu();
  
  // Draw with the initial settings
  inititalize( &screen, &view );
  computeResults( &screen, &view, 16 );
//  results2pixels( &screen, screenMap );
//  showPixels();


  
  while(1)
  {
    continueDrawing = true;
    giveMenu();

    while(!redraw)
    {
      checkAndHandleInput(&screen, &view, &redraw);
    }

    inititalize( &screen, &view );
//    viewToZoom( &screen, &view );

    computeResults( &screen, &view, 16 );
//    results2pixels( &screen, screenMap );
//    showPixels();

    giveMenu();
  }
  // Never reached
}

void computeResults( pixelView* ppix, fractalView* pfrac, uint8_t divisor )
{

    for(uint8_t indd = divisor; indd >= 1; indd/=2)
    {
      if(continueDrawing)
      {
        for( f_ext_t indi = 0; indi < ppix->xExt; indi+=indd )
        {
          if(continueDrawing)
          {
            for( f_ext_t indj = 0; indj < ppix->yExt; indj+=indd)
            {
              if(continueDrawing)
              {
                f_it_t numIt = 0;
                
                if(!ppix->pComputed[indj + (ppix->yExt * indi)])
                {
                  bigComplexNumber c;
                  c.a = (pfrac->center.a) + ((BigNumber(indi) / BigNumber(ppix->xExt)) - BigNumber("0.5")) * (pfrac->realExt);
                  c.b = (pfrac->center.b) + ((BigNumber(indj) / BigNumber(ppix->yExt)) - BigNumber("0.5")) * (pfrac->imagExt);
            
                  // Check here if you are in any known regions that will be zero (because they take the longest to run)! TODO
                  if(!knownRegions(c)) 
                  {
                    numIt = computeMandelbrot(c, ppix->iterations);
                  }
                  
                  ppix->presults[indj + (ppix->yExt * indi)] = numIt;      
                  ppix->pComputed[indj + (ppix->yExt * indi)] = true;
                }
          
                // Check min/max values
                if((indi == 0) && (indj == 0) && (indd == divisor))
                {
                  ppix->maxValue = numIt;
                  ppix->minValue = numIt;
                }
                else
                {
                  if(numIt !=0)
                  {
                    ppix->maxValue = max(ppix->maxValue, numIt);
                    ppix->minValue = min(ppix->minValue, numIt);
                  }
                }
    //          Serial.print(ppix->presults[indj + (ppix->yExt * indi)]);
              }
    //        Serial.println();
            }
            checkAndHandleInput(ppix, pfrac, &redraw);
            results2pixels( &screen, screenMap );
            showPixels();
          }
        }
      }
    }
}

f_it_t computeMandelbrot(bigComplexNumber c, f_it_t maxIterations)
{
  bigComplexNumber result;
  result.a = 0;
  result.b = 0;

  for(f_it_t indi = 0; indi < maxIterations; indi++)
  {
    result = cn_multiply(result, result); // result^2 + c
    result.a = result.a + c.a; 
    result.b = result.b + c.b;
    if(cn_magnitude(result) > BigNumber("2.0"))
    {
      return indi;
    }
  }
  return 0;
}

bool knownRegions( bigComplexNumber c )
{
  if(cn_magnitude(c) < BigNumber("0.25")){ return true; } // This excludes all points within the quarter-unit circle centered on zero
  return false;
}

bigComplexNumber cn_multiply(bigComplexNumber i1, bigComplexNumber i2)
{
  bigComplexNumber retval;
  retval.a =  (((i1.a) * (i2.a)) - ((i1.b) * (i2.b)));
  retval.b =  (((i1.a) * (i2.b)) + ((i1.b) * (i2.a)));
  return retval;
}

BigNumber cn_magnitude(bigComplexNumber n)
{
  BigNumber result =  ((n.a * n.a) + (n.b*n.b));
  result = result.sqrt();
  return result;
}


void giveMenu( void )
{
  SERIAL_PORT.println("Press 'r' to redraw");
  SERIAL_PORT.println("Press 'wasd' to move center");
  SERIAL_PORT.println("Press '-' or '=' to zoom in/out");
  SERIAL_PORT.println("Press 'z' to apply new window");
  
}



void inititalize( pixelView* ppix, fractalView* pfrac )
{
  myTFT.clearDisplay();
  for( f_ext_t indi = 0; indi < ppix->xExt; indi++ )
  {
    for( f_ext_t indj = 0; indj < ppix->yExt; indj++ )
    {
      ppix->pComputed[indj + (ppix->yExt * indi)] = false;
    }
  }
}



void results2pixels( pixelView* ppix, ILI9163C_color_18_t* screenMap )
{
  uint8_t red, green, blue;
  ILI9163C_color_18_t color;
  bool rowComputed;
  f_ext_t lastIndi = 0;
  
  for( f_ext_t indi = 0; indi < ppix->xExt; indi++ )
  {
    red = 0;
    green = 0;
    blue = 0;

//    rowComputed = ppix->pComputed[0 + (ppix->yExt * indi)];
    rowComputed = false;
    for( f_ext_t indj = 0; indj < ppix->yExt; indj++)
    {
      if( ppix->pComputed[0 + (ppix->yExt * indi)] )
      {
        rowComputed = true;
      }
    }
    
    if(rowComputed)
    {
      lastIndi = indi;
    }
//    else
//    {
//      lastIndi = 0;
//    }

//        Serial.print("Row computed = "); Serial.print(rowComputed);

    for( f_ext_t indj = 0; indj < ppix->yExt; indj++)
    { 
      if(rowComputed)
      {
        if(ppix->pComputed[indj + (ppix->yExt * indi)])
        {
        
          f_it_t value = ppix->presults[indj + (ppix->yExt * indi)];
    
          if( value != 0)
          {
            uint16_t hue = getHue( ppix, value );
            fast_hsv2rgb_8bit(hue, 255, 255, &red, &green , &blue);
          }
          else
          {
            red = 0;
            green = 0;
            blue = 0;
          }
        }
        color.r = red;
        color.b = blue;
        color.g = green;
        *(screenMap + indj + (ppix->yExt * indi)) = color;
      }
      else
      {
        color = *(screenMap + indj + (ppix->yExt * lastIndi));
        *(screenMap + indj + (ppix->yExt * indi)) = color;
      }
    }
  }
}

uint16_t getHue( pixelView* ppix, f_it_t value )
{
  double range = (ppix->maxValue - ppix->minValue);
  double percent = (value - ppix->minValue)/range;

  return (uint16_t)((percent)*(HSV_HUE_MAX-HSV_HUE_MIN) + HSV_HUE_MIN);
}

void showPixels( void )
{
  ILI9163C_color_18_t rectColor;
        rectColor.r = 255;
        rectColor.g = 255;
        rectColor.b = 255;
        
  myTFT.fillFromArray(0, 0, 127, 159, (color_t)screenMap,  NUM_PIX, true);
  myTFT.rectangle((screen.zoomCX-(screen.zoomEX/2)), (screen.zoomCY-(screen.zoomEY/2)), (screen.zoomCX+(screen.zoomEX/2)), (screen.zoomCY+(screen.zoomEY/2)), false, (color_t)&rectColor);
}

void loop() {
  // Never reached
}


void viewToZoom( pixelView* ppix, fractalView* pfrac )
{
  // Apply the zoom level (in pixels) to the view
  

  bigComplexNumber newc;
  newc.a = (pfrac->center.a) + ((BigNumber(ppix->zoomCX) / BigNumber(ppix->xExt)) - BigNumber("0.5")) * (pfrac->realExt);
  newc.b = (pfrac->center.b) + ((BigNumber(ppix->zoomCY) / BigNumber(ppix->yExt)) - BigNumber("0.5")) * (pfrac->imagExt);

  pfrac->center.a = newc.a;
  pfrac->center.b = newc.a;

  BigNumber newRealExt = ((BigNumber(ppix->zoomEX) / BigNumber(ppix->xExt)) * pfrac->realExt);
  BigNumber newImagExt = ((BigNumber(ppix->zoomEY) / BigNumber(ppix->yExt)) * pfrac->imagExt);

  pfrac->realExt = newRealExt;
  pfrac->imagExt = newImagExt;
          


  // Reset the default zoom
  screen.zoomCX = X_PIX/2;
  screen.zoomCY = Y_PIX/2;
  screen.zoomEX = X_PIX/2;
  screen.zoomEY = Y_PIX/2;
}

void checkAndHandleInput(pixelView* ppix, fractalView* pfrac, bool* predraw )
{ 
  while(SERIAL_PORT.available())
  {
    Serial.print(SERIAL_PORT.available());
    char c = SERIAL_PORT.read();
//        SERIAL_PORT.write(c);

    switch(c)
    {
      case 'r' : 
        *predraw = true;
        continueDrawing = false;
        break;

      case 'i' : 
         ppix->iterations += 50;
        *predraw = true;
        continueDrawing = false;
        break;

      case 'k' :
        if(ppix->iterations > 50)
        {
          ppix->iterations -= 50;
          *predraw = true;
          continueDrawing = false;
        }
        if(ppix->iterations > 10)
        {
          ppix->iterations -= 10;
          *predraw = true;
          continueDrawing = false;
        }
        if(ppix->iterations > 1)
        {
          ppix->iterations -= 1;
          *predraw = true;
          continueDrawing = false;
        }
        break;

      case 'w' :
        ppix->zoomCY = ppix->zoomCY-1;
        break;

      case 's' : 
       ppix->zoomCY = ppix->zoomCY+1;
       break;

      case 'a' :
        Serial.println(ppix->zoomCX);
        ppix->zoomCX = ppix->zoomCX-1;
        Serial.println(ppix->zoomCX);
        break;

      case 'd' :
        ppix->zoomCX = ppix->zoomCX+1;
        break;
  
      case '-' : 
        // Zoom out
        break;

      case '=' :
        if( (ppix->zoomEX >= 4+2) && (ppix->zoomEY >= 5+2) )
        {
          ppix->zoomEX -= 4;
          ppix->zoomEY -= 5;
        }
        break;

      case 'z' :
        viewToZoom( ppix, pfrac );
        *predraw = true;
        continueDrawing = false;
        break;
    }
  }
}

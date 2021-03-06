#include <Adafruit_NeoPixel.h>                                                                                                                                                                                                                 
// NeoPixel Data line
#define PIN 7

Adafruit_NeoPixel strip = Adafruit_NeoPixel(28, PIN, NEO_GRB + NEO_KHZ800);

// The rainbow comprises four arcs, starting from the bottom, they are
// zero-indexed as follows...
#define BLUE   0
#define RED    1
#define YELLOW 2
#define GREEN  3

#define WHITE  255
#define BLACK  0


int row_count = 4;
int row_offsets[5] = {0,4,9,16,28};
int cols[13][4] = {
  {0,-1,-1,-1},
  {1,-1,-1,-1},
  {2,0,0,0},
  {3,1,1,1},
  {4,2,1,1},
  {5,3,2,2},
  {6,3,2,2},
  {7,4,3,2},
  {8,5,4,3},
  {9,6,4,3},
  {10,7,5,4},
  {11,7,5,4},
  {12,13,13,13}
};

//linear doesn't work very well... just fudge a table.
int falloff_brightness[11] = {255, 50, 25, 10, 0, 0, 0, 0, 0, 0, 0};

// I hope you learned somehting about physical layouts not reflecting reality
int phy_pixel_map[28] = {0, 1, 2, 3, 8, 7, 6, 5, 4, 9, 10, 11, 12, 13, 14, 15, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16};


void setup() {
  strip.begin();
  strip.show();
  Serial.begin(9600);
}

// custom non-linear brightness functions.
int custom_brightness(int func, int brightness) {
  switch(func) {
    case 1:
      return brightness;
  }
}

void loop() {
  int i, j, d, a, b, f;
  d=0;
  strip.setBrightness(32);

  for(j=0;j<20;j++) {
    flash_arc(GREEN, 50);
    delay(50);
  }

  for(j=0;j<20;j++) {
    flash_arc(YELLOW, 50);
    delay(50);
  }

  for(j=0;j<20;j++) {
    flash_arc(RED, 50);
    delay(50);
  }

  for(j=0;j<20;j++) {
    flash_arc(BLUE, 50);
    delay(50);
  }

  for(j=0;j<5;j++) {
    pulse_rainbow(0, 3);
  }


// vu style grow right, shrink back to the left
  for(j=0;j<5;j++) {
    // left -> right sector
    for (i=0;i<12;i++) {
      sector(0,i, 1); delay(d);
    }
    // right -> left sector
    for (i=12;i>=0;i--) {
      sector(0,i, 1); delay(d);
    }
  }


// clock-wise rotation
  for(j=0;j<5;j++) {
    for (i=0;i<=12;i++) {
      sector(0,i, 1); delay(10);
    }
    delay(300);
    for (i=0;i<12;i++) {
      sector(i,12, 1); delay(10);
    }
  }

//// initial brightness of each arc
////  int arc_brightness[4] = {0, 85, 170, 255};
  int arc_brightness[4] = {0, 25, 50, 255};
  int arc_step_dir[4] = {1, 1, 1, 1};
  int arc_step = 1;
  int dir = 1;
  for(j=0;j<8000;j++) {
// adjust brightness differential after each iteration
    if (arc_brightness[0] == 255) {
      arc_step += dir;
      if (arc_step > 32) {
        arc_step = 32;
        dir=-1;
      }
      if (arc_step < 0) {
        arc_step = 0;
        dir=1;
      }
    }

    for(a=0;a<4;a++) {
      arc(a, arc_brightness[a]);
      arc_brightness[a]+= arc_step*arc_step_dir[a];
      if(arc_brightness[a] >255) {
        arc_brightness[a] = 255;
        arc_step_dir[a] *= -1;
      }
      if(arc_brightness[a] < 0) {
        arc_brightness[a] = 0;
        arc_step_dir[a] *= -1;
      }
    }
    delay(5);
  }
}

void flash_arc(int arc_num, int d) {
  arc(arc_num, 255);
  strip.show();
  delay(d);
  arc(arc_num, 0);
  strip.show();
}

void pulse_rainbow(int d, int step) {
  int i, b;

    for (b=0;b<=255;b+=step) {
      for (i=0;i<4;i++) {
        arc(i, b);
      }
      strip.show();
      delay(d);
    }
    for (b=255;b>=0;b-=step) {
      for (i=0;i<4;i++) {
        arc(i, b);
      }
      strip.show();
      delay(d);
    }
}

// light one of the four arcs with constant brightness
void arc(int arc, int brightness) {
  int i;
  uint32_t color = strip.Color(brightness,brightness,brightness);
  if (brightness > 255)
    brightness = 255;
  if (brightness < 0)
    brightness = 0;

  // loop over pixels in a particular arc
  for(i=row_offsets[arc]; i<row_offsets[arc+1]; i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
}

void sector(int left, int right, int fade) {
  int row, c,l,r, tmp;
  uint32_t black, white, grey;
  black = strip.Color(0,0,0);
  white = strip.Color(255,255,255);
  for (row=0; row<row_count; row++) {
    l = row_offsets[row] + cols[left][3-row];
    r = row_offsets[row] + cols[right][3-row];
    for(c=row_offsets[row]; c<row_offsets[row+1]; c++) {
      if (c < l) {
        if(fade) {
          tmp = falloff_brightness[abs(l-c)];
          grey = strip.Color(tmp, tmp, tmp);
        } else {
          grey=black;
        }        
        strip.setPixelColor(phy_pixel_map[c], grey);
      } else if (c > r) {
        if(fade) {
          tmp = falloff_brightness[abs(r-c)];
          grey = strip.Color(tmp, tmp, tmp);
        } else {
          grey=black;
        }       
        strip.setPixelColor(phy_pixel_map[c], grey);
      } else {
        strip.setPixelColor(phy_pixel_map[c], white);
      }
    }
  }
      strip.show();
delay(20);
}



// to be removed...


//
//void rainbow(uint8_t wait) {
//  uint16_t i, j;
//  
//  for(j=0; j<256; j++) {
//    for(i=0; i<strip.numPixels(); i++) {
//      strip.setPixelColor(i, Wheel((i+j) & 255));
//    }
//    strip.show();
//    delay(wait);
//  }
//}   
//
//// Slightly different, this makes the rainbow equally distributed throughout
//void rainbowCycle(uint8_t wait) {
//  uint16_t i, j;
//  
//  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
//    for(i=0; i< strip.numPixels(); i++) {
//      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
//    }
//    strip.show();
//    delay(wait);
//  }   
//}       
//    
//// Input a value 0 to 255 to get a color value.
//// The colours are a transition r - g - b - back to r.
//uint32_t Wheel(byte WheelPos) {
//  WheelPos = 255 - WheelPos;
//  if(WheelPos < 85) {
//    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
//  }
//  if(WheelPos < 170) {
//    WheelPos -= 85;
//    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
//  }
//  WheelPos -= 170;
//  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
//}


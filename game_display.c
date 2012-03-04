/*
 *  * Copyright (c) 2004 Xilinx, Inc.  All rights reserved.
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A 
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR 
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION 
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE 
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO 
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO 
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE 
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY 
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 */

// Located in: ppc405_0/include/xparameters.h
#include "xparameters.h"
#include "xutil.h"

#define HORIZONTAL_PIXELS 640
#define VERTICAL_PIXELS 480


enum _COLOR {
        white = 0,
	black,
	yellow,
	cyan,
	green,
	magenta,
	red,
	blue
} typedef COLOR;


// funtion prototypes
int runTestPatternRGB(int  word_count);
void colorPixel(COLOR c, int x, int y, unsigned int **pDisplay_data, int *x0, int *y0);
void drawSmiley(int x0, int y0, int radius, int is_alive);
int drawDigit(int x, int y, char digit, int scale_factor, COLOR c);


//Helper functions for drawSmiley
void drawCircle(int x0, int y0, int radius, COLOR border);
void drawArc(int x0, int y0, int radius, COLOR border, int is_upside_down);
unsigned int* drawX(int x, int y, int pixels_in_branch_of_x, COLOR c);
//====================================================

int main (void) {

   print("-- Entering main() --\r\n");

    runTestPatternRGB(309760);

	//while(1){
		print("dbg\r\n");
		//runTestPatternRGB(309760);
		//drawSmiley(430,350,65,1);
		//drawSmiley(430,100,65,0);
		//drawDigit(100, 100, '0', 1, cyan);

	//}
  

   print("-- Exiting main() --\r\n");
   return 0;
}


/* 
   drawX is a helper function for drawSmiley. It will
   draw the letter "X" with color c centered at x,y. pixels_in_each_branch represents 
   how many pixels are in each of the 4 branches that compose "X". The larger
   pixels_in_each_branch is, the larger the "X" displayed will be.
*/

unsigned int* drawX(int x, int y, int pixels_in_branch_of_x, COLOR c) {
    unsigned int* pDisplay_data   = (unsigned int  *)0x07E00000;
	unsigned int* center_of_x;
    int x_current = 0;
    int y_current = 0;
	int pixel_coordX_to_top_left_of_current = 0;
	int pixel_coordY_to_top_left_of_current = 0;
	int i = 0;
	//Color the center of the X
    colorPixel(c,x,y,&pDisplay_data,&x_current,&y_current);
	center_of_x = pDisplay_data;

    //Make the top left branch
	for(i = 0; i < pixels_in_branch_of_x; i++) {
	    //Color the pixel directly to the top left of the current pixel
		pixel_coordX_to_top_left_of_current = x_current - 1;
		pixel_coordY_to_top_left_of_current = y_current - 1;
		colorPixel(c,pixel_coordX_to_top_left_of_current, 
		                           pixel_coordY_to_top_left_of_current, &pDisplay_data, &x_current, &y_current);
	    if(!pDisplay_data){
		    print("Going out of bounds!!"); //We went out of bounds!!
			return;
	    }
    } 

        //reset display pointer back to center of X	
	x_current = x;
	y_current = y;
	pDisplay_data = center_of_x;
	
	 //Make the top right branch
	for(i = 0; i < pixels_in_branch_of_x; i++) {
	    //Color the pixel directly to the top left of the current pixel
		pixel_coordX_to_top_left_of_current = x_current + 1;
		pixel_coordY_to_top_left_of_current = y_current - 1;
		colorPixel(c,pixel_coordX_to_top_left_of_current, 
		                           pixel_coordY_to_top_left_of_current, &pDisplay_data, &x_current, &y_current);
	    if(!*pDisplay_data){
		    print("Going out of bounds!!"); //We went out of bounds!!
			return;
	    }
    } 
	
	x_current = x;
	y_current = y;
	pDisplay_data = center_of_x;
	
	 //Make the bottom left branch
	for(i = 0; i < pixels_in_branch_of_x; i++) {
	    //Color the pixel directly to the top left of the current pixel
		pixel_coordX_to_top_left_of_current = x_current - 1;
		pixel_coordY_to_top_left_of_current = y_current + 1;
		colorPixel(c,pixel_coordX_to_top_left_of_current, 
		                           pixel_coordY_to_top_left_of_current, &pDisplay_data, &x_current, &y_current);
	    if(!pDisplay_data){
		    print("Going out of bounds!!"); //We went out of bounds!!
			return;
	    }
		
    } 
	
	x_current = x;
	y_current = y;
	pDisplay_data = center_of_x;
	
    //Make the bottom right branch
	for(i = 0; i < pixels_in_branch_of_x; i++) {
	    //Color the pixel directly to the top left of the current pixel
		pixel_coordX_to_top_left_of_current = x_current + 1;
		pixel_coordY_to_top_left_of_current = y_current + 1;
		colorPixel(c,pixel_coordX_to_top_left_of_current, 
		                           pixel_coordY_to_top_left_of_current, &pDisplay_data, &x_current, &y_current);
	     if(!pDisplay_data){
		    print("Going out of bounds!!"); //We went out of bounds!!
			return;
	    }
    } 
}



/* Call this function to draw a face centered at
   x0,y0 with the specified radius. is_alive specifies
   whether the face is dead or alive (0 = dead, 1 = alive).
   If the face is dead, draw a frowning face with X'ed out
   eyes. If the face has not been hit, draw a smiling
   face with circles for eyes.
*/


void drawSmiley(int x, int y, int radius, int is_alive) {
    int dist_mouth_from_center = radius*.6;
	int dist_eyes_from_center = radius/2;
	int pixels_in_branch_of_x = 8; //How many pixels in each of the 4 branches that compose the letter "X"
	COLOR border_color = yellow;
        COLOR mouth_color  = magenta;
	COLOR eye_color = blue;

    //Draw the smiley's border
    drawCircle(x,y,radius,border_color);
	
	//Draw the smiley's eyes and mouth
	if(is_alive) {
	    //Draw the right eye first. .707 is approximately cos(45)= sin(45)
	    drawCircle(x + (int)(dist_eyes_from_center * .707) , 
		      y - (int)(dist_eyes_from_center * .707), 10, cyan);
	    //Draw the left eye next. 
		drawCircle(x - (int)(dist_eyes_from_center * .707) , 
		      y - (int)(dist_eyes_from_center * .707), 10, cyan);	
			  
	    //Now draw an arc to represent a smiley face
		drawArc(x, y + dist_mouth_from_center, 20, magenta, 0);	
	}
	else {
	    //Draw an X in place of each eye
		
	    //Draw the right eye first. .707 is approximately cos(45)= sin(45)
		drawX(x + (int)(dist_eyes_from_center * .707) , 
		      y - (int)(dist_eyes_from_center * .707), 10, cyan);
	    //Draw the left eye next.
		drawX(x - (int)(dist_eyes_from_center * .707) , 
		      y - (int)(dist_eyes_from_center * .707), 10, cyan);	
			  
	    //Now draw an upside down arc to represent a frowning face
	    drawArc(x, y + dist_mouth_from_center, 20, magenta, 1);	
	}
	
}



/* drawCircle will draw a circle of color border centered at x0,y0 
   with the specified radius. MAKE SURE YOU DON'T DRAW ANYTHING
   OUT OF BOUNDS BECAUSE THERE AREN'T REALLY ANY MAJOR ERROR
   CHECKS!!!!
*/

void drawCircle(int x0, int y0, int radius, COLOR border) {
  unsigned int* pDisplay_data   = (unsigned int  *)0x07E00000;

  int f = 1 - radius;
  int ddF_x = 1;
  int ddF_y = -2 * radius;
  int x = 0;
  int y = radius;
  int x_current = 0;
  int y_current = 0;
 
 
 
  //Bottom Middle
  colorPixel(border, x0, y0 + radius, &pDisplay_data, &x_current, &y_current);
  
  //Top Middle
  colorPixel(border, x0, y0 - radius, &pDisplay_data, &x_current, &y_current);
  
  //Right Middle
  colorPixel(border, x0 + radius, y0, &pDisplay_data, &x_current, &y_current);
  
  //Left Middle
  colorPixel(border, x0 - radius, y0, &pDisplay_data, &x_current, &y_current);
 

  while(x < y)
  {
    // ddF_x == 2 * x + 1;
    // ddF_y == -2 * y;
    // f == x*x + y*y - radius*radius + 2*x - y + 1;
    if(f >= 0) 
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;    
	
        colorPixel(border, x0 + x, y0 + y, &pDisplay_data, &x_current, &y_current);
	colorPixel(border, x0 - x, y0 + y, &pDisplay_data, &x_current, &y_current);
	colorPixel(border, x0 + x, y0 - y, &pDisplay_data, &x_current, &y_current);
	colorPixel(border, x0 - x, y0 - y, &pDisplay_data, &x_current, &y_current);
	colorPixel(border, x0 + y, y0 + x, &pDisplay_data, &x_current, &y_current);
	colorPixel(border, x0 - y, y0 + x, &pDisplay_data, &x_current, &y_current);
	colorPixel(border, x0 + y, y0 - x, &pDisplay_data, &x_current, &y_current);
        colorPixel(border, x0 - y, y0 - x, &pDisplay_data, &x_current, &y_current);
  }
}


/* Helper function to draw the smile/frown on the face. is_upside_down
   indicates whether the arc is upside down or not. x,y determine where
   the arc should be centered at, border determines the color of the arc,
   and radius determines the radius of the circle the arc would belong to
   */


void drawArc(int x0, int y0, int radius, COLOR border, int is_upside_down) {
  unsigned int* pDisplay_data   = (unsigned int  *)0x07E00000;

  int f = 1 - radius;
  int ddF_x = 1;
  int ddF_y = -2 * radius;
  int x = 0;
  int y = radius;
  int x_current = 0;
  int y_current = 0;
  
  if(is_upside_down) {
      y0 += radius;
      colorPixel(border, x0, y0 - radius, &pDisplay_data, &x_current, &y_current);
   }
  else {
       y0 -= radius;
       colorPixel(border, x0, y0 + radius, &pDisplay_data, &x_current, &y_current);
  }
	  
  while(x < y)
  {
    // ddF_x == 2 * x + 1;
    // ddF_y == -2 * y;
    // f == x*x + y*y - radius*radius + 2*x - y + 1;
    if(f >= 0) 
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;    
	
	if(is_upside_down) {
	    colorPixel(border, x0 + x, y0 - y, &pDisplay_data, &x_current, &y_current);
	    colorPixel(border, x0 - x, y0 - y, &pDisplay_data, &x_current, &y_current);
	}
	else {
	      colorPixel(border, x0 + x, y0 + y, &pDisplay_data, &x_current, &y_current);
	      colorPixel(border, x0 - x, y0 + y, &pDisplay_data, &x_current, &y_current);
	}
  }
}


/* 
    Color a pixel at location x,y with color c. x0,y0 mark the current position of the
display pointer pDisplay_data in terms of pixels with x0 representing pixel column and
y0 representing pixel row. Returns a pointer to new pixel position. If an invalid pixel
position was specified, return NULL. Treat the top left of the screen as pixel 0,0
*/

void colorPixel(COLOR c, int x, int y, unsigned int **pDisplay_data, int *x0, int *y0){
    volatile unsigned int rgb_value_array[16] = {0x00FFFFFF, 0x00000000, 0x00FFFF00, 0x0000FFFF, 0x0000FF00
	  											  ,0x00FF00FF, 0x00FF0000, 0x000000FF};
    int pixels_away = 0; //How many pixels away is the destination pixel, counting from left to right
											
	//Make sure x,y are in bounds	
	if(x < 0 || x >= HORIZONTAL_PIXELS || y < 0 || y >= VERTICAL_PIXELS) {
	    *pDisplay_data = NULL;
	    return ;
    }
		
	//Advance display pointer to correct position.
	pixels_away = (y - *y0)*(1024) + (x-*x0);
	*pDisplay_data += pixels_away;
	**pDisplay_data = rgb_value_array[c];

        //update current position
        *y0 = y;
        *x0 = x;
}										
												  


//Old code from the subtitle project

int runTestPatternRGB(int  word_count) {
  	unsigned int i, x, wordnum;
  	volatile unsigned int rgb_value;
  	unsigned int block_start; 
  	unsigned int  *pDisplay_data;
	volatile unsigned int rgb_value_array[16] = {0x00FFFFFF, 0x00000000, 0x00FFFF00, 0x0000FFFF, 0x0000FF00
	  											  ,0x00FF00FF, 0x00FF0000, 0x000000FF};

	//  pDisplay_data   = (unsigned int  *)0x0FE00000;
	pDisplay_data   = (unsigned int  *)0x07E00000;

  	wordnum = 0;
  	x = 0;
	i = 0;

	print("\n\rRunning rgb Test Pattern...\t");
  	print("...\t");

	// write pattern to DDR
	while (wordnum < word_count)
		{
		// Color pattern
		if (i < 60)
			rgb_value = rgb_value_array[1]; // white
		else if (i < 120)
			rgb_value = rgb_value_array[1]; // black
		else if (i < 180)
			rgb_value = rgb_value_array[1]; // yellow
		else if (i < 240)
			rgb_value = rgb_value_array[1]; // cyan
		else if (i < 300)
			rgb_value = rgb_value_array[1]; // green
		else if (i < 360)
			rgb_value = rgb_value_array[1]; // magenta
		else if (i < 420)
			rgb_value = rgb_value_array[1]; // red
		else if (i < 480)
			rgb_value = rgb_value_array[1]; // blue
			

		*(pDisplay_data) = rgb_value;
		pDisplay_data++;
		wordnum++;
		x++;

		if (x >= 640){
			x = 0;
		 	pDisplay_data += (1024 - 640);
			i++;
		}
	}
  	
  	print("Passed!\n\r");
  	return ;

} // end runTestPattern




/* Draw a digit. If scale_factor = 1, draw a digit that has dimensions
   of 8 x 12 pixels. If scale_factor = 2, draw a digit that has dimensions
   of 16 * 24. If scale_factor = 3, draw a digit that has dimenstions
   of 24 * 36, and so on. pDisplay_data is a pointer to the display
   position and x,y represent the top leftmost position of the digit.
   This top leftmost position is determined by the templates
   from lab3.
*/

int drawDigit(int x, int y, char digit, int scale_factor, COLOR c) {
    unsigned int* pDisplay_data   = (unsigned int  *)0x07E00000;
    int horizontal_pixels = 8 * scale_factor; //X dimension of our pixel
    int vertical_pixels = 12 * scale_factor;  //Y dimension of our pixel
    int x_current = 0;
    int y_current = 0;
    int i = 0;
    int j = 0;

    //Error check
    if( (x+horizontal_pixels) >= HORIZONTAL_PIXELS || x < 0 ||
        (y+vertical_pixels) >= VERTICAL_PIXELS || y < 0 ) 
        return -1; //Out of bounds!!!!!!
    
    //Draw digit 0,1......or 9
    switch(digit) {
        case '0':
            //Draw a '0'. Go column by column

            for(j = 0; j < scale_factor; j++) {
                //column 1 and column 8 
                for(i = 0; i < 8*scale_factor; i++) {
                    colorPixel(c, x, y + 2 + i, &pDisplay_data, &x_current, &y_current);
                    colorPixel(c, x + (8*scale_factor - 1 - j)), y + 2 + i, &pDisplay_data, &x_current, &y_current);
                }
                x++;
            }
 
            for(j = 0; j < scale_factor; j++) { 
                //column 2 and 7
                for(i = 0; i < 10*scale_factor; i++) {
                    colorPixel(c, x, y + 1 + i, &pDisplay_data, &x_current, &y_current);
                    colorPixel(c, x + (8*scale_factor - scale_factor*2 - 1 - j), y + 1 + i, &pDisplay_data, &x_current, &y_current);
                }
                x++;
            }

            for(j = 0; j < scale_factor; j++) {
                //column 3 and 6
                for(i = 0; i < 3*scale_factor; i++) {
                    colorPixel(c, x, y + i, &pDisplay_data, &x_current, &y_current);
                    colorPixel(c, x, y + 9 + i,&pDisplay_data, &x_current, &y_current);
                    colorPixel(c, x + (8*scale_factor - scale_factor*4 - 1 - j), y + i, &pDisplay_data, &x_current, &y_current);
                    colorPixel(c, x + (8*scale_factor - scale_factor*4 - 1 - j), y + 9 + i,&pDisplay_data, &x_current, &y_current);
                }
                x++; 
            }

            for(j = 0; j < scale_factor; j++) {
                //column 4 and 5
                for(i = 0; i < 2*scale_factor; i++) {
                    colorPixel(c, x, y + i, &pDisplay_data, &x_current, &y_current);
                    colorPixel(c, x, y + 5, &pDisplay_data, &x_current, &y_current);
                    colorPixel(c, x, y + 10 + i,&pDisplay_data, &x_current, &y_current);
                    colorPixel(c, x + (8*scale_factor - scale_factor*6 - 1 - j), y + i, &pDisplay_data, &x_current, &y_current);
                    colorPixel(c, x + (8*scale_factor - scale_factor*6 - 1 - j), y + 5, &pDisplay_data, &x_current, &y_current);
                    colorPixel(c, x + (8*scale_factor - scale_factor*6 - 1 - j), y + 10 + i,&pDisplay_data, &x_current, &y_current);

                }
            }
            break;
        case '1':
            //Draw a '1'. Go column by column

            //column 1 (empty)
            for(j = 0; j < scale_factor; j++)
                x++;
        
            //column 2 and column 3
            for(j = 0; j < scale_factor; j++) {
                for(i = 0; i < 2*scale_factor; i++) {
                    colorPixel(c, x, y + 1 + i, &pDisplay_data, &x_current, &y_current);
                    colorPixel(c, x + (scale_factor*2 - 1 - j), y + 1 + i, &pDisplay_data, &x_current, &y_current);
                }
                x++; 
            }           

            for(j = 0; j < scale_factor; j++) { 
                //column 4 and 5  
                for(i = 0; i < 12*scale_factor; i++) {
                    colorPixel(c, x, y + i, &pDisplay_data, &x_current, &y_current);
                    colorPixel(c, x + (scale_factor*2 - 1 - j), y + i, &pDisplay_data, &x_current, &y_current);
                }
                x++;
            }

            //column 6,7,8 (all empty)
            break;
        case '2':
           //Draw a '2'.

           //column 1
           
           //Multiply by scale_factor to get updated y offset

           for(j = 0; j < scale_factor; j++) {
               for(i = 0; i < scale_factor; i++) {
                   colorPixel(c, x, y + 2*scale_factor + i,&pDisplay_data, &x_current, &y_current);
               }
               for(i = 0; i < scale_factor*3; i++) {
                   colorPixel(c, x, y + 9*scale_factor + i,&pDisplay_data, &x_current, &y_current);
               }
               x++;
           }


           //column 2
           for(j = 0; j < scale_factor; j++) {
               for(i = 0; i < scale_factor*2; i++) {
                   colorPixel(c, x, y + 1*scale_factor + i,&pDisplay_data, &x_current, &y_current);
               }
               for(i = 0; i < scale_factor*4; i++) {
                   colorPixel(c, x, y + 8*scale_factor + i,&pDisplay_data, &x_current, &y_current);
               }
               x++;
           }

            
           //column 3
           for(j = 0; j < scale_factor; j++) {
               for(i = 0; i < scale_factor*3; i++) {
                   colorPixel(c, x, y + i,&pDisplay_data, &x_current, &y_current);
               }
               for(i = 0; i < scale_factor*5; i++) {
                   colorPixel(c, x, y + 7*scale_factor + i,&pDisplay_data, &x_current, &y_current);
               }
               x++;
           }

           //column 4
           for(j = 0; j < scale_factor; j++) {
               for(i = 0; i < scale_factor*2; i++) {
                   colorPixel(c, x, y + i,&pDisplay_data, &x_current, &y_current);
               }
               for(i = 0; i < scale_factor*3; i++) {
                   colorPixel(c, x, y + 6*scale_factor + i,&pDisplay_data, &x_current, &y_current);
               }
               for(i = 0; i < scale_factor*2; i++) {
                   colorPixel(c, x, y + 10*scale_factor + i,&pDisplay_data, &x_current, &y_current);
               }
               x++;
           }

           //column 5
           for(j = 0; j < scale_factor; j++) {
               for(i = 0; i < scale_factor*2; i++) {
                   colorPixel(c, x, y + i,&pDisplay_data, &x_current, &y_current);
               }
               for(i = 0; i < scale_factor*3; i++) {
                   colorPixel(c, x, y + 5*scale_factor + i,&pDisplay_data, &x_current, &y_current);
               }
               for(i = 0; i < scale_factor*2; i++) {
                   colorPixel(c, x, y + 10*scale_factor + i,&pDisplay_data, &x_current, &y_current);
               }
               x++;
           }

           
           //column 6
           for(j = 0; j < scale_factor; j++) {
               for(i = 0; i < scale_factor*3; i++) {
                   colorPixel(c, x, y + i,&pDisplay_data, &x_current, &y_current);
               }
               for(i = 0; i < scale_factor*3; i++) {
                   colorPixel(c, x, y + 4*scale_factor + i,&pDisplay_data, &x_current, &y_current);
               }
               for(i = 0; i < scale_factor*2; i++) {
                   colorPixel(c, x, y + 10*scale_factor + i,&pDisplay_data, &x_current, &y_current);
               }
               x++;
           }

           //column 7
           for(j = 0; j < scale_factor; j++) {
               for(i = 0; i < scale_factor*5; i++) {
                   colorPixel(c, x, y + scale_factor + i,&pDisplay_data, &x_current, &y_current);
               }
               for(i = 0; i < scale_factor*2; i++) {
                   colorPixel(c, x, y + 10*scale_factor + i,&pDisplay_data, &x_current, &y_current);
               }
               x++;
           }

           //column 8
           for(j = 0; j < scale_factor; j++) {
               for(i = 0; i < scale_factor*3; i++) {
                   colorPixel(c, x, y + 2*scale_factor + i,&pDisplay_data, &x_current, &y_current);
               }
               for(i = 0; i < scale_factor*2; i++) {
                   colorPixel(c, x, y + 10*scale_factor + i,&pDisplay_data, &x_current, &y_current);
               }
               x++;
           }
           break;

       case '3':
           //column 1
           for(j = 0; j < scale_factor; j++) {
               for(i = 0; i < scale_factor*1; i++) {
                   colorPixel(c, x, y + 2*scale_factor + i,&pDisplay_data, &x_current, &y_current);
               }
               for(i = 0; i < scale_factor*1; i++) {
                   colorPixel(c, x, y + 9*scale_factor + i,&pDisplay_data, &x_current, &y_current);
               }
               x++;
           }
       
          //column 2
	   for(j = 0; j < scale_factor; j++) {
	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y + 1*scale_factor + i,&pDisplay_data, &x_current, &y_current);
	       }
	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y + 9*scale_factor + i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   } 

           //column 3
	   for(j = 0; j < scale_factor; j++) {
	       for(i = 0; i < scale_factor*3; i++) {
		   colorPixel(c, x, y + i,&pDisplay_data, &x_current, &y_current);
	       }
	       for(i = 0; i < scale_factor*3; i++) {
		   colorPixel(c, x, y + 9*scale_factor + i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }

           
           //column 4
	   for(j = 0; j < scale_factor; j++) {
	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y + i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y + 5*scale_factor + i,&pDisplay_data, &x_current, &y_current);
	       }
	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y + 10*scale_factor + i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }

            
           //column 5
	   for(j = 0; j < scale_factor; j++) {
	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y + i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y + 5*scale_factor + i,&pDisplay_data, &x_current, &y_current);
	       }
	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y + 10*scale_factor + i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }

            
           //column 6
	   for(j = 0; j < scale_factor; j++) {
	       for(i = 0; i < scale_factor*3; i++) {
		   colorPixel(c, x, y + i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*4; i++) {
		   colorPixel(c, x, y + 4*scale_factor + i,&pDisplay_data, &x_current, &y_current);
	       }
	       for(i = 0; i < scale_factor*3; i++) {
		   colorPixel(c, x, y + 9*scale_factor + i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }

           //column 7
	   for(j = 0; j < scale_factor; j++) {
	       for(i = 0; i < scale_factor*4; i++) {
		   colorPixel(c, x, y + 1*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*5; i++) {
		   colorPixel(c, x, y + 6*scale_factor + i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }

           
           //column 8
	   for(j = 0; j < scale_factor; j++) {
	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y + 2*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*3; i++) {
		   colorPixel(c, x, y + 7*scale_factor + i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }
           break;

       case '4':

           //column 1
	   for(j = 0; j < scale_factor; j++) {
	       for(i = 0; i < scale_factor*3; i++) {
		   colorPixel(c, x, y + 5*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }

           
           //column 2
	   for(j = 0; j < scale_factor; j++) {
	       for(i = 0; i < scale_factor*8; i++) {
		   colorPixel(c, x, y + i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }

           
           //column 3
	   for(j = 0; j < scale_factor; j++) {
	       for(i = 0; i < scale_factor*5; i++) {
		   colorPixel(c, x, y + i,&pDisplay_data, &x_current, &y_current);
	       }
 	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y + 6*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }              
	       x++;
	   }

           //column 4
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y + 6*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }              
	       x++;
	   }
 
           //column 5
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y + 6*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }              
	       x++;
	   }

           //column 6
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*11; i++) {
		   colorPixel(c, x, y + 1*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }              
	       x++;
	   }
 
           //column 7
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*11; i++) {
		   colorPixel(c, x, y + 1*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }              
	       x++;
	   }
          
           //column 6
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y + 6*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }              
	       x++;
	   }

       case '5':
           //column 1
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*6; i++) {
		   colorPixel(c, x, y + i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*1; i++) {
		   colorPixel(c, x, y + 9*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }              
	       x++;
	   }

           //column 2
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*6; i++) {
		   colorPixel(c, x, y + i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y + 9*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }              
	       x++;
	   }


           //column 3
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y + i,&pDisplay_data, &x_current, &y_current);
	       }
 	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y + 4*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*3; i++) {
		   colorPixel(c, x, y + 9*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }              
	       x++;
	   }


           //column 4
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y + i,&pDisplay_data, &x_current, &y_current);
	       }
 	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y + 4*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y + 10*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }              
	       x++;
	   }


           //column 5
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y + i,&pDisplay_data, &x_current, &y_current);
	       }
 	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y + 4*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y + 10*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }              
	       x++;
	   }


           //column 6
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y + i,&pDisplay_data, &x_current, &y_current);
	       }
 	       for(i = 0; i < scale_factor*3; i++) {
		   colorPixel(c, x, y + 4*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*3; i++) {
		   colorPixel(c, x, y + 9*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }              
	       x++;
	   }


           //column 7
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y + i,&pDisplay_data, &x_current, &y_current);
	       }
 	       for(i = 0; i < scale_factor*6; i++) {
		   colorPixel(c, x, y + 5*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }

           //column 8
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y + i,&pDisplay_data, &x_current, &y_current);
	       }
 	       for(i = 0; i < scale_factor*4; i++) {
		   colorPixel(c, x, y + 6*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }
     
       case '6'
           //column 1
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*8; i++) {
		   colorPixel(c, x, y + 2*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   } 

           //column 2
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*10; i++) {
		   colorPixel(c, x, y + 1*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }

           //column 3
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*3; i++) {
		   colorPixel(c, x, y+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+6*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*3; i++) {
		   colorPixel(c, x, y+9*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
               x++;
           }


           //column 4
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+5*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+10*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
               x++;
           }
             
           //column 5
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+5*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+10*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }

	       x++;
	   }

           //column 6
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*3; i++) {
		   colorPixel(c, x, y+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*3; i++) {
		   colorPixel(c, x, y+5*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*3; i++) {
		   colorPixel(c, x, y+9*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }

	       x++;
	   }


           //column 7
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+scale_factor*1+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*5; i++) {
		   colorPixel(c, x, y+6*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }


           //column 8
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*1; i++) {
		   colorPixel(c, x, y+scale_factor*2+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*3; i++) {
		   colorPixel(c, x, y+7*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }
           break;

       case '7':

           //column 1
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }

           //column 2
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }
           break;

           //column 3
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*7; i++) {
		   colorPixel(c, x, y+5*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }

           
           //column 4
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*8; i++) {
		   colorPixel(c, x, y+4*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }


           //column 5
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+3*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }


           //column 6
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*4; i++) {
		   colorPixel(c, x, y+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }

           //column 7
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*3; i++) {
		   colorPixel(c, x, y+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }

           //column 8 blank
           break;
        
       case '8':
           //column 1
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+2*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*3; i++) {
		   colorPixel(c, x, y+7*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }


           //column 2
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*4; i++) {
		   colorPixel(c, x, y+1*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*5; i++) {
		   colorPixel(c, x, y+6*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }


           //column 3
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*3; i++) {
		   colorPixel(c, x, y+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*4; i++) {
		   colorPixel(c, x, y+4*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*3; i++) {
		   colorPixel(c, x, y+9*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }

           //column 4
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+6*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+10*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }


           //column 5
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+6*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+10*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }


           //column 6
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*3; i++) {
		   colorPixel(c, x, y+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*4; i++) {
		   colorPixel(c, x, y+4*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*3; i++) {
		   colorPixel(c, x, y+9*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }

           //column 7
           for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*4; i++) {
		   colorPixel(c, x, y+1*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*5; i++) {
		   colorPixel(c, x, y+6*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }



           //column 8
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+2*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*3; i++) {
		   colorPixel(c, x, y+7*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }

	   break;

      case '9':
           //column 1
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*3; i++) {
		   colorPixel(c, x, y+2*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*1; i++) {
		   colorPixel(c, x, y+9*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }


           //column 2
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*5; i++) {
		   colorPixel(c, x, y+1*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+9*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }
 
           //column 3
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*3; i++) {
		   colorPixel(c, x, y+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*3; i++) {
		   colorPixel(c, x, y+4*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*3; i++) {
		   colorPixel(c, x, y+9*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }

           //column 4
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+5*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+10*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }

           //column 5
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+5*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+10*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }        

           //column 6
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*3; i++) {
		   colorPixel(c, x, y+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y+4*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
               for(i = 0; i < scale_factor*3; i++) {
		   colorPixel(c, x, y+9*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }


           //column 7
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*10; i++) {
		   colorPixel(c, x, y+1*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }

           //column 8
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*8; i++) {
		   colorPixel(c, x, y+2*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }
	       x++;
	   }

    }

    return 1; 
}

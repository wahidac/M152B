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
#include <xbasic_types.h>
#include <xi2c_l.h>
#include <uart.h>
#include <xgpio.h>
#include <stdio.h>
#include "xtime_l.h"

#define HORIZONTAL_PIXELS 640
#define VERTICAL_PIXELS 480

// GPIO stuff
XGpio video_mung;

#define GPO_REG_OFFSET 0x124
//#define DECODER_ADDR 0x21  //Read: 0x43, Write: 0x42
#define DECODER_ADDR 0x20 //Read: 0x41, Write: 0x40
#define SEND_CNT 3
#define RECV_CNT 3
#define GPO_RESETS_OFF 1
#define GPO_RESET_IIC 3
#define GPO_RESET_DECODER 0

struct VideoModule {
  Xuint8 addr;
  Xuint8 config_val;
  Xuint8 actual_val;
};

#define DECODER_SVID_CONFIG_CNT 17
struct VideoModule decoder_svid[] = { 
  { 0x00, 0x06, 0 },
  { 0x15, 0x00, 0 },
  { 0x27, 0x58, 0 },
  { 0x3a, 0x12, 0 },
  { 0x50, 0x04, 0 },
  { 0x0e, 0x80, 0 },
  { 0x50, 0x20, 0 },
  { 0x52, 0x18, 0 }, 
  { 0x58, 0xed, 0 },
  { 0x77, 0xc5, 0 },
  { 0x7c, 0x93, 0 },
  { 0x7d, 0x00, 0 },
  { 0xd0, 0x48, 0 },
  { 0xd5, 0xa0, 0 },
  { 0xd7, 0xea, 0 },
  { 0xe4, 0x3e, 0 },
  { 0xea, 0x0f, 0 }, 
  { 0x0e, 0x00, 0 } };

#define DECODER_COMP_CONFIG_CNT 18
struct VideoModule decoder_comp[] = { 
  { 0x00, 0x04, 0 },
  { 0x15, 0x00, 0 },
  { 0x17, 0x41, 0 },
  { 0x27, 0x58, 0 },
  { 0x3a, 0x16, 0 }, 
  { 0x50, 0x04, 0 },
  { 0x0e, 0x80, 0 },
  { 0x50, 0x20, 0 },
  { 0x52, 0x18, 0 }, 
  { 0x58, 0xed, 0 },
  { 0x77, 0xc5, 0 },
  { 0x7c, 0x93, 0 },
  { 0x7d, 0x00, 0 },
  { 0xd0, 0x48, 0 },
  { 0xd5, 0xa0, 0 },
  { 0xd7, 0xea, 0 },
  { 0xe4, 0x3e, 0 },
  { 0xea, 0x0f, 0 }, 
  { 0x0e, 0x00, 0 } };


#define DECODER_CMPNT_CONFIG_CNT 13
struct VideoModule decoder_cmpnt[] = { 
  { 0x00, 0x0a, 0 },
  { 0x27, 0xd8, 0 },
  { 0x50, 0x04, 0 },
  { 0x0e, 0x80, 0 },
  { 0x52, 0x18, 0 }, 
  { 0x58, 0xed, 0 },
  { 0x77, 0xc5, 0 },
  { 0x7c, 0x93, 0 },
  { 0x7d, 0x00, 0 },
  { 0xd0, 0x48, 0 },
  { 0xd5, 0xa0, 0 },
  { 0xe4, 0x3e, 0 },
  { 0x0e, 0x00, 0 } };

//
// funtion prototypes
//

void configDecoder(struct VideoModule *decoder, int config_cnt );
void Reset_xup_decoder(void);
void main_memu(void);
unsigned char get_hex_byte();
void edit_i2c_reg(void);

//////////////////////////////////////////////////////////////////////

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


/************ FUNCTION PROTOTYPES ****************/

//Hand display functions
void updateHandPosition(int x_current, int y_current, unsigned int* old_pixel_values, 
                        int x, int y, int hand_radius, COLOR hand_color);
void savePixels(int x0, int y0, int radius, unsigned int *old_pixel_values);
void restorePixels(int x0, int y0, int radius, unsigned int *old_pixel_values);


//Gameplay functions
void gameLoop();
int removeTimeoutSmiley(int* smileyPos, XTime* time, int time_allowed);
int drawRandomSmileys(int numSmileys, int* smileyPos, XTime* timeOfSpawn);
int checkSmiley (int x, int y);
int killSmiley(int* smileyPos, XTime* timeOfDeath, int deadSmileyPos);
int cleanUpDeadSmileys(int* smileyPos, XTime* timeOfDeath);
void endGame(int score);


//Drawing functions
int colorScreen(int  word_count, COLOR c);
int drawScore( int s, COLOR COL_USED, int SCORE_LOC, int SCORE_Y, int SCALE );
int drawMisses ( int s, COLOR COL_USED);
void drawSmiley(int x0, int y0, int radius, int is_alive, int is_removing_a_smiley);
void drawGrid(COLOR c);
int drawDigit(int x, int y, char digit, int scale_factor, COLOR c);

//Basic pixel manipulation functions
void colorPixel(COLOR c, int x, int y, unsigned int **pDisplay_data, int *x0, int *y0);
unsigned int returnPixelValue(int x, int y, unsigned int **pDisplay_data, int *x0, int *y0);
void setPixel(unsigned int value, int x, int y, unsigned int **pDisplay_data, int *x0, int *y0);


//Helper functions for drawSmiley
void drawCircle(int x0, int y0, int radius, COLOR border);
void drawArc(int x0, int y0, int radius, COLOR border, int is_upside_down);
unsigned int* drawX(int x, int y, int pixels_in_branch_of_x, COLOR c);

//Debugging functions
void calculateGridPosition(int* GridX, int* GridY, unsigned int x, unsigned int y);

//====================================================

int main (void) {
  Xuint8 start_addr = 0;
  Xuint8 send_data[SEND_CNT] = {0};
  Xuint32 send_cnt;
  Xuint8 recv_data[RECV_CNT] = {0};
  Xuint32 recv_cnt;
  Xuint8 i;
  Xuint8 hold = 0;
  int wait_delay = 50000000;
 
  Xuint32 maxbright, xorval = 0x01;
  Xuint32 frame = 0;

    uartInit(XPAR_RS232_UART_1_BASEADDR);
	print("\r\nup!\r\n");

    XI2c_mWriteReg(XPAR_I2C_BASEADDR, GPO_REG_OFFSET, GPO_RESETS_OFF);// deassert reset to vid decoder
    recv_cnt = 0;
	print("\r\nXUP-V2Pro Video Decoder Expansion Board Video Pass Through Test  ");
    print("\r\nDetecting Video Decoder...\t");
    XI2c_mWriteReg(XPAR_I2C_BASEADDR, GPO_REG_OFFSET, GPO_RESET_IIC);
    XI2c_mWriteReg(XPAR_I2C_BASEADDR, GPO_REG_OFFSET, GPO_RESETS_OFF);
    recv_cnt = XI2c_RSRecv(XPAR_I2C_BASEADDR, DECODER_ADDR, start_addr, 
			   recv_data, 1);
    if( recv_cnt != 1 )
      print("No device detected!\r\n");
    else {
	  configDecoder(decoder_comp, DECODER_COMP_CONFIG_CNT);
      print("Decoder detected! Configuring for composite video -  default.\r\n");
	  XGpio_Initialize(&video_mung, XPAR_VIDEO_MUNG_DEVICE_ID);
	  main_memu();
	  
	  // begin handling brightness...
	}
  
  print("\r\nTest Complete!\r\n\r\n");

   //print("-- Entering main() --\r\n");

    //Color the screen black
    colorScreen(309760, black);
    //Start the main game loop
    gameLoop();

   print("-- Exiting main() --\r\n");
   return 0;
}





/***************************** BEGIN MAIN GAME LOOP ***************************************************/



/* Main game loop */

void gameLoop() {
    int smileyPos[9];
    XTime timeOfDeath[9];
	XTime timeOfSpawn[9];
    int i = 0;
    int user_score = 0;  //For testing to make sure as score increases, so does randomness + speed
	int user_miss = 0;  //Starting Misses
	int smileyRemoved = 0; // Variable for checking if a smiley was removed by game (didnt die)
    int num_smileys_on_screen = 0;
	
    int time_allowed = 600000000;   // Time To Live for Smileys, Decreases with Score
    int add_face = 100000;          // Frequency of Spawn, Increases with Score
	int num_smileys_allowed = 4;    // Number of Smileys at one time, Increases with Score
	
    int random_number = 0;
	unsigned int sampling_frequency = 2000; //How frequently should we sample for hand position?
	unsigned int counter = 0;
	int gridPosX = 0;
	int gridPosY = 0; //For testing
	Xuint32 pos_x = 0;
    Xuint32 pos_y = 0;
	XTime start = 0;
	XTime end = 0;
	int num_removed = 0;
	
	
    //initialization

    //initialize grid to empty and timeDeadAndOnScreen to 0 for all boxes
    for(i = 0; i < 9; i++) {
	    smileyPos[i] = 0;	
		timeOfDeath[i] = 0;
		timeOfSpawn[i] = -1;
    }
	    
	//draw in the 3x3 grid
    drawGrid(green);
	drawScore( user_score, white, 600, 10, 1 );
	drawMisses ( user_miss, magenta );
	

    //NOTE: the 100 is arbitrary. actually calculate it eventually
    //and pass it as an argument to savePixels and restorePixels!!

    unsigned int old_pixel_values[100];

    //Find initial location of user's hands at the start of the game
    Xuint32 pos_x_old = 0;
    Xuint32 pos_y_old = 0;

    pos_x = HORIZONTAL_PIXELS-XGpio_DiscreteRead(&video_mung, 1);
    pos_y = XGpio_DiscreteRead(&video_mung, 2);

    //Save the pixel values of the space you are about to overwrite
    savePixels(pos_x, pos_y, 15, old_pixel_values);

    //Draw in the placement of the new hand
    drawCircle(pos_x,pos_y,15,magenta);

    pos_x_old = pos_x;
    pos_y_old = pos_y;

    //The main loop
	
    while(1) {
	
		/* Remove Smilys based on their time to live */
        if(num_smileys_on_screen > 0) {
			if ( (num_removed = removeTimeoutSmiley(smileyPos, timeOfSpawn, time_allowed)) ) {
				drawMisses ( user_miss, black );       // Clear Misses
				user_miss = user_miss+num_removed;     // Update Misses
				drawMisses ( user_miss, magenta );     // Draw Misses
				num_smileys_on_screen = num_smileys_on_screen-num_removed;	
            }
        }

        if(num_smileys_on_screen < num_smileys_allowed) {
           /* Generate discrete random number from range 1 to add_face.
		   Only if random number is 1 should you add a random smiley face.
		   The higher add_face is in value, the smaller the possibility
		   that the random number generated will equal 1. */
			random_number = rand() % add_face;
            if(random_number == 1) {
                num_smileys_on_screen += drawRandomSmileys(1, smileyPos, timeOfSpawn); 
            }
        } 

        //For testing, decrease probabilities steadly here


        //check where the user's hands are on the grid and display
		if(counter == sampling_frequency) {
		    counter = 0;
	        pos_x = HORIZONTAL_PIXELS-XGpio_DiscreteRead(&video_mung, 1);
          	pos_y = XGpio_DiscreteRead(&video_mung, 2);
            // xil_printf("x coordinate: %d\n", pos_x);
            // xil_printf("y coordinate: %d\n", pos_y);
            updateHandPosition(pos_x_old, pos_y_old, old_pixel_values, 
                           pos_x, pos_y, 15, magenta);
            pos_x_old = pos_x; 
            pos_y_old = pos_y;
			
			//testing!!eventually use function to check both if hit and if in circle, not grid
			
			//NOTE: theres a bug in one of the following functions??
			int index = checkSmiley((int) pos_x, (int) pos_y);
			if (index != -1)
				if ( killSmiley(smileyPos, timeOfDeath, (int)index) ) {
					drawScore(user_score, black, 600, 10, 1);
					drawScore(++user_score, white, 600, 10, 1);
				}
				
            num_smileys_on_screen -= cleanUpDeadSmileys(smileyPos,timeOfDeath);	
            xil_printf("num smileys on screen:%d\r\n",num_smileys_on_screen);			
		}
		else {
		    counter++;
		}
		
		// Simple Difficulty Increasing Switch
		switch (user_score) {
		case 20:
			time_allowed = 250000000;
			add_face = 60000;
			num_smileys_allowed = 4;
			break;
		case 40:
			time_allowed = 100000000;
			add_face = 45000;
			num_smileys_allowed = 4;
			break;
		case 60:
			time_allowed = 80000000;
			add_face = 40000;
			num_smileys_allowed = 5;
			break;
		case 80:
			time_allowed = 75000000;
			add_face = 32000;
			num_smileys_allowed = 6;
			break;
		case 100:
			time_allowed = 70000000;
			add_face = 27000;
			num_smileys_allowed = 7;
			break;
		default:
			break;
		} 
		
		if (user_miss >= 10)
			endGame(user_score);
   
        //check user grid position and see if he/she hit anything. 
	   //if so, mark mole dead and take note
	
	    //call killSmileys. Need to adjust num_smileys_on_screen in
		//the smiley cleanup routine
		//num_smileys_on_screen -= cleanUpDeadSmileys(smileyPos,timeOfDeath);

	
        //of when it died. Remove it maybe 1 second later or so (can decrease as the game speeds up so
        //make proportional to remove_face or add_face? maybe keep an array of times to check on this?
	
		
	}
	
            
}


/********************************* END MAIN GAME LOOP ***************************************************/




/******************************** BEGIN FUNCTIONS FOR DEBUGGING *****************************************/


/* Change the value of GridX,GridY to specify the box the arguments x,y lie in.
   GridX,GridY take values from 0,0 to 2,2 and represent the boxes in the grid
   treating the top-left most box as box 0,0 and the bottom right box
   as box 2,2. Set GridX and GridY to -1,-1 if x,y falls in no grid.
*/

void calculateGridPosition(int* GridX, int* GridY, unsigned int x, unsigned int y) {
    if(x >= 0 && x < 215) { //Lies in first column
        *GridX = 0;
        if( y >= 0 && y < 160)
            *GridY = 0;
        else if( y > 160 && y < 321 )
            *GridY = 1;
        else if( y > 321 && y < 480 )
            *GridY = 2;
        return; 
    }
    if(x > 215 && x < 431) { //Lies in second column
        *GridX = 1;
        if( y >= 0 && y < 160)
            *GridY = 0;
        else if( y > 160 && y < 321 )
            *GridY = 1;
        else if( y > 321 && y < 480 )
            *GridY = 2;
        return;
    }
    if(x > 431 && x < 640) { //Lies in third column
        *GridX = 2;
        if( y >= 0 && y < 160)
            *GridY = 0;
        else if( y > 160 && y < 321 )
            *GridY = 1;
        else if( y > 321 && y < 480 )
            *GridY = 2;
        return;
    }
 
}




/******************************* END FUNCTIONS FOR DEBUGGING **********************************/













/*************************** BEGIN FUNCTIONS TO HANDLE DISPLAYING HAND POSITION *************************/



/* Save all of the original pixels that are going to get covered up by the new hand
   and save them in the old_pixel_values array. radius represents the amount of
   pixels that need to be stored. This function is a near direct clone of
   drawCircle, except in all parts of drawCircle where colorPixel is used
   to color a pixel, the current pixel value is saved in the old_pixel_values
   array.

   //NOTE: implement some sort of bounds checking functionality here!!!!!
*/

void savePixels(int x0, int y0, int radius, unsigned int *old_pixel_values) {
  unsigned int* pDisplay_data   = (unsigned int  *)0x07E00000;

  int f = 1 - radius;
  int ddF_x = 1;
  int ddF_y = -2 * radius;
  int x = 0;
  int y = radius;
  int x_current = 0;
  int y_current = 0;
 
  //Bottom Middle
  *old_pixel_values = returnPixelValue(x0,y0 + radius, &pDisplay_data, &x_current, &y_current);
   old_pixel_values++;

  //Top Middle
  *old_pixel_values = returnPixelValue(x0,y0 - radius, &pDisplay_data, &x_current, &y_current);
  old_pixel_values++;
  
  //Right Middle
  *old_pixel_values = returnPixelValue(x0+radius,y0, &pDisplay_data, &x_current, &y_current);
  old_pixel_values++;
  
  //Left Middle
  *old_pixel_values = returnPixelValue(x0-radius,y0, &pDisplay_data, &x_current, &y_current);
  old_pixel_values++;
  
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

   *old_pixel_values = returnPixelValue(x0 + x,y0 + y, &pDisplay_data, &x_current, &y_current);
    old_pixel_values++;
    
   *old_pixel_values = returnPixelValue(x0 - x,y0 + y, &pDisplay_data, &x_current, &y_current);
    old_pixel_values++;
   
   *old_pixel_values = returnPixelValue(x0 + x,y0 - y, &pDisplay_data, &x_current, &y_current);
    old_pixel_values++;
    
   *old_pixel_values = returnPixelValue(x0 - x,y0 - y, &pDisplay_data, &x_current, &y_current);
    old_pixel_values++;
    
   *old_pixel_values = returnPixelValue(x0 + y,y0 + x, &pDisplay_data, &x_current, &y_current);
    old_pixel_values++;

    *old_pixel_values = returnPixelValue(x0 - y,y0 + x, &pDisplay_data, &x_current, &y_current);
    old_pixel_values++;

    *old_pixel_values = returnPixelValue(x0 + y,y0 - x, &pDisplay_data, &x_current, &y_current);
    old_pixel_values++;

    *old_pixel_values = returnPixelValue(x0 - y,y0 - x, &pDisplay_data, &x_current, &y_current);
    old_pixel_values++;
  }
}


/* Nearly identical to savePixels except we are restoring the pixels rather than saving them */

void restorePixels(int x0, int y0, int radius, unsigned int *old_pixel_values) {
  unsigned int* pDisplay_data   = (unsigned int  *)0x07E00000;

  int f = 1 - radius;
  int ddF_x = 1;
  int ddF_y = -2 * radius;
  int x = 0;
  int y = radius;
  int x_current = 0;
  int y_current = 0;
 
  //Bottom Middle
  setPixel(*old_pixel_values, x0, y0 + radius, &pDisplay_data, &x_current, &y_current);
  old_pixel_values++;

  //Top Middle
  setPixel(*old_pixel_values, x0,y0 - radius, &pDisplay_data, &x_current, &y_current);
  old_pixel_values++;
  
  //Right Middle
  setPixel(*old_pixel_values, x0+radius,y0, &pDisplay_data, &x_current, &y_current);
  old_pixel_values++;
  
  //Left Middle
  setPixel(*old_pixel_values, x0-radius,y0, &pDisplay_data, &x_current, &y_current);
  old_pixel_values++;
  
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

    setPixel(*old_pixel_values, x0 + x,y0 + y, &pDisplay_data, &x_current, &y_current);
    old_pixel_values++;
    
    setPixel(*old_pixel_values,x0 - x,y0 + y, &pDisplay_data, &x_current, &y_current);
    old_pixel_values++;
   
    setPixel(*old_pixel_values, x0 + x,y0 - y, &pDisplay_data, &x_current, &y_current);
    old_pixel_values++;
    
    setPixel(*old_pixel_values, x0 - x,y0 - y, &pDisplay_data, &x_current, &y_current);
    old_pixel_values++;
    
    setPixel(*old_pixel_values, x0 + y,y0 + x, &pDisplay_data, &x_current, &y_current);
    old_pixel_values++;

    setPixel(*old_pixel_values, x0 - y,y0 + x, &pDisplay_data, &x_current, &y_current);
    old_pixel_values++;

    setPixel(*old_pixel_values, x0 + y,y0 - x, &pDisplay_data, &x_current, &y_current);
    old_pixel_values++;

    setPixel(*old_pixel_values, x0 - y,y0 - x, &pDisplay_data, &x_current, &y_current);
    old_pixel_values++;
  }
}



/* Need to save state of what the pixels are that the hand is going over. On call to
   updateHandPosition, restore all the pixels the circle centered at x_current, y_current
   originally covered up when it was first drawn. */

void updateHandPosition(int x_current, int y_current, unsigned int* old_pixel_values, 
                        int x, int y, int hand_radius, COLOR hand_color) {

    //Restore the original pixel values
    restorePixels(x_current, y_current, hand_radius, old_pixel_values);

    //Save the pixel values of the space you are about to overwrite
    savePixels(x, y, hand_radius, old_pixel_values);

    //Draw in the placement of the new hand
    drawCircle(x,y,hand_radius,hand_color);
}


/************************** END FUNCTIONS TO HANDLE DISPLAYING HAND POSITION ***************************/






/************************* BEGIN FUNCTIONS TO HANDLE GAMEPLAY DYNAMICS ********************************/


/* Iterate through smileyPos array and find smileys who are
   dead and are still displayed on the screen. Remove them
   if MaxTimeDeadOnScreen has elapsed. Return the number
   of Smileys that were cleared from the screen;
*/

int cleanUpDeadSmileys(int* smileyPos, XTime* timeOfDeath) {
    int i = 0;
	int x = 0;
    int y = 0;
	int numberCleared = 0;
	XTime MaxTimeDeadOnScreen = 90000000;
	
	
	for(i = 0; i < 9; i++) {
	    if(smileyPos[i] == -1) {
		    //calculate how long the dead smiley has been displayed
		    XTime current = 0;
			XTime_GetTime(&current);
            if(current - timeOfDeath[i] > MaxTimeDeadOnScreen) {
			    //clear the box
                numberCleared++;
                smileyPos[i] = 0;
                timeOfDeath[i] = 0;
				
                //physically remove the Smiley from screen	
                x = i % 3;
                y = i/3;
                x = (215 * x) + 215/2; 
                y = (160 * y) + 160/2;	
                drawSmiley(x,y,75,0,1);				
		    }
		}
	}
	
	return numberCleared;
}


/* Remove smiley at the position indicated and
   replace it with a dead smiley. Update the timeOfDeath
   with the time the smiley died
 */
int killSmiley(int* smileyPos, XTime* timeOfDeath, int deadSmileyPos) {
	int x = 0;
    int y = 0;
	
	if(deadSmileyPos < 0 || deadSmileyPos > 8 ||
	   smileyPos[deadSmileyPos] != 1) {
	    return 0; 
	}//ignore function call if no live smiley is in the box
	
    XTime_GetTime(&timeOfDeath[deadSmileyPos]);
	smileyPos[deadSmileyPos] = -1; //Mark the Smiley as dead
	
	x = deadSmileyPos % 3;
    y = deadSmileyPos/3;
    x = (215 * x) + 215/2;
    y = (160 * y) + 160/2;
    
    //Remove the smiley
    drawSmiley(x,y,75,1,1);
    //Draw a dead one in place
    drawSmiley(x,y,75,0,0);
	
	return 1;
	
}


/* Remove Timed Out Smileys, Return the number removed
*/

int removeTimeoutSmiley(int* smileyPos, XTime* timeOfSpawn, int time_allowed) {
    int i = 0;
    int j = 0;
    int x = 0;
    int y = 0;
    int size = 0;
    int num_removed = 0;
    int availableBoxes[9];
	XTime current = 0;

    // Find all Onscreen Smileys, Count Them
    for(i = 0; i < 9; i++) {
         if(smileyPos[i] == 1) {
             availableBoxes[size] = i;
             size++;
         }
    }

    // If there arent any, return
    if(size == 0) {
        return 0; 
    }

    // Check all Smiley Time, Remove them if needed
    for (j = 0; j<size; j++)
    {
        XTime_GetTime(&current);
        
        // If its time to die
        if (current - timeOfSpawn[availableBoxes[j]] > time_allowed) {
            y = availableBoxes[j] / 3;
            x = availableBoxes[j] % 3;
            x = (215 * x) + 215/2; //approximate the center
            y = (160 * y) + 160/2;

            // Remove the smiley
            drawSmiley(x,y,75,1,1);
            // Mark box as empty
            smileyPos[availableBoxes[j]] = 0;
            // Remove Spawn time
            timeOfSpawn[availableBoxes[j]] = -1;
            
            num_removed++;
        }
    }
  
    return num_removed;

}

/* Check if a position x,y is within a smiley in a box.
   If it is not within a smiley, return -1;
*/

int checkSmiley(int x, int y)
{
	int r = 75;                      // Current Radius of Smileys
    int x1, y1, dist1, dist2, dist;  // Needed Variables
        
    int x0 = 0;                      // Iterators
    int y0 = 0;
    
    for (x0; x0 < 3; x0++) {
		for (y0 = 0; y0 < 3; y0++) {
			x1 = (215 * x0) + (215/2);
			y1 = (160 * y0) + (160/2);
			
			dist1 = (x-x1)*(x-x1);
			dist2 = (y-y1)*(y-y1);
			dist = dist1+dist2;
			
		if (dist <= (r*r))
			return x0+(y0*3);
		}
	}

	return -1;
}

/* Draws Random smileys in different grid positions that aren't
   currently occupied. smileyPos represents positions of Smileys.
   A value of 0 for a given index means that box is empty, a value
   of 1 for a given index means that there is a smiley in the box,
   and a value of -1 for a given index means that there is a dead
   smiley in that box.
*/

int drawRandomSmileys(int numSmileys, int *smileyPos, XTime* timeOfSpawn) {
    int i = 0;
    int j = 0;
    int availableBoxes[9];
    int size = 0;
    int x = 0;
    int y = 0;
    int num_drawn = 0;
	int randomIndex = 0;
	
  //  unsigned int seed = 0;  // = clock();
	//time_t seconds;
	//time(&seconds);
	//unsigned int seed = (unsigned int) seconds;
	
    
    for(j = 0; j < numSmileys; j++) {

        size = 0;
        for(i = 0; i < 9; i++) {
            if(!smileyPos[i]) { //empty Box
                availableBoxes[size] = i;
                size++;
            }
        }

        if(size == 0) {
            return 0; //We can't possibly draw anything because the grid is full
        }

        //Randomly pick an index from 0 to size - 1. Draw smiley there

        // generate a random number in range from 0 to size - 1 to pick a random smiley
		// from the array availableBoxes
        randomIndex = rand();
		randomIndex = randomIndex % size;
		randomIndex = availableBoxes[randomIndex];
		
        x = randomIndex % 3;
        y = randomIndex/3;
        x = (215 * x) + (215/2); //approximate the center
        y = (160 * y) + (160/2);

        //Draw the smiley
        drawSmiley(x,y,75,1,0);
        smileyPos[randomIndex] = 1; //We've got a new smiley!
		
        XTime_GetTime(&timeOfSpawn[randomIndex]); // Save the Spawn Time
		
        num_drawn++;
    } 
    return num_drawn;

}

/* Function Called to end the game */

void endGame(int score) {

	// Initialize Screen Variables
	unsigned int* pDisplay_data   = (unsigned int  *)0x07E00000;
    int x_current = 0;
    int y_current = 0;
	
	// Color Screen Black
	int horiz;         // Hardcoded for Scale 1, This is good
	int vert;          // Hardcoded for Scale 1, This is good
	for (horiz = 0; horiz < 640; horiz++) {
		for (vert = 0; vert < 480; vert++) {
			colorPixel(black, horiz, vert,&pDisplay_data, &x_current, &y_current);
		}
	}
	
	int gOver[12][85] = {      // Game Over Matrix
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},  
    {0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,0,0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,1,1,1,0,0,0,1,1,1,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0,0},  
    {0,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,0,0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0},  
    {1,1,1,1,0,0,1,1,1,0,0,1,1,1,1,1,1,1,0,0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,1,1,1,1,0,0,0,1,1,1,0,1,1,1,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0,1,1,1,0},  
    {1,1,1,0,0,0,0,0,0,0,0,1,1,1,0,1,1,1,0,0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,1,1,1,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0,1,1,1,0},  
    {1,1,1,0,0,1,1,1,1,0,0,1,1,1,0,1,1,1,0,0,0,1,1,0,1,0,0,1,0,1,1,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,1,1,1,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0},  
    {1,1,1,0,0,1,1,1,1,0,0,1,1,1,0,1,1,1,0,0,0,1,1,0,1,1,1,1,0,1,1,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,1,1,0,1,1,0,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,0,0,0},  
    {1,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,1,0,0,0,1,1,0,1,1,1,1,0,1,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,1,1,0,1,1,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,1,1,1,0,0},  
    {1,1,1,1,0,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,1,1,1,1,0,0,0,0,1,1,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0,1,1,1,0},  
    {0,1,1,1,1,1,1,1,1,0,1,1,1,0,0,0,1,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,1,1,1,1,1,1,1,0,0,1,1,1,0,0,1,1,1,0},  
    {0,0,1,1,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,1,1,1,1,1,1,0,0,1,1,1,0,0,0,1,1,1}
    };
	
	// Print "GAME OVER" 
	for (horiz = 0; horiz < 85; horiz++) {
		for (vert = 0; vert < 12; vert++) {
			if (gOver[vert][horiz] == 1)
				colorPixel(magenta, (362 - 85 + horiz), (vert + 212),&pDisplay_data, &x_current, &y_current);
		}
	}
	
	// Print Out Final Score
	drawScore(score, magenta, 316, 228, 2);
	
	// Infinite Loop
	while(1){
	}
}


/************************* END FUNCTIONS TO HANDLE GAMEPLAY DYNAMICS ********************************/







/********************************** BEGIN BASIC DRAWING FUNCTIONS ***********************************/

/* Draw a 3 x 3 grid onto the screen. The dimensions are as follows:
   Box(0,0) = 0,0 to 214,159
   Box(1,0) = 216,0 to 430,159
   Box(2,0) = 432,0 to 639,159
   Box(1,0) = 0,161 to 214,320
   Box(1,1) = 216,161 to 430,320
   Box(1,2) = 432,161 to 639,320
   Box(2,0) = 0,322 to 214,479
   Box(2,1) = 216,322 to 430,479
   Box(2,2) = 432,322 to  639,479 
*/

void drawGrid(COLOR c) {
    unsigned int* pDisplay_data   = (unsigned int  *)0x07E00000;
    int i = 0;
    int x_current = 0; 
    int y_current = 0;

    //Draw horizontal lines
    for(i = 0; i < 640; i++) {
        colorPixel(c, i , 160, &pDisplay_data, &x_current, &y_current);
    }
    //Draw horizontal lines
    for(i = 0; i < 640; i++) {
        colorPixel(c,i,321,&pDisplay_data, &x_current, &y_current);
    }

    //Draw vertical lines
    for(i = 0; i < 480; i++) {
        colorPixel(c,215,i,&pDisplay_data, &x_current, &y_current);
    }

    //Draw vertical lines
    for(i = 0; i < 480; i++) {
        colorPixel(c,431,i,&pDisplay_data, &x_current, &y_current);
    }
}


/* Draws "SCORE: %d", s and returns the number of digits from score which were written 
   This assumes that there are only 4 possible digits in a score. If not, it will only print
   the first 4 digits. Negative score appear as 0000 */

int drawScore ( int s, COLOR COL_USED, int SCORE_LOC, int SCORE_Y, int SCALE ) {
	
	// Variable Switchboard
	//int SCORE_LOC = 600;     // Define Starting Position for Score
	//int SCORE_Y = 10;        // How Far Down to Draw Score
	//int SCALE = 1;           // Define Digit Scale
	
	// Screen Variables
	unsigned int* pDisplay_data   = (unsigned int  *)0x07E00000;
    int x_current = 0;
    int y_current = 0;
	
	int each_digit = 0;        // Grab Each Digit
	int score[12][57] = {      // Score Matrix
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
	{0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0}, 
	{1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0}, 
	{1,1,1,0,0,1,1,0,0,1,1,1,0,0,1,1,1,0,0,1,1,1,1,0,1,1,1,1,0,0,1,1,1,0,0,1,1,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
	{1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,1,1,1,0,0,1,1,1,0,0,1,1,1,0,0,0,0,0,1,1,1,0,0,0,0,0,0}, 
	{0,1,1,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0,1,1,1,0,0,0,0,0,0}, 
	{0,0,1,1,1,1,1,0,0,1,1,1,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,0,1,1,1,0,0,0,0,0,0}, 
	{0,0,0,0,1,1,1,0,0,1,1,1,0,0,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,1,1,1,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
	{1,1,0,0,1,1,1,0,0,1,1,1,0,0,1,1,1,0,0,1,1,1,1,0,1,1,1,1,0,0,1,1,1,0,0,1,1,1,0,0,1,1,1,0,0,0,0,0,1,1,1,0,0,0,0,0,0}, 
	{1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,1,0,0,1,1,1,0,0,1,1,1,1,1,1,1,0,1,1,1,0,0,0,0,0,0}, 
	{0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,1,1,1,0,0,0,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,0,0,0,0,0,0}
	};

	// Print "SCORE: " 
	int horiz;         // Hardcoded for Scale 1, This is good
	int vert;          // Hardcoded for Scale 1, This is good
	for (horiz = 0; horiz < 57; horiz++) {
		for (vert = 0; vert < 12; vert++) {
			if (score[vert][horiz] == 1)
				colorPixel(COL_USED, (SCORE_LOC - 57 + horiz), (vert + SCORE_Y),&pDisplay_data, &x_current, &y_current);
		}
	}
	
	
	// Print 4 Digits at Requested Scale
	if (s > 0) {
	
		// Digit 1
		if ( (each_digit = (s % 10)) < s) {
			drawDigit(SCORE_LOC + SCALE*24, SCORE_Y, each_digit+48, SCALE, COL_USED);       // Draw First Digit
			s = (s - each_digit)/10;                                                     // Remove Digit
		}
		else {
			drawDigit(SCORE_LOC, SCORE_Y, '0', SCALE, COL_USED);
			drawDigit(SCORE_LOC + SCALE*8, SCORE_Y, '0', SCALE, COL_USED);
			drawDigit(SCORE_LOC + SCALE*16, SCORE_Y, '0', SCALE, COL_USED);
			drawDigit(SCORE_LOC + SCALE*24, SCORE_Y, s+48, SCALE, COL_USED);                // Draw Only Digit
			return 1;
		}

		// Digit 2
		if ( (each_digit = (s % 10)) < s) {
			drawDigit(SCORE_LOC + SCALE*16, SCORE_Y, each_digit+48, SCALE, COL_USED);       // Draw First Digit
			s = (s - each_digit)/10;                                                     // Remove Digit
		}
		else {
			drawDigit(SCORE_LOC, SCORE_Y, '0', SCALE, COL_USED);
			drawDigit(SCORE_LOC + SCALE*8, SCORE_Y, '0', SCALE, COL_USED);
			drawDigit(SCORE_LOC + SCALE*16, SCORE_Y, s+48, SCALE, COL_USED);               // Draw Last Digit
			return 2;
		}
			
		// Digit 3
		if ( (each_digit = (s % 10)) < s) {
			drawDigit(SCORE_LOC + SCALE*8, SCORE_Y, each_digit+48, SCALE, COL_USED);       // Draw First Digit
			s = (s - each_digit)/10;                                                    // Remove Digit
		}
		else {
			drawDigit(SCORE_LOC, SCORE_Y, '0', SCALE, COL_USED);
			drawDigit(SCORE_LOC + SCALE*8, SCORE_Y, s+48, SCALE, COL_USED);                // Draw Last Digit
			return 3;
		}
		
		drawDigit(SCORE_LOC, SCORE_Y, s+48, SCALE, COL_USED);                              // Draw Last Digit
		return 4;

	}
	// Draw 0 Score
	else {
		drawDigit(SCORE_LOC, SCORE_Y, '0', SCALE, COL_USED);
		drawDigit(SCORE_LOC + SCALE*8, SCORE_Y, '0', SCALE, COL_USED);
		drawDigit(SCORE_LOC + SCALE*16, SCORE_Y, '0', SCALE, COL_USED);
		drawDigit(SCORE_LOC + SCALE*24, SCORE_Y, '0', SCALE, COL_USED);
	}

}

/* Draws "MISSES: %d", s and returns the number of digits from score which were written 
   This assumes that there are only 2 possible digits in number of misses. If not, it 
   will only print the first 2 digits. Negative score appear as 00 
*/

int drawMisses ( int s, COLOR COL_USED) {
    
    // Variable Switchboard
    int SCORE_LOC = 70;     // Define Starting Position for Score
    int SCORE_Y = 10;       // How Far Down to Draw Score
    int SCALE = 1;          // Define Digit Scale
    
    // Screen Variables
    unsigned int* pDisplay_data   = (unsigned int  *)0x07E00000;
    int x_current = 0;
    int y_current = 0;
    
    int each_digit = 0;        // Grab Each Digit
    int misses[12][61] = {      // Miss Matrix
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},  
    {1,1,1,0,0,0,0,1,1,1,0,0,1,1,1,0,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0},  
    {1,1,1,0,0,0,0,1,1,1,0,0,1,1,1,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0},  
    {1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,0,0,1,1,1,0,0,1,1,0,0,1,1,1,0,0,1,1,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0},  
    {1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,1,1,1,0,0,0,0,0,0},  
    {1,1,0,1,0,0,1,0,1,1,0,0,1,1,1,0,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,0,0,1,1,1,0,0,0,0,0,0},  
    {1,1,0,1,1,1,1,0,1,1,0,0,1,1,1,0,0,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,0,1,1,1,0,0,0,0,0,0},  
    {1,1,0,1,1,1,1,0,1,1,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0},  
    {1,1,0,0,1,1,0,0,1,1,0,0,1,1,1,0,0,1,1,0,0,1,1,1,0,0,1,1,0,0,1,1,1,0,0,1,1,1,0,0,0,0,0,0,1,1,0,0,1,1,1,0,1,1,1,0,0,0,0,0,0},  
    {1,1,0,0,1,1,0,0,1,1,0,0,1,1,1,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0,1,1,1,0,0,0,0,0,0},  
    {1,1,0,0,1,1,0,0,1,1,0,0,1,1,1,0,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,0,0,1,1,1,0,0,0,0,0,0} 
    };

    // Print "MISSES: " 
    int horiz;         // Hardcoded for Scale 1, This is good
    int vert;          // Hardcoded for Scale 1, This is good
    for (horiz = 0; horiz < 61; horiz++) {
        for (vert = 0; vert < 12; vert++) {
            if (misses[vert][horiz] == 1)
                colorPixel(COL_USED, (SCORE_LOC - 61 + horiz), (vert + SCORE_Y),&pDisplay_data, &x_current, &y_current);
        }
    }
    
    
    // Print 2 Digits at Requested Scale
    if (s > 0) {
    
        // Digit 1 and 2
        if ( (each_digit = (s % 10)) < s) {
            drawDigit(SCORE_LOC + SCALE*8, SCORE_Y, each_digit+48, SCALE, COL_USED);     // Draw First Digit
            s = (s - each_digit)/10;                                                     // Remove Digit
            
            
            drawDigit(SCORE_LOC, SCORE_Y, s+48, SCALE, COL_USED);                        // Draw Last Digit
            return 2;
        }
        else {
            drawDigit(SCORE_LOC, SCORE_Y, '0', SCALE, COL_USED);
            drawDigit(SCORE_LOC + SCALE*8, SCORE_Y, s+48, SCALE, COL_USED);
            return 1;
        }

    }
    // Draw 0 Score
    else {
        drawDigit(SCORE_LOC, SCORE_Y, '0', SCALE, COL_USED);
        drawDigit(SCORE_LOC + SCALE*8, SCORE_Y, '0', SCALE, COL_USED);
    }

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
	  
    } 
}


/* Call this function to draw a face centered at
   x0,y0 with the specified radius. is_alive specifies
   whether the face is dead or alive (0 = dead, 1 = alive).
   If the face is dead, draw a frowning face with X'ed out
   eyes. If the face has not been hit, draw a smiling
   face with circles for eyes. Use is_removing_a_smiley to
   specify whether you intend to remove a smiley.
*/


void drawSmiley(int x, int y, int radius, int is_alive, int is_removing_a_smiley) {
    int dist_mouth_from_center = radius*.6;
	int dist_eyes_from_center = radius/2;
	int pixels_in_branch_of_x = 8; //How many pixels in each of the 4 branches that compose the letter "X"
	COLOR border_color = yellow;
    COLOR mouth_color  = magenta;
	COLOR eye_color = cyan;

        if(is_removing_a_smiley) {
            border_color = black;
            mouth_color = black;
            eye_color = black;
        }

    //Draw the smiley's border
    drawCircle(x,y,radius,border_color);
	
	//Draw the smiley's eyes and mouth
	if(is_alive) {
	    //Draw the right eye first. .707 is approximately cos(45)= sin(45)
	    drawCircle(x + (int)(dist_eyes_from_center * .707) , 
		      y - (int)(dist_eyes_from_center * .707), 10, eye_color);
	    //Draw the left eye next. 
		drawCircle(x - (int)(dist_eyes_from_center * .707) , 
		      y - (int)(dist_eyes_from_center * .707), 10, eye_color);	
			  
	    //Now draw an arc to represent a smiley face
		drawArc(x, y + dist_mouth_from_center, 20, mouth_color, 0);	
	}
	else {
	    //Draw an X in place of each eye
		
	    //Draw the right eye first. .707 is approximately cos(45)= sin(45)
		drawX(x + (int)(dist_eyes_from_center * .707) , 
		      y - (int)(dist_eyes_from_center * .707), 10, eye_color);
	    //Draw the left eye next.
		drawX(x - (int)(dist_eyes_from_center * .707) , 
		      y - (int)(dist_eyes_from_center * .707), 10, eye_color);	
			  
	    //Now draw an upside down arc to represent a frowning face
	    drawArc(x, y + dist_mouth_from_center, 20, mouth_color, 1);	
	}
	
}



/* drawCircle will draw a circle of color border centered at x0,y0 
   with the specified radius. 
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



/* Color the screen with color c. A lot of this code is taken from the
old subtitle project. I have no idea what word_count is so I just left
it in there. */

int colorScreen(int  word_count, COLOR c) {
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

  	print("...\t");

	// write pattern to DDR
	while (wordnum < word_count)
		{
		if (i < 480)
	        	rgb_value = rgb_value_array[c]; 

		*(pDisplay_data) = rgb_value;
		pDisplay_data++;
		wordnum++;
		x++;

		if (x >= HORIZONTAL_PIXELS){
			x = 0;
		 	pDisplay_data += (1024 - HORIZONTAL_PIXELS);
			i++;
		}
	}
  	return ;
} 



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
            //Draw a '0'
           //column 1
           
           //Multiply by scale_factor to get updated y offset

           for(j = 0; j < scale_factor; j++) {
               for(i = 0; i < scale_factor*8; i++) {
                   colorPixel(c, x, y + 2*scale_factor + i,&pDisplay_data, &x_current, &y_current);
               }
               x++;
           }

           //column 2
           for(j = 0; j < scale_factor; j++) {
               for(i = 0; i < scale_factor*10; i++) {
                   colorPixel(c, x, y + 1*scale_factor + i,&pDisplay_data, &x_current, &y_current);
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
               for(i = 0; i < scale_factor*3; i++) {
                   colorPixel(c, x, y + 9*scale_factor + i,&pDisplay_data, &x_current, &y_current);
               }
               x++;
           }


           //column 7
           for(j = 0; j < scale_factor; j++) {
               for(i = 0; i < scale_factor*10; i++) {
                   colorPixel(c, x, y + 1*scale_factor + i,&pDisplay_data, &x_current, &y_current);
               }
               x++;
           }


           //column 8
           for(j = 0; j < scale_factor; j++) {
               for(i = 0; i < scale_factor*8; i++) {
                   colorPixel(c, x, y + 2*scale_factor + i,&pDisplay_data, &x_current, &y_current);
               }
               x++;
           }
           break;

        case '1':
            //Draw a '1'. Go column by column

           //column 1
           
           //Multiply by scale_factor to get updated y offset

           for(j = 0; j < scale_factor; j++) {
               x++;
           }

           //column 2
           for(j = 0; j < scale_factor; j++) {
               for(i = 0; i < scale_factor*2; i++) {
                   colorPixel(c, x, y + 1*scale_factor + i,&pDisplay_data, &x_current, &y_current);
               }
               x++;
           }

           //column 3
           for(j = 0; j < scale_factor; j++) {
               for(i = 0; i < scale_factor*2; i++) {
                   colorPixel(c, x, y + 1*scale_factor + i,&pDisplay_data, &x_current, &y_current);
               }
               x++;
           }
           

           //column 4
           for(j = 0; j < scale_factor; j++) {
               for(i = 0; i < scale_factor*12; i++) {
                   colorPixel(c, x, y + i,&pDisplay_data, &x_current, &y_current);
               }
               x++;
           }

           //column 5
           for(j = 0; j < scale_factor; j++) {
               for(i = 0; i < scale_factor*12; i++) {
                   colorPixel(c, x, y + i,&pDisplay_data, &x_current, &y_current);
               }
               x++;
           }
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
          
           //column 8
	   for(j = 0; j < scale_factor; j++) {
 	       for(i = 0; i < scale_factor*2; i++) {
		   colorPixel(c, x, y + 6*scale_factor+i,&pDisplay_data, &x_current, &y_current);
	       }              
	       x++;
	   }
	   break;

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
       break;
	   
       case '6':
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
	   
	   break;

    }

    return 1; 
}




/********************************** END BASIC DRAWING FUNCTIONS ***********************************/



/******************************** BEGIN PIXEL MANIPULATION FUNCTIONS ******************************/

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
	    return ; //don't do anything
    }
		
	//Advance display pointer to correct position.
	pixels_away = (y - *y0)*(1024) + (x-*x0);
	*pDisplay_data += pixels_away;
    **pDisplay_data = rgb_value_array[c];

        //update current position
        *y0 = y;
        *x0 = x;
}			


/* return the numeric value of the pixel located at coordinates x,y */

unsigned int returnPixelValue(int x, int y, unsigned int **pDisplay_data, int *x0, int *y0){
    int pixels_away = 0; //How many pixels away is the destination pixel, counting from left to right
											
	//Make sure x,y are in bounds	
	if(x < 0 || x >= HORIZONTAL_PIXELS || y < 0 || y >= VERTICAL_PIXELS) {
	    return -1; 
    }
		
	//Advance display pointer to correct position.
	pixels_away = (y - *y0)*(1024) + (x-*x0);
	*pDisplay_data += pixels_away;

        //update current position
        *y0 = y;
        *x0 = x;
		
	return **pDisplay_data;
}			


/* More general version of setPixel that lets you set a pixel to any possible value */

void setPixel(unsigned int value, int x, int y, unsigned int **pDisplay_data, int *x0, int *y0){
    volatile unsigned int rgb_value_array[16] = {0x00FFFFFF, 0x00000000, 0x00FFFF00, 0x0000FFFF, 0x0000FF00
	  											  ,0x00FF00FF, 0x00FF0000, 0x000000FF};
    int pixels_away = 0; //How many pixels away is the destination pixel, counting from left to right
											
	//Make sure x,y are in bounds	
	if(x < 0 || x >= HORIZONTAL_PIXELS || y < 0 || y >= VERTICAL_PIXELS) {
	    return ; //don't do anything
    }
		
	//Advance display pointer to correct position.
	pixels_away = (y - *y0)*(1024) + (x-*x0);
	*pDisplay_data += pixels_away;
        **pDisplay_data = value;

        //update current position
        *y0 = y;
        *x0 = x;
}


/******************************** END PIXEL MANIPULATION FUNCTIONS ******************************/




/******************************** BEGIN MISC HELPER FUNCTIONS **********************************/



////////////////////////////////////////////////////////////////////////////////////////
void main_memu(void)
{
  unsigned char x, s;
   s = 0;


	Reset_xup_decoder();
	configDecoder(decoder_svid, DECODER_SVID_CONFIG_CNT);
  return;
} // end main_menu


void configDecoder(struct VideoModule *decoder, int config_cnt ) 
{
  Xuint16 send_cnt, i;
  Xuint8 send_data[2] = {0};
  Xuint8 success = 1;
  send_cnt = 2;
  print("  Configuring Decoder...\t");
  for( i = 0; i < config_cnt; i++ )
   {

    XI2c_mWriteReg(XPAR_I2C_BASEADDR, GPO_REG_OFFSET, GPO_RESET_IIC);
    XI2c_mWriteReg(XPAR_I2C_BASEADDR, GPO_REG_OFFSET, GPO_RESETS_OFF);
    send_data[0] = decoder[i].addr;

    send_data[1] = decoder[i].config_val;

    send_cnt = XI2c_Send(XPAR_I2C_BASEADDR, DECODER_ADDR, send_data, 2);

    if( send_cnt != 2 ) 
	 {
      xil_printf("Error writing to address %02x\r\n", decoder[i].addr);
      success = 0;
      break;
     }
   }

  if( success )
    print("SUCCESS!\r\n");

} // end configDecoder()

void Reset_xup_decoder(void)
{
int send_cnt = 0;
int wait_delay = 500000;
    XI2c_mWriteReg(XPAR_I2C_BASEADDR, GPO_REG_OFFSET, GPO_RESET_DECODER);  // reset  to vid decoder
	while(wait_delay)
   {
    wait_delay = wait_delay -1;
	}
    XI2c_mWriteReg(XPAR_I2C_BASEADDR, GPO_REG_OFFSET, GPO_RESETS_OFF);  // set resets de - asserted
	wait_delay = 5000000;
   while(wait_delay)
   {
    wait_delay = wait_delay -1;
	}
   return;
}  // end Reset_xup_decoder

unsigned char get_hex_byte()
{
  unsigned char s ,x, pass;
  pass = 0;
  s = 0;
  x = 0;
    while( s != 13)
    {
    s = uartRead();
	xil_printf("%c",s); //echo back 
	if((s < '0' ||  s >'f')  && s != 13){
	  print(" invalid entry start over \n\r");
	  x = 0;
	  }
	if( s == 13){
	  break;
	  }
	 else{
	  if(pass != 0){
	   x= x*16;
	   }
	  if(s > 47 && s < 58){
	  s = ( s - '0');
	  x = x + s;
	  }
	  else if( s == 'a'){
	   x = x + 10;
	   }
	   else if( s == 'b'){
	   x = x + 11;
	   }
	   else if( s == 'c'){
	   x = x + 12;
	   }
	   else if( s == 'd'){
	   x = x + 13;
	   }
	   else if( s == 'e'){
	   x = x + 14;
	   }
	   else if( s == 'f'){
	   x = x + 15;
	   }
      pass++;
	  }
	}

	return x;
}
void edit_i2c_reg(void)
{
  Xuint8 send_data[2] = {0};
  int send_cnt = 0;
  char s = 0;
  
    XI2c_mWriteReg(XPAR_I2C_BASEADDR, GPO_REG_OFFSET, GPO_RESET_IIC);
    XI2c_mWriteReg(XPAR_I2C_BASEADDR, GPO_REG_OFFSET, GPO_RESETS_OFF);
	print("\r\nEdit I2C Register Settings Mode.\r\n");
   do {
		print("\r\nEnter I2C Hex Register Address  ");
		send_data[0] = get_hex_byte();
		print("\r\nEnter I2C Hex Register Data     ");
		send_data[1] = get_hex_byte();
		send_cnt = XI2c_Send(XPAR_I2C_BASEADDR, DECODER_ADDR, send_data, 2);
	    if( send_cnt != 2 ) 
		  {
	      xil_printf("Error writing to address %02x\r\n", send_data[0]);
	      }
	    print("\r\nWriting New Reg Value.\r\n\r\nType c to continue edits, Or  q to quit. ");
		s =uartRead();
	} while( s != 'q' );
  print("\r\n\r\n");
  return;
}

/******************************** END MISC HELPER FUNCTIONS **********************************/

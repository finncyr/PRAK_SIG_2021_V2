#include <math.h>
#include "dsk6713.h"
#include "dsk6713_aic23.h"
#define pi 3.141592653589
#define NUM_TRACKS 4
#define MAX_TONES_PER_TRACK 40

#define LEN(arr) ((int) (sizeof (arr) / sizeof (arr)[0]))


int slotLen=9000; //num of samples of shortest tone/pause;
                  //preset to 1/8 note per slot @ 200 BPM of 1/4 note -> now sixteenth @ 80 BPM
struct tone{
  int bSlot;
  int eSlot;
  double freq;
  double amp;
};
struct tone to[NUM_TRACKS][MAX_TONES_PER_TRACK];

/* Codec configuration settings */
/* C A U T I O N:
 * 'config' must not be the first variable, otherwise it will be placed at address
 * 0x0000, and this causes a compare config==NULL somewhere down in the BSL to
 * become true and then load default settings !!!!
 */
volatile int dummy[10];
static DSK6713_AIC23_Config config = { \
  0x0017,  /* 0 DSK6713_AIC23_LEFTINVOL  Left line input channel volume 0dB*/ \
  0x0017,  /* 1 DSK6713_AIC23_RIGHTINVOL Right line input channel volume 0dB*/\
  0x00f9,  /* 2 DSK6713_AIC23_LEFTHPVOL  Left channel headphone volume 0dB*/  \
  0x00f9,  /* 3 DSK6713_AIC23_RIGHTHPVOL Right channel headphone volume 0dB*/ \
  0x0011,  /* 4 DSK6713_AIC23_ANAPATH    Analog audio path control    */      \
  0x0000,  /* 5 DSK6713_AIC23_DIGPATH    Digital audio path control    */     \
  0x0000,  /* 6 DSK6713_AIC23_POWERDOWN  Power down control    */             \
  0x0043,  /* 7 DSK6713_AIC23_DIGIF      Digital audio interface format    */ \
  0x0081,  /* 8 DSK6713_AIC23_SAMPLERATE Sample rate control    */            \
  0x0001   /* 9 DSK6713_AIC23_DIGACT     Digital interface activation    */   \
};

/*
 * main.c
 */
int main(void) {
  int i,n,actSlot;
  double frequenzen[NUM_TRACKS];
  double amplituden[NUM_TRACKS];
  int numToneSlots=128;
  short out=0;

	double fr = 48000;
	double T = 1/fr;
	double sinarg = 2*pi*T;
	double arg[NUM_TRACKS];
	double y[NUM_TRACKS];
	double tempout = 0;

	double A1 = 2000;
	double A2 = 6000;

	double tA[NUM_TRACKS]={4000,3000,5000,7000};

  DSK6713_AIC23_CodecHandle hCodec;
  // Initialize the board support library, must be called first
  DSK6713_init();
  // Start the codec
  hCodec = DSK6713_AIC23_openCodec(0, &config);

  // initialize array of structures

  //Track 0

    to[0][0].bSlot=   0, to[0][0].eSlot=  17, to[0][0].freq= 329.627557, to[0][0].amp= tA[0]; // e'
    to[0][1].bSlot=  18, to[0][1].eSlot=  19, to[0][1].freq= 293.664768, to[0][1].amp= tA[0]; // d'
    to[0][2].bSlot=  20, to[0][2].eSlot=  21, to[0][2].freq= 261.625565, to[0][2].amp= tA[0]; // c'
    to[0][3].bSlot=  22, to[0][3].eSlot=  23, to[0][3].freq= 246.941651, to[0][3].amp= tA[0]; // b
    to[0][4].bSlot=  24, to[0][4].eSlot=  25, to[0][4].freq= 261.625565, to[0][4].amp= tA[0]; // c'
    to[0][5].bSlot=  26, to[0][5].eSlot=  27, to[0][5].freq= 293.664768, to[0][5].amp= tA[0]; // d'
    to[0][6].bSlot=  28, to[0][6].eSlot=  29, to[0][6].freq= 329.627557, to[0][6].amp= tA[0]; // e'
    to[0][7].bSlot=  30, to[0][7].eSlot=  31, to[0][7].freq= 261.625565, to[0][7].amp= tA[0]; // c'
    to[0][8].bSlot=  32, to[0][8].eSlot=  43, to[0][8].freq= 440.000000, to[0][8].amp= tA[0]; // a'
    to[0][9].bSlot=  44, to[0][9].eSlot=  45, to[0][9].freq= 493.883301, to[0][9].amp= tA[0]; // b'
    to[0][10].bSlot= 46, to[0][10].eSlot= 47, to[0][10].freq=523.251131, to[0][10].amp=tA[0]; // c''
    to[0][11].bSlot= 48, to[0][11].eSlot= 48, to[0][11].freq=349.228231, to[0][11].amp=tA[0]; // f'
    to[0][12].bSlot= 49, to[0][12].eSlot= 49, to[0][12].freq=329.627557, to[0][12].amp=tA[0]; // e'
    to[0][13].bSlot= 50, to[0][13].eSlot= 59, to[0][13].freq=349.228231, to[0][13].amp=tA[0]; // f'
    to[0][14].bSlot= 60, to[0][14].eSlot= 63, to[0][14].freq=293.664768, to[0][14].amp=tA[0]; // d'

    to[0][15].bSlot= 64, to[0][15].eSlot= 75, to[0][15].freq=493.883301, to[0][15].amp=tA[0]; // b'
    to[0][16].bSlot= 76, to[0][16].eSlot= 77, to[0][16].freq=523.251131, to[0][16].amp=tA[0]; // c''
    to[0][17].bSlot= 78, to[0][17].eSlot= 79, to[0][17].freq=587.329536, to[0][17].amp=tA[0]; // d''

    to[0][18].bSlot= 80, to[0][18].eSlot= 80, to[0][18].freq=391.995436, to[0][18].amp=tA[0]; // g'
    to[0][19].bSlot= 81, to[0][19].eSlot= 81, to[0][19].freq=349.228231, to[0][19].amp=tA[0]; // f'
    to[0][20].bSlot= 82, to[0][20].eSlot= 89, to[0][20].freq=391.995436, to[0][20].amp=tA[0]; // g'
    to[0][21].bSlot= 90, to[0][21].eSlot= 91, to[0][21].freq=349.228231, to[0][21].amp=tA[0]; // f'
    to[0][22].bSlot= 92, to[0][22].eSlot= 93, to[0][22].freq=329.627557, to[0][22].amp=tA[0]; // e'
    to[0][23].bSlot= 94, to[0][23].eSlot= 95, to[0][23].freq=349.228231, to[0][23].amp=tA[0]; // f'

    to[0][24].bSlot= 96, to[0][24].eSlot= 97, to[0][24].freq=349.228231, to[0][24].amp=tA[0]; // f'
    to[0][25].bSlot= 98, to[0][25].eSlot= 99, to[0][25].freq=329.627557, to[0][25].amp=tA[0]; // e'
    to[0][26].bSlot=100, to[0][26].eSlot=101, to[0][26].freq=293.664768, to[0][26].amp=tA[0]; // d'
    to[0][27].bSlot=102, to[0][27].eSlot=103, to[0][27].freq=261.625565, to[0][27].amp=tA[0]; // c'
    to[0][28].bSlot=104, to[0][28].eSlot=105, to[0][28].freq=261.625565, to[0][28].amp=tA[0]; // c'
    to[0][29].bSlot=106, to[0][29].eSlot=107, to[0][29].freq=293.664768, to[0][29].amp=tA[0]; // d'
    to[0][30].bSlot=108, to[0][30].eSlot=109, to[0][30].freq=329.627557, to[0][30].amp=tA[0]; // e'
    to[0][31].bSlot=110, to[0][31].eSlot=111, to[0][31].freq=349.228231, to[0][31].amp=tA[0]; // f'

    to[0][32].bSlot=112, to[0][32].eSlot=113, to[0][32].freq=246.941651, to[0][32].amp=tA[0]; // b
    to[0][33].bSlot=114, to[0][33].eSlot=115, to[0][33].freq=261.625565, to[0][33].amp=tA[0]; // c'
    to[0][34].bSlot=116, to[0][34].eSlot=117, to[0][34].freq=293.664768, to[0][34].amp=tA[0]; // d'
    to[0][35].bSlot=118, to[0][35].eSlot=119, to[0][35].freq=329.627557, to[0][35].amp=tA[0]; // e'
    to[0][36].bSlot=120, to[0][36].eSlot=121, to[0][36].freq=349.228231, to[0][36].amp=tA[0]; // f'
    to[0][37].bSlot=122, to[0][37].eSlot=123, to[0][37].freq=329.627557, to[0][37].amp=tA[0]; // e'
    to[0][38].bSlot=124, to[0][38].eSlot=125, to[0][38].freq=349.228231, to[0][38].amp=tA[0]; // f'
    to[0][39].bSlot=126, to[0][39].eSlot=127, to[0][39].freq=293.664768, to[0][39].amp=tA[0]; // d'

  //Track 1
    to[1][0].bSlot=   0, to[1][0].eSlot=  19, to[1][0].freq= 261.625565, to[1][0].amp= tA[1]; // c'
    to[1][1].bSlot=  32, to[1][1].eSlot=  47, to[1][1].freq= 349.228231, to[1][1].amp= tA[1]; // f'
    to[1][2].bSlot=  48, to[1][2].eSlot=  63, to[1][2].freq= 293.664768, to[1][2].amp= tA[1]; // d'

    to[1][3].bSlot=  64, to[1][3].eSlot=  79, to[1][3].freq= 391.995436, to[1][3].amp= tA[1]; // g'

    to[1][4].bSlot=  80, to[1][4].eSlot=  87, to[1][4].freq= 329.627557, to[1][4].amp= tA[1]; // e'

    to[1][5].bSlot=  96, to[1][5].eSlot= 103, to[1][5].freq= 261.625565, to[1][5].amp= tA[1]; // c'
    to[1][6].bSlot=  104, to[1][6].eSlot=111, to[1][6].freq= 349.228231, to[1][6].amp= tA[1]; // f'

    to[1][7].bSlot=  112, to[1][7].eSlot=119, to[1][7].freq= 391.995436, to[1][7].amp= tA[1]; // g'
    to[1][8].bSlot=  120, to[1][8].eSlot=123, to[1][8].freq= 261.625565, to[1][8].amp= tA[1]; // c'
    to[1][9].bSlot=  124, to[1][9].eSlot=127, to[1][9].freq= 195.997718, to[1][9].amp= tA[1]; // g

  //Track 2
    to[2][0].bSlot=   0, to[2][0].eSlot=  31, to[2][0].freq= 195.997718, to[2][0].amp= tA[2]; // g
    to[2][1].bSlot=  32, to[2][1].eSlot=  47, to[2][1].freq= 261.625565, to[2][1].amp= tA[2]; // c'
    to[2][2].bSlot=  48, to[2][2].eSlot=  63, to[2][2].freq= 220.000000, to[2][2].amp= tA[2]; // a

    to[2][3].bSlot=  64, to[2][3].eSlot=  79, to[2][3].freq= 293.664768, to[2][3].amp= tA[2]; // d'

    to[2][4].bSlot=  80, to[2][4].eSlot=  95, to[2][4].freq= 246.941651, to[2][4].amp= tA[2]; // b

    to[2][5].bSlot=  96, to[2][5].eSlot= 103, to[2][5].freq= 195.997718, to[2][5].amp= tA[2]; // g
    to[2][6].bSlot= 104, to[2][6].eSlot= 111, to[2][6].freq= 440.000000, to[2][6].amp= tA[2]; // a'

  //Track 3
    to[3][0].bSlot=   0, to[3][0].eSlot=   3, to[3][0].freq= 130.812783, to[3][0].amp= tA[3]; // c
    to[3][1].bSlot=   6, to[3][1].eSlot=   7, to[3][1].freq= 130.812783, to[3][1].amp= tA[3]; // c
    to[3][2].bSlot=   8, to[3][2].eSlot=  11, to[3][2].freq= 123.470825, to[3][2].amp= tA[3]; // B
    to[3][3].bSlot=  14, to[3][3].eSlot=  15, to[3][3].freq= 123.470825, to[3][3].amp= tA[3]; // B
    to[3][4].bSlot=  16, to[3][4].eSlot=  19, to[3][4].freq= 110.000000, to[3][4].amp= tA[3]; // A
    to[3][5].bSlot=  22, to[3][5].eSlot=  23, to[3][5].freq= 110.000000, to[3][5].amp= tA[3]; // A
    to[3][6].bSlot=  24, to[3][6].eSlot=  27, to[3][6].freq=  97.998859, to[3][6].amp= tA[3]; // G
    to[3][7].bSlot=  30, to[3][7].eSlot=  31, to[3][7].freq=  97.998859, to[3][7].amp= tA[3]; // G
    to[3][8].bSlot=  32, to[3][8].eSlot=  35, to[3][8].freq=  87.307058, to[3][8].amp= tA[3]; // F
    to[3][9].bSlot=  38, to[3][9].eSlot=  39, to[3][9].freq=  87.307058, to[3][9].amp= tA[3]; // F
    to[3][10].bSlot= 40, to[3][10].eSlot= 43, to[3][10].freq= 82.406889, to[3][10].amp=tA[3]; // E
    to[3][11].bSlot= 46, to[3][11].eSlot= 47, to[3][11].freq= 82.406889, to[3][11].amp=tA[3]; // E
    to[3][12].bSlot= 48, to[3][12].eSlot= 51, to[3][12].freq= 73.416192, to[3][12].amp=tA[3]; // D
    to[3][13].bSlot= 54, to[3][13].eSlot= 55, to[3][13].freq= 73.416192, to[3][13].amp=tA[3]; // D
    to[3][14].bSlot= 56, to[3][14].eSlot= 59, to[3][14].freq= 65.406391, to[3][14].amp=tA[3]; // C
    to[3][15].bSlot= 62, to[3][15].eSlot= 63, to[3][15].freq= 65.406391, to[3][15].amp=tA[3]; // C

    to[3][16].bSlot= 64, to[3][16].eSlot= 67, to[3][16].freq= 97.998859, to[3][16].amp=tA[3]; // G
    to[3][17].bSlot= 70, to[3][17].eSlot= 71, to[3][17].freq= 97.998859, to[3][17].amp=tA[3]; // G
    to[3][18].bSlot= 72, to[3][18].eSlot= 75, to[3][18].freq= 87.307058, to[3][18].amp=tA[3]; // F
    to[3][19].bSlot= 78, to[3][19].eSlot= 79, to[3][19].freq= 87.307058, to[3][19].amp=tA[3]; // F

    to[3][20].bSlot= 80, to[3][20].eSlot= 83, to[3][20].freq= 82.406889, to[3][20].amp=tA[3]; // E
    to[3][21].bSlot= 86, to[3][21].eSlot= 87, to[3][21].freq= 82.406889, to[3][21].amp=tA[3]; // E
    to[3][22].bSlot= 88, to[3][22].eSlot= 91, to[3][22].freq= 73.416192, to[3][22].amp=tA[3]; // D
    to[3][23].bSlot= 94, to[3][23].eSlot= 95, to[3][23].freq= 73.416192, to[3][23].amp=tA[3]; // D

    to[3][24].bSlot= 96, to[3][24].eSlot= 99, to[3][24].freq= 65.406391, to[3][24].amp=tA[3]; // C
    to[3][25].bSlot= 100, to[3][25].eSlot= 103, to[3][25].freq= 82.406889, to[3][25].amp=tA[3]; // E
    to[3][26].bSlot= 104, to[3][26].eSlot= 107, to[3][26].freq= 87.307058, to[3][26].amp=tA[3]; // F
    to[3][27].bSlot= 110, to[3][27].eSlot= 111, to[3][27].freq= 87.307058, to[3][27].amp=tA[3]; // F

    to[3][28].bSlot= 112, to[3][28].eSlot= 119, to[3][28].freq= 97.998859, to[3][28].amp=tA[3]; // G
    to[3][29].bSlot= 120, to[3][29].eSlot= 123, to[3][29].freq=110.000000, to[3][29].amp=tA[3]; // A
    to[3][30].bSlot= 124, to[3][30].eSlot= 127, to[3][30].freq=123.470825, to[3][30].amp=tA[3]; // A



  // define the song




  // generate the song; at the end: repeat from start
  while (1){
    // initializations each time the song starts again (if any)

    // work through all tone slots of the song
    for (actSlot=0; actSlot<numToneSlots; actSlot++){
      // for each tone slot:
      // determine which tracks contribute a tone (frequency, amplitude)
      for (i=0; i<NUM_TRACKS; i++){
    	  int j = 0;
    	  for(j = 0; j < LEN(to[i]); j++){
    		  if(actSlot >= to[i][j].bSlot && actSlot <= to[i][j].eSlot){
    			  frequenzen[i] = to[i][j].freq;
    			  amplituden[i] = to[i][j].amp;
    			  break;
    		  }
    		  else {
    			  frequenzen[i] = 0;
    			  amplituden[i] = 0;
    		  }
    	  }
      }
      // work through all samples of the tone slot
      for (n=0; n<slotLen; n++){
    	  out = 0;
    	  tempout = 0;


        // for each sample: calculate sample value from all tracks
        for (i=0; i<NUM_TRACKS; i++){
        	arg[i] += sinarg * frequenzen[i];
        	if(arg[i] > 2*pi){
        		arg[i] -= 2*pi;
        	}

        	y[i] = amplituden[i]*sin(arg[i]);
        	tempout += y[i];
        }

        out = (short) tempout;

        // Send the sample to the left channel
        while (!DSK6713_AIC23_write(hCodec, out));
        // Send same sample to the right channel
        while (!DSK6713_AIC23_write(hCodec, out));
      }
    }
  }

  // Close the codec
  DSK6713_AIC23_closeCodec(hCodec);
  return 0;
}

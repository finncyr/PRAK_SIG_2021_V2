#include <math.h>
#include "dsk6713.h"
#include "dsk6713_aic23.h"
#define pi 3.141592653589
#define NUM_TRACKS 2
#define MAX_TONES_PER_TRACK 6

#define LEN(arr) ((int) (sizeof (arr) / sizeof (arr)[0]))


int slotLen=19200; //num of samples of shortest tone/pause;
                  //preset to 1/8 note per slot @ 200 BPM of 1/4 note -> now quarter at 150BPM
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
  int numToneSlots=9;
  short out=0;

	double fr = 48000;
	double T = 1/fr;
	double sinarg = 2*pi*T;
	double arg[NUM_TRACKS];
	double y[NUM_TRACKS];
	double tempout = 0;

	double A1 = 2000;
	double A2 = 6000;

  DSK6713_AIC23_CodecHandle hCodec;
  // Initialize the board support library, must be called first
  DSK6713_init();
  // Start the codec
  hCodec = DSK6713_AIC23_openCodec(0, &config);

  // initialize array of structures

  // Track 1
  to[0][0].bSlot=1, to[0][0].eSlot=1, to[0][0].freq=293.664768, to[0][0].amp=A1; // d'
  to[0][1].bSlot=3, to[0][1].eSlot=3, to[0][1].freq=329.627557, to[0][1].amp=A1; // e'
  to[0][2].bSlot=4, to[0][2].eSlot=4, to[0][2].freq=293.664768, to[0][2].amp=A1; // d'
  to[0][3].bSlot=5, to[0][3].eSlot=5, to[0][3].freq=329.627557, to[0][3].amp=A1; // e'
  to[0][4].bSlot=6, to[0][4].eSlot=7, to[0][4].freq=349.228231, to[0][4].amp=A1; // f'
  to[0][5].bSlot=8, to[0][5].eSlot=8, to[0][5].freq=391.995436, to[0][5].amp=A1; // g'

  // Track  2
  to[1][0].bSlot=0, to[1][0].eSlot=0, to[1][0].freq=146.832384, to[1][0].amp=A2; // d'
  to[1][1].bSlot=1, to[1][1].eSlot=1, to[1][1].freq=174.614116, to[1][1].amp=A2; // f
  to[1][2].bSlot=2, to[1][2].eSlot=2, to[1][2].freq=195.997718, to[1][2].amp=A2; // g
  to[1][3].bSlot=3, to[1][3].eSlot=5, to[1][3].freq=130.812783, to[1][3].amp=A2; // c
  to[1][4].bSlot=6, to[1][4].eSlot=6, to[1][4].freq=164.813778, to[1][4].amp=A2; // e
  to[1][5].bSlot=7, to[1][5].eSlot=7, to[1][5].freq=174.614116, to[1][5].amp=A2; // f


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

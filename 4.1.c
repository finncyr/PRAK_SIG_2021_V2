#include <math.h>
#include "dsk6713.h"
#include "dsk6713_aic23.h"
#define pi 3.141592653589

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
  int run=1;
  short out=0;
  DSK6713_AIC23_CodecHandle hCodec;
  // Initialize the board support library, must be called first
  DSK6713_init();
  // Start the codec
  hCodec = DSK6713_AIC23_openCodec(0, &config);
  // now generate the sound

  double A = 14000; //Ziel 0.6 V
  double f0 = 500;
  double fr = 48000;
  double T = 1/fr;
  double sinarg = 2*pi*f0*T;
  double fixedarg = 0;
  double y = 0;

  while(run==1){
    // generate next sample

	  fixedarg += sinarg;
	  if(fixedarg > 2*pi){
		  fixedarg -= 2*pi;
	  }
	  y = A*sin(fixedarg);

	  out = (short) y;


    // Send the sample to the left channel
    while (!DSK6713_AIC23_write(hCodec, out));
    // Send same sample to the right channel
    while (!DSK6713_AIC23_write(hCodec, out));
  }
  // Close the codec
  DSK6713_AIC23_closeCodec(hCodec);
  return 0;
}



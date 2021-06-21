	#include <math.h>
	#include "dsk6713.h"
	#include "dsk6713_aic23.h"

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
	  int i=0;
	  DSK6713_AIC23_CodecHandle hCodec;
	  // Initialize the board support library, must be called first
	  DSK6713_init();
	  // Start the codec
	  hCodec = DSK6713_AIC23_openCodec(0, &config);

	  // AUFGABE 3.1

	  double x = 0;
	  double y = 0;
	  double faktor = 1.2;

	  // define the filter

	  short sample = 0;
	  short out = 0;

	  Uint32 in = 0;


	  /* now process one sample after the other */
	  //forever
	  for (;;){
		// Get a sample from the left channel and do something
		  while(!DSK6713_AIC23_read(hCodec,&in));
		  sample=(short)in & 0x0000FFFF;

		  x = (double) sample;
		  y = x * faktor;

		  out = (short) y;
		  while(!DSK6713_AIC23_write(hCodec,out));

		  // Do right Channel magic
		  while(!DSK6713_AIC23_read(hCodec,&in));
		  while(!DSK6713_AIC23_write(hCodec,out));


	 }

	  // Close the codec
	  DSK6713_AIC23_closeCodec(hCodec);
	  return 0;
	}



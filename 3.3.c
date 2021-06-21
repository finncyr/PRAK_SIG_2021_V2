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

	  // AUFGABE 3.3

	  double faktor = 1.2;
	  double x[9] = {0};
	  double y[9] = {0};

	  double puffer = 0;
	  short hilfe = 0;
	  // define the filter

	  short sample = 0;
	  short out = 0;

	  Uint32 in = 0;

	  double a[9] = {0};
	  double b[9] = {0};


	  b[0] = 4.39093235787733e-007;
	  b[1] = 0;
	  b[2] = -1.75637294315093e-006;
	  b[3] = 6.35274710440725e-022;
	  b[4] = 2.6345594147264e-006;
	  b[5] = 6.35274710440725e-022;
	  b[6] = -1.75637294315093e-006;
	  b[8] = 4.39093235787733e-007;

	  a[0] = 0;
	  a[1] = -7.79858636871234;
	  a[2]= 26.6721407843522;
	  a[3] = -52.2520262694319;
	  a[4] = 64.1306815821225;
	  a[5] = -50.4945875361744;
	  a[6] = 24.9081591103847;
	  a[7] = -7.03788987957032;
	  a[8] = 0.872108645089876;


	  /* now process one sample after the other */
	  //forever
	  for (;;){
		// Get a sample from the left channel and do something
		  while(!DSK6713_AIC23_read(hCodec,&in));
		  sample=(short)(in & 0x0000FFFF);

		  x[0] = (double) sample;

		  // Filtergleichung
		  int i = 0;
		  y[0] = 0;
		  for(i = 0; i < 9; i++){
			  y[0] += x[i]*b[i];
			  if (i != 0){
				  y[0] -= y[i]*a[i];
			  }
		  }


		  // Array Shift
		  for(i = 8; i > 0; i--){
			  x[i] = x[i-1];
			  y[i] = y[i-1];
		  }


		  out = (short) (y[0] * faktor);

		  while(!DSK6713_AIC23_write(hCodec,out));

		  // Do right Channel magic
		  while(!DSK6713_AIC23_read(hCodec,&in));
		  while(!DSK6713_AIC23_write(hCodec,out));


	 }

	  // Close the codec
	  DSK6713_AIC23_closeCodec(hCodec);
	  return 0;
	}



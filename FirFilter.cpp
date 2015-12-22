/*
  FirFilter.cpp
  Finite Impulse Response Filter library for the Arduino 8-bit AVR processors
  Copyright (c) 2015 Phillip Schmidt.  All right reserved.

  All operations during filtering are done with integer math to increase speed.
  
  http://www.labbookpages.co.uk/audio/firWindowing.html
  
	!!! All input data must be type INT.  !!!
	
	Processing Time on an 16Mhz Arduino
	7 window  = 48us
	9 window  = 61us
	11 Window = 73us
	13 Window = 86us
	21 Window = 137us
 
*/

#include "FirFilter.h"

	

FirFilter::FirFilter(byte filterSize, int cutOffFreq, int sampleFreq) // initialize filter
{	

	if(!(B00000001 & filterSize)) filterSize++;	// window size must be odd
	filterWindowSize = filterSize;
	
	if(cutOffFreq > sampleFreq / 2 - 1) cutOffFreq = sampleFreq / 2 - 1;  // cut off must be less than half sampling freq
	
	DataList = (int*) calloc (filterSize, sizeof(int));		// create array for data
	CoefList = (int*) calloc (filterSize, sizeof(int));		// create array for coefficients

	float normalizedCutoff = float(cutOffFreq) / float(sampleFreq);
	
	int filterOrderHalf = (filterSize - 1) / 2;

	for(int n = 0; n < filterSize; n++)	// Generate weight values
	{
		float stepsFromCenter;
		float weight;
		
		stepsFromCenter = float(n - filterOrderHalf);
		
		if(n != filterOrderHalf)	// either side of center
		{	
			weight = sin(6.2832f * normalizedCutoff * stepsFromCenter) / (3.1416 * stepsFromCenter);
		}
		else						// center point
		{
			weight = 2.0f * normalizedCutoff;
		}
		
		weight *= 0.54f - 0.46f * cos(6.2832f * float(n) / float(filterSize - 1)); // apply Hamming window to reduce ringing 

		CoefList[n] = int(weight * 32767.0f);	// offset by 15bits, convert to INT
	}

}


int FirFilter::in(int value) // data input function
{  
	
	DataList[OldestDataPoint] = value; // replace oldest data in list
	
	// multiply sample by coefficient
	result.L = mul(DataList[OldestDataPoint], CoefList[0]); // accumulate data in a Long to eliminate rounding errors
	
	for(int i = 1; i < filterWindowSize; i++) // iterate through the rest of the data (stopping one short of start point)
	{
		OldestDataPoint++;	// increment and wrap pointer
		if(OldestDataPoint == filterWindowSize)  OldestDataPoint = 0;
		
		// multiply sample by coefficient 
		result.L += mul(DataList[OldestDataPoint], CoefList[i]);
	}
	
	result.L = result.L << 1; // bitshift to make up for non 16bit coefficient offset
	
	return result.I[1]; // the H16 part of the Long is the result
}


int FirFilter::out() // return the value of the most recent result 
{
	return result.I[1];;   
}


// **** Special Integer Multiplication ****
// signed16 * signed16 to signed32
// 22 cycles
// http://mekonik.wordpress.com/2009/03/18/arduino-avr-gcc-multiplication/
#define MultiS16X16to32(longRes, intIn1, intIn2) \
asm volatile ( \
"clr r26 \n\t" \
"mul %A1, %A2 \n\t" \
"movw %A0, r0 \n\t" \
"muls %B1, %B2 \n\t" \
"movw %C0, r0 \n\t" \
"mulsu %B2, %A1 \n\t" \
"sbc %D0, r26 \n\t" \
"add %B0, r0 \n\t" \
"adc %C0, r1 \n\t" \
"adc %D0, r26 \n\t" \
"mulsu %B1, %A2 \n\t" \
"sbc %D0, r26 \n\t" \
"add %B0, r0 \n\t" \
"adc %C0, r1 \n\t" \
"adc %D0, r26 \n\t" \
"clr r1 \n\t" \
: \
"=&r" (longRes) \
: \
"a" (intIn1), \
"a" (intIn2) \
: \
"r26" \
)


// multiplication wrapper function
int32_t inline FirFilter::mul(int16_t a, int16_t b) {
	int32_t r;
	MultiS16X16to32(r, a, b);
	return r;
}
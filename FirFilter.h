/*
  FirFilter.h 
  Copyright (c) 2013 Phillip Schmidt.  All right reserved.

 */

#ifndef FirFilter_h

	#define FirFilter_h

	#include "Arduino.h"

	class FirFilter
	{
	
		public: 
			FirFilter(byte filterSize, int cutOffFreq, int sampleFreq);
			int in(int value);
			int out();
		
		private: 
			int32_t mul(int16_t a, int16_t b);
			
			byte filterWindowSize;	// number of samples window	
			byte OldestDataPoint;	// oldest data point location	
			
			int * DataList;			// array for data	
			int * CoefList;			// array for coefficients	
			
			union TypeConverter
			{
				long L;
				int  I[2];
			} result;

	};

#endif

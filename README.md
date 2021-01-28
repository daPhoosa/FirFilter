FIR Low Pass Filter
===================

Fast Finite Impulse Response Low Pass Filter library for the Arduino 8-bit AVR processors

Written by Phillip Schmidt

Based on: http://www.labbookpages.co.uk/audio/firWindowing.html
  
Filtering is fast due to special assembly code integer multiply operation and optimized algorithm that leverages pre-computed constants and ring buffers.  Smooth precise filtering due to fixed point math that prevents truncation artifacts from injecting noise.
   
Object Creation:
```
FirFilter myFilter( filter_Size, cut_Off_Frequency, sample_Frequency );
```

`filter_Size:` Number of taps in the filter, should be an odd number.  Larger sizes improve filter smoothness but increase computational time.

Processing Time on a 16Mhz 8-bit Arduino
    7 window =  48us  
    9 window =  61us  
   11 Window =  73us  
   13 Window =  86us  
   21 Window = 137us  

`cut_Off_Frequency:` Frequency at 3db point of transistion.

`sample_Frequency:` Rate at which samples are submitted to the filter.  This should be consistant.


Filter Usage:
```
filtered_value = myFilter.in( unfiltered_value );
```
Input and output is a 16bit int.
  
Current output without passing in a new sample:
```
filtered_value = myFilter.out();
```
This executes with minimal computational cost.


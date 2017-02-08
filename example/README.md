# HDC codec sample data

The [hdc.txt](hdc.txt) file contains sample packets of HDC codec.  All of the packets in this file are contiguous in time and all passed CRC validation.
I stored bits received in the LSB position first.  So the first bit received is the LSB of byte 0.  The eighth bit received is the MSB of byte 0, etc.  It appears to be in the correct order based upon parsed headers, but if things look wrong, try reversing the bit order of each byte.

I don't know anything about the HDC codec, except that it was supposedly co-developed with Coding Technologies, the people behind AAC+ and some of the technologies in HE-AAC.

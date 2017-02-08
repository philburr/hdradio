# HD Radio SDR Receiver

This project is a partial implemention of the [NSRC-5](http://nrscstandards.org/NRSC-5-C.asp) HD Radio Air Interface for FM IBOC digital transmissions.  The specification does not (sadly) include a description of the iBiquity [HDC](https://en.wikipedia.org/wiki/High-Definition_Coding) codec.  As a result, it is not possible at this time to listen to IBOC digital transmissions.

## How to use this software

The executable expects a single parameter to a file which contains the IQ data captured with the FM station tuned in sampled at a sampling rate of 744187.5 samples per second.  The software only support service mode MP1 (all of the FM stations transmitting IBOC near me were using this mode).  It will dump out to the console the audio packets of the first (primary) logical channel.  There will be one line per audio packet with the data dumped in hex format.  If there is an asterisk (*) at the end of the line, it failed the CRC checksum.

## Why write this software

Admittedly this software has little practical use.  For me it was a tool for learning about digital transmissions, OFDM, convolutional coding, viterbi decoding, etc.

This software may be of interest to anyone interested in the iBiquity HDC codec.  If you figure out how to decode HDC audio, let me know and I'll be happy to make the software more robust (additional modes, more efficient decoding, soft viterbi decoding, etc).

## HDC sample audio

In the [example subdirectory](example/README.md) there is a dump of HDC data in text format.


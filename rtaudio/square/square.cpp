#include "RtAudio.h"
#include <iostream>
#include <cstdlib>
#include <stdint.h>
#include <unistd.h>
// Two-channel sawtooth wave generator.

int64_t counter=0;
int period=100;

int saw( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status, void *userData )
{
  unsigned int i=0;
  int16_t *buffer = (int16_t *) outputBuffer;
  double *lastValues = (double *) userData;
  if ( status )
    std::cout << "Stream underflow detected!" << std::endl;

  for ( i=0; i<nBufferFrames; i++ )
    {
      if (counter<period)
	buffer[i]=-8000;
      if (counter>=period)
	buffer[i]=8000;
      if (counter>(period*2))
	counter=0;
      counter++;
    }
  //write(1,buffer,nBufferFrames*2);
  
  return 0;
}
int main()
{
  RtAudio dac;
  if ( dac.getDeviceCount() < 1 ) {
    std::cout << "\nNo audio devices found!\n";
    exit( 0 );
  }
  RtAudio::StreamParameters parameters;
  parameters.deviceId = dac.getDefaultOutputDevice();
  parameters.nChannels = 1;
  parameters.firstChannel = 0;
  unsigned int sampleRate = 44100;
  unsigned int bufferFrames = 256; // 256 sample frames
  double data[2];
  try {
    dac.openStream( &parameters, NULL, RTAUDIO_SINT16,
                    sampleRate, &bufferFrames, &saw, (void *)&data );
    dac.startStream();
  }
  catch ( RtAudioError& e ) {
    e.printMessage();
    exit( 0 );
  }
  
  char input;
  std::cout << "\nPlaying ... press <enter> to quit.\n";
  std::cin.get( input );
  try {
    // Stop the stream
    dac.stopStream();
  }
  catch (RtAudioError& e) {
    e.printMessage();
  }
  if ( dac.isStreamOpen() ) dac.closeStream();
  return 0;
}

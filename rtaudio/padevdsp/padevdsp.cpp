#include "RtAudio.h"
#include <iostream>
#include <cstdlib>
#include <stdint.h>
#include <unistd.h>

// Two-channel sawtooth wave generator.

int64_t counter=0;

int saw( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status, void *userData )
{
  unsigned int i=0;
  int16_t *buffer = (int16_t *) outputBuffer;
  double *lastValues = (double *) userData;
  int countr=0;
  int countw=0;
  if ( status )
    std::cout << "Stream underflow detected!" << std::endl;

  // for ( i=0; i<nBufferFrames; i++ )
  //   {
  //     if (counter<100)
  // 	buffer[i]=-16000;
  //     if (counter>=100)
  // 	buffer[i]=-16000;
  //     counter++;
  //     if (counter>200)
  // 	counter=0;
  //   }

  countr=read(0,buffer,nBufferFrames*sizeof(int16_t));
  //countw=write(1,buffer,nBufferFrames*sizeof(int16_t));
  int j;
  int nbpos=0;
  int nbneg=0;
  for (j=0;j<nBufferFrames;j++)
    {
      if (buffer[j]<0)
	nbpos++;
      else
	nbneg++;
    }
  
		   
  //count=write(1,buffer,4096);
  if (0)
    fprintf(stderr,"%d %d %d\n",countr,nbpos,nbneg);

  //fprintf(stderr,"%d \n",count);
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
  sleep(60);
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

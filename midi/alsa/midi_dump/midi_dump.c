#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>

void  error                     (const char *format, ...);


void print_card_list(void) {
   int status;
   int card = -1;  // use -1 to prime the pump of iterating through card list
   char* longname  = NULL;
   char* shortname = NULL;

   if ((status = snd_card_next(&card)) < 0) {
      error("cannot determine card number: %s", snd_strerror(status));
      return;
   }
   if (card < 0) {
      error("no sound cards found");
      return;
   }
   while (card >= 0) {
      printf("Card %d:", card);
      if ((status = snd_card_get_name(card, &shortname)) < 0) {
         error("cannot determine card shortname: %s", snd_strerror(status));
         break;
      }
      if ((status = snd_card_get_longname(card, &longname)) < 0) {
         error("cannot determine card longname: %s", snd_strerror(status));
         break;
      }
      printf("\tLONG NAME:  %s\n", longname);
      printf("\tSHORT NAME: %s\n", shortname);
      if ((status = snd_card_next(&card)) < 0) {
         error("cannot determine card number: %s", snd_strerror(status));
         break;
      }
   } 
}


int main(int argc, char *argv[]) {
    print_card_list();
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <MIDI port name>\n", argv[0]);
        return 1;
    }

    const char *portname = argv[1];
    snd_rawmidi_t *input;
    int status;

    // Open the MIDI input port for reading
    if ((status = snd_rawmidi_open(&input, NULL, portname, SND_RAWMIDI_NONBLOCK)) < 0) {
        fprintf(stderr, "Problem opening MIDI input: %s\n", snd_strerror(status));
        return 1;
    }

    printf("Reading MIDI input from port '%s'. Press Ctrl+C to exit.\n", portname);

    while (1) {
        unsigned char buffer[1024];
        int n = snd_rawmidi_read(input, buffer, sizeof(buffer));

        if (n < 0) {
            // Handle error or wait
            //snd_rawmidi_wait(input, 100);
	    usleep(100);
        } else if (n > 0) {
            // Print the MIDI message in hexadecimal format
            for (int i = 0; i < n; i++) {
                printf("%02X ", buffer[i]);
            }
            printf("\n");
        }
    }

    // Close the MIDI port (unreachable in this example)
    snd_rawmidi_close(input);
    return 0;
}

#include <SDL/SDL.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <list>


#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480
#define SCREEN_DEPTH	16
#define NCOEF	(32)

int y_m=SCREEN_HEIGHT/2;
int divide=128;
bool redraw=true;
int loop=0;
int offset=0;
float zoom=1.0;
int quit;

char * filename;

int len=0;
Sint16 * file_buffer=0;
Sint16 * vector_buffer=0;
int filesize_octet=0;

SDL_Surface *screen=NULL;

bool left=false;
bool right=false;
bool up=false;
bool down=false;
bool pageup=false;
bool pagedown=false;
bool ctrl=false;



/*
cat /proc/fs/ldiskfs/loop0/mb_groups
#group: free free frags first pa [ 2^0 2^1 2^2 2^3 2^4 2^5 2^6 2^7 2^8 2^9 2^10 2^11 2^12 2^13 ]
#0 : 2936 2936 1 42 0 [ 0 0 0 1 1 1 1 2 0 1 2 0 0 0 ]
where columns are:

#group number
available blocks in the group
blocks free on a disk
number of free fragments
the very first free block in the group
number of preallocated chunks (not blocks)
a series of available chunks of different sizes

 */

struct mb_group_blockgroup
{
  int32_t group_number;
  int32_t available_block_in_group;
  int32_t block_free_on_a_disk;
  int32_t number_of_free_fragment;
  int32_t number_of_preallocated_chunks;

  int16_t av_chunk[13]; 
} mb_bg;

std::list <struct mb_group_blockgroup*> l_mb_bg;


void init_video()
{
  if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
    {
      fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
      exit(1);
    }
  
  printf("SDL initialized.\n");
  screen = SDL_SetVideoMode(SCREEN_WIDTH, 
			    SCREEN_HEIGHT, 
			    SCREEN_DEPTH,			
			    SDL_SWSURFACE|SDL_DOUBLEBUF|SDL_HWACCEL);
  
  if (screen == NULL)
    {
      fprintf(stderr, "Couldn't set video mode: %s\n", SDL_GetError());
      SDL_Quit();
      exit(1);
    }  
}


void load_raw(char * filepath)
{

  struct stat fileStat;
  FILE * FP;
  char line[1024];
  mb_group_blockgroup * mb;
  int size;
  
  FP=fopen(filepath,"r");
  for (int i=0;i<10000;i++)
    {
      //new m=mb_group_blockgroup
      mb=new(mb_group_blockgroup);
      mb->group_number=i;
      l_mb_bg.push_back(mb);
      fgets(line,1024,FP);
      //l_mb_bg
      printf("%d %s",i,line);
    }
  size=l_mb_bg.size();
  printf("size: %d\n",size);
  
  for (int i=0;i<size;i++)
    {
      //printf("%d %d\n",i);
      mb=*l_mb_bg.begin();
      printf("%d \n",mb->group_number);
      delete(mb);
      l_mb_bg.pop_front();
    }
  
  fclose(FP);  
}


void handle_key()
{
  SDL_Event event;
  if (SDL_PollEvent(&event))
    {
      printf("loop %d\n",loop);
      loop++;

      //printf("[%d %d %d %d]\n",SDL_KEYUP,SDL_KEYDOWN,event.type,event.key.keysym.sym);
      switch (event.type)
	{
	case SDL_QUIT:
	  printf("Exiting...\n");
	  quit = 1;
	  free(file_buffer);
	  exit(0);
	  break; 	  
	  
	case SDL_KEYUP:
	  switch (event.key.keysym.sym)
	    {
	    case SDLK_LCTRL:
	      ctrl=false;
	      break;

	    case SDLK_RCTRL:
	      ctrl=false;
	      break;

	    case SDLK_LEFT:
	      left=false;
	      break;
	      
	    case SDLK_RIGHT:
	      right=false;
	      break;
	      
	    case SDLK_UP:
	      up=false;
	      break;
	      
	    case SDLK_DOWN:
	      down=false;
	      break;

	    case SDLK_PAGEUP:
	      pageup=false;
	      break;
	      
	    case SDLK_PAGEDOWN:
	      pagedown=false;
	      break;

	    default:
	      break;
	    }
	  break;
	  
	case SDL_KEYDOWN:
	  switch (event.key.keysym.sym)
	    {
	    case SDLK_ESCAPE:
	      printf("Exiting...\n");
	      quit = 1;
	      exit(0);
	      break;
	      
	    case SDLK_LCTRL:
	      ctrl=true;
	      break;

	    case SDLK_RCTRL:
	      ctrl=true;
	      break;

	    case SDLK_LEFT:
	      left=true;
	      break;
	      
	    case SDLK_RIGHT:
	      right=true;
	      break;
	      
	    case SDLK_UP:
	      up=true;
	      break;
	      
	    case SDLK_DOWN:
	      down=true;
	      break;	 	  
	      	     
	    case SDLK_PAGEUP:
	      pageup=true;
	      break;
	      
	    case SDLK_PAGEDOWN:
	      pagedown=true;
	      break;
	      
	    default:
	      break;
	      
	    }

	  break;

	}
      printf("new event:%d %s\n",event.type,SDL_GetKeyName(event.key.keysym.sym));
    }

  printf("No new event\n");

  
  if(up)
    {
    if (divide<=32) divide=32;
    divide=divide/1.1;
    redraw=true;
    printf("key up\n");
    }

  if(down)
    {
      if (divide>=4096) divide=4096;
      divide=divide*1.1;
      redraw=true;
      printf("key down\n");
    }

  if(left)
    {
      offset=offset--;
      if (!ctrl) 
	offset=offset-(10*zoom);
      else
	offset=offset-SCREEN_WIDTH;

      if (offset <=0) offset=0;
      redraw=true;
      printf("key left\n");            
    }

  if (right)
    {
      offset=offset++;
      if (!ctrl) 
	offset=offset+(10*zoom);
      else
	offset=offset+SCREEN_WIDTH;

      redraw=true;
      printf("key right\n");      
    }

  if(pageup)
    {
      zoom=zoom*1.1;
      if (zoom>1024.0) zoom=1024;
      redraw=true;
      printf("key pgup %f\n",zoom);      
    }

  if(pagedown)
    {
      zoom=zoom/1.1;
      if (zoom<0.01) zoom=0.01;
      redraw=true;
      printf("key pgdown %f\n",zoom);
    }

  //No user activity => sleeping a while to keep cpu cool
  //it keep cpu under 100% usage
  if ((up || down || left || right || pagedown || pageup)==false)
    {
      printf("sleeping 100ms");
      SDL_Delay(100);
    }
  else
    {
      printf("sleeping 10ms");
      SDL_Delay(10);
    }
}

void prepare_vector_buffer()
{
  Sint16 y_value;
  Sint32 j;
  Sint32 last_j;
  Sint32 idx=0;
  Sint64 average=0;
  Sint64 counter=0;
  for (int i = 0; i < SCREEN_WIDTH-1 ; i++)
    {
      last_j=j;
      j=offset+i*zoom;
      if (j<=filesize_octet*2)
	{
	  y_value=file_buffer[j]/divide;
 	}
      else
	{
	  y_value=0;      
	}
      vector_buffer[i]=y_value;
    }
}

void draw_screen()
{
  SDL_Rect rect;
  short color;

  // rect.x rect.y rect.w rect.h
  // int x the x location of the rectangle's upper left corner
  // int y the y location of the rectangle's upper left corner
  // int w the width of the rectangle
  // int h the height of the rectangle

  //Blank screen
  SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));               

  for (int i = 0; i < SCREEN_WIDTH-1 ; i++)
    {
      rect.x = i;
      rect.w = 1;

      if (vector_buffer[i]>0)
	{
	  rect.y=((SCREEN_HEIGHT/2)-vector_buffer[i]);
	  rect.h=vector_buffer[i];
	}
      else
	{
	  rect.y=((SCREEN_HEIGHT/2));
	  rect.h=vector_buffer[i]*-1;	  
	}

      // Create a vertical colored line
      color=vector_buffer[i];
      SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, color, 0, 128));      
    }

  //Draw Min lower bar
  rect.x=0;
  rect.w=SCREEN_WIDTH;
  rect.y=((SCREEN_HEIGHT/2)+(float)(16384.0/divide));
  rect.h=1;
  SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, color, 0, 128));      


  //Draw Max upper bar
  rect.x=0;
  rect.w=SCREEN_WIDTH;
  rect.y=((SCREEN_HEIGHT/2)-(float)(16384.0/divide));
  rect.h=1;
  SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, color, 0, 128));      



  SDL_Flip(screen);
  SDL_Delay(1);
  redraw=false;  
}



int main(int argc,char ** argv)
{	  
  if (argc!=2) { printf("arg please\n"); exit(1); }
  
  struct stat fileStat;
  if(stat(argv[1],&fileStat) < 0)    
    return 1;


  filename=argv[1];
  load_raw(filename);

  init_video();
  /* Main loop */
  quit = 0;
  while(!quit)
    {
      if (redraw==true)
	{	  
	  //prepare_vector_buffer();
	  //draw_screen();
	  redraw=false;
	}
      else 
	{ 
	  handle_key(); 
	}
    }

  return 0;
}


#include <visualizer.h>
#include<SDL2/SDL_image.h> //use in future to implement the background picture in the main visualizer window
int main(int argc,char **argv)
{

SDL_AudioDeviceID device;
SDL_AudioSpec wavSpec;
SDL_AudioSpec obtained;
SDL_Window* win;
SDL_Event event;
Uint8 *wavBuffer;
Uint32 wavLength;
struct AudioData *audio =(struct AudioData *)malloc(sizeof(struct AudioData));
sem_init(&play,0,0);

Uint32 startTime=0;
Uint32 endTime=0;
Uint32 delta=0;
short fps=60;
short timePerFrame=15;

char *f;
int running=1;
int playing=0;
MODE=1;


f=argv[1];

if(f==NULL)
{
printf("You Need to Specify a File \n");
exit(1);
}

fprintf(stderr,"Playing File : %s",f);



audio->in=(fftw_complex*)fftw_malloc(sizeof(fftw_complex)*2048);
audio->out=(fftw_complex*)fftw_malloc(sizeof(fftw_complex)*2048);
audio->color=(struct RGB* )malloc(sizeof(struct RGB));

audio->timeDomain=(SDL_Point*)malloc(sizeof(SDL_Point)*2048);

//plan dft in 1d,FORWARD
audio->plan=fftw_plan_dft_1d(2048,audio->in,audio->out,FFTW_FORWARD,FFTW_MEASURE);



if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)==-1)
{
exit(1);

}

win=SDL_CreateWindow("PLAYER ",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,WIDTH,HEIGHT,SDL_WINDOW_SHOWN);

audio->renderer=SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED);



if(SDL_LoadWAV(f,&wavSpec,&wavBuffer,&wavLength)==NULL)
{

printf("Unable to load File");
printf("%s is Not a Wav File \n",f);
return 0;
}

audio->position=wavBuffer;
audio->length=wavLength;
wavSpec.freq=SAMPLE_RATE;
wavSpec.channels=2;
wavSpec.samples=2048;
wavSpec.callback=forwardChunk;
wavSpec.userdata=audio;
wavSpec.format=AUDIO_S16;
audio->format=wavSpec.format;

device=SDL_OpenAudioDevice(NULL,0,&wavSpec,&obtained,SDL_AUDIO_ALLOW_FORMAT_CHANGE);

if(device==0)
{
printf("FAILED TO LOCATE ANY AUDIO DEVICE \n");
exit(1);
}

SDL_PauseAudioDevice(device,playing);
while(running)
{
if(!startTime)
{
startTime=SDL_GetTicks();
}
else
{
delta=endTime-startTime;

}

if(delta<timePerFrame)
SDL_Delay(timePerFrame-delta);

if(delta>timePerFrame)
{
fps=1000/delta;
}

while(SDL_PollEvent(&event))
{
if(event.type==SDL_QUIT)
running=0;


if(event.type==SDL_KEYDOWN)
{

switch(event.key.keysym.sym)
{
case SDLK_q:
playing=1;
running=0;
break;



case SDLK_p:
playing=!playing;
SDL_PauseAudioDevice(device,playing);
break;

case SDLK_m:
changeMode();
break;
}
}
}
}


startTime=endTime;
endTime=SDL_GetTicks();




fftw_destroy_plan(audio->plan);
fftw_cleanup();
fftw_free(audio->in);
fftw_free(audio->out);
    
SDL_FreeWAV(wavBuffer);
SDL_CloseAudioDevice(device);
SDL_DestroyRenderer(audio->renderer);
SDL_DestroyWindow(win);
free(audio->color);
win=NULL;
audio->renderer=NULL;
audio->color=NULL;
SDL_Quit();




return 0;
}

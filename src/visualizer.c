#include <visualizer.h>
void createRGB(struct RGB* color,double r,double g,double b)
{
color->r=r;
color->g=g;
color->b=b;
}
void changeMode()
{
MODE=!MODE;

}
void conversion(struct HSV in,struct RGB* out)
{
double c,m,x;
c=m=x=0.0;

c=in.v * in.s;
x=c * (1.0 - fabs(fmod(in.h / 60,2) - 1.0));
m=in.v - c;

if(in.h>=0.0 && in.h<60.0)
createRGB(out,c+m,x+m,m);
else if(in.h>=60.0 && in.h<120.0)
createRGB(out,x+m,c+m,m);
else if(in.h>=120.0 && in.h<180.0)
createRGB(out,m,c+m,x+m);
else if(in.h>=180.0 && in.h<240.0)
createRGB(out,m,x+m,c+m);
else if(in.h>=240.0 && in.h<300.0)
createRGB(out,x+m,m,c+m);
else if(in.h>=300 && in.h<360.0)
createRGB(out,c+m,m,x+m);
else
createRGB(out,m,m,m);
out->r *= 255;
out->g *= 255;
out->b *= 255;
}
void forwardChunk(void* userData,Uint8* stream,int len)
{
struct AudioData* audio=(struct AudioData*)userData;
pthread_t th;
struct wrapper wrap;
if(audio->length==0)
return;
wrap.stream=stream;
wrap.audio=audio;
pthread_create(&th,NULL,visualizerOutput,(void*)&wrap);
Uint32 length=(Uint32) len;
length=(length > audio->length?audio->length:length); 
sem_wait(&play);
SDL_memcpy(stream,audio->position,length);
audio->position+=length;
audio->length-=length;   
//printf("Audio position  : %d\n",*(audio->position));
//printf("Audio Length : %d",audio->length);
}
double Get16bitAudioSample(Uint8* bytebuffer,SDL_AudioFormat format)
{
Uint16 val= 0x0;

//Incase of little ENDIAN format machine

if(SDL_AUDIO_ISLITTLEENDIAN(format))
{
val=(uint16_t)bytebuffer[0] | ((uint16_t)bytebuffer[1] << 8);
//printf("%d\n",sizeof(val));
}
else// incase of big Endian Note: didnt check the code b//ut it should works correctly
{
val=((uint16_t)bytebuffer[0] << 8) | (uint16_t)bytebuffer[1];
}
if(SDL_AUDIO_ISSIGNED(format))
{
return ((int16_t)val)/16383.0;
}
val=val/65535.0;
return val;
}


void* visualizerOutput(void* arg)
{
SDL_Texture *gTexture=NULL;
struct wrapper* wrap=(struct wrapper*)arg;
double max[BARS];
double multiplier;
int window_size=2;
int count=0;
int sum;
double freq_bin[BARS+1];
double re,im;
float CONSTANT=(float)2048/WIDTH;
float freq;
double magnitude;
int startx=0,starty=HEIGHT;
struct HSV hsv;
static int colorstart=0;
for(int i=0;i<BARS;i++)
{
max[i]=1.7E-308;
freq_bin[i]=i*(SAMPLE_RATE/2048)+i;
//printf("%d:%lf\t",i,freq_bin[i]);
}
freq_bin[BARS]=SAMPLE_RATE/2;
for(int i=0;i<2048;i++)
{
//getting values from stream and applying hann windowing function
//in an online article it is recomeded to first apply hann windowing function and then pass it to fftw


multiplier= 0.5*(1-cos(2*M_PI*i/2048));



//printf("%d\n",*(wrap->stream));
wrap->audio->in[i][0]=Get16bitAudioSample(wrap->stream,wrap->audio->format)*multiplier;
//wrap->audio->in[i][0]=*(wrap->stream)*multiplier;
//printf("%f\n",wrap->audio->in[i][0]);
wrap->audio->in[i][1]=0.0;
wrap->stream+=2;
}
//time domain visualizer 
if(MODE)
{
SDL_SetRenderDrawColor(wrap->audio->renderer,0,0,0,0);
SDL_RenderClear(wrap->audio->renderer);
hsv.h=( 2 + colorstart) % 360;
hsv.s=hsv.v=1.0;
conversion(hsv,wrap->audio->color);
SDL_SetRenderDrawColor(wrap->audio->renderer,wrap->audio->color->r,wrap->audio->color->g,wrap->audio->color->b,255);
for(int i=0;i<2048;i++)
{
wrap->audio->timeDomain[i].x=i/CONSTANT;
wrap->audio->timeDomain[i].y=300-wrap->audio->in[i][0]*70;
}
SDL_RenderDrawLines(wrap->audio->renderer,wrap->audio->timeDomain,2048);
//ENDING TIME DOMAIN MODE
}
else
{
//INIT BARS MODE
fftw_execute(wrap->audio->plan);

//calculate magnitudes
for(int j=0;j < 2048/2;j++)
{
re=wrap->audio->out[j][0];
im=wrap->audio->out[j][1];
magnitude=sqrt((re*re)+(im*im));
freq=j*((double)SAMPLE_RATE/2048);
for(int i=0;i < BARS;i++)
if((freq>freq_bin[i]) && (freq<=freq_bin[i+1]))
if(magnitude > max[i]) 
max[i]=magnitude;
}
    
SDL_SetRenderDrawColor(wrap->audio->renderer,0,0,0,0);
SDL_RenderClear(wrap->audio->renderer);
for(int i=0;i<BARS;i++)
{
hsv.h=( (i * 2) + colorstart) % 360;
hsv.s=hsv.v=1.0;
conversion(hsv,wrap->audio->color);
SDL_SetRenderDrawColor(wrap->audio->renderer,wrap->audio->color->r,wrap->audio->color->g,wrap->audio->color->b,255);

if(max[i]>2.0)
max[i]=log(max[i]);

for(int j=0;j<THICKNESS;j++)
SDL_RenderDrawLine(wrap->audio->renderer,startx+(i*DISTANCE+j),starty,startx+(i*DISTANCE+j),starty-(FIT_FACTOR*max[i]));
}
//ENDING BARS MODE
}



colorstart+=2;
SDL_RenderPresent(wrap->audio->renderer); 
sem_post(&play);
} 




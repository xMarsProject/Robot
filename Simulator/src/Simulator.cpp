#include <SDL.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <ctime>
#include "Simulator.h"
#include "Vertex.h"

using namespace std;

void Simulator::Run()
{
    //Start up SDL and create window

    if( !Init() )
    {
        printf( "Failed to initialize!\n" );
    }
    else
    {
        time_t tbegin,tend;
        int NbMsec;
        InitBot(50,50,10,10,90);
        int tx,ty,d,tangle;
        tx=xBot;
        ty=yBot;
        tangle=angleBot;
        d=0;
        while (!Bot->Stop && !quit)
        {
            tbegin=time(NULL);
            quit=WindowClosed();

            if (Bot->left)
            {
                tangle=(tangle-1)<0?360:tangle-1;
                xBot=tx;
                yBot=ty;
                d=0;
            }
            if (Bot->right)
            {
                tangle=(tangle+1)>360?0:tangle+1;
                xBot=tx;
                yBot=ty;
                d=0;
            }
            if (Bot->bback)
            {
                d-=Unit;
                Bot->bot_wheel_l-=Unit;
                Bot->bot_wheel_r-=Unit;
                Translate(&tx,&ty,tangle,d);
            }
            if (Bot->advance)
            {
                Bot->bot_wheel_l+=Unit;
                Bot->bot_wheel_r+=Unit;
                d+=Unit;
                Translate(&tx,&ty,tangle,d);
            }
            Clear();
            DrawScene();
            Bot->azimuth=(tangle-angleBot)<0?360-tangle-angleBot:(tangle-angleBot);
            DrawBot(tx,ty,tangle);
            Bot->ScanFront=ScanFront();
            Bot->ScanRight=ScanRight();
            Bot->ScanLeft=ScanLeft();

            SDL_RenderPresent( gRenderer );
            tend=time(NULL);
            NbMsec=Delay-difftime(tend,tbegin);
            Sleep(NbMsec>0?NbMsec:0);
        }
    }
    Close();
}

void Simulator::InitBot(int x,int y,int h, int w, int angle)
{
    xBot=x;
    yBot=y;
    hBot=h*Ratio;
    wBot=w;
    angleBot=angle;
}

void Simulator::Translate(int *x,int *y, int angle, int d)
{
    angle=(angle-90)<0?360+(angle-90):angle-90;
    *x=xBot+d*cos(angle*M_PI/180);
    *y=yBot+d*sin(angle*M_PI/180);
}

void Simulator::Clear()
{
    SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );
    SDL_RenderClear( gRenderer );
}

void Simulator::InitScene()
{
    SDL_Surface*  Scene = SDL_LoadBMP( "C:/Users/vpi/Documents/CPP/Simulator/Scenes/Scene02.bmp" );
    if( Scene == NULL )
    {
        printf( "Unable to load the scene\n" );
    }
    else
    {
        Uint32 pixel;
        nbPix=-1;
        for (int x=0; x<640; x++)
        {
            for (int y=0; y<480; y++)
            {
                pixel=getPixel(Scene, x, y);
                if (pixel!=255)
                {
                    nbPix++;
                    xScene[nbPix]=x;
                    yScene[nbPix]=y;
                }
            }
        }
    }
    printf("%d pixels\n",nbPix);
}

/*
void Simulator::DrawBox(int x, int y, int c)
{
    c=c/2;Bot->ScanFront=ScanFront();
    int c1=c*Ratio;
    int vx[5];
    int vy[5];

    vx[0]=x-c;
    vy[0]=y-c1;
    vx[1]=x+c;
    vy[1]=y-c1;
    vx[2]=x+c;
    vy[2]=y+c1;
    vx[3]=x-c;Bot->ScanFront=ScanFront();
    vy[3]=y+c1;
    vx[4]=x;
    vy[4]=y-c1;

    SDL_RenderDrawPoint(gRenderer, x,y);
    SDL_RenderDrawLine( gRenderer, vx[0],vy[0],vx[1],vy[1]);
    SDL_RenderDrawLine( gRenderer, vx[1],vy[1],vx[2],vy[2]);
    SDL_RenderDrawLine( gRenderer, vx[3],vy[3],vx[2],vy[2]);
    SDL_RenderDrawLine( gRenderer, vx[0],vy[0],vx[3],vy[3]);
}
*/

void Simulator::DrawScene()
{
    SDL_SetRenderDrawColor( gRenderer, 0x80, 0x80, 0x00, 0xFF );
    for (int i=0; i<nbPix; i++)
    {
        SDL_RenderDrawPoint(gRenderer, xScene[i],yScene[i]);
    }
}

Uint32 Simulator::getPixel(SDL_Surface *surface, int x, int y)
{

    int nbOctetsParPixel = surface->format->BytesPerPixel;

    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * nbOctetsParPixel;

    switch(nbOctetsParPixel)
    {
    case 1:
        return *p;
    case 2:
        return *(Uint16 *)p;
    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
    case 4:
        return *(Uint32 *)p;
    default:
        return 0;
    }
}

bool Simulator::ScanFrontBox(int x, int y)
{
    //return isInside(xScanner[0], yScanner[0], xScanner[1], yScanner[1], xScanner[2], yScanner[2], x, y);

    int as_x = x-xScanner[0];
    int as_y = y-yScanner[0];

    bool s_ab = (xScanner[1]-xScanner[0])*as_y-(yScanner[1]-yScanner[0])*as_x > 0;
    if((xScanner[2]-xScanner[0])*as_y-(yScanner[2]-yScanner[0])*as_x > 0 == s_ab) return false;
    if((xScanner[2]-xScanner[1])*(y-yScanner[1])-(yScanner[2]-yScanner[1])*(x-xScanner[1]) > 0 != s_ab) return false;
    return true;

}

bool Simulator::ScanRightBox(int x, int y)
{
    int as_x = x-xScanner[3];
    int as_y = y-yScanner[3];

    bool s_ab = (xScanner[4]-xScanner[3])*as_y-(yScanner[4]-yScanner[3])*as_x > 0;
    if((xScanner[5]-xScanner[3])*as_y-(yScanner[5]-yScanner[3])*as_x > 0 == s_ab) return false;
    if((xScanner[5]-xScanner[4])*(y-yScanner[4])-(yScanner[5]-yScanner[4])*(x-xScanner[4]) > 0 != s_ab) return false;
    return true;
}

bool Simulator::ScanLeftBox(int x, int y)
{
    int as_x = x-xScanner[6];
    int as_y = y-yScanner[6];

    bool s_ab = (xScanner[7]-xScanner[6])*as_y-(yScanner[7]-yScanner[6])*as_x > 0;
    if((xScanner[8]-xScanner[6])*as_y-(yScanner[8]-yScanner[6])*as_x > 0 == s_ab) return false;
    if((xScanner[8]-xScanner[7])*(y-yScanner[7])-(yScanner[8]-yScanner[7])*(x-xScanner[7]) > 0 != s_ab) return false;
    return true;
}


int Simulator::ScanFront()
{
    double px,py;
    int d=99999;
    int n;
    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x00, 0xFF, 0xFF );
    for (int i=0; i<nbPix; i++)
    {
        if (ScanFrontBox(xScene[i],yScene[i]))
        {
            SDL_RenderDrawPoint(gRenderer, xScene[i],yScene[i]);
            px = abs(xScene[i] - xScanner[0]);
            py = abs(yScene[i] - yScanner[0]);
            n=sqrt( px*px + py*py );
            if (n<d && n>0)
                d=n;
        }
    }
    return d;
}

int Simulator::ScanRight()
{
    double px,py;
    int d=99999;
    int n;
    SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0xFF, 0xFF );
    for (int i=0; i<nbPix; i++)
    {
        if (ScanRightBox(xScene[i],yScene[i]))
        {
            SDL_RenderDrawPoint(gRenderer, xScene[i],yScene[i]);
            px = abs(xScene[i] - xScanner[3]);
            py = abs(yScene[i] - yScanner[3]);
            n=sqrt( px*px + py*py );
            if (n<d && n>0)
                d=n;
        }
    }
    return d;
}

int Simulator::ScanLeft()
{
    double px,py;
    int d=99999;
    int n;
    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x80, 0x00, 0xFF );
    for (int i=0; i<nbPix; i++)
    {
        if (ScanLeftBox(xScene[i],yScene[i]))
        {
            SDL_RenderDrawPoint(gRenderer, xScene[i],yScene[i]);
            px = abs(xScene[i] - xScanner[6]);
            py = abs(yScene[i] - yScanner[6]);
            n=sqrt( px*px + py*py );
            if (n<d && n>0)
                d=n;
        }
    }
    return d;
}

void Simulator::DrawBot(int x,int y, int angle)
{
    int vx[13];
    int vy[13];
    int tx,ty;
    int a1;
    int a2;
    int d;

    vx[0]=x-wBot/2;
    vy[0]=y-hBot/2;
    vx[1]=x+wBot/2;
    vy[1]=y-hBot/2;
    vx[2]=x+wBot/2;
    vy[2]=y+hBot/2;
    vx[3]=x-wBot/2;
    vy[3]=y+hBot/2;

    // Front Scanner

    vx[4]=x;
    vy[4]=y-hBot;

    a1=90*3+45/2;
    a2=90*3-45/2;
    d=hBot*2;

    vx[5]=vx[4]+d*cos(a1*M_PI/180);
    vy[5]=vy[4]+d*sin(a1*M_PI/180);

    vx[6]=vx[4]+d*cos(a2*M_PI/180);
    vy[6]=vy[4]+d*sin(a2*M_PI/180);

    // Right Scanner


    a1=45/2;
    a2=90*4-45/2;


    //a1=45/2;
    //a2=a1+45;

    vx[7]=x+wBot/2;
    vy[7]=y;

    vx[8]=vx[7]+d*cos(a1*M_PI/180);
    vy[8]=vy[7]+d*sin(a1*M_PI/180);

    vx[9]=vx[7]+d*cos(a2*M_PI/180);
    vy[9]=vy[7]+d*sin(a2*M_PI/180);

    // Left Scanner

    a1=90*2-45/2;
    a2=a1+45;

    vx[10]=x-wBot/2;
    vy[10]=y;

    vx[11]=vx[10]+d*cos(a1*M_PI/180);
    vy[11]=vy[10]+d*sin(a1*M_PI/180);

    vx[12]=vx[10]+d*cos(a2*M_PI/180);
    vy[12]=vy[10]+d*sin(a2*M_PI/180);

    for (int i=0; i<13; i++)
    {
        tx=vx[i];
        ty=vy[i];
        vx[i]=x+((tx-x)*cos(angle*M_PI/180)-(ty-y)*sin(angle*M_PI/180));
        vy[i]=y+((ty-y)*cos(angle*M_PI/180)+(tx-x)*sin(angle*M_PI/180));
    }

    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0x80, 0xFF );
    SDL_RenderDrawPoint(gRenderer, x,y);
    SDL_RenderDrawLine( gRenderer, vx[0],vy[0],vx[1],vy[1]);
    SDL_RenderDrawLine( gRenderer, vx[1],vy[1],vx[2],vy[2]);
    SDL_RenderDrawLine( gRenderer, vx[2],vy[2],vx[3],vy[3]);
    SDL_RenderDrawLine( gRenderer, vx[3],vy[3],vx[0],vy[0]);

    SDL_RenderDrawLine( gRenderer, vx[0],vy[0],vx[4],vy[4]);
    SDL_RenderDrawLine( gRenderer, vx[4],vy[4],vx[1],vy[1]);

    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x00, 0x00, 0xFF );

    SDL_RenderDrawLine( gRenderer, vx[4],vy[4],vx[5],vy[5]);
    SDL_RenderDrawLine( gRenderer, vx[5],vy[5],vx[6],vy[6]);
    SDL_RenderDrawLine( gRenderer, vx[6],vy[6],vx[4],vy[4]);

    xScanner[0]=vx[4];
    yScanner[0]=vy[4];
    xScanner[1]=vx[5];
    yScanner[1]=vy[5];
    xScanner[2]=vx[6];
    yScanner[2]=vy[6];

    SDL_RenderDrawLine( gRenderer, vx[7],vy[7],vx[8],vy[8]);
    SDL_RenderDrawLine( gRenderer, vx[8],vy[8],vx[9],vy[9]);
    SDL_RenderDrawLine( gRenderer, vx[9],vy[9],vx[7],vy[7]);

    xScanner[3]=vx[7];
    yScanner[3]=vy[7];
    xScanner[4]=vx[8];
    yScanner[4]=vy[8];
    xScanner[5]=vx[9];
    yScanner[5]=vy[9];

    SDL_RenderDrawLine( gRenderer, vx[10],vy[10],vx[11],vy[11]);
    SDL_RenderDrawLine( gRenderer, vx[11],vy[11],vx[12],vy[12]);
    SDL_RenderDrawLine( gRenderer, vx[12],vy[12],vx[10],vy[10]);

    xScanner[6]=vx[10];
    yScanner[6]=vy[10];
    xScanner[7]=vx[11];
    yScanner[7]=vy[11];
    xScanner[8]=vx[12];
    yScanner[8]=vy[12];

}

bool Simulator::Init()
{
    gWindow=NULL;
    quit = false;
    //The window renderer
    gRenderer=NULL;
    //Initialization flag
    bool success = true;
    logfile = fopen("simu.txt", "w+");
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Set texture filtering to linear
        if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
        {
            printf( "Warning: Linear texture filtering not enabled!" );
        }

        //Create window
        gWindow = SDL_CreateWindow( "Robot Simulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ScreenWidth, ScreenHeight, SDL_WINDOW_SHOWN );
        if( gWindow == NULL )
        {
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            //Create renderer for window
            gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
            if( gRenderer == NULL )
            {
                printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
                success = false;
            }
            else
            {
                //Initialize renderer color
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
            }
        }
    }
    return success;
}

bool Simulator::WindowClosed ()
{
    //Handle events on queueSimulator
    while( SDL_PollEvent( &e ) != 0 )
    {
        //User requests quit
        if( e.type == SDL_QUIT )
        {
            return true;
        }
    }
    return false;
}

void Simulator::Close()
{
    while( !quit )
    {
        quit=WindowClosed();
        Sleep(1);
    }
    //Destroy window
    SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;
    gRenderer = NULL;

    //Quit SDL subsystems
    SDL_Quit();
    fclose(logfile);
}

Simulator::Simulator(int ScreenWidth, int ScreenHeight, int NbFps, int Unit)
{
    this->ScreenWidth=ScreenWidth;
    this->ScreenHeight=ScreenHeight;
    this->NbFps=NbFps;
    this->Unit=Unit;
    Delay=1000/NbFps;
    Ratio=ScreenWidth/ScreenHeight;
    InitScene();
}

Simulator::~Simulator()
{
    //dtor
}

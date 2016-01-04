#ifndef SIMULATOR_H
#define SIMULATOR_H
#include <SDL.h>
#include "Robot.h"
#include "Vertex.h"

class Simulator
{
    public:
        Simulator(int ScreenWidth, int ScreenHeight,int NbFps, int Unit);
        void Run();
        virtual ~Simulator();
        Robot *Bot;
    protected:
    private:
        int ScreenWidth;
        int ScreenHeight;
        int NbFps;
        float Ratio;
        int Unit;
        int Delay;

        SDL_Window* gWindow;
        SDL_Renderer* gRenderer;
        bool quit;
        SDL_Event e;
        FILE* logfile;

        int xBot;
        int yBot;
        int hBot;
        int wBot;
        int angleBot;

        int xScene[307200];
        int yScene[307200];
        int nbPix;

        int xScanner[9];
        int yScanner[9];

        void Close();
        bool Init();
        void InitBot(int x,int y,int h, int w, int angle);
        void DrawBot(int x,int y, int angle);
        void InitScene();
        void DrawScene();
        void DrawBox(int x, int y, int c);
        Uint32 getPixel(SDL_Surface *surface, int x, int y);
        bool ScanFrontBox(int x, int y);
        bool ScanLeftBox(int x, int y);
        bool ScanRightBox(int x, int y);
        int ScanFront();
        int ScanLeft();
        int ScanRight();
        void Translate(int *x,int *y, int angle, int d);
        bool WindowClosed();
        void Clear();
};

#endif // SIMULATOR_H

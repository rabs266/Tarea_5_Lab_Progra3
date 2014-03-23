
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include <string>
#include <fstream>

using namespace std;

char tablero[5][5] = {{'-', '-', '-', '#','-'},
                      {'0', '#', '-', '#','-'},
                      {'-', '-', '-', '#','-'},
                      {'#', '#', '#', '#','#'},
                      {'#', '#', '#', '-','#'}};

void initTablero()
{
    ifstream i("Laberinto.txt");

    char c;
    for(int x = 0; x < 5; x++)
        for(int y = 0; y < 5; y++)
        {
            i >> c;
            tablero[x][y] = c;
        }
    i.close();
}

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

SDL_Surface *background = NULL;
SDL_Surface *screen = NULL;
SDL_Surface *camino = NULL;
SDL_Surface *obstaculo = NULL;
SDL_Surface *pared = NULL;
SDL_Surface *caminante = NULL;

SDL_Event event;

TTF_Font *font = NULL;

SDL_Color textColor = { 0, 0, 0 };

SDL_Surface *load_image( std::string filename )
{
    //The image that's loaded
    SDL_Surface* loadedImage = NULL;

    //The optimized surface that will be used
    SDL_Surface* optimizedImage = NULL;

    //Load the image
    loadedImage = IMG_Load( filename.c_str() );

    //If the image loaded
    if( loadedImage != NULL )
    {
        //Create an optimized surface
        optimizedImage = SDL_DisplayFormat( loadedImage );

        //Free the old surface
        SDL_FreeSurface( loadedImage );

        //If the surface was optimized
        if( optimizedImage != NULL )
        {
            //Color key surface
            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, 0, 0xFF, 0xFF ) );
        }
    }

    //Return the optimized surface
    return optimizedImage;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL )
{
    //Holds offsets
    SDL_Rect offset;

    //Get offsets
    offset.x = x;
    offset.y = y;

    //Blit
    SDL_BlitSurface( source, clip, destination, &offset );
}

bool init()
{
    //Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;
    }

    //Set up the screen
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

    //If there was an error in setting up the screen
    if( screen == NULL )
    {
        return false;
    }

    //Initialize SDL_ttf
    if( TTF_Init() == -1 )
    {
        return false;
    }

    //Set the window caption
    SDL_WM_SetCaption( "Laberinto", NULL );

    //If everything initialized fine
    return true;
}

bool load_files()
{
    background = load_image( "background.png" );
    pared = load_image("pared.png");
    camino = load_image("camino.png");
    obstaculo = load_image("obstaculo.png");
    caminante = IMG_Load("caminante.png");

    font = TTF_OpenFont( "lazy.ttf", 72 );

    //If there was a problem in loading the background
    if( background == NULL )
    {
        return false;
    }

    //If there was an error in loading the font
    if( font == NULL )
    {
        return false;
    }

    //If everything loaded fine
    return true;
}

void clean_up()
{
    //Free the surfaces
    SDL_FreeSurface( background );
    SDL_FreeSurface( pared );
    SDL_FreeSurface( camino );
    SDL_FreeSurface( obstaculo );
    SDL_FreeSurface( caminante );

    //Close the font
    TTF_CloseFont( font );

    //Quit SDL_ttf
    TTF_Quit();

    //Quit SDL
    SDL_Quit();
}

void imprimirTablero()
{
    for(int x = 0; x<5; x++)
            for(int y = 0; y<5; y++)
        {
            if(tablero[y][x] == '#')
            {
                apply_surface(x*64,y*64,pared,screen);
            }

            if(tablero[y][x] == '0')
            {
                apply_surface(x*64,y*64,obstaculo,screen);
            }

            if(tablero[y][x] == '-')
            {
                apply_surface(x*64,y*64,camino,screen);
            }
        }
}

bool puedoLLegar(int x_inicio, int y_inicio, int x_final, int y_final, char tablero[5][5], int pasos)
{
    imprimirTablero();

    SDL_Flip(screen);

    if(pasos < 0)
    {
        return false;
    }

    if(x_inicio<0 || x_inicio>=5 || y_inicio >= 5 || y_inicio<0)
    {
        return false;
    }

    if(x_final<0 || x_final>=5 || y_final >= 5 || y_final<0)
    {
        return false;
    }

    if(tablero[y_final][x_final] == '#' || tablero[y_inicio][x_inicio]== '#')
    {
        return false;
    }

    if(x_inicio == x_final && y_inicio == y_final)
    {
        apply_surface(x_inicio*64, y_inicio*64, caminante, screen);
        return true;
    }

    if(puedoLLegar(x_inicio+1, y_inicio, x_final, y_final, tablero, pasos-1))
    {
        apply_surface(x_inicio*64, y_inicio*64, caminante, screen);
        return true;
    }

    if(puedoLLegar(x_inicio, y_inicio+1, x_final, y_final, tablero, pasos-1))
    {
        apply_surface(x_inicio*64, y_inicio*64, caminante, screen);
        return true;
    }

    if(puedoLLegar(x_inicio-1, y_inicio, x_final, y_final, tablero, pasos-1))
    {
        apply_surface(x_inicio*64, y_inicio*64, caminante, screen);
        return true;
    }

    if(puedoLLegar(x_inicio, y_inicio-1, x_final, y_final, tablero, pasos-1))
    {
        apply_surface(x_inicio*64, y_inicio*64, caminante, screen);
        return true;
    }
        return false;
}

int main( int argc, char* args[] )
{
    //Quit flag
    bool quit = false;

    //Initialize
    if( init() == false )
    {
        return 1;
    }

    //Load the files
    if( load_files() == false )
    {
        return 1;
    }
    initTablero();

    //Generate the message surfaces
    //Apply the background
    apply_surface( 0, 0, background, screen );

    puedoLLegar(0,0,4,0,tablero,8);
    SDL_Flip(screen);
    SDL_Delay(1000);
    //Clean up
    clean_up();

    return 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * 
 * Autor: Stefano Vidaurre Olite                   *
 * Fecha: 24/11/2016                               *
 * Descripcion: Libreria encargada de la gestion de*
 * los graficos, renderizacion, y transmision de   *
 * los frames generados, al TFT mediante SPI.      *
 *                                                 *
 * V: 1.1.6                                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __GPUCORE_H__
#define	__GPUCORE_H__

#include <stdint.h>
#include <stdlib.h>
#include "../IOCore/IOCore.h"
#include <xc.h>

#define GPU_TILES_SIZE 60
#define GPU_PIXELS_SIZE_I 4
#define GPU_PIXELS_SIZE_J 8
#define GPU_PALETTES_SIZE 8
#define GPU_COLORS_SIZE 16
#define GPU_SPRITES_SIZE 256
#define GPU_WIDTH 320
#define GPU_HEIGHT 240
#define GPU_TILES_Y 30
#define GPU_TILES_X 40



typedef struct GPU *GPU;
typedef struct Pixel Pixel;
typedef struct Tile *Tile;
typedef struct Palette *Palette;
typedef struct TableDrawables TableDrawables;
typedef struct Drawable Drawable;
typedef struct InfoDraw InfoDraw;
typedef struct InfoMove InfoMove;
typedef struct InfoAnim InfoAnim;
typedef struct Map Map;

//===== Pixel =====
// Clase usada para guardar la informacion de dos pixels, uno situado (color2)
// inmediatamente debajo del otro (color1)
struct Pixel{
    uint8_t color1 : 4; // Color del pixel superior
    uint8_t color2 : 4; // Color del pixel inferior
};

//===== Tile =====
// Clase usada para dibujar un mapa de pixels. Es la unidad minima de dibujo
// que permite el motor grafico, la cual requiera una renderizacion. Permite
// definir tanto fondos, como sprites o baldosas estaticas
struct Tile{
    Pixel pixels[GPU_PIXELS_SIZE_I][GPU_PIXELS_SIZE_J]; // Matriz de pixeles
};

//===== Palette =====
// Clase usada para proporcionar un ahorro significativo de memoria. Permitiendo
// acceder a toda la paleta de colores de 16bits con tan solo 4bits. Cada paletta
// por defecto puede almacenar 16 colores.
struct Palette{
    uint16_t colors[GPU_COLORS_SIZE];
};

//===== Map =====
// Clase usada para posicionar las baldosas en un plano 2D. Nos permite saber
// que baldosa va en cada lugar en todo momento. Tambien nos permite movernos
// a lo largo de un mapa mas grande que la pantalla gracias al puntero, el 
// cual nos indica que posicion empezamos a visualizar
struct Map{
    uint16_t height; //Altura del mapa (Se usa para leer el array)
    uint16_t width; //Anchura del mapa (Se usa para leer el array)
    int32_t pY; //Indica donde empieza a leer en Y (Son necesarios los negativos para el scroll)
    int32_t pX; //Indica donde empueza a leer en X (Son necesarios los negativos para el scroll)
    uint8_t *tilesMap; //Puntero que contiene el mapa de baldosas
};

//===== InfoDraw =====
struct InfoDraw{
    uint8_t back;        // Posicion de la baldosa de fondo
    uint8_t palette : 3; // Paleta seleccionada para esta baldosa[0-7]
    uint8_t visible : 1;  // Nos indica si hay que dibujarlo o no
};

//===== InfoMove =====
struct InfoMove{
    uint16_t sY;         // Posicion en el eje y en el tilesMap
    uint16_t sX;         // Posicion en el eje x en el tilesMap
    uint8_t step : 1;    // Nos indica si se puede pisar dicha baldosa o no
    uint8_t sprite : 1;  // Nos indica si es una baldosa estatica o movible
};

//===== InfoAnim =====
struct InfoAnim{
    uint8_t ini;     // Baldosa inicial del ciclo de animacion       
    uint8_t view;    // Baldosa que se esta viendo actualmente
    uint8_t *timer;  // Velocidad de la animacion
    uint8_t *next;   // Baldosa a la que pasa dentro del sprite
    uint8_t *tile;
};

//===== Drawable =====
// Clase usada para administrar las baldosas que son dinamicas, Sprites y fondos
// dinamicos. Se encarga de organizarlos para facilitar la renderizacion dinamica,
// dado que al no ser staticos en el tiempo hay que hacer modificaciones en el 
// tilesMap.
struct Drawable{
    Tile tile; //Puntero hacia la baldosa que se muestra
    InfoDraw infoDraw;
    InfoMove infoMove;
    InfoAnim infoAnim;
};

//===== TableDrawables =====
// Clase que almacena el conjunto de objetos dibujables existentes asi como del
// tiempo global usado para las animaciones pasivas.
struct TableDrawables{
    Drawable drawables[GPU_SPRITES_SIZE];
    uint8_t length;
    uint8_t globalTimer;
};

//===== GPU =====
// Clase principal que engloba a todas las demas. Esta clase es la unica que se debe de
// manejar mas haya de los constructores de Tile y Palette. La api proporciona todas
// las posibilidades de la GPU usando unicamente esta clase, de esta forma obtenemos
// una buena encapsulacion.
struct GPU{
    struct Tile tiles[GPU_TILES_SIZE];
    struct Palette palettes[GPU_PALETTES_SIZE];
    TableDrawables tableDrawables;
    Map map;
};

GPU GPU_new();
void GPU_init();
Tile GPU_newTile(uint8_t pixels[GPU_PIXELS_SIZE_I][GPU_PIXELS_SIZE_J]);
Palette GPU_newPalette(uint8_t colors[GPU_COLORS_SIZE][3]);

void GPU_loadMap(uint8_t *tilesMap, uint16_t height, uint16_t width);
void GPU_loadTile(Tile *tiles, uint8_t ini, uint8_t length);
void GPU_loadPalette(Palette *palettes, uint8_t ini, uint8_t length);
uint16_t GPU_RGB(uint8_t r, uint8_t g, uint8_t b);

void GPU_black();
uint8_t GPU_scroll(int pX, int pY);
void GPU_draw();

void GPU_setTile(uint16_t y, uint16_t x, uint8_t tile);

uint8_t GPU_addSprite(uint8_t ini, uint8_t length, uint16_t sX, uint16_t sY);
uint8_t GPU_addStatic(uint8_t ini, uint16_t length, uint8_t timer, uint8_t back, uint8_t step);
uint8_t GPU_spriteMove(uint8_t idA, uint16_t sX, uint16_t sY);
uint8_t GPU_spriteNext(uint8_t sprite);
uint8_t GPU_reorderAnimation(uint8_t draw, uint8_t tile, uint8_t next);

#endif	/* GPUCORE_H */


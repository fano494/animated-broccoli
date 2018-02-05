#include "HALCore.h"

typedef void* (*Func)();
Func fun;

void newTile(uint8_t index, uint8_t pixels[GPU_PIXELS_SIZE_I][GPU_PIXELS_SIZE_J]){
    GPU_newTile(index, pixels);
}

void newPalette(uint8_t index, uint8_t colors[GPU_COLORS_SIZE][3]){
    GPU_newPalette(index, colors);
}

void loadMap(uint8_t *tilesMap, uint16_t height, uint16_t width){
    GPU_loadMap(tilesMap, height, width);
}

uint16_t RGB(uint8_t r, uint8_t g, uint8_t b){
    return GPU_RGB(r,g,b);
}

void black(){
    GPU_black();
}

uint8_t scroll(int pX, int pY){
    return GPU_scroll(pX, pY);
}

void draw(){
    GPU_draw();
}

void run(){
    GPU_run();
}

uint8_t addSprite(uint8_t ini, uint8_t length, uint8_t step){
    return GPU_addSprite(ini, length, step);
}

uint8_t addStatic(uint8_t ini, uint16_t length, uint8_t timer, uint8_t back, uint8_t step){
    return GPU_addStatic(ini, length, timer, back, step);
}

uint8_t spriteMove(uint8_t idA, uint16_t sX, uint16_t sY){
    return GPU_spriteMove(idA, sX, sY);
}

uint8_t staticSet(uint8_t idA, uint16_t sX, uint16_t sY){
    return GPU_staticSet(idA, sX, sY);
}

uint8_t spriteNext(uint8_t sprite){
    return GPU_spriteNext(sprite);
}

uint8_t reorderAnimation(uint8_t draw, uint8_t tile, uint8_t next){
    return GPU_reorderAnimation(draw, tile, next);
}

void init(){
    IO_init();
    GPU_init();
}

uint8_t HAL_button_0(){
    return EVENT_BUTTON_0;
}

uint8_t HAL_button_1(){
    return EVENT_BUTTON_1;
}

uint8_t HAL_button_2(){
    return EVENT_BUTTON_2;
}

uint8_t HAL_button_3(){
    return EVENT_BUTTON_3;
}

uint8_t HAL_button_INT(){
    return EVENT_BUTTON_INT;
}

void loadFont(){
    GPU_loadFont();
}

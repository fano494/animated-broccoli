/* 
 * File:   HALCore.h
 * Author: Fano
 *
 * Created on 21 de marzo de 2017, 0:51
 */

#ifndef HALCORE_H
#define	HALCORE_H

#include "../GPUCore/GPUCore.h"

#define BUTTON_0 HAL_button_0()
#define BUTTON_1 HAL_button_1()
#define BUTTON_2 HAL_button_2()
#define BUTTON_3 HAL_button_3()
#define BUTTON_INT HAL_button_4()

Tile __attribute__((address(0x9D001000))) newTile(uint8_t pixels[GPU_PIXELS_SIZE_I][GPU_PIXELS_SIZE_J]);

Palette __attribute__((address(0x9D001100))) newPalette(uint8_t colors[GPU_COLORS_SIZE][3]);

void __attribute__((address(0x9D001200))) loadMap(uint8_t *tilesMap, uint16_t height, uint16_t width);

void __attribute__((address(0x9D001300))) loadTile(Tile *tiles, uint8_t ini, uint8_t length);

void __attribute__((address(0x9D001400))) loadPalette(Palette *palettes, uint8_t ini, uint8_t length);

uint16_t __attribute__((address(0x9D001500))) RGB(uint8_t r, uint8_t g, uint8_t b);

void __attribute__((address(0x9D001600))) black();

uint8_t __attribute__((address(0x9D001700))) scroll(int pX, int pY);

void __attribute__((address(0x9D001800))) draw();

void __attribute__((address(0x9D001900))) run();

uint8_t __attribute__((address(0x9D001A00))) addSprite(uint8_t ini, uint8_t length, uint8_t step);

uint8_t __attribute__((address(0x9D001B00))) addStatic(uint8_t ini, uint16_t length, uint8_t timer, uint8_t back, uint8_t step);

uint8_t __attribute__((address(0x9D001C00))) spriteMove(uint8_t idA, uint16_t sX, uint16_t sY);

uint8_t __attribute__((address(0x9D001D00))) staticSet(uint8_t idA, uint16_t sX, uint16_t sY);

uint8_t __attribute__((address(0x9D001E00))) spriteNext(uint8_t sprite);

uint8_t __attribute__((address(0x9D001F00))) reorderAnimation(uint8_t draw, uint8_t tile, uint8_t next);

void __attribute__((address(0x9D002000))) init();

uint8_t __attribute__((address(0x9D002100))) HAL_button_0();

uint8_t __attribute__((address(0x9D002200))) HAL_button_1();

uint8_t __attribute__((address(0x9D002300))) HAL_button_2();

uint8_t __attribute__((address(0x9D002400))) HAL_button_3();

uint8_t __attribute__((address(0x9D002500))) HAL_button_INT();

// GPU_controller()##### ABSTRACT #####
// Ent: --
// Sal: --
// Des: Funcion abstracta en la que incluiremos la logica del programa. Usar solo si se 
// esta programando mediante el modelo gestionado por GPU_run
void __attribute__((section(".funcdin"))) controller();

#endif	/* HALCORE_H */


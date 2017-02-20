#include "GPUCore.h"

//-----  Macros de seleccion de modo -----//
#define GPU_TFT_SELECT GPU_PIN_TFT_CS=0               //Activa CS del TFT
#define GPU_TFT_DESELECT GPU_PIN_TFT_CS=1             //Desactiva CS del TFT
#define GPU_TFT_CMD GPU_PIN_TFT_DC=0                  //Indica al TFT que se le pasa comando
#define GPU_TFT_DATA GPU_PIN_TFT_DC=1                 //Indoca al TFT que se le pasa datos

//----- Macros de configuracion del TFT -----// 

//--- Encendido y apagado ---//
#define GPU_TFT_OFF 0x0028    //Apaga la pantalla
#define GPU_TFT_ON  0x0029    //Enciende la pantalla


//--- Comando vacio ---//
#define GPU_TFT_NULL     0x0000   //No causa ningun efecto

//--- Reiniciar configuracion ---//
#define GPU_TFT_CONF_RESET 0x0001 //Reinicia la configuracion del TFT

//--- Modos del TFT ---//
#define GPU_TFT_SLEEP_IN   0x0010     //Modo sleep encendido
#define GPU_TFT_SLEEP_OUT  0x0011     //Modo sleep apagado
#define GPU_TFT_NORMAL     0x0013     //Activa modo normal (toda la pantalla)

//--- Lectura y Escritura de la RAM ---//
#define GPU_TFT_COL_SET   0x002A  //Seleccion de entre que dos columnas de la RAM actuaremos
#define GPU_TFT_RAW_SET   0x002B  //Seleccion de entre que dos filas de la RAM actuaremos
#define GPU_TFT_RAM_WR   0x002C   //Opcion de escritura


//--- Control de escritura RAM ---//
#define GPU_TFT_MEM_CTRL  0x0036  //Configurar metodo de escritura de memoria
#define GPU_TFT_WR_IN  0x2000 //Cambia filas por columnas
#define GPU_TFT_WR_RGB 0x0800 // Modo color RGB

//--- Tamaño de cada pixel ---//
#define GPU_TFT_PIX_SIZE    0x003A //Tamaño del pixel
#define GPU_TFT_PIX_16      0x5500 //16 bit por pixel

#define GPU_TFT_FRMCTR1 0x00B1
#define GPU_TFT_PWCTR1  0x00C0
#define GPU_TFT_PWCTR2  0x00C1
#define GPU_TFT_PWCTR3  0x00C2
#define GPU_TFT_VMCTR1  0x00C5
#define GPU_TFT_VMCTR2  0x00C7



//==============================================================================
//========================== Funciones privadas ================================
//==============================================================================


uint32_t GPU_SPI_write(uint32_t data){
    SPI1BUF = data;
    while(SPI1STATbits.SPIBUSY != 0);
    return SPI1BUF;
}

void GPU_writeCmd(uint32_t cmd){
    GPU_TFT_CMD;
    GPU_TFT_SELECT;
    GPU_SPI_write(cmd);
    GPU_TFT_DESELECT;
}

void GPU_writeData(uint32_t data){
    GPU_TFT_DATA;
    GPU_TFT_SELECT;
    GPU_SPI_write(data);
    GPU_TFT_DESELECT;
}

void GPU_TFT_manufacturerCMD(){
    // Los siguientes comandos no estan documentados por el fabricante, pero
    // indica que debemos incluirlos en cualquier configuracion inicial
    // para el funcionamiento correcto del TFT
    GPU_writeCmd(0x00EF);
    GPU_writeData(0x0380);
    GPU_writeData(0x0200);

    GPU_writeCmd(0x00CF);  
    GPU_writeData(0x00C1);
    GPU_writeData(0X3000); 

    GPU_writeCmd(0x00ED);  
    GPU_writeData(0x6403);  
    GPU_writeData(0X1281); 

    GPU_writeCmd(0x00E8);  
    GPU_writeData(0x8500); 
    GPU_writeData(0x7800); 

    GPU_writeCmd(0x00CB);  
    GPU_writeData(0x392C); 
    GPU_writeData(0x0034); 
    GPU_writeData(0x0200); 

    GPU_writeCmd(0x00F7);  
    GPU_writeData(0x2000); 

    GPU_writeCmd(0x00EA);  
    GPU_writeData(0x0000); 
    
    GPU_writeCmd(0x00B6);    // Display Function Control 
    GPU_writeData(0x0A82); 
    GPU_writeData(0x2700);
    
}

void GPU_TFT_init(){
    GPU_TFT_CS_ENABLE;
    GPU_TFT_DC_ENABLE;
    GPU_TFT_DESELECT;
    SPI1CONbits.MODE32 = 0; 
    
    GPU_writeCmd(GPU_TFT_SLEEP_IN);
     
    GPU_writeCmd(GPU_TFT_CONF_RESET);
    GPU_writeCmd(GPU_TFT_NORMAL);
    GPU_TFT_manufacturerCMD();       //Configuracion inicial dada por el fabricante
    
    GPU_writeCmd(GPU_TFT_PWCTR1);    //Power control 
    GPU_writeData(0x2300);   //VRH[5:0] 

    GPU_writeCmd(GPU_TFT_PWCTR2);    //Power control 
    GPU_writeData(0x0000);   //SAP[2:0];BT[3:0] 
    
    GPU_writeCmd(GPU_TFT_PWCTR3);
    GPU_writeData(0xB200);

    GPU_writeCmd(GPU_TFT_VMCTR1);    //VCM control 
    GPU_writeData(0x3e28); 

    GPU_writeCmd(GPU_TFT_VMCTR2);    //VCM control2 
    GPU_writeData(0x8600);  

    GPU_writeCmd(GPU_TFT_MEM_CTRL); 
    GPU_writeData(GPU_TFT_WR_RGB);

    GPU_writeCmd(GPU_TFT_PIX_SIZE);    
    GPU_writeData(GPU_TFT_PIX_16); 

    GPU_writeCmd(GPU_TFT_FRMCTR1);    
    GPU_writeData(0x001f);   
    
    GPU_writeCmd(0x00F6);
    GPU_writeData(0x0100);
    GPU_writeData(0x0000);
    GPU_writeCmd(GPU_TFT_SLEEP_OUT);	
    GPU_writeCmd(GPU_TFT_ON);
    
    
    GPU_writeCmd(GPU_TFT_COL_SET);
    GPU_writeData(0x0000);
    GPU_writeData(GPU_HEIGHT-1);
    GPU_writeCmd(GPU_TFT_RAW_SET);
    GPU_writeData(0x0000);
    GPU_writeData(GPU_WIDTH-1);
    
    SPI1CONbits.MODE32 = 1; 
}

void GPU_SPI_init(){
    int rData;
    
    IEC0bits.T5IE = 0;   // Apagamos el timer5/error, se usan para el control de 
    IEC0bits.IC5EIE = 0; // interrupciones por lectura, escritura o error
    SPI1CON = 0;         // Reiniciamos la configuracion de SPI1 y lo apagamos
    rData = SPI1BUF;     // Vaciamos el buffer de SPI1
    IFS0bits.IC5IF = 0;  // Limpiamos el flag del timer5
    IFS0bits.IC5EIF = 0; // Limpiamos el flag del timer5 error
    IPC5bits.IC5IP = 3;  // Indicamos una prioridad de 3 para el timer 5
    IPC5bits.IC5IS = 1;  // Indicamos una subprioridad de 1 para el timer 5
    //---- TENER CUIDADO CON LAS PRIORIDADES, DADO QUE EL SISTEMA PUEDE ESTAR ----//
    //----------- MANEJANDO MAS DE UNA INTERRUPCION DE FORMA SIMULTANEA ----------//
    IEC0bits.T5IE = 1;   // Encendemos el timer5 y timer5 error una vez 
    IEC0bits.IC5EIE = 1; // configurados
    SPI1BRG = 0x00;      // Elimina cualquier divisor de la frecuencia de reloj SPI1
    SPI1STATbits.SPIROV = 0; // Limpiamos el flag de overflow de SPI1
    SDI1_ENABLE;         // Activamos el pin de MISO
    SDO1_ENABLE;         // Activamos el pin de MOSI
    SPI1CONbits.ENHBUF = 1; // Activar modo mejorado de buffer, (colas de tx y rx)
    SPI1CONbits.MODE32 = 1; // Transmision de bloques de 32bits
    SPI1CONbits.MODE16 = 1; // Transmision de bloques de 16bit, no activa pero si preparada
    SPI1CONbits.SMP = 1; // Muestrear el dato de entrada despues de enviar el dato de salida
    SPI1CONbits.CKE = 1; // Se considera pulso el paso de activado a desactivado
    SPI1CONbits.CKP = 0; // CKP == 0 -> Activo = HIGH | Inactivo = LOW
    SPI1CONbits.MSTEN = 1; // Modo master
    SPI1CONbits.ON = 1; // Encendemos el modulo SPI1
    
}

void GPU_spritesRender(TableDrawables *table, Map *map, Tile tiles){
    uint8_t i;
    for(i = 0; i < table->length; i++){
        Drawable *curr = &(table->drawables[i]);
        if(curr->infoDraw.visible){
            if(curr->infoAnim.timer[curr->infoAnim.view]){
                
                if(table->globalTimer % curr->infoAnim.timer[curr->infoAnim.view] == 0){
                    uint8_t next = curr->infoAnim.next[curr->infoAnim.view];
                    curr->tile = &(tiles[curr->infoAnim.tile[next]]);
                    curr->infoAnim.view = next;
                }
            }
            if(curr->infoMove.sprite)
                map->tilesMap[curr->infoMove.sY * map->width + curr->infoMove.sX] = i;
                
        }
    }
    table->globalTimer++;
}

void GPU_spritesRenderClear(TableDrawables *table, Map *map){
    uint8_t i;
    for(i = 0; i < table->length; i++){
        Drawable *curr = &(table->drawables[i]);
        if(curr->infoMove.sprite)
            map->tilesMap[curr->infoMove.sY * map->width + curr->infoMove.sX] = curr->infoDraw.back;
    }
}

void GPU_init(){
    GPU_SPI_init();
    GPU_TFT_init();
    
    
    GPU_black();
    GPU_black();
    GPU_black();
    GPU_black(); 
}


//==============================================================================
//========================== Funciones publicas ================================
//==============================================================================

//------------------------------ Constructores ---------------------------------

GPU GPU_new(){
    uint8_t i,j,k;
    
    GPU gpu = (GPU) malloc(sizeof(struct GPU));
    gpu->tableDrawables.length = 0;
    gpu->tableDrawables.globalTimer = 0;
    
    for(k = 0; k <= GPU_TILES_SIZE - 1; k++)
        for(i = 0; i < GPU_PIXELS_SIZE_I; i++)
            for(j = 0; j < GPU_PIXELS_SIZE_J; j++){
                gpu->tiles[k].pixels[i][j].color1 = 0;
                gpu->tiles[k].pixels[i][j].color2 = 0;
            }
    
    for(k = 0; k < GPU_PALETTES_SIZE; k++)
        for(i = 0; i < GPU_COLORS_SIZE; i++)
            gpu->palettes[k].colors[i] = 0;
    
    GPU_init();
    return gpu;
}

Tile GPU_newTile(uint8_t pixels[GPU_PIXELS_SIZE_I][GPU_PIXELS_SIZE_J]){
    uint8_t i,j;
    Tile tile = (Tile) malloc(sizeof(struct Tile));
    
    for(i = 0; i < GPU_PIXELS_SIZE_I; i++)
        for(j = 0; j < GPU_PIXELS_SIZE_J; j++){
            tile->pixels[i][j].color1 = pixels[i][j] >> 4;
            tile->pixels[i][j].color2 = pixels[i][j] & 0x0f;
        }
    
    return tile;
}

Palette GPU_newPalette(uint8_t colors[GPU_COLORS_SIZE][3]){
    uint8_t i;
    Palette palette = (Palette) malloc(sizeof(struct Palette));
    
    for(i = 0; i < GPU_COLORS_SIZE; i++)
        palette->colors[i] = GPU_RGB(colors[i][0], colors[i][1], colors[i][2]);
    
    return palette;
}

//------------------------------------------------------------------------------



//------------------------------- Cargadores -----------------------------------

uint16_t GPU_RGB(uint8_t r, uint8_t g, uint8_t b){
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void GPU_loadMap(GPU gpu, uint8_t *tilesMap, uint16_t height, uint16_t width){
    if(gpu->map.tilesMap != NULL)
        free(gpu->map.tilesMap);
    gpu->map.tilesMap = tilesMap;
    gpu->map.height = height;
    gpu->map.width = width;
    gpu->map.pX = 0;
    gpu->map.pY = 0;
}

void GPU_loadTile(GPU gpu, Tile *tiles, uint8_t ini, uint8_t length){
    uint8_t i;
    
    for(i = 0; i < length; i++){
        gpu->tiles[i + ini] = *(tiles[i]);
        free(tiles[i]);
        tiles[i] = &gpu->tiles[i + ini];
    }
}

void GPU_loadPalette(GPU gpu, Palette *palettes, uint8_t ini, uint8_t length){
    uint8_t i;
    
    for(i = 0; i < length; i++){
        gpu->palettes[i + ini] = *(palettes[i]);
        free(palettes[i]);
        palettes[i] = &gpu->palettes[i + ini];
    }
}

//------------------------------------------------------------------------------



//----------------------------- Funciones mapa ---------------------------------

void GPU_black(){
    uint16_t i,j;
    SPI1CONbits.MODE32 = 0; // Transmision de bloques de 32bits
    GPU_writeCmd(0x002C);
    
    GPU_TFT_DATA;
    GPU_TFT_SELECT;
    
    for(i = 0; i < GPU_HEIGHT; i++){
        for(j = 0; j < GPU_WIDTH; j++){
                SPI1BUF = 0x0000;
                while(SPI1STATbits.SPITBE == 0);
        } 
    }
    while(SPI1STATbits.SPIBUSY != 0);
    SPI1CONbits.MODE32 = 1; // Transmision de bloques de 32bits
    GPU_TFT_DESELECT;
}

void GPU_scroll(GPU gpu, int pX, int pY){
    gpu->map.pX += pX;
    gpu->map.pY += pY;
    
    if(gpu->map.pX < 0)
        gpu->map.pX = 0;
    else if(gpu->map.pX > gpu->map.width - GPU_TILES_X)
            gpu->map.pX = gpu->map.width - GPU_TILES_X;
    
    if(gpu->map.pY < 0)
        gpu->map.pY = 0;
    else if(gpu->map.pY > gpu->map.height - GPU_TILES_Y)
            gpu->map.pY = gpu->map.height - GPU_TILES_Y;

}

void GPU_draw(GPU gpu){
    uint8_t q;
    uint16_t i, k;
    Pixel pixels1, pixels2, pixels3, pixels4;
    Tile tile, back;
    Palette p;
    int32_t pX = gpu->map.pX;
    int32_t pY = gpu->map.pY;
    
    uint16_t kFin = pX + GPU_TILES_X;
    uint16_t iFin = pY + GPU_TILES_Y;
    uint32_t salt = GPU_TILES_X * GPU_TILES_Y;
    
    GPU_spritesRender(&gpu->tableDrawables, &gpu->map, gpu->tiles);
    SPI1CONbits.MODE32 = 0;
    GPU_writeCmd(GPU_TFT_RAM_WR);
    SPI1CONbits.MODE32 = 1;
    GPU_TFT_DATA;
    GPU_TFT_SELECT;

    uint8_t *nextTile = gpu->map.tilesMap + (pY * gpu->map.width + pX);
    uint8_t currentTile = *(nextTile)+1;
    for(k = pX; k < kFin; k++){
        for(q = 0; q < GPU_PIXELS_SIZE_J; q++){
            for(i = pY; i < iFin; i++){
                if(*(nextTile) != currentTile){
                    currentTile = *(nextTile);
                    tile = gpu->tableDrawables.drawables[currentTile].tile;
                    p = gpu->palettes + gpu->tableDrawables.drawables[currentTile].infoDraw.palette;
                    back = gpu->tableDrawables.drawables[gpu->tableDrawables.drawables[currentTile].infoDraw.back].tile; 

                    //=== COLORES 0-1 ===
                    pixels1 = tile->pixels[0][q];
                    if(!pixels1.color1)
                        pixels1.color1 = back->pixels[0][q].color1;
                    if(!pixels1.color2)
                        pixels1.color2 = back->pixels[0][q].color2;

                    //=== COLORES 2-3 ===
                    pixels2 = tile->pixels[1][q];
                    if(!pixels2.color1)
                        pixels2.color1 = back->pixels[1][q].color1;
                    if(!pixels2.color2)
                        pixels2.color2 = back->pixels[1][q].color2;

                    //=== COLORES 4-5 ===
                    pixels3 = tile->pixels[2][q];
                    if(!pixels3.color1)
                        pixels3.color1 = back->pixels[2][q].color1;
                    if(!pixels3.color2)
                        pixels3.color2 = back->pixels[2][q].color2;

                    //=== COLORES 6-7 ===
                    pixels4 = tile->pixels[3][q];
                    if(!pixels4.color1)
                        pixels4.color1 = back->pixels[3][q].color1;
                    if(!pixels4.color2)
                        pixels4.color2 = back->pixels[3][q].color2;
                }
                nextTile += GPU_TILES_X;
                SPI1BUF = p->colors[pixels1.color1] << 16 | p->colors[pixels1.color2];
                SPI1BUF = p->colors[pixels2.color1] << 16 | p->colors[pixels2.color2];
                SPI1BUF = p->colors[pixels3.color1] << 16 | p->colors[pixels3.color2];
                SPI1BUF = p->colors[pixels4.color1] << 16 | p->colors[pixels4.color2];
                while(SPI1STATbits.SPITBE == 0);
            } 
            nextTile -= salt;
            currentTile = *(nextTile) + 1;
        }
        nextTile++;
    }
    SPI1CONbits.ON = 0;
    SPI1CONbits.ON = 1;
    GPU_TFT_DESELECT;
    GPU_spritesRenderClear(&gpu->tableDrawables, &gpu->map);
}

//------------------------------------------------------------------------------



//--------------------------- Funciones baldosas -------------------------------

uint8_t GPU_spriteMove(GPU gpu, uint8_t idA, uint16_t sX, uint16_t sY){
    uint8_t idB = *(gpu->map.tilesMap + (sY * gpu->map.width + sX));
    Drawable *sprite = &gpu->tableDrawables.drawables[idA];
    Drawable *back = &gpu->tableDrawables.drawables[idB]; 

    if(sprite->infoMove.sprite)
        if(back->infoMove.step){
            gpu->map.tilesMap[sprite->infoMove.sY * gpu->map.width + sprite->infoMove.sX] = sprite->infoDraw.back;
            sprite->infoDraw.back = idB;
            sprite->infoMove.sX = sX;
            sprite->infoMove.sY = sY;
            return 1;
        }
    return 0;
}

void GPU_setTile(GPU gpu, uint16_t y, uint16_t x, uint8_t tile){
    gpu->map.tilesMap[y * gpu->map.width + x] = tile;
}

uint8_t GPU_addSprite(GPU gpu, uint8_t ini, uint8_t length, uint16_t sX, uint16_t sY){
    uint16_t i;
    Drawable *sprite = &(gpu->tableDrawables.drawables[gpu->tableDrawables.length++]);

    sprite->tile = &gpu->tiles[ini];
    sprite->infoAnim.ini = ini;
    sprite->infoDraw.back = gpu->map.tilesMap[sY * gpu->map.width + sX];
    sprite->infoAnim.view = 0;
    sprite->infoMove.sY = sY;
    sprite->infoMove.sX = sX;
    sprite->infoMove.sprite = 1;
    sprite->infoMove.step = 1;
    sprite->infoDraw.palette = 0;
    sprite->infoDraw.visible = 1;
    sprite->infoAnim.next = (uint8_t *) malloc(sizeof(uint8_t) * length);
    for(i = 0; i < length-1; i++)
        sprite->infoAnim.next[i] = i+1;
    sprite->infoAnim.next[i] = 0;
    sprite->infoAnim.tile = (uint8_t *) malloc(sizeof(uint8_t) * length);
    for(i = 0; i < length; i++)
        sprite->infoAnim.tile[i] = ini+i;
    sprite->infoAnim.timer = (uint8_t *) malloc(sizeof(uint8_t) * length);
    for(i = 0; i < length; i++)
        sprite->infoAnim.timer[i] = 0;
    
    return gpu->tableDrawables.length - 1;
}

uint8_t GPU_addStatic(GPU gpu, uint8_t ini, uint16_t length, uint8_t timer, uint8_t back, uint8_t step){
    uint16_t i;
    Drawable *stac = &(gpu->tableDrawables.drawables[gpu->tableDrawables.length++]);

    stac->tile = &gpu->tiles[ini];
    stac->infoAnim.ini = ini;
    stac->infoDraw.back = back;
    stac->infoAnim.view = 0;
    stac->infoMove.sprite = 0;
    stac->infoMove.step = step;
    stac->infoDraw.palette = 0;
    stac->infoDraw.visible = 1;
    stac->infoAnim.next = (uint8_t *) malloc(sizeof(uint8_t) * length);
    for(i = 0; i < length-1; i++)
        stac->infoAnim.next[i] = i+1;
    stac->infoAnim.next[i] = 0;
    stac->infoAnim.tile = (uint8_t *) malloc(sizeof(uint8_t) * length);
    for(i = 0; i < length; i++)
        stac->infoAnim.tile[i] = ini+i;
    stac->infoAnim.timer = (uint8_t *) malloc(sizeof(uint8_t) * length);
    for(i = 0; i < length; i++)
        stac->infoAnim.timer[i] = timer;
    
    return gpu->tableDrawables.length - 1;
}

//------------------------------------------------------------------------------

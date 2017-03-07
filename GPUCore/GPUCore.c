#include "GPUCore.h"

//==============================================================================
//======================= Macros configuracion TFT =============================
//==============================================================================

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

//=== Variable global interna  ===//
GPU __GPU; // Se declara esta variable global interna la cual simplifica el codigo
           // de cara a la API de la GPU, haciendo una encapsulacion mas fuerte y
           // centrando al programador unicamente en implementar el juego y no necesitar
           // tocar la implentacion de la GPU
//================================//


//==============================================================================
//========================== Funciones privadas ================================
//==============================================================================

void GPU_TFT_manufacturerCMD(){
    // Los siguientes comandos no estan documentados por el fabricante, pero
    // indica que debemos incluirlos en cualquier configuracion inicial
    // para el funcionamiento correcto del TFT
    SPI_writeCmd(0x00EF);
    SPI_writeData(0x0380);
    SPI_writeData(0x0200);

    SPI_writeCmd(0x00CF);  
    SPI_writeData(0x00C1);
    SPI_writeData(0X3000); 

    SPI_writeCmd(0x00ED);  
    SPI_writeData(0x6403);  
    SPI_writeData(0X1281); 

    SPI_writeCmd(0x00E8);  
    SPI_writeData(0x8500); 
    SPI_writeData(0x7800); 

    SPI_writeCmd(0x00CB);  
    SPI_writeData(0x392C); 
    SPI_writeData(0x0034); 
    SPI_writeData(0x0200); 

    SPI_writeCmd(0x00F7);  
    SPI_writeData(0x2000); 

    SPI_writeCmd(0x00EA);  
    SPI_writeData(0x0000); 
    
    SPI_writeCmd(0x00B6);    // Display Function Control 
    SPI_writeData(0x0A82); 
    SPI_writeData(0x2700);
    
}

void GPU_TFT_init(){
    SPI_TFT_CS_ENABLE;
    SPI_TFT_DC_ENABLE;
    SPI_TFT_DESELECT;
    
    SPI_writeCmd(GPU_TFT_SLEEP_IN);
     
    SPI_writeCmd(GPU_TFT_CONF_RESET);
    SPI_writeCmd(GPU_TFT_NORMAL);
    GPU_TFT_manufacturerCMD();       //Configuracion inicial dada por el fabricante
    
    SPI_writeCmd(GPU_TFT_PWCTR1);    //Power control 
    SPI_writeData(0x2300);   //VRH[5:0] 

    SPI_writeCmd(GPU_TFT_PWCTR2);    //Power control 
    SPI_writeData(0x0000);   //SAP[2:0];BT[3:0] 
    
    SPI_writeCmd(GPU_TFT_PWCTR3);
    SPI_writeData(0xB200);

    SPI_writeCmd(GPU_TFT_VMCTR1);    //VCM control 
    SPI_writeData(0x3e28); 

    SPI_writeCmd(GPU_TFT_VMCTR2);    //VCM control2 
    SPI_writeData(0x8600);  

    SPI_writeCmd(GPU_TFT_MEM_CTRL); 
    SPI_writeData(GPU_TFT_WR_RGB);

    SPI_writeCmd(GPU_TFT_PIX_SIZE);    
    SPI_writeData(GPU_TFT_PIX_16); 

    SPI_writeCmd(GPU_TFT_FRMCTR1);    
    SPI_writeData(0x001f);   
    
    SPI_writeCmd(0x00F6);
    SPI_writeData(0x0100);
    SPI_writeData(0x0000);
    SPI_writeCmd(GPU_TFT_SLEEP_OUT);	
    SPI_writeCmd(GPU_TFT_ON);
    
    
    SPI_writeCmd(GPU_TFT_COL_SET);
    SPI_writeData(0x0000);
    SPI_writeData(GPU_HEIGHT-1);
    SPI_writeCmd(GPU_TFT_RAW_SET);
    SPI_writeData(0x0000);
    SPI_writeData(GPU_WIDTH-1);
}

void GPU_drawablesRender(TableDrawables *table, Tile tiles){
    uint8_t i;
    for(i = 0; i < table->length; i++){ // Recorremos la tabla para renderizar todos los dibujables antes de dibujar la pantalla
        Drawable *curr = &(table->drawables[i]);
        
        if(curr->infoAnim.timer[curr->infoAnim.view]){ // Combrueba si el timer esta activo (timer > 0)
                
            if(table->globalTimer % curr->infoAnim.timer[curr->infoAnim.view] == 0){ // Combrueba si es necesario un cambio pasivo
                uint8_t next = curr->infoAnim.next[curr->infoAnim.view];  // Obtiene cual es la siguiente baldosa
                curr->tile = &(tiles[curr->infoAnim.tile[next]]);  // La busca en memoria y se la pasa al puntero del dibujable
                curr->infoAnim.view = next; // Modifica el indice para saber cual se esta viendo ahora
            }
        }/*
        else{
            if(curr->infoMove.sprite){
                if(map->tilesMap[curr->infoMove.sY * map->width + curr->infoMove.sX] == i)
            }
        }*/
    }
    table->globalTimer++;
}

void GPU_init(){
    GPU_TFT_init();
    __GPU = GPU_new();
    
    
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
    // Inicializa un driver para la gpu, reserva la memoria minima para poder usarla
    // y prepara algunos de las sub clases de las que se compone.
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

void GPU_loadMap(uint8_t *tilesMap, uint16_t height, uint16_t width){
    __GPU->map.tilesMap = tilesMap;
    __GPU->map.height = height;
    __GPU->map.width = width;
    __GPU->map.pX = 0;
    __GPU->map.pY = 0;
}

void GPU_loadTile(Tile *tiles, uint8_t ini, uint8_t length){
    uint8_t i;
    
    for(i = 0; i < length; i++){
        __GPU->tiles[i + ini] = *(tiles[i]);
        free(tiles[i]);
        tiles[i] = &__GPU->tiles[i + ini];
    }
}

void GPU_loadPalette(Palette *palettes, uint8_t ini, uint8_t length){
    uint8_t i;
    
    for(i = 0; i < length; i++){
        __GPU->palettes[i + ini] = *(palettes[i]);
        free(palettes[i]);
        palettes[i] = &__GPU->palettes[i + ini];
    }
}

//------------------------------------------------------------------------------



//----------------------------- Funciones mapa ---------------------------------

void GPU_black(){
    uint16_t i,j;

    SPI_writeCmd(GPU_TFT_RAM_WR);
    SPI_MODE_32;
    SPI_TFT_DATA;
    SPI_TFT_SELECT;
    
    for(i = 0; i < GPU_HEIGHT/2; i++){
        for(j = 0; j < GPU_WIDTH; j++){
                SPI1BUF = 0x00000000;
                while(SPI1STATbits.SPITBE == 0);
        } 
    }
    while(SPI1STATbits.SPIBUSY != 0);

    SPI_TFT_DESELECT;
}

uint8_t GPU_scroll(int pX, int pY){
    __GPU->map.pX += pX;
    __GPU->map.pY += pY;
    
    // Comprueba que no nos salimos de los limites del mapa
    if(__GPU->map.pX < 0){
        __GPU->map.pX = 0;
        return 0;
    }
    else if(__GPU->map.pX > __GPU->map.width - GPU_TILES_X){
        __GPU->map.pX = __GPU->map.width - GPU_TILES_X;
        return 0;
    }
    if(__GPU->map.pY < 0){
        __GPU->map.pY = 0;
        return 0;
    }
    else if(__GPU->map.pY > __GPU->map.height - GPU_TILES_Y){
        __GPU->map.pY = __GPU->map.height - GPU_TILES_Y;
        return 0;
    }
    return 1;

}

void GPU_draw(){
    uint8_t q;
    uint16_t i, k;
    Pixel pixels1, pixels2, pixels3, pixels4;
    Tile tile, back;
    Palette p;
    int32_t pX = __GPU->map.pX; // Cargamos los punteros de scroll en varibles 
    int32_t pY = __GPU->map.pY; // auxiliares para evitar conflictos con un scroll
                              // mediante interrupcion
    uint16_t kFin = pX + GPU_TILES_X;
    uint16_t iFin = pY + GPU_TILES_Y;
    uint32_t salt = __GPU->map.width * GPU_TILES_Y; // variable auxiliar que se usa para
                                               // retroceder el puntero de lectura del mapa

    // Preparamos el TFT para ser escrito
    SPI_writeCmd(GPU_TFT_RAM_WR);
    SPI_MODE_32;
    SPI_TFT_DATA;
    SPI_TFT_SELECT;
    
    // Buscamos la primera baldosa visible del mapa
    uint8_t *nextTile = __GPU->map.tilesMap + (pY * __GPU->map.width + pX);
    uint8_t currentTile = *(nextTile)+1; // Esta variable se usa para comprobar si la baldosa
                                         // que se va a escribir es la misma que la anterior 
                                         // y reducir tiempos de carga
    for(k = pX; k < kFin; k++){
        for(q = 0; q < GPU_PIXELS_SIZE_J; q++){
            for(i = pY; i < iFin; i++){
                if(*(nextTile) != currentTile){ // Comprueba si es distinta a la anterior
                    currentTile = *(nextTile);  // Si es asi, carga los colores de la nueva baldosa
                    tile = __GPU->tableDrawables.drawables[currentTile].tile; // Primero buscamos la baldosa
                    p = __GPU->palettes + __GPU->tableDrawables.drawables[currentTile].infoDraw.palette; // La paleta
                    back = __GPU->tableDrawables.drawables[__GPU->tableDrawables.drawables[currentTile].infoDraw.back].tile; // Y el fondo

                    //=== COLORES 0-1 ===
                    pixels1 = tile->pixels[0][q]; // Vamos cargando los indices correspondientes a los colores de la paleta
                    if(!pixels1.color1)           // En caso de ser 0, quiere decir que es transparente
                        pixels1.color1 = back->pixels[0][q].color1; // y se carga el pixel correspondiente en la baldosa de fondo
                    if(!pixels1.color2)
                        pixels1.color2 = back->pixels[0][q].color2;
                    // Como podemos ver se a desenrollado el bucle for para mejorar la velocidad de escritura del TFT
                    
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
                nextTile += __GPU->map.width; // La baldosa siguiente es la de abajo, no la de su derecha
                while(SPI1STATbits.TXBUFELM > 2); // Evitamos colapsar la cola de envio
                SPI1BUF = p->colors[pixels1.color1] << 16 | p->colors[pixels1.color2]; // Cargamos los colores, obtenidos
                SPI1BUF = p->colors[pixels2.color1] << 16 | p->colors[pixels2.color2]; // a raiz de la paleta, cada pixel,
                SPI1BUF = p->colors[pixels3.color1] << 16 | p->colors[pixels3.color2]; // se representa mediate 16bits, pero
                while(SPI1STATbits.TXBUFELM > 4);// Evitamos colapsar la cola de envio //
                SPI1BUF = p->colors[pixels4.color1] << 16 | p->colors[pixels4.color2]; // se mandan de dos en dos para mejorar la
                                                                                       // velocidad de carga
                // Las dos sentencias while estan puestas de esa forma tras prueba y error, buscando eficiencia y a la vez
            }   // que envie de forma correcta todos los bits. El problema es que el TFT es mas lento que la comunicacion (60ns por bit)
            nextTile -= salt; // Retrocedemos el puntero hasta la primera fila (El recorrido es por filas)
            currentTile = *(nextTile) + 1; // Obligamos a cargar la siguiente baldosa ya que aunque sea la misma no hay que olvidar
        }                                  // que internamente estan formada por 8x8 pixels y lo ultimo cargado es la columna mas a la derecha
                                           // de la baldosa
        nextTile++; // Avanzamos a la siguiente columna del mapa
    }
    SPI1CONbits.ON = 0; // Reiniciamos el modulo ya que es posible que la cola de entrada este en estado de overflow debido a colapsar
    SPI1CONbits.ON = 1; // la comunicacion con el envio intensivo de bits a TFT
    SPI_TFT_DESELECT;
}

void GPU_run(){
    CLICK_ON;
    while(1){
        GPU_draw();
        GPU_controller();
        CLICK_next();
    }
}

//------------------------------------------------------------------------------



//--------------------------- Funciones baldosas -------------------------------

uint8_t GPU_spriteMove(uint8_t idA, uint16_t sX, uint16_t sY){
    uint8_t idB = __GPU->map.tilesMap[sY * __GPU->map.width + sX];
    if(idB != idA){
        Drawable *draw = &__GPU->tableDrawables.drawables[idA];
        Drawable *toGo = &__GPU->tableDrawables.drawables[idB];
        if(toGo->infoMove.step){
            if(draw->infoDraw.sup){
                Drawable *sup = &__GPU->tableDrawables.drawables[draw->infoDraw.sup];
                sup->infoDraw.back = draw->infoDraw.back;
                draw->infoDraw.sup = 0;
            }
            else{
                Drawable *back = &__GPU->tableDrawables.drawables[draw->infoDraw.back];
                back->infoDraw.sup = 0;
                __GPU->map.tilesMap[draw->infoMove.sY * __GPU->map.width + draw->infoMove.sX] = draw->infoDraw.back;
            }
            draw->infoMove.sX = sX;
            draw->infoMove.sY = sY;
            if(toGo->infoMove.sprite){
                toGo->infoDraw.sup = idA;
            }
            draw->infoDraw.back = idB;
            __GPU->map.tilesMap[draw->infoMove.sY * __GPU->map.width + draw->infoMove.sX] = idA;
        }
        else
            return 0;
    }
    return 1;
}

uint8_t GPU_spriteNext(uint8_t sprite){
    __GPU->tableDrawables.drawables[sprite].infoAnim.view = __GPU->tableDrawables.drawables[sprite].infoAnim.next[__GPU->tableDrawables.drawables[sprite].infoAnim.view];
    __GPU->tableDrawables.drawables[sprite].tile = &__GPU->tiles[__GPU->tableDrawables.drawables[sprite].infoAnim.tile[__GPU->tableDrawables.drawables[sprite].infoAnim.view]];
    return 0;
}

uint8_t GPU_reorderAnimation(uint8_t draw, uint8_t tile, uint8_t next){
    __GPU->tableDrawables.drawables[draw].infoAnim.tile[tile] = next;
    return 0;
}

void GPU_setTile(uint16_t y, uint16_t x, uint8_t tile){
    __GPU->map.tilesMap[y * __GPU->map.width + x] = tile;
}

uint8_t GPU_addSprite(uint8_t ini, uint8_t length, uint8_t step){
    uint16_t i;
    Drawable *sprite = &(__GPU->tableDrawables.drawables[__GPU->tableDrawables.length++]);

    sprite->tile = &__GPU->tiles[ini];
    sprite->infoAnim.ini = ini;
    sprite->infoDraw.back = 0;
    sprite->infoDraw.sup = 0;
    sprite->infoAnim.view = 0;
    sprite->infoMove.sY = 0;
    sprite->infoMove.sX = 0;
    sprite->infoMove.sprite = 1;
    sprite->infoMove.step = step;
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
    
    return __GPU->tableDrawables.length - 1;
}

uint8_t GPU_addStatic(uint8_t ini, uint16_t length, uint8_t timer, uint8_t back, uint8_t step){
    uint16_t i;
    Drawable *stac = &(__GPU->tableDrawables.drawables[__GPU->tableDrawables.length++]);

    stac->tile = &__GPU->tiles[ini];
    stac->infoAnim.ini = ini;
    stac->infoDraw.back = back;
    stac->infoDraw.sup = 0;
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
    
    return __GPU->tableDrawables.length - 1;
}

//------------------------------------------------------------------------------

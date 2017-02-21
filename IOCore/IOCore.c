#include "IOCore.h"

uint32_t SPI_transfer(uint32_t data){
    SPI1BUF = data;
    while(SPI1STATbits.SPIBUSY != 0);
    return SPI1BUF;
}

void SPI_writeCmd(uint32_t cmd){
    SPI_TFT_CMD;
    SPI_MODE_16;
    SPI_TFT_SELECT;
    SPI_transfer(cmd);
    SPI_TFT_DESELECT;
}

void SPI_writeData(uint32_t data){
    SPI_TFT_DATA;
    SPI_MODE_16;
    SPI_TFT_SELECT;
    SPI_transfer(data);
    SPI_TFT_DESELECT;
}

void SPI_init(){
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
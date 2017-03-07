/* * * * * * * * * * * * * * * * * * * * * * * * * * 
 * Autor: Stefano Vidaurre Olite                   *
 * Fecha: 24/11/2016                               *
 * Descripcion: Libreria encargada de inplentar las*
 * diferentes maneras de comunicacion del pic con  *
 * el dispositivos externos. Ademas de la          * 
 * comunicacion interna mediate eventos.           *
 * V: 1.1.5                                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __IOCORE_H__
#define	__IOCORE_H__

#include <stdint.h>
#include <xc.h>
#include <sys/attribs.h>

//-----  Macros para manejo de pines -----//

#define SDI1_ENABLE SDI1R=0b0001            //Pin 5 gestiona SDI1
#define SDO1_ENABLE RPB2R=0b0011            //Pin 6 gestiona SDO1

#define SPI_TFT_CS_ENABLE TRISBbits.TRISB15=0;  //Pin 26 gestiona CS del TFT
#define SPI_TFT_DC_ENABLE TRISBbits.TRISB13=0;  //Pin 24 gestiona DC del TFT
#define SPI_TFT_CS LATBbits.LATB15          //||
#define SPI_TFT_DC LATBbits.LATB13          //Macros axiliares

//-----  Macros de seleccion de modo -----//
#define SPI_TFT_SELECT SPI_TFT_CS=0               //Activa CS del TFT
#define SPI_TFT_DESELECT SPI_TFT_CS=1             //Desactiva CS del TFT
#define SPI_TFT_CMD SPI_TFT_DC=0                  //Indica al TFT que se le pasa comando
#define SPI_TFT_DATA SPI_TFT_DC=1                 //Indoca al TFT que se le pasa datos
#define SPI_MODE_32 SPI1CONbits.MODE32=1;
#define SPI_MODE_16 SPI1CONbits.MODE32=0;

#define EVENT_ANALOG_0 PORTAbits.RA0
#define EVENT_ANALOG_1 PORTAbits.RA1
#define EVENT_BUTTON_0 PORTBbits.RB0==0
#define EVENT_BUTTON_1 PORTBbits.RB3==0
#define EVENT_BUTTON_2 PORTBbits.RB4==0
#define EVENT_BUTTON_3 PORTBbits.RB5==0

//----- Macros manejo crono -----//
#define CLICK_ON T4CONbits.ON=1;
#define CLICK_OFF T4CONbits.ON=0;

void SPI_writeData(uint32_t data);
void SPI_writeCmd(uint32_t cmd);
void IO_init();

inline void CLICK_next();
void IO_listener(); //--ABSTRACT--//
void IO_interrupt(); //--ABSTRACT--//

#endif	/* IOCORE_H */


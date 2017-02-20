/* * * * * * * * * * * * * * * * * * * * * * * * * * 
 * Autor: Stefano Vidaurre Olite                   *
 * Fecha: 24/11/2016                               *
 * Descripcion: Libreria encargada de inplentar las*
 * diferentes maneras de comunicacion del pic con  *
 * el dispositivos externos. Ademas de la          * 
 * comunicacion interna mediate eventos.           *
 * V: 1.1.3                                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __IOCORE_H__
#define	__IOCORE_H__

//-----  Macros para manejo de pines -----//

#define SDI1_ENABLE SDI1R=0b0001            //Pin 5 gestiona SDI1
#define SDO1_ENABLE RPB2R=0b0011            //Pin 6 gestiona SDO1

#define GPU_TFT_CS_ENABLE TRISBbits.TRISB15=0;  //Pin 26 gestiona CS del TFT
#define GPU_TFT_DC_ENABLE TRISBbits.TRISB13=0;  //Pin 24 gestiona DC del TFT
#define GPU_PIN_TFT_CS LATBbits.LATB15          //||
#define GPU_PIN_TFT_DC LATBbits.LATB13          //Macros axiliares



#endif	/* IOCORE_H */


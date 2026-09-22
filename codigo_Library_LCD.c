#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdio.h>

#ifndef LAVADORA_LCD_H
#define	LAVADORA_LCD_H

#include <xc.h>  
#include <stdio.h>

// ENTRADAS DE LA LAVADORA Puerto B
#define DIP_SW1          RB0  // Interruptor 1 del DIP Switch
#define DIP_SW2          RB3  // Interruptor 2 del DIP Switch
#define Boton_Inicio     RB1  // Botón Iniciar/Continuar programa
#define Sensor_Puerta    RB2  // Sensor: 0 = Abierta, 1 = Cerrada

// SALIDAS DE LA LAVADORA Puerto C
#define Candado         RC0  // LED simulando el bloqueo de puerta
#define BUZZER          RC1  // Alarma audible
#define Drenado         RC2  // Valvula de drenaje
#define Valvula_Agua    RA0  // Valvula de agua
#define MotorH          RC6  // Giro del motor (Puente H)
#define MotorL          RC7  // Giro del motor (Puente H)

// PINES DE CONTROL LCD Puerto E
#define RS              RE0  // Selección de registro
#define RW              RE1  // Lectura/Escritura
#define EN              RE2  // Enable

// PUERTO DE DATOS LCD Puerto D
#define LCD_Port        PORTD // LCD conectada al Puerto D
#define LCD_Tris        TRISD // Configuracion de entrada/salida

// CONSTANTES
#define Delay_LCD       20    // Retardo para el pin ENABLE
#define Delay_Shift     100   
#define Delay_Auto      500   
#define Delay_LED       200   

// Comandos para la LCD display
#define ClrScreen       0x01        // LCD clear display screen
#define ReturnHome      0x02        // LCD return home
#define DecCursor       0x04        // LCD decrement cursor (shift cursor to left)
#define IncCursor       0x06        // LCD increment cursor (shift cursor to right)
#define ShiftRight      0x05        // Shift display right
#define ShiftLeft       0x07        // Shift display left
#define DispOFFCurOFF   0x08        // Display OFF, cursor OFF
#define DispOFFCurON    0x0A        // Display OFF, cursor ON
#define DispONCurOFF    0x0C        // Display ON, cursor OFF
#define DispONCurBk     0x0E        // Display ON cursor Blinking
#define DispOFFCurBk    0x0F        // Display OFF cursor Blinking
#define ShiftCurLeft    0x10        // Shift cursor position to left
#define ShiftCurRight   0x14        // Shift cursor position to right
#define ShiftDispLeft   0x18        // Shift entire display to the left
#define ShiftDispRight  0x1C        // Shift entire display to the right
#define FirstLine       0x80        // Cursor beginning of first line
#define SecondLine      0xC0        // Cursor beginning of second line
#define TwoLines57Mat   0x38        // Two lines, 5x7 matrix

// FUNCIONES DE LA LCD

void Init_Variables(){
    LCD_Port = 0;                   // Initialise display PORT buffer
    PORTB = 0;                      // Initialise PORTB
}

// Sending commands to LCD
void Lcd_CmdWrite(unsigned char c){
    LCD_Port = c;                   // Place char ASCII in LCD data bus
    RS = 0;                         // To send commands to LCD: RS = 0
    RW = 0;                         // This one always to GND
    EN = 1;                         // Send data to LCD now
    __delay_ms(Delay_LCD);          // Wait for line to stabilise
    EN = 0;                         // Ready, all sent
}

// Escribe informacion para el LCD
void Lcd_DataWrite(unsigned char d){
    LCD_Port = d;                   // Place char ASCII in LCD data bus
    RS = 1;                         // To send data to LCD: RS = 1
    RW = 0;                         // This one always to GND
    EN = 1;                         // Send data to LCD now
    __delay_ms(Delay_LCD);          // Wait for line to syabilise
    EN = 0;                         // Ready, all sent 
}

// Mensaje para la LCD
void Message_LCD(unsigned char *s){
    while(*s){
        Lcd_DataWrite(*s++);
    }
}

// Posicionaminto del cursor de la pantalla LCD
void Init_LCD(){
    Lcd_CmdWrite(TwoLines57Mat);
    Lcd_CmdWrite(DispONCurOFF);
    Lcd_CmdWrite(ClrScreen);
    Lcd_CmdWrite(FirstLine);
}

void Instructions_Msg(){
    Lcd_CmdWrite(ClrScreen);
    Lcd_CmdWrite(FirstLine);
    Message_LCD("Lavadora Pablito");
    Lcd_CmdWrite(SecondLine);
    Message_LCD("Oli Gro");
    __delay_ms(1000);
    Lcd_CmdWrite(ClrScreen);
    Lcd_CmdWrite(FirstLine);
    Message_LCD("MR2006B");
    Lcd_CmdWrite(SecondLine);
    Message_LCD("Act 2");
    __delay_ms(1000);
    Lcd_CmdWrite(ClrScreen);
    Lcd_CmdWrite(FirstLine);
    Message_LCD("Selecciona modo");
    Lcd_CmdWrite(SecondLine);
    Message_LCD("de lavado");
    __delay_ms(1000);
}

// USO DE LA IA PARA LA CREACION DE LAS ANIMACIONES DE LA LCD
void Lcd_CreateChar(unsigned char char_idx, unsigned char *map) {
    char_idx &= 0x07;
    Lcd_CmdWrite(0x40 | (char_idx << 3));
    for (int i = 0; i < 8; i++) {
        Lcd_DataWrite(map[i]);
    }
    Lcd_CmdWrite(FirstLine);
}

void Init_Custom_Chars(void){
    unsigned char candado_cerrado[8] = {0x0E, 0x11, 0x11, 0x1F, 0x1B, 0x1B, 0x1F, 0x00}; // Char 0
    unsigned char candado_abierto[8] = {0x0E, 0x10, 0x10, 0x1F, 0x1B, 0x1B, 0x1F, 0x00}; // Char 1
    unsigned char buzzer1[8]       = {0x01, 0x03, 0x0F, 0x0F, 0x0F, 0x03, 0x01, 0x00}; // Char 2
    unsigned char buzzer2[8]       = {0x09, 0x07, 0x1F, 0x1F, 0x1F, 0x07, 0x09, 0x00}; // Char 3
    unsigned char gota[8]            = {0x04, 0x04, 0x0A, 0x0A, 0x11, 0x11, 0x0E, 0x00}; // Char 4
    unsigned char ola[8]             = {0x00, 0x00, 0x0E, 0x11, 0x0A, 0x04, 0x00, 0x00}; // Char 5

    Lcd_CreateChar(0, candado_cerrado);
    Lcd_CreateChar(1, candado_abierto);
    Lcd_CreateChar(2, buzzer1);
    Lcd_CreateChar(3, buzzer2);
    Lcd_CreateChar(4, gota);
    Lcd_CreateChar(5, ola);
}

#endif


#ifdef	__cplusplus

#endif



#ifdef	__cplusplus

#endif 

#endif

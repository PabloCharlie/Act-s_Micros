#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h>  
#include <stdio.h>

// Puertos del PIC
#define RS              RB0            //LATBbits.LATB0
#define RW              RB1            //LATBbits.LATB1
#define EN              RB2            //LATBbits.LATB2
#define Switch_manual_auto       RB7  // Configuración del Dipswitch manual/automatico
#define Switch_temp            RB5    // Switch para temperatura 
#define Switch_luz       RB4         // Switch para la luz

// Definicion de los puertos de la PIC
#define LCD_Port        PORTD       // LCD display connected to PORTD
#define LCD_Tris        TRISD       // LCD display I/O pin selection

// Constantes
#define Delay_LCD       20          // Delay for LCD ENABLE pin
#define Delay_Shift     100         // Delay for char shifts in LCD 
#define Delay_Auto      500         // Delay for automatic display
#define Delay_LED       200         // Delay for blinking LED 

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

// STRINGS que se veran en la LCD //Intrucciones
void Instructions_Msg(){
    Lcd_CmdWrite(ClrScreen);
    Lcd_CmdWrite(FirstLine);
    Message_LCD("Pablito Olivares");
    Lcd_CmdWrite(SecondLine);
    Message_LCD("Guerrero :)");
    __delay_ms(3550);
    Lcd_CmdWrite(ClrScreen);
    Lcd_CmdWrite(FirstLine);
    Message_LCD("Automatizacion ");
    Lcd_CmdWrite(SecondLine);
    Message_LCD("Industrial ");
    __delay_ms(3550);
    Lcd_CmdWrite(ClrScreen);
    Lcd_CmdWrite(FirstLine);
    Message_LCD("Act 1 ");
    Lcd_CmdWrite(SecondLine);
    Message_LCD("MR2006B ");
    __delay_ms(3000);
    Lcd_CmdWrite(ClrScreen);
    Lcd_CmdWrite(FirstLine);
    Message_LCD("Sistema auto ");
    Lcd_CmdWrite(SecondLine);
    Message_LCD("de monitoreo ");
    __delay_ms(2550);
}

#ifdef	__cplusplus

#endif 


#ifdef	__cplusplus

#endif 

#endif

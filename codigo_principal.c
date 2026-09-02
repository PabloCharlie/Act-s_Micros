// CONFIGURATION BITS

// PIC18F4550 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1L
#pragma config PLLDIV = 1       // PLL Prescaler Selection bits (No prescale (4 MHz oscillator input drives PLL directly))
#pragma config CPUDIV = OSC1_PLL2// System Clock Postscaler Selection bits ([Primary Oscillator Src: /1][96 MHz PLL Src: /2])
#pragma config USBDIV = 1       // USB Clock Selection bit (used in Full-Speed USB mode only; UCFG:FSEN = 1) (USB clock source comes directly from the primary oscillator block with no postscale)

// CONFIG1H
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator (HS))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOR = OFF        // Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
#pragma config BORV = 3         // Brown-out Reset Voltage bits (Minimum setting 2.05V)
#pragma config VREGEN = OFF     // USB Voltage Regulator Enable bit (USB voltage regulator disabled)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = ON      // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer 1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = OFF     // Stack Full/Underflow Reset Enable bit (Stack full/underflow will not cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config ICPRT = OFF      // Dedicated In-Circuit Debug/Programming Port (ICPORT) Enable bit (ICPORT disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) is not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) is not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) is not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) is not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) is not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM is not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) is not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) is not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) is not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) is not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) are not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) is not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM is not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-001FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) is not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) is not protected from table reads executed in other blocks)

// LEDS indicadores de estado
#define LED_AUTO   RC0    // Pin 15 Modo Automatico
#define LED_MANUAL RC1    // Pin 16 Modo Manual
#define LED_ERROR  RC2    // Pin 17 Mensaje de Error

#include <xc.h>
#define _XTAL_FREQ     20000000     // 20-MHz crystal frequency
#include "LIBRARY_LCD.h"
#include <stdio.h>

char data[16];

// Inicializar Puertos
void Init_Ports(){
    TRISC0 = 0;                    
    TRISC1 = 0;
    TRISC2 = 0;
    
    TRISB0 = 0;                     // RS declared as output for LCD display
    TRISB1 = 0;                     // RW declared as output for LCD display
    TRISB2 = 0;                     // EN declared as output for LCD display
    TRISB7 = 1;                     // Pin 1 Dipswitch
    TRISB6 = 1;                     // Entrada digital
    TRISB5 = 1;                     // Entrada digital
    TRISB4 = 1;
    LCD_Tris = 0;                   // All LCD pins declared as output
    LCD_Port = 0;                   // Port for LCD is declared as output 
}

// Inicializar modulos ADC
void Inic_ADC_modu(){
    ADFM = 1;
    
    VCFG1 = 0;
    VCFG0 = 0;
    
    PCFG3 = 1;
    PCFG2 = 1;
    PCFG1 = 0;
    PCFG0 = 1;
    
    ADCS2 = 0;
    ADCS1 = 1; // sino cambiar a 0
    ADCS0 = 0;
    
    ADON = 1;
}

void CHS_T(){
    CHS3 = 0;
    CHS2 = 0;
    CHS1 = 0;
    CHS0 = 0;
}

void CHS_L(){
    CHS3 = 0;
    CHS2 = 0;
    CHS1 = 0;
    CHS0 = 1;
}

void Leer_analogo_Input () {
    float ADC_read;
    Lcd_CmdWrite(FirstLine);
    Message_LCD("Voltaje");
    GO_DONE = 1;
    while(GO_DONE){
    } 
    ADC_read = ((ADRESH*256.0+ADRESL))*(5/1023.0);
    sprintf(data,"%.3f",ADC_read);
    Message_LCD(data);
    Message_LCD(" V");
    // regresar al ADC_read
}

void Sensor_Temp(void){
    float voltaje, temp_c;
    CHS_T();
    
    Lcd_CmdWrite(ClrScreen);
    Lcd_CmdWrite(FirstLine);
    Message_LCD("Temperatura: ");
    GO_DONE = 1;
    while(GO_DONE){
    }
    voltaje = ((ADRESH*256.0+ADRESL))*(5/1023.0);
    temp_c = (voltaje*150)/1.5;
    sprintf(data, "%.2f C", temp_c);
    Lcd_CmdWrite(SecondLine);
    Message_LCD(data);
}

void Sensor_Luz(void){
    float voltaje_l, luz_convertidor; 
    CHS_L();
    
    Lcd_CmdWrite(ClrScreen);
    Lcd_CmdWrite(FirstLine);
    Message_LCD("Luz: ");
        GO_DONE = 1;
    while(GO_DONE){
    }
    voltaje_l = ((ADRESH*256.0+ADRESL))*(5/1023.0);
    luz_convertidor = (voltaje_l*100)/5;
    sprintf(data, "%.2f", luz_convertidor);
    Lcd_CmdWrite(SecondLine);
    Message_LCD(data);
}

void Msg_Error(void){
    Lcd_CmdWrite(ClrScreen);
    Lcd_CmdWrite(FirstLine);
    Message_LCD("Error Seleccion");
    Lcd_CmdWrite(SecondLine);
    Message_LCD("no valida");
    __delay_ms(1550);
}

void main(void) {
    // Inicializacion
    Init_Ports();
    Init_Variables();
    Init_LCD();
    Inic_ADC_modu();

    // Mensaje inicial de instrucciones
    Instructions_Msg();

    while(1) {
        // MODO AUTO
        if (Switch_manual_auto == 1) {
            LED_AUTO = 1;
            LED_MANUAL = 0;
            LED_ERROR = 0;
            
            Sensor_Temp();
            __delay_ms(1500);
            Sensor_Luz();
            __delay_ms(1500);
        }
        // MODO MANUAL
        else {
            // el error es que ambos interruptores estan activos (1,1) o ambos desactivados (0,0)
            if ((Switch_temp == 1 && Switch_luz == 1) || (Switch_temp == 0 && Switch_luz == 0)) {
                LED_AUTO = 0;
                LED_MANUAL = 0;
                LED_ERROR = 1;
                Msg_Error();
            }
            // Muestra unicamente temperatura
            else if (Switch_temp == 1 && Switch_luz == 0) {
                LED_AUTO = 0;
                LED_MANUAL = 1;
                LED_ERROR = 0;
                
                Sensor_Temp();
                __delay_ms(1800);
            }
            // Muestra unicamente la luz
            else if (Switch_temp == 0 && Switch_luz == 1) {
                LED_AUTO = 0;
                LED_MANUAL = 1;
                LED_ERROR = 0;
                
                Sensor_Luz();
                __delay_ms(1500);
            }
        }
    }
}

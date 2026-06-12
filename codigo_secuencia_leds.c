/*
 * File:   ACT_LUCES_SECUENCIALES.c
 * Author: pablo
 *
 * Created on 27 de mayo de 2026, 08:07 PM
 */

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
#pragma config PWRT = ON        // Power-up Timer Enable bit (PWRT enabled)
#pragma config BOR = OFF        // Se cambio de ON a OFF -- Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
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
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
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

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// Librerias importadas para el cristal 
#include <xc.h>
#define _XTAL_FREQ 20000000 // Cristal de 20 MHz

// Definicion para que las variables
#define BOTON_INICIO PORTBbits.RB4 
#define BUZZER LATCbits.LATC0
#define LEDS LATD // Tomar la barra de LEDs está hacia el puerto D
                 // Puerto D --> RD en el PIC


// Función main principal
// Configuracion de los registros y bits
void main() {
    
    ADCON1 = 0x0F; // Vuelve todos los pines digitales I/O
    
    TRISD = 0x00;  // Puerto D como salida 0 para la barra de LEDs
    TRISCbits.TRISC0 = 0; // RC0 como salida 0 para el buzzer
    
    // Puerto B como entrada 1 para los 4 switches y el boton
    TRISBbits.TRISB0 = 1; 
    TRISBbits.TRISB1 = 1;
    TRISBbits.TRISB2 = 1;
    TRISBbits.TRISB3 = 1;
    TRISBbits.TRISB4 = 1;

    // Inicializar salidas apagadas
    LEDS = 0x00;
    BUZZER = 0;

    // Inicio del loop
    while(1) { 
        
        // FASE 1: Llenado de barra (1 segundo por LED)
        LEDS = 0x00;
        unsigned char leds_encendidos = 0x00;
        
        for(int i = 0; i < 8; i++) { 
            leds_encendidos = (leds_encendidos << 1) | 0x01;
            LEDS = leds_encendidos;
            __delay_ms(1000); 
        }
        LEDS = 0x00;

        // FASE 2: Aviso a buzzer
        for(int i = 0; i < 3; i++) {
            BUZZER = 1;
            __delay_ms(500);
            BUZZER = 0;
            __delay_ms(500);
        }

        // FASE 3: Espera de boton
        int secuencia_iniciada = 0;
        
        while(secuencia_iniciada == 0) { 
            LEDS = 0xFF; 
            __delay_ms(400);
            LEDS = 0x00; 
            __delay_ms(400);
            
            if(BOTON_INICIO == 0) { 
                __delay_ms(20); // Espera a que se termine la Fase 1
                if(BOTON_INICIO == 0) {
                    secuencia_iniciada = 1;
                    while (BOTON_INICIO == 0); // Espera a que se suelte el boton
                }
            }
        }

        // FASE 4: Ejecución de secuencias
        int ejecutar_secuencia = 1;
        unsigned char lectura_anterior = PORTB & 0x0F; // Lee el estado de los 8 pines del Puerto B
        int paso = 0;                                 // Esto hace que los 4 pines de derecha a isquierda se lean
                   // unsigned le dice al compilador que los 8 bits se utilizaran exclusivamente positivos - valores enteros
                   // char es un dato primario en C que le dice al compilador que reserve los 8 bits
                        // y lo guarde en su memoria

        while(ejecutar_secuencia == 1) {
            unsigned char lectura_dip = PORTB & 0x0F; 
            
            // Detectar si el switch cambió de estado
            if (lectura_dip != lectura_anterior) {
                LEDS = 0x00;       
                BUZZER = 1;        
                __delay_ms(300);   
                BUZZER = 0;        
                __delay_ms(300);   
                lectura_anterior = lectura_dip; 
                paso = 0;          
            }
            
            switch(lectura_dip) {
                case 0x08: // (Binario 1000) - Secuencia 1
                    if (paso == 0) LEDS = 0xAA;
                    else if (paso == 1) LEDS = 0x55;
                    
                    paso++;
                    if (paso > 1) paso = 0;
                    __delay_ms(225); 
                    break;
                    
                case 0x0C: // (Binario 1100) - Secuencia 2
                    if (paso == 0) LEDS = 0x80;
                    else if (paso == 1) LEDS = 0x40;
                    else if (paso == 2) LEDS = 0x20;
                    else if (paso == 3) LEDS = 0x10;
                    else if (paso == 4) LEDS = 0x08;
                    else if (paso == 5) LEDS = 0x04;
                    else if (paso == 6) LEDS = 0x02;
                    else if (paso == 7) LEDS = 0x01;
                    else if (paso == 8) LEDS = 0x02;
                    else if (paso == 9) LEDS = 0x04;
                    else if (paso == 10) LEDS = 0x08;
                    else if (paso == 11) LEDS = 0x10;
                    else if (paso == 12) LEDS = 0x20;
                    else if (paso == 13) LEDS = 0x40;
                    
                    paso++;
                    if (paso > 13) paso = 0;
                    __delay_ms(100);
                    break;
                    
                case 0x0E: // (Binario 1110) - Secuencia 3
                    if (paso == 0) LEDS = 0x01;
                    else if (paso == 1) LEDS = 0x03;
                    else if (paso == 2) LEDS = 0x07;
                    else if (paso == 3) LEDS = 0x0F;
                    else if (paso == 4) LEDS = 0x1F;
                    else if (paso == 5) LEDS = 0x3F;
                    else if (paso == 6) LEDS = 0x7F;
                    else if (paso == 7) LEDS = 0xFF;
                    else if (paso == 8) LEDS = 0x00;
                    
                    paso++;
                    if (paso > 8) paso = 0;
                    __delay_ms(200); 
                    break;
                    
                case 0x0F: // (Binario 1111) - Secuencia 4
                    if (paso == 0) LEDS = 0xFF;
                    else if (paso == 1) LEDS = 0x00;
                    
                    paso++;
                    if (paso > 1) paso = 0;
                    __delay_ms(100);
                    break;
                
                case 0x00: // Estado de reposo (Todos los switches apagados)
                    LEDS = 0x00; 
                    paso = 0;
                    __delay_ms(50); // Solo espera, no aborta ni se ejecuta
                    break;    
                    
                default: // Combinación inválida
                    LEDS = 0x00; 
                    BUZZER = 1;
                    __delay_ms(1000); // 1 segundo de pitido
                    BUZZER = 0;
                    ejecutar_secuencia = 0; // Hace reiniciar la secuencia desde la Fase 1
                    break;
            }
            
            // Verificar si se presiona el botón para reiniciar el programa desde el inicio
            if(BOTON_INICIO == 0) {
                __delay_ms(20);
                if(BOTON_INICIO == 0) {
                    ejecutar_secuencia = 0; 
                    while(BOTON_INICIO == 0); 
                }
            }
        }
    }
}

// Act Lavadora Inteligente || Profesor: Aragon

// CONFIGURATION BITS
// PIC18F4550 Configuration Bit Settings

// CONFIG1L
#pragma config PLLDIV = 1        // PLL Prescaler Selection bits
#pragma config CPUDIV = OSC1_PLL2// System Clock Postscaler Selection bits
#pragma config USBDIV = 1        // USB Clock Selection bit

// CONFIG1H
#pragma config FOSC = HS         // Oscillator Selection bits
#pragma config FCMEN = OFF       // Fail-Safe Clock Monitor Enable bit
#pragma config IESO = OFF        // Internal/External Oscillator Switchover bit

// CONFIG2L
#pragma config PWRT = OFF        // Power-up Timer Enable bit
#pragma config BOR = OFF         // Brown-out Reset Enable bits
#pragma config BORV = 3          // Brown-out Reset Voltage bits
#pragma config VREGEN = OFF      // USB Voltage Regulator Enable bit

// CONFIG2H
#pragma config WDT = OFF         // Watchdog Timer Enable bit
#pragma config WDTPS = 32768     // Watchdog Timer Postscale Select bits

// CONFIG3H
#pragma config CCP2MX = ON       // CCP2 MUX bit
#pragma config PBADEN = OFF      // PORTB A/D Enable bit
#pragma config LPT1OSC = OFF     // Low-Power Timer 1 Oscillator Enable bit
#pragma config MCLRE = ON        // MCLR Pin Enable bit

// CONFIG4L
#pragma config STVREN = OFF      // Stack Full/Underflow Reset Enable bit
#pragma config LVP = OFF         // Single-Supply ICSP Enable bit
#pragma config ICPRT = OFF       // Dedicated In-Circuit Debug/Programming Port
#pragma config XINST = OFF       // Extended Instruction Set Enable bit

// CONFIG5L
#pragma config CP0 = OFF         // Code Protection bit
#pragma config CP1 = OFF         // Code Protection bit
#pragma config CP2 = OFF         // Code Protection bit
#pragma config CP3 = OFF         // Code Protection bit

// CONFIG5H
#pragma config CPB = OFF         // Boot Block Code Protection bit
#pragma config CPD = OFF         // Data EEPROM Code Protection bit

// CONFIG6L
#pragma config WRT0 = OFF        // Write Protection bit
#pragma config WRT1 = OFF        // Write Protection bit
#pragma config WRT2 = OFF        // Write Protection bit
#pragma config WRT3 = OFF        // Write Protection bit

// CONFIG6H
#pragma config WRTC = OFF        // Configuration Register Write Protection bit
#pragma config WRTB = OFF        // Boot Block Write Protection bit
#pragma config WRTD = OFF        // Data EEPROM Write Protection bit

// CONFIG7L
#pragma config EBTR0 = OFF       // Table Read Protection bit
#pragma config EBTR1 = OFF       // Table Read Protection bit
#pragma config EBTR2 = OFF       // Table Read Protection bit
#pragma config EBTR3 = OFF       // Table Read Protection bit

// CONFIG7H
#pragma config EBTRB = OFF       // Boot Block Table Read Protection bit

#include <xc.h>
#define _XTAL_FREQ     20000000  // 20-MHz crystal frequency
#include "LIBRARY_LCD_LAVADORA.h"
#include <stdio.h>

// LEDS indicadores de estado y botones
#define BOTON_PAUSA_PLAY RA1      // Pin 3 BOTON DE PAUSA Y PLAY
#define LED_PRE_LAVADO   RB4      // Pin 37 MODO PRE LAVADO
#define LED_LAVADO       RB5      // Pin 38 MODO LAVADO
#define LED_ENJUAGUE     RB6      // Pin 39 MODO ENJUAGUE
#define LED_CENTRI       RB7      // Pin 40 MODO CENTRIFUGADO

char data[16];

void Init_Ports(){
    ADCON1 = 0x0F;  // Apagar lecturas analógicas (Todos los pines a DIGITAL)
    
    // Limpiar salidas para asegurar arranque en OFF (0V)
    LATA = 0;
    LATB = 0;
    LATC = 0;
    LATD = 0;
    LATE = 0;
    
    // Entradas de la lavadora Puerto B
    TRISB0 = 1;    // Entrada Botón Ajuste
    TRISB1 = 1;    // Entrada Botón Inicio / Continuar
    TRISB2 = 1;    // Entrada Switch de Puerta
    TRISB3 = 1;
    TRISB4 = 0;
    TRISB5 = 0;
    TRISB6 = 0;
    TRISB7 = 0;
    
    // Salidas de la lavadora Puerto C y A
    TRISC0 = 0;    // Salida Candado
    TRISC1 = 0;    // Salida BUZZER
    TRISC2 = 0;    // Salida Drenado
    TRISA0 = 0;    // Salida Válvula de agua
    TRISC6 = 0;    // Salida MotorH
    TRISC7 = 0;    // Salida MotorL
    
    // Salidas para la LCD
    LCD_Tris = 0;  // Puerto D como salida (Datos LCD)
    LCD_Port = 0;  // Limpiar Puerto D
    TRISE0 = 0;    // Salida RS
    TRISE1 = 0;    // Salida RW
    TRISE2 = 0;    // Salida EN
}

// Hace parpadear los LEDs de los 4 ciclos para indicar pausa o ajuste
void Flash_LEDS(void){
    LED_PRE_LAVADO = !LED_PRE_LAVADO;   // El signo ! es un operador logico NOT
    LED_LAVADO     = !LED_LAVADO;      // Si el LED está encendido (1), !1 da como resultado 0 (lo apaga)
    LED_ENJUAGUE   = !LED_ENJUAGUE;   // Si el LED está apagado (0), !0 da como resultado 1 (lo enciende)
    LED_CENTRI     = !LED_CENTRI;
    __delay_ms(200);
}

// Genera un pitido limpio y asegura apagar el buzzer
void Alerta_BUZZER(void){
    BUZZER = 1;
    __delay_ms(150);
    BUZZER = 0;
    __delay_ms(100);
}

void Enable_Interrupts(){
    INTEDG0 = 0;    // Flanco de bajada en RB0
    INT0IF = 0;     // Limpiar bandera INT0
    GIE = 1;        // Habilitar interrupciones globales
    INT0IE = 1;     // Habilitar interrupción INT0 (Boton_Ajuste)
}

// Declaración de banderas globales
volatile unsigned char flag_ajuste = 0;
volatile unsigned char flag_pausa_play = 0;

void __interrupt() Interrupcion_Lavadora(void){
    // Evento por pulsación en RB0 (Boton_Ajuste vía INT0)
    if (INT0IF){
        flag_ajuste = 1; // Levanta la bandera para atenderla en el main
        INT0IF = 0;      // Borra la bandera de inmediato
    }
}

// Función para pausar y congelar el lavado
void Pausa_Boton(void){
    if (BOTON_PAUSA_PLAY == 0) { 
        __delay_ms(50); // Dejar presionado el boton para pausar
        if (BOTON_PAUSA_PLAY == 0) {
            Lcd_CmdWrite(ClrScreen);
            Lcd_CmdWrite(FirstLine);
            Message_LCD("   LAVADORA   ");
            Lcd_CmdWrite(SecondLine);
            Message_LCD("   EN PAUSA   ");
            
            while(BOTON_PAUSA_PLAY == 0); // Espera a que se suelte el botón
            __delay_ms(50);
            while(BOTON_PAUSA_PLAY == 1); // Congela la lavadora hasta presionar de nuevo
            while(BOTON_PAUSA_PLAY == 0); // Espera a que se suelte otra vez
            __delay_ms(50);
            Lcd_CmdWrite(ClrScreen);
        }
    }
}

void Msg_Error(void){
    Lcd_CmdWrite(ClrScreen);
    Lcd_CmdWrite(FirstLine);
    Message_LCD("Error");
    Lcd_CmdWrite(SecondLine);
    Message_LCD("selecciona modo");
    __delay_ms(1550);
}

// Animación del Candado (0 = Abierto, 1 = Cerrado)
void Anim_Candado(unsigned char estado){
    Lcd_CmdWrite(ClrScreen);
    Lcd_CmdWrite(FirstLine);
    Message_LCD("Estado Puerta:");
    Lcd_CmdWrite(SecondLine);
    
    if(estado == 1){
        Lcd_DataWrite(0); // Icono candado cerrado
        Message_LCD(" BLOCKED ");
    } else {
        Lcd_DataWrite(1); // Icono candado abierto
        Message_LCD(" UNLOCKED ");
    }
}

// LAS ANIMACIONES DE BOCINA, GOTA DE AGUA, OLA Y CANDADO FUERON HECHAS POR IA
// PERO LA ESTRUCTURA ES PROPIA YA QUE TIENE ESTRUCTURA COMO LAS DEMAS ANIMACIONES
// Animación de BUZZER con sonido real
void Anim_BUZZER(void){
    Lcd_CmdWrite(ClrScreen);
    Lcd_CmdWrite(FirstLine);
    Message_LCD(" ALERTA SOUND");
    
    Lcd_CmdWrite(SecondLine);
    Lcd_DataWrite(2); 
    Message_LCD(" BEEP! BEEP! ");
    Lcd_DataWrite(3); 
    
    Alerta_BUZZER();
    Alerta_BUZZER();
}

// Animación Válvula de Agua
void Anim_Valvula_Agua(void){
    Lcd_CmdWrite(ClrScreen);
    Lcd_CmdWrite(FirstLine);
    Message_LCD("Llenando Agua");
    Lcd_CmdWrite(SecondLine);
    Lcd_DataWrite(4); Message_LCD(" ");
    Lcd_DataWrite(4); Message_LCD(" ");
    Lcd_DataWrite(4); Message_LCD(" ");
    Lcd_DataWrite(4); Message_LCD(" ");
    __delay_ms(200);
}

// Animación Drenado
void Anim_Drenado(void){
    Lcd_CmdWrite(ClrScreen);
    Lcd_CmdWrite(FirstLine);
    Message_LCD("Drenando Agua");
    Lcd_CmdWrite(SecondLine);
    Lcd_DataWrite(5); Message_LCD(" "); 
    Lcd_DataWrite(5); Message_LCD(" "); 
    Lcd_DataWrite(5); Message_LCD(" "); 
    Lcd_DataWrite(5); Message_LCD(" ");
    __delay_ms(200);
}

// Animación Motor Giro Derecha (MotorH)
void Anim_MotorH(void){
    Lcd_CmdWrite(ClrScreen);
    Lcd_CmdWrite(FirstLine);
    Message_LCD("MotorH");
    Lcd_CmdWrite(SecondLine);
    Message_LCD("Girando ->");
    __delay_ms(150);
    Lcd_CmdWrite(SecondLine);
    Message_LCD("Girando ->");
    __delay_ms(150);
}

// Animación Motor Giro Izquierda (MotorL)
void Anim_MotorL(void){
    Lcd_CmdWrite(ClrScreen);
    Lcd_CmdWrite(FirstLine);
    Message_LCD("MotorL");
    Lcd_CmdWrite(SecondLine);
    Message_LCD("Girando <-");
    __delay_ms(150);
    Lcd_CmdWrite(SecondLine);
    Message_LCD("Girando <-");
    __delay_ms(150);
}

// Animación Modo Pre-lavado
void Anim_Prelavado(void){
    Lcd_CmdWrite(ClrScreen);
    Lcd_CmdWrite(FirstLine);
    Message_LCD("MODO PRELAVADO");
    Lcd_CmdWrite(SecondLine);
    Message_LCD("Preparando.   "); __delay_ms(200);
    Lcd_CmdWrite(SecondLine);
    Message_LCD("Preparando..  "); __delay_ms(200);
    Lcd_CmdWrite(SecondLine);
    Message_LCD("Preparando... "); __delay_ms(200);
}

// Animación Modo Lavado
void Anim_Lavado(void){
    Lcd_CmdWrite(ClrScreen);
    Lcd_CmdWrite(FirstLine);
    Message_LCD("CICLO: LAVADO");
    Lcd_CmdWrite(SecondLine);
    Message_LCD("Lavando"); Message_LCD(" "); Lcd_DataWrite(4);
    __delay_ms(200);
    Lcd_CmdWrite(SecondLine);
    Message_LCD("Lavando"); Message_LCD(" "); Lcd_DataWrite(4);
    __delay_ms(200);
}

// Animación Modo Enjuague
void Anim_Enjuague(void){
    Lcd_CmdWrite(ClrScreen);
    Lcd_CmdWrite(FirstLine);
    Message_LCD("CICLO: ENJUAGUE");
    Lcd_CmdWrite(SecondLine);
    Lcd_DataWrite(5); Message_LCD(" Limpiando "); Lcd_DataWrite(5);
    __delay_ms(200);
}

// Animación Modo Centrifugado
void Anim_Centrifugado(void){
    Lcd_CmdWrite(ClrScreen);
    Lcd_CmdWrite(FirstLine);
    Message_LCD("CENTRIFUGANDO");
    Lcd_CmdWrite(SecondLine);
    Message_LCD(">>> "); Message_LCD(" "); Message_LCD(" <<<");
    __delay_ms(100);
}

void main(void) {
    // Inicialización
    Init_Ports();
    Init_Variables();
    Init_LCD();
    Init_Custom_Chars();
    Enable_Interrupts();

    // Mensaje inicial en la LCD
    Instructions_Msg();

    while(1) {
        // Atención a la interrupción de Botón de Ajuste vía flag
        if (flag_ajuste == 1) {
            Alerta_BUZZER();
            flag_ajuste = 0;
        }
        
        // Espera directa al Botón de Inicio vía consulta de pin (Polling)
        if (Boton_Inicio == 0) {
            Flash_LEDS();
            __delay_ms(200); // Anti-rebote para el botón
            
            // CONDICIÓN DE ERROR: Ambos interruptores en OFF (1,1)
            if (DIP_SW1 == 1 && DIP_SW2 == 1) {
                Msg_Error();
            }   
            // MODO 1: NORMAL (DIP_SW1 = 0, DIP_SW2 = 1)
            else if (DIP_SW1 == 0 && DIP_SW2 == 1) {
                // Verificación de Puerta
                if (Sensor_Puerta == 0) {
                    Candado = 0;
                    Anim_Candado(0);  // UNLOCKED
                    Anim_BUZZER();
                    __delay_ms(1500);
                } 
                else {
                    Candado = 1;
                    Anim_Candado(1);  // BLOCKED
                    __delay_ms(1000);

                    // PRELAVADO
                    Pausa_Boton();
                    LED_PRE_LAVADO = 1;
                    Anim_Prelavado();
                    Valvula_Agua = 1;
                    Anim_Valvula_Agua();
                    Valvula_Agua = 0;
                    LED_PRE_LAVADO = 0;

                    // LAVADO
                    Pausa_Boton();
                    LED_LAVADO = 1;
                    Anim_Lavado();
                    MotorH = 1; MotorL = 0; Anim_MotorH(); MotorH = 0; MotorL = 0;
                    MotorH = 0; MotorL = 1; Anim_MotorL(); MotorH = 0; MotorL = 0;
                    Pausa_Boton();
                    Drenado = 1; Anim_Drenado(); Drenado = 0;
                    LED_LAVADO = 0;

                    // ENJUAGUE
                    Pausa_Boton();
                    LED_ENJUAGUE = 1;
                    Valvula_Agua = 1; Anim_Valvula_Agua(); Valvula_Agua = 0;
                    Anim_Enjuague();
                    MotorH = 1; MotorL = 0; Anim_MotorH(); MotorH = 0; MotorL = 0;
                    Pausa_Boton();
                    Drenado = 1; Anim_Drenado(); Drenado = 0;
                    LED_ENJUAGUE = 0;

                    // CENTRIFUGADO
                    Pausa_Boton();
                    LED_CENTRI = 1;
                    Drenado = 1; MotorH = 1;
                    Anim_Centrifugado();
                    MotorH = 0; Drenado = 0;
                    LED_CENTRI = 0;

                    // FIN DE LAVADO
                    Pausa_Boton();
                    Candado = 0;
                    Anim_BUZZER();
                    Anim_BUZZER();
                    Lcd_CmdWrite(ClrScreen);
                    Lcd_CmdWrite(FirstLine);
                    Message_LCD("  LAVADO FIN   ");
                    Lcd_CmdWrite(SecondLine);
                    Message_LCD(" PUERTA ABIERTA");
                    __delay_ms(3000);
                }
            }

            // MODO 2: RÁPIDO (DIP_SW1 = 1, DIP_SW2 = 0)
            else if (DIP_SW1 == 1 && DIP_SW2 == 0) {
                // Verificación de Puerta
                if (Sensor_Puerta == 0) {
                    Candado = 0;
                    Anim_Candado(0);  // UNLOCKED
                    Anim_BUZZER();
                    __delay_ms(1500);
                } 
                else {
                    Candado = 1;
                    Anim_Candado(1);  // BLOCKED
                    __delay_ms(1000);

                    // ETAPA 1: LAVADO
                    Pausa_Boton();
                    LED_LAVADO = 1;
                    Anim_Lavado();
                    MotorH = 1; MotorL = 0; Anim_MotorH(); MotorH = 0; MotorL = 0;
                    MotorH = 0; MotorL = 1; Anim_MotorL(); MotorH = 0; MotorL = 0;
                    Pausa_Boton();
                    Drenado = 1; Anim_Drenado(); Drenado = 0;
                    LED_LAVADO = 0;

                    // ETAPA 2: ENJUAGUE
                    Pausa_Boton();
                    LED_ENJUAGUE = 1;
                    Valvula_Agua = 1; Anim_Valvula_Agua(); Valvula_Agua = 0;
                    Anim_Enjuague();
                    MotorH = 1; MotorL = 0; Anim_MotorH(); MotorH = 0; MotorL = 0;
                    Pausa_Boton();
                    Drenado = 1; Anim_Drenado(); Drenado = 0;
                    LED_ENJUAGUE = 0;

                    // ETAPA 3: CENTRIFUGADO
                    Pausa_Boton();
                    LED_CENTRI = 1;
                    Drenado = 1; MotorH = 1;
                    Anim_Centrifugado();
                    MotorH = 0; Drenado = 0;
                    LED_CENTRI = 0;

                    // FIN DE LAVADO
                    Pausa_Boton();
                    Candado = 0;
                    Anim_BUZZER();
                    Anim_BUZZER();
                    Lcd_CmdWrite(ClrScreen);
                    Lcd_CmdWrite(FirstLine);
                    Message_LCD("  LAVADO FIN   ");
                    Lcd_CmdWrite(SecondLine);
                    Message_LCD(" PUERTA ABIERTA");
                    __delay_ms(3000);
                }
            }
            
            // MODO 3: CENTRIFUGADO (DIP_SW1 = 0, DIP_SW2 = 0)
            else if (DIP_SW1 == 0 && DIP_SW2 == 0) {
                // Verificación de Puerta
                if (Sensor_Puerta == 0) {
                    Candado = 0;
                    Anim_Candado(0);  // UNLOCKED
                    Anim_BUZZER();
                    __delay_ms(1500);
                } 
                else {
                    Candado = 1;
                    Anim_Candado(1);  // BLOCKED
                    __delay_ms(1000);

                    // CENTRIFUGADO
                    Pausa_Boton();
                    LED_CENTRI = 1;
                    Drenado = 1; MotorH = 1;
                    Anim_Centrifugado();
                    Anim_Centrifugado();
                    Anim_Centrifugado();
                    Anim_Centrifugado();
                    Anim_Centrifugado();
                    MotorH = 0; Drenado = 0;
                    LED_CENTRI = 0;

                    // FIN DE LAVADO
                    Pausa_Boton();
                    Candado = 0;
                    Anim_BUZZER();
                    Anim_BUZZER();
                    Lcd_CmdWrite(ClrScreen);
                    Lcd_CmdWrite(FirstLine);
                    Message_LCD("  LAVADO FIN   ");
                    Lcd_CmdWrite(SecondLine);
                    Message_LCD(" PUERTA ABIERTA");
                    __delay_ms(3000);
                }
            }
        }
    }
}

// Act Lavadora Inteligente || Profesor: Aragon

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

#include <xc.h>
#define _XTAL_FREQ     20000000     // 20-MHz crystal frequency
#include "LIBRARY_LCD_LAVADORA.h"
#include <stdio.h>

// LEDS indicadores de estado
#define BOTON_PAUSA_PLAY RA1      // Pin 3 BOTON DE PAUSA Y PLAY
#define LED_PRE_LAVADO   RB4     // Pin 37 MODO PRE LAVADO
#define LED_LAVADO       RB5    // Pin 38 MODO LAVADO
#define LED_ENJUAGUE     RB6   // Pin 39 MODO ENJUAGUE
#define LED_CENTRI       RB7  // Pin 40 MODO CENTRIFUGADO


char data[16];

void Init_Ports(){
    ADCON1 = 0x0F;  // Apagar lecturas analógicas (Todos los pines a DIGITAL)
    
    //Entradas de la lavadora Puerto B
    TRISB0 = 1;    // Entrada Botón Ajuste
    TRISB1 = 1;   // Entrada Botón Inicio / Continuar
    TRISB2 = 1;  // Entrada Sensor de Puerta
    TRISB3 = 1;
    TRISB4 = 0;
    TRISB5 = 0;
    TRISB6 = 0;
    TRISB7 = 0;
    
    //Salidas de la lavadora Puerto C
    TRISC0 = 0;                     // Salida Candado
    TRISC1 = 0;                     // Salida BUZZER
    TRISC2 = 0;                     // Salida Drenado
    TRISA0 = 0;                     // Salida Valvula de agua
    TRISC6 = 0;                     // Salida MotorH
    TRISC7 = 0;                     // Salida MotorL
    
    //Salidas para la LCD (Puerto D y Puerto E
    LCD_Tris = 0;                   // Puerto D como salida (Datos LCD)
    LCD_Port = 0;                   // Limpiar Puerto D
    TRISE0 = 0;                     // Salida RS
    TRISE1 = 0;                     // Salida RW
    TRISE2 = 0;                     // Salida EN
}

// Hace parpadear los LEDs de los 4 ciclos para indicar pausa o ajuste
void Flash_LEDS(void){
    LED_PRE_LAVADO = !LED_PRE_LAVADO;   //El signo ! es un operador logico NOT 
    LED_LAVADO     = !LED_LAVADO;       //Ejemplo: Si el LED está apagado (0), !0 se convierte en 1 (encendido)
    LED_ENJUAGUE   = !LED_ENJUAGUE;     //Si el LED está encendido (1), !1 se convierte en 0 (apagado)
    LED_CENTRI     = !LED_CENTRI;
    __delay_ms(200);
}

// Alterna la alarma sonora/zumbador para avisos de estado o alerta
void Alerta_BUZZER(void){
    BUZZER = !BUZZER;
    __delay_ms(200);
}

volatile unsigned char Reset = 0;

void Enable_Interrupts(){
    INTEDG0 = 0;    // Flanco de bajada en RB0
    INTEDG1 = 0;    // Caída de flanco en RB1 (Boton_Inicio)
    INT0IF = 0;     // Limpiar bandera INT0
    INT1IF = 0;     // Limpia bandera del Boton_Inicio (Sustituye a RBIF = 0)
    GIE = 1;        // Habilitar interrupciones globales
    INT0IE = 1;     // Habilitar interrupción INT0 (Boton_Ajuste)
    INT1IE = 0;     // Limpia pulsaciones fantasma acumuladas durante el lavado
}                    //Se usa IF, no IE, para no desactivar el botón

// Declaración de banderas globales
volatile unsigned char flag_ajuste = 0;
volatile unsigned char flag_inicio = 0;
volatile unsigned char flag_pausa_play = 0;

void __interrupt() Interrupcion_Lavadora(void){
    // Evento por pulsación en RB0 (Boton_Ajuste vía INT0)
    if (INT0IF){
        flag_ajuste = 1; // Levanta la bandera para atenderla en el main
        INT0IF = 0;      // Borra la bandera de inmediato
    }

    // Evento por pulsación en RB1 (Boton_Inicio por INT1)
    if (INT1IF){
        flag_inicio = 1; // Levanta la bandera para atenderla en el main
        INT1IF = 0;      // Borra la bandera de hardware de inmediato
    }
}

// Función para pausar y congelar el lavado
void Pausa_Boton(void){
    // Verifica si el botón fue presionado (recibe 0V)
    if (BOTON_PAUSA_PLAY == 0) { 
        __delay_ms(50); //Dejar presionado el boton para hacer pausa
        if (BOTON_PAUSA_PLAY == 0) {
            Lcd_CmdWrite(ClrScreen);
            Lcd_CmdWrite(FirstLine);
            Message_LCD("   LAVADORA   ");
            Lcd_CmdWrite(SecondLine);
            Message_LCD("   EN PAUSA   ");
            // 1. Espera a que se suelte el botón para no registrar múltiples toques
            while(BOTON_PAUSA_PLAY == 0);
            __delay_ms(50);
            // 2. CONGELA LA LAVADORA aquí hasta que se vuelva a presionar
            while(BOTON_PAUSA_PLAY == 1);
            // 3. Espera a que se suelte el boton otra vez para continuar
            while(BOTON_PAUSA_PLAY == 0);
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

// TODAS LAS ANIMACIONES FUERON HECHAS POR IA, PERO LA ESTRUCTURA ES PROPIA
// POR ESO NO SE VE TAN HECHO POR UNA IA YA QUE TIENE LA ESTRUCTURA TIPICA QUE SE USA PARA LA LCD

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

// Animación de BUZZER
void Anim_BUZZER(void){
    Lcd_CmdWrite(ClrScreen);
    Lcd_CmdWrite(FirstLine);
    Message_LCD(" ALERTA SOUND");
    
    Lcd_CmdWrite(SecondLine);
    Lcd_DataWrite(2); // Altavoz 1
    Message_LCD(" BEEP! BEEP! ");
    Lcd_DataWrite(3); // Altavoz 2 con ondas
    __delay_ms(250);
}

// Animación Válvula de Agua (Gota cayendo)
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

// Animación Drenado (Olas de agua fluyendo)
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

// Animación Modo Pre-lavado (Espera con puntos)
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

// Animación Modo Lavado (Agitador girando)
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

// Animación Modo Enjuague (Agua agitada)
void Anim_Enjuague(void){
    Lcd_CmdWrite(ClrScreen);
    Lcd_CmdWrite(FirstLine);
    Message_LCD("CICLO: ENJUAGUE");
    Lcd_CmdWrite(SecondLine);
    Lcd_DataWrite(5); Message_LCD(" Limpiando "); Lcd_DataWrite(5);
    __delay_ms(200);
}

// Animación Modo Centrifugado (Giro rápido)
void Anim_Centrifugado(void){
    Lcd_CmdWrite(ClrScreen);
    Lcd_CmdWrite(FirstLine);
    Message_LCD("CENTRIFUGANDO");
    Lcd_CmdWrite(SecondLine);
    Message_LCD(">>> "); Message_LCD(" "); Message_LCD(" <<<");
    __delay_ms(100);
}

void main(void) {
    // Inicializacion
    Init_Ports();
    Init_Variables();
    Init_LCD();
    Init_Custom_Chars();
    Enable_Interrupts();

    // Mensaje inicial que muestra la LCD
    Instructions_Msg();

    while(1) {
        // Atención a la interrupción de Botón de Ajuste vía flag
        if (flag_ajuste == 1) {
            Alerta_BUZZER();
            flag_ajuste = 0;
        }
        // Espera a que se presione el Botón de Inicio o se active por interrupción
        if (flag_inicio == 1 || Boton_Inicio == 0) {
            flag_inicio = 0;
            Flash_LEDS();
            __delay_ms(200); // Anti-rebote para el botón
            
            // CONDICIÓN DE ERROR: Ambos interruptores en OFF (1,1)
            if (DIP_SW1 == 1 && DIP_SW2 == 1) {
                Msg_Error(); // Muestra el mensaje de error porque no se selecciona algun modo
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

            // MODO RÁPIDO (DIP_SW1 = 1, DIP_SW2 = 0)
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

                    // ETAPA 1: LAVADO (Omite Prelavado)
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
            
            // MODO 3 CENTRIFUGADO (DIP_SW1 = 0, DIP_SW2 = 0)
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
            
            flag_inicio = 0; // Estas dos líneas se utilizan para limpiar banderas
            INT1IF = 0;     // y reiniciar estados del sistema para que quede listo para un nuevo ciclo o evento
                            // Esto es para que cuando se haga una seleccion de lavado o un cambio de modo de lavado
                            // el Boton_Incicio siga siendo un boton de confirmacion   
            
        }
        
    }
}

/********************************* PRATICA 1 CAT341 ... *************************
Autor vers�o orginal: Alan Kardek
Adptado por: Gustavo Henrique 
***********************************************************************************/

// Este programa demonstra o controle dos pinos de I/O
// onde o estado de um bot�o, poder� ser identificado por meio de um led.

// DEFINI��O DAS VARI�VEIS INTERNAS DO PIC
#include   <18f4550.h>   // microcontrolador utilizado

// Configura��es para grava��o
#fuses HS,WDT,PROTECT,NOMCLR, NOLVP,NODEBUG,USBDIV,PLL5,CPUDIV1,VREGEN  // configura��o dos fus�veis

// Defini��es para uso de Rotinas de Delay
#use   delay(clock=20000000, RESTART_WDT) // Define um clock e limpa o wacthdog timer

// Defini��o e inicializa��o das vari�veis

// Este programa n�o utiliza nenhuma vari�vel de usu�rio

// Defini��o de Constantes internas

// Este programa n�o utiliza nenhuma constante de usu�rio

// Declara��o dos flags de software
// A defini��o de flags ajuda na programa��o e economiza mem�ria RAM.
// Este programa n�o utiliza nenhum flag de usu�rio

// Defini��o e inicializa��o dos ports

#use   fast_io(b)

// Defini��o dos endere�os de mem�ria das portas

#byte   portb = 0xf81 // Atribui ao registrador 0xf81 (PORTB) o nome de portb

// DEFINI��O DAS ENTRADAS
#bit   botao = portb.3  // atribui o nome botao ao pino RB3
                
// DEFINI��O DAS SA�DAS
#bit   led = portb.1 // atribui o nome led ao pino RB1
#bit   led2 = portb.4 // atribui o nome led2 ao pino RB4

// Configura��es do Microcontrolador
void main () {
   

   
   set_tris_b(0b11101101); // define os pinos RB1 e RB4 como saidas e RB0 como entrada 

  
   portb=0b00000010;         // coloca o pino RB1 como defaut em nivel alto e os demais em nivel baixo 


// Loop principal
   while(TRUE) {
      RESTART_WDT();
      if(botao== 1){     // testa bot�o est� em n�vel alto
         led = 0; // desliga led
         led2 = 1;// liga led2
         }  
      else { // o inverso 
      led=1;
      led2=0;       
   }
               // FIM DO PROGRAMA
 }
}

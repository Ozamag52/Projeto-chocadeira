/********************************* PRATICA 1 CAT341 ... *************************
Autor versão orginal: Alan Kardek
Adptado por: Gustavo Henrique 
***********************************************************************************/

// Este programa demonstra o controle dos pinos de I/O
// onde o estado de um botão, poderá ser identificado por meio de um led.

// DEFINIÇÃO DAS VARIÁVEIS INTERNAS DO PIC
#include   <18f4550.h>   // microcontrolador utilizado

// Configurações para gravação
#fuses HS,WDT,PROTECT,NOMCLR, NOLVP,NODEBUG,USBDIV,PLL5,CPUDIV1,VREGEN  // configuração dos fusíveis

// Definições para uso de Rotinas de Delay
#use   delay(clock=20000000, RESTART_WDT) // Define um clock e limpa o wacthdog timer

// Definição e inicialização das variáveis

// Este programa não utiliza nenhuma variável de usuário

// Definição de Constantes internas

// Este programa não utiliza nenhuma constante de usuário

// Declaração dos flags de software
// A definição de flags ajuda na programação e economiza memória RAM.
// Este programa não utiliza nenhum flag de usuário

// Definição e inicialização dos ports

#use   fast_io(b)

// Definição dos endereços de memória das portas

#byte   portb = 0xf81 // Atribui ao registrador 0xf81 (PORTB) o nome de portb

// DEFINIÇÃO DAS ENTRADAS
#bit   botao = portb.3  // atribui o nome botao ao pino RB3
                
// DEFINIÇÃO DAS SAÍDAS
#bit   led = portb.1 // atribui o nome led ao pino RB1
#bit   led2 = portb.4 // atribui o nome led2 ao pino RB4

// Configurações do Microcontrolador
void main () {
   

   
   set_tris_b(0b11101101); // define os pinos RB1 e RB4 como saidas e RB0 como entrada 

  
   portb=0b00000010;         // coloca o pino RB1 como defaut em nivel alto e os demais em nivel baixo 


// Loop principal
   while(TRUE) {
      RESTART_WDT();
      if(botao== 1){     // testa botão está em nível alto
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

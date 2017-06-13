#include<18F4550.h>
#include <stdlib.h>
#INCLUDE <math.h>
#fuses XT,NOWDT,PUT,NOBROWNOUT,NOLVP    // bits de configura��o
#use   delay(clock=20000000)// definir isso direito 
#priority timer0, timer1 // define a prioridade dos timer
//modo r�pido de inicializa��o das portas
#use   fast_io(a)
#use   fast_io(b)
#use   fast_io(d)
#use   fast_io(e)
#byte   porta = 0xf80 // atribui o nome "porta" ao registrador do port 0xf80
#byte   portb = 0xf81 
#byte   portd = 0xf83
#byte   porte = 0xf84
// pinos de entrada
#bit NTC = porta.0
#bit DHT11 = porta.1
#bit POT_SET_TEMP = porta.2
#bit POT_SET_UMI = porta.3
#bit BOT_BLK_LTH = portb.0
#bit BOT_MORE_INF = portb.1
// pinos de saida
#bit RELE_LAMP = portb.2
#bit PIN_IN1_MOTPAS = portb.3
#bit PIN_IN2_MOTPAS = portb.4
#bit PIN_IN3_MOTPAS = portb.5
#bit PIN_IN4_MOTPAS = portb.6
#bit PIN_COOLER = portb.7
float ntc_val = 0;
float rntc = 0;
float b1=0;
float c1=0;
float temp=0;
const float a = 0.0011303;
const float b = 0.0002339;
const double c = 0.00000008863;

#int_timer0

void interrupcao_ntc (){
set_adc_channel(0); //  escolhe o pino analogico para ler 
delay_us(10); // delay para come�ar a convers�o 
ntc_val = read_adc(); // le o valor na porta analogica
ntc_val *=5; // 
ntc_val/=1023;
rntc=33000/ntc_val;    
       rntc=rntc-10000;      
       b1=log(rntc);          
       b1=b1*b;               
       c1=log(rntc);          
       c1=pow(c1,3);          
       c1=c1*c;               
       temp=a+b1+c1;          
       temp=1/temp;           
       temp=temp-273.15;

// colocar aqui o codigo para obter o valor da temperatura
/// temos que amostrar a temperatura de tempos em tempos
// calculo do do estouro do timer 2
// verificar a temp a cada 30 segundos

}
#int_timer1
void interrupcao_dht11 (){














// colcoar aqui o codigo para obter o valor da umidade
// amostrar a umidade a cada 30 min
// calculo do estouro do timer 1


}



void main (void){
setup_adc(  ADC_CLOCK_INTERNAL  ); //  configura o clock utilizado para o conversor ad
setup_adc_ports( AN0_TO_AN3 );
//set_tris_a(0b00001111); // verificar se pinos analogicos necessitam de defini��o de tris
set_tris_b(0b11000000);  // define os pinos RB0 E RB1 como entrada e os demais como saida
set_tris_b(0b11000000);
set_tris_d(0b00000000);
set_tris_e(0b00000100);
 // CONFIGURA��O DO TIMER 0 E TIMER 1 P/ INTERRUP��O
   /* setup_timer_0 (RTCC_INTERNAL | RTCC_8_BIT | RTCC_DIV_64);   ISSO � SO UM ESCOPO CONFIGURAR ISSO DA FORMA ADEQUADA 
    setup_timer_1 (T1_EXTERNAL | T1_DIV_BY_2);
    enable_interrupts(GLOBAL);//chave principal das interrup��es
    enable_interrupts(int_timer0); // interrup��o do TIMER 0*/


  while(true){
  
  
  
  
  
  // take the loop here
  
  
  
  
  
  
  
  
  
  }
  
  

































}

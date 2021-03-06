#include<18F4550.h>
#include <stdlib.h>
#INCLUDE <math.h>
#fuses XT,NOWDT,PUT,NOBROWNOUT,NOLVP    // bits de configura��o
#use   delay(clock=40000000)// definir isso direito 
#use   fast_io(a)
#use   fast_io(b)
#use   fast_io(c)
#use   fast_io(d)
#use   fast_io(e)

#byte   porta = 0xf80
#byte   portb = 0xf81
#byte   portc = 0xf82
#byte   portd = 0xf83
#byte   porte = 0xf84
#bit NTC = porta.0
#bit RELE_LAMP = portb.2
#bit PIN_COOLER = portb.7
#bit rs = porte.0     // via do lcd que sinaliza recep��o de dados ou comando
#bit enable = porte.1 // enable do lcd
#byte   DISPLAY = portd  //tem que ser a mesma porta escolhida dentro do arquivo lcd.h
// Bibliotecas
#include   <lcd.h>

float ntc_val = 0;
float rntc = 0;
float b1=0;
float c1=0;
float temp=0;
float temperatura;
const float a = 0.0011303;
const float b = 0.0002339;
const double c = 0.00000008863;

      float ntc(){
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
             return temp;
      
      }
 
 void main (void){
 
         setup_adc(  ADC_CLOCK_INTERNAL  ); 
         setup_adc_ports( AN0_TO_AN3 );
         set_tris_a(0b00001111); 
         set_tris_b(0b11000000); 
         set_tris_c(0b11111111);
      set_tris_d(0b00000000);
      set_tris_e(0b00000100);
            porta=0x00;            // limpa porta
      portb=0x00;            // limpa portb
      portc=0x00;            // limpa portc
      portd=0x00;            // limpa portd
      porte=0x00;            // limpa porte

      inicializa_lcd();         // configura o lcd
         
                  while(true){
                 temperatura = ntc();
                 comando_lcd(0x83);      // posiciona o cursor no endere�o 83h
      printf(escreve_lcd, "ola");
      delay_ms(1000);
       limpa_lcd();
      delay_ms(1000);
                 
                 if(temperatura<=38){
                 PIN_COOLER =0 ;
                 RELE_LAMP = 1 ;
                }
                if(temperatura>38){
                PIN_COOLER =1 ;
                RELE_LAMP = 0 ; 
                }
                delay_ms(2000);   
              }
  
}

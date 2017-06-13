#include<18F4550.h>
#include <stdlib.h>
#INCLUDE <math.h>
#fuses XT,NOWDT,PUT,NOBROWNOUT,NOLVP    // bits de configura��o
#use   delay(clock=20000000)// definir isso direito 
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
//#bit Data_Pin = 0x06.7������������//�Pin�mapped�to�PORTB.7      //CONFIGURAR ISSO PARA O PINO CERTO E DO 18F4550
//#bit Data_Pin_Direction�=�0x86.7�������������//�Pin�direction�mapped�to�TRISB.7
char message2[] = "RH���=�00.0�%";
short Time_out;
unsigned  int8 RH_byte1, RH_byte2, CheckSum;

float ntc_val = 0;
float rntc = 0;
float b1=0;
float c1=0;
float temp=0;
float temperatura;
float umidade;
const float a = 0.0011303;
const float b = 0.0002339;
const double c = 0.00000008863;
// FUN��O POR INICIAR A DHT11

void start_signal(){
��Data_Pin_Direction�=�0;��������������//�Configure�connection�pin�as�output
��Data_Pin�=�0;������������������������//�Connection�pin�output�low
��delay_ms(25);
��Data_Pin�=�1;������������������������//�Connection�pin�output�high
��delay_us(30);
��Data_Pin_Direction�=�1;��������������//�Configure�connection�pin�as�input
}

// FUN�A� POR VERIFICAR SE O SENSOR ESTA RESPONDENDO 
short check_response(){
  delay_us(40); 
  if(!Data_Pin){���������������������//�Read�and�test�if�connection�pin�is�low
����delay_us(80);
����if(Data_Pin){��������������������//�Read�and�test�if�connection�pin�is�high
������delay_us(50);
������return�1;}
����}
}


// FUN��O PARA A OBTEN��O DO VALOR DE HUMIDADE
unsigned�int8�Read_Data(){
��unsigned�int8�i,�k,�_data�=�0;�����//�k�is�used�to�count�1�bit�reading�duration
��if(Time_out)
����break;
��for(i�=�0;�i�<�8;�i++){
����k�=�0;
����while(!Data_Pin){��������������������������//�Wait�until�pin�goes�high
������k++;
������if�(k�>�100)�{Time_out�=�1;�break;}
������delay_us(1);}
����delay_us(30);
����if(!Data_Pin)
������bit_clear(_data,�(7�-�i));���������������//�Clear�bit�(7�-�i)
����else{
������bit_set(_data,�(7�-�i));�����������������//�Set�bit�(7�-�i)
������while(Data_Pin){�������������������������//�Wait�until�pin�goes�low
������k++;
������if�(k�>�100)�{Time_out�=�1;�break;}
������delay_us(1);}
����}
��}
��return�_data;
}

// FUN��O PRINCIPAL QUE RETORNARA A UMIDADE 
void dht11(){
   delay_ms(1000);
   Time_out�=�0;
   
   Start_signal();
   if(check_response()){��������������������//�If�there�is�response�from�sensor
   RH_byte1�=�Read_Data();�����������������//�read�RH�byte1
   RH_byte2�=�Read_Data();�����������������//�read�RH�byte2
   Checksum�=�Read_Data();�����������������//�read�checksum             ������
         �if(CheckSum�==�((RH_Byte1�+�RH_Byte2)�&�0xFF)){           ��������
            �message2[7]��=�RH_Byte1/10�+�48;
             message2[8]��=�RH_Byte1%10�+�48;
             message2[10]�=�RH_Byte2/10�+�48;
             /*��lcd_putc('\f');�����������������������//�LCD�clear
             lcd_gotoxy(1,�2);���������������������//�Go�to�column�1�row�2
             printf(lcd_putc,�message2);�����������//�Display�message2 */                                
           }
                     �� }
   return message2;
}





void_ntc (){
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
setup_adc(  ADC_CLOCK_INTERNAL  ); //  configura o clock utilizado para o conversor ad
setup_adc_ports( AN0_TO_AN3 );
//set_tris_a(0b00001111); // verificar se pinos analogicos necessitam de defini��o de tris
set_tris_b(0b11000000);  // define os pinos RB0 E RB1 como entrada e os demais como saida
set_tris_b(0b11000000);
set_tris_d(0b00000000);
set_tris_e(0b00000100);
 


  while(true){
  
  temperatura = ntc();
  umidade = dht11();
  
  // take the loop here
  
  
 
  
  
  
  
  
  
  }
  
  

































}

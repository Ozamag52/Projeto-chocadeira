
#INCLUDE<18F4550.h>//   Inclui a biblioteca respos�vel por estabelecer os parametros do pic utilizado no projeto
#DEVICE ADC =10  // seleciona o conversor a/d com 10 bits
#INCLUDE <stdlib.h>  
#INCLUDE <math.h> //  Inclui a biblioteca responsavel por fornecer fun��es de calculos matematicos 
#define  t_filtro 400 
#fuses HS,NOWDT,PUT,NOBROWNOUT,NOLVP   // configura��o dos fuses do pic 
#use   delay(clock=20000000)// Informa a fun��o delay, o clock utilizado
#BIT Data_Pin = 0x06.7                       // Pin mapped to PORTB.7
#BIT Data_Pin_Direction = 0x86.7             // Pin direction mapped to TRISB.7
#use   fast_io(a)//modo r�pido de inicializa��o das portas
#use   fast_io(b)//modo r�pido de inicializa��o das portas
#use   fast_io(c)
#use   fast_io(d)//modo r�pido de inicializa��o das portas
#use   fast_io(e)//modo r�pido de inicializa��o das portas

#byte   porta = 0xf80 // atribui o nome "porta" ao registrador do port 0xf80
#byte   portb = 0xf81 
#byte   portd = 0xf83
#byte   porte = 0xf84
#byte   portc = 0xf82
// pinos de entrada e saida 
#bit NTC = porta.0    //  Pino de leitura do NTC 
#bit PIN_COOLER = portc.0  //  Pino de leitura do DHT11
#bit POT_SET_TEMP = porta.1 //Pino de leitura do potenciometro de set da temperatura 
#bit POT_SET_UMI = porta.2 //Pino de leitura do potenciometro de set da umidade 
#bit BOT_BLK_LTH = portb.0 // Pino do botao paraligar a luz do lcd
#bit BOT_MORE_INF = portb.1 // botao para mais informa�oes no lcd
#bit    config_Bot  = portb.4 // botao para navega��o no lcd; auxilia na configura�ao dos parametros
// pinos de saida
#bit PIN_SERV = portb.3
#bit RELE_LAMP = portb.2  //  pino que acionamento do rel� da lampada/aquecedor
#bit DHT11 = portb.7 // Pino de acionamento do cooler via transistor 
#bit rs =porte.0 //  via do lcd que sinaliza recep��o de dados ou comando 
#bit enable = porte.1 // habilita o lcd
#byte DISPLAY = portd //  seleciona o port no qual o lcd esta ligado ( o mesmo valor que esta no arquivo.h)
#INCLUDE <lcd.h> // inclui a biblioteca do modulo lcd


char message2[] = "RH���=�00.0�%"; //  vetor de mensagem para a umidade 
short Time_out; //  variavel para as fun�oes referentes ao sensor de umidade dht11
unsigned  int8 RH_byte1, RH_byte2, CheckSum; //  variaveis para as fun�oes do dht11

float ntc_val = 0; // variavel responsavel por receber o valor digital do ntc
float rntc = 0; //  valor da resistencia do divisor de tensao do ntc 
float b1=0;  //  variavel auxiliar para fun��o do ntc
float c1=0;//  variavel auxiliar para fun��o do ntc
float temp=0;//  variavel auxiliar para fun��o do ntc
float temperatura=0;//  variavel auxiliar para fun��o do ntc
char umidade;//  variavel auxiliar para fun��o do ntc
 double a = 0.0011303; //  constantes fisicas do sensor ntc 10k
 double b = 0.0002339; //  constantes fisicas do sensor ntc 10k
 double c = 0.00000008863; //  constantes fisicas do sensor ntc 10k
 int cont=0;
 long valor_AD; // recebe o valor da conversao AD
 long INT filtro_b1  = t_filtro;
 int flags=0;            // utilizada para no deboccing para sinalizar
int cont_config=0;            // variavel q sinaliza quando o botao configura�ao � pressionado por mais de 500ms
float value1;         // auxilia na convers�o do valor OBTIDO APOS ADC para TEMPERATURA E UMIDADE
float set_temp =0;   // valor de refeencia pra temperatura maxima
float set_umi =0; // valor de referencia para a umidade
#bit  BTN_B1 = flags.0 


// FUN��O POR INICIAR A DHT11

void start_signal(){
  Data_Pin_Direction = 0;              // Configure connection pin as output
  Data_Pin = 0;                        // Connection pin output low
  delay_ms(25);
  Data_Pin = 1;                        // Connection pin output high
  delay_us(30);
  Data_Pin_Direction = 1;              // Configure connection pin as input
}

// FUN�A� POR VERIFICAR SE O SENSOR ESTA RESPONDENDO 

  short check_response(){
  delay_us(40);
  if(!Data_Pin){                     // Read and test if connection pin is low
    delay_us(80);
    if(Data_Pin){                    // Read and test if connection pin is high
      delay_us(50);
      return 1;}
    }
}


// FUN��O PARA A OBTEN��O DO VALOR DE HUMIDADE
unsigned int8 Read_Data(){
  unsigned int8 i, k, _data = 0;     // k is used to count 1 bit reading duration
  if(Time_out)
    break;
  for(i = 0; i < 8; i++){
    k = 0;
    while(!Data_Pin){                          // Wait until pin goes high
      k++;
      if (k > 100) {Time_out = 1; break;}
      delay_us(1);}
    delay_us(30);
    if(!Data_Pin)
      bit_clear(_data, (7 - i));               // Clear bit (7 - i)
    else{
      bit_set(_data, (7 - i));                 // Set bit (7 - i)
      while(Data_Pin){                         // Wait until pin goes low
      k++;
      if (k > 100) {Time_out = 1; break;}
      delay_us(1);}
    }
  }
  return _data;
}

// FUN��O PRINCIPAL QUE RETORNARA A UMIDADE 

char dht11(){
   delay_ms(1000);
   Time_out = 0;
   Start_signal();
   if(check_response()){//�If�there�is�response�from�sensor
   RH_byte1=Read_Data();//�read�RH�byte1
   RH_byte2=Read_Data();//�read�RH�byte2
   Checksum=Read_Data();//�read�checksum             ������
      if(CheckSum==((RH_Byte1+RH_Byte2)& 0xFF)){
      message2[7]=RH_Byte1/10+48;
             message2[8]= RH_Byte1%10+48;
             message2[10] = RH_Byte2/10+48;                            
       }
   } 
   return message2;
}



// Fun��o Para conversao do valor obtido na ADC, para o fundo de escala trabalhando para a temperatura 0->50 
void conversao_temp (){  

      set_adc_channel(2); // seleciona o canal 2 do pic, para a ADC
      delay_us(10);
      while(!adc_done()); // espera o fim enquanto a ADC 
      valor_AD = read_adc(); // lendo o valor da ADC
      value1= (float)valor_AD; // passa o valor de 0 a 1023 para float; atribiu este a variavel auxiliar value1
      value1=value1*0.5/10.23; // Faz o ajuste de escala 0�->0 e 50�->1023; o valor 0.5 � o resolu�ao
      
      value1 = (float)((int)((value1 + 0.4f) * 2.0f)) * 0.5f;
}

// Fun��o Para conversao do valor obtido na ADC, para o fundo de escala trabalhando para a umidade 0->100 
void conversao_umi (){

     set_adc_channel(2);// seleciona o canal 2 do pic, para a ADC
      delay_us(10);
      while(!adc_done());// espera o fim enquanto a ADC 
      valor_AD = read_adc();// lendo o valor da ADC
      value1= (float)valor_AD; // passa o valor de 0 a 1023 para float; atribiu este a variavel auxiliar value1
      value1= value1*10/102.3;// Faz o ajuste de escala 0->0 e 100->1023; o valor 10 � o resolu�ao
      value1 = (float)((int)((value1 + 0.4f) * 2.0f)) * 0.5f;
}


// Fun�ao para a configura�ao dos parametros de temperatura e umidade da incubadora
void configuracao () {

                      cont_config++;// toda vez que o botao � pressionado o contador � incrementado
                      limpa_lcd (); // limpa o lcd
                    if(cont_config==5){
                       cont_config=0;
                    }
                    if(cont_config==1){
                        printf(escreve_lcd, "CONFIGURACAO");
                        comando_lcd(0xC0);
                        printf(escreve_lcd, "INCUBADORA");
                       
                        delay_ms (1000);
                    
                    }
                   while(cont_config==1){ // neste primeiro la�o a temperatura max. dever� ser definida por meio do poteciometro
                        conversao_temp ();// chama a fun�ao para a conversao 
                        printf(escreve_lcd, "CONFIGURACAO");
                        comando_lcd(0xC0);// posiciona o cursor no endere�o 83h
                 
                        printf(escreve_lcd, "SET TEMP: %.1f       ", value1);  // escrever no lcd os valores de temperatura correpontedes a varia�ao do potenciometro
                        delay_ms (400); //  delay necessario para que o estado da porta onde o botao est� ligado volte ao estado incial apos o aperto.
                     
                      
                        
                        if(config_Bot==0){ // ao pressionar o bot�o por um tempo superior a 500ms Ffinaliza o primeiro la�o e entra no segundo la�o.
                            limpa_lcd ();
                            set_temp = value1; // ATUALIZA�AO DO VALOR DA VARIAVEL DE REFERENCIA APOS A ESCOLHA DO VALOR ATRAV�S DO POTENCIOMETRO
                             cont_config=2;
                        }
                   } // fim do 1 la�o
                        while(cont_config==2){ // Neste segundo la�o ser� defenido o valor da temperatura min. e imprimmi no lcd
                           conversao_umi (); 
                          printf(escreve_lcd, "CONFIGURACAO");
                           comando_lcd(0xC0);
                           printf(escreve_lcd, "SET UMI: %.1f      ", value1);
                           delay_ms (400);
                            
                           
                      
                           if(config_Bot==0){
                               limpa_lcd ();
                               set_umi = value1; // ATUALIZA�AO DO VALOR DA VARIAVEL DE REFERENCIA APOS A ESCOLHA DO VALOR ATRAV�S DO POTENCIOMETRO
                               cont_config=4;
                           }
                        }//fim do 2 la�o
                            
                              while(cont_config==4){ // fim da configura��o 
                              
                              comando_lcd(0x80);
                              printf(escreve_lcd, "SET TEMP:  %.1f        ", set_temp);
                              delay_ms (2000);
                             
                              comando_lcd(0xC0);
                              printf(escreve_lcd, "SET UMI:   %.1f       ", set_umi);
                              delay_ms (2000);
                              
                              cont_config=5;
                              
                              
                              limpa_lcd();
                              comando_lcd(0x80);
                              printf(escreve_lcd, "FIM DA");
                              comando_lcd(0xC0);
                              printf(escreve_lcd, "CONFIGURACAO");
                              delay_ms (1000);
                              limpa_lcd();
                              
                              comando_lcd(0x80);
                              printf(escreve_lcd, "Aperte o botao");
                              comando_lcd(0xC0);
                              printf(escreve_lcd, "INICIAR");
                              delay_ms (1000);
                              /* 
                              #### AQUI TERMINA A CONFIGURA��O DOS PAR�METROS DA INCUBADORA
                                  AO PRESSIONAR O BOTAO DE CONFIGURA��O  2 VEZES VOLTA-SE PARA CONFIGURAR NOVAMENTE OS
                                  PAR�METROS.
                              
                              
                              */
                             }
                          



} // FIM DA FUN��O CONFIGURA�AO










float ntc(){
set_adc_channel(0); //  escolhe o pino analogico para ler  
delay_us(10); // delay para come�ar a convers�o
read_adc(ADC_START_ONLY );
while(!adc_done()){
}
ntc_val = read_adc(ADC_READ_ONLY ); // le o valor na porta analogica
ntc_val *=5; // multiplica o valor digitalizado pelo valor de tens�o de alimenta��o
ntc_val/=1023;  // divide  o valor acima por 1023
// Inicio calculo de equa��o de  Steinhart & Hart 
rntc=50000/ntc_val;     
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
// Fim do calculo de equa��o de  Steinhart & Hart
}




void mexeovos () {
// Essa fun��o � respons�vel por controlar o servo motor que mexe os ovos
for( int j=0; j<=10; j++){  // j=50 � cerca de 5 min 
 // para garantir que a temperatura varie pouco enquanto o ovosest�o mexendo fa�a  
   limpa_lcd();
   comando_lcd(0x83); //  posiciona o cursor no endere�o 0x83
   printf(escreve_lcd,"MEXENDO" );
   if(j%2==0){
   RELE_LAMP = 1;
   PIN_COOLER = 0;
   }
     if(j%2!=0){
     RELE_LAMP = 0;
     PIN_COOLER =1;
     }
// termina aqui esse arficio 
      for(int i=0;  i<50; i++){                      
      PIN_SERV=1;
      delay_us(800);
      PIN_SERV=0;
      delay_us(19200);
      }
      delay_ms(2000);
      for(i=0;  i<50; i++){
      PIN_SERV=1;
      delay_us(1500);
      PIN_SERV=0;
      delay_us(18500);
      }
      delay_ms(2000);
 }
   
}

void main (void){
setup_adc(ADC_CLOCK_DIV_32); //  configura o clock utilizado para o conversor ad e o pior clock ele usa a malha RC � lenta 
setup_adc_ports( AN0_TO_AN2 ); // selecionaos pinos A0 a A2 como pinos de sinais analogicos 
set_tris_a(0b00001111); // verificar se pinos analogicos necessitam de defini��o de tris
set_tris_b(0b11010000);  
set_tris_c(0b00000000);// define os pinos RB0 E RB1 como entrada e os demais como saida
set_tris_d(0b00000000);  //  Idem como supracitado acima 
set_tris_e(0b00000100);
inicializa_lcd(); // fun��o responsavel por inicializar o modulo lcd

  while(true){
  temperatura = ntc(); //  le o valor  de temperatura em graus celsius que a fun��o do ntc retorna 
  umidade = dht11();


if (!config_Bot){   // o botao do motor 1 foi pressionado?
            if (! (BTN_B1)){ // O bot�o UP j� foi tratato ?
            
               // n�o.
               if (filtro_b1 != 0) // Fim do filtro do bot�o UP ?
                  filtro_b1--; // N�o, ent�o decrementa o filtro
                  else { 
                      configuracao (); // ao clicar no bot�o de configura�ao chama a fun��o de configura��o, onde ser�o passados os parametros de temperatura e umidade
                  }
            }        
         } 
          else{
              filtro_b1 = t_filtro;
              BTN_B1 = 0; 
      } 












    // implementa��o da rotina de controle de  temperatura 
   if(temperatura<=set_temp){   //  verifica se da temperatura de menor que 38 graus e faz  a rotina 
     PIN_COOLER =0 ;
     RELE_LAMP = 1 ;
    }
    if(temperatura>set_temp){
    PIN_COOLER =1 ;
    RELE_LAMP = 0 ; 
    }
    //delay_ms(2000);   
  }
  
  


































}

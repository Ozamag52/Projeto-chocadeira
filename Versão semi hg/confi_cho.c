
#INCLUDE<18F4550.h>//   Inclui a biblioteca respos�vel por estabelecer os parametros do pic utilizado no projeto
#DEVICE ADC =10  // seleciona o conversor a/d com 10 bits
#INCLUDE <stdlib.h>  
#INCLUDE <math.h> //  Inclui a biblioteca responsavel por fornecer fun��es de calculos matematicos 
#fuses HS,NOWDT,PUT,NOBROWNOUT,NOLVP   // configura��o dos fuses do pic 
#define  t_filtro 400 
#use   delay(clock=20000000)// Informa a fun��o delay, o clock utilizado
#BIT Data_Pin = 0x06.7                       // Pin mapped to PORTB.7
#BIT Data_Pin_Direction = 0x86.7             // Pin direction mapped to TRISB.7

   //modo r�pido de inicializa��o das portas
   #use   fast_io(a)
   #use   fast_io(b)
   #use   fast_io(d)
   #use   fast_io(e)
   
   // atribui o nome "porta" ao registrador  port 
   #byte   porta = 0xf80 
   #byte   portb = 0xf81 
   #byte   portd = 0xf83
   #byte   porte = 0xf84
   #byte   portc = 0xf82
   
   // pinos de entrada 
   #bit NTC = porta.0    //  Pino de leitura do NTC 
   #bit PIN_COOLER = portc.0  //  Pino de leitura do DHT11
  // #bit POT_SET_TEMP = porta.1 //Pino de leitura do potenciometro de set da temperatura 
   //#bit POT_SET_UMI = porta.2 //Pino de leitura do potenciometro de set da umidade 
   #bit BOT_BLK_LTH = portb.0 // Pino do botao paraligar a luz do lcd
   #bit config_Bot  = portb.4 // botao para navega��o no lcd; auxilia na configura�ao dos parametros
   #bit BOT_INI  = portb.1 // botao para navega��o no lcd; auxilia na configura�ao dos parametros
  
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
int cont=0; // auxila no que vai ser mostrado na tela default
float cont_int = 0;
int horas=0;
      //vari�veis
      int cont_ini=0; // variavel contadora utilizada todda vez q o btao inciar � pressionado
      int cont_aux=0;
      int cont_rotina=0;// usada pra fazer o la�o de rotina do progrma quando o botao iniciar for pressionado
      long valor_AD; // recebe o valor da conversao AD
      long INT filtro_b1  = t_filtro;
      long INT filtro_INI  = t_filtro;
      int flags=0;            // utilizada para no deboccing para sinalizar
      int cont_config=0;            // variavel q sinaliza quando o botao configura�ao � pressionado por mais de 500ms
      float value1;         // auxilia na convers�o do valor OBTIDO APOS ADC para TEMPERATURA E UMIDADE
      float set_temp =0;   // valor de refeencia pra temperatura maxima
      float set_umi =0; // valor de referencia para a umidade
      #bit  BTN_B1 = flags.0 // flag do botao configura�ao
      #bit  BTN_INI = flags.1 // flag do botao inciar
      #bit  UNLOCK = flags.2 // flag para bloquear o inicio do programa se a configura��o nao tiver sido feita



// Interrup��o para acionar o servo motor a cada 3 horas
#int_timer0
void trata_tmr0 ()
{
   set_timer0(131 + get_timer0());
   cont_int++;
   if(cont_int > 2246400) // uma hora 2246400  MIN 37440
   {
      cont_int = 0;
      horas++;      
   }
   
 
}



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
   limpa_lcd();
   comando_lcd(0x83); //  posiciona o cursor no endere�o 0x83
   printf(escreve_lcd,"MEXENDO" );
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



// Fun��o Para conversao do valor obtido na ADC, para o fundo de escala trabalhando para a temperatura 0->50 
void conversao_temp (){  
       set_adc_channel(2); //  escolhe o pino analogico para ler  
       delay_us(10); // delay para come�ar a convers�o
       read_adc(ADC_START_ONLY );
      while(!adc_done()){
      }
      valor_AD = read_adc(ADC_READ_ONLY ); // le o valor na porta analogica
    
      value1= (float)valor_AD; // passa o valor de 0 a 1023 para float; atribiu este a variavel auxiliar value1
      value1=value1*0.5/10.23; // Faz o ajuste de escala 0�->0 e 50�->1023; o valor 0.5 � o resolu�ao
      
      value1 = (float)((int)((value1 + 0.4f) * 2.0f)) * 0.5f;
}

// Fun��o Para conversao do valor obtido na ADC, para o fundo de escala trabalhando para a umidade 0->100 
void conversao_umi (){

      set_adc_channel(2); //  escolhe o pino analogico para ler  
      delay_us(10); // delay para come�ar a convers�o
      read_adc(ADC_START_ONLY );
      while(!adc_done()){
      }
     valor_AD= read_adc(ADC_READ_ONLY ); // le o valor na porta analogica
     
      value1= (float)valor_AD; // passa o valor de 0 a 1023 para float; atribiu este a variavel auxiliar value1
      value1= value1*10/102.3;// Faz o ajuste de escala 0->0 e 100->1023; o valor 10 � o resolu�ao
      value1 = (float)((int)((value1 + 0.4f) * 2.0f)) * 0.5f;
}

// Fun�ao para a configura�ao dos parametros de temperatura e umidade da incubadora
void configuracao () {
                 
                     cont_config++;// toda vez que o botao � pressionado o contador � incrementado
                     limpa_lcd (); // limpa o lcd
                    if(cont_config==7){
                       cont_config=0;
                    }
                    if(cont_config==1){
                        printf(escreve_lcd, "CONFIGURACAO");
                        comando_lcd(0xC0);
                        printf(escreve_lcd, "INCUBADORA");
                         delay_ms (3000);
                    
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
                            
                              while(cont_config==4){ 
                                 cont_aux++;
                             if (cont_aux==1){
                              limpa_lcd();
                              comando_lcd(0x80);
                              printf(escreve_lcd, "FIM DA");
                              comando_lcd(0xC0);
                              printf(escreve_lcd, "CONFIGURACAO");
                              delay_ms (1000);
                              
                              limpa_lcd();
                              comando_lcd(0x80);
                              printf(escreve_lcd, "SET TEMP:  %.1f        ", set_temp);
                              comando_lcd(0xC0);
                              printf(escreve_lcd, "SET UMI:   %.1f       ", set_umi);
                              delay_ms (3000);
                              cont_aux++;
                         
                             }
                              
                           
                              
                         limpa_lcd();
                         comando_lcd(0x80);
                         printf(escreve_lcd, "P/VOLTAR A ");
                         comando_lcd(0xC0);
                         printf(escreve_lcd, "CONFIGURACAO");
                         delay_ms (1000);
                         limpa_lcd();
                         comando_lcd(0x80);
                         printf(escreve_lcd, "PRESSIONE O ");
                         comando_lcd(0xC0);
                         printf(escreve_lcd, "BOT. CONFIGURACAO");
                         delay_ms (1000);
                         limpa_lcd();
                         comando_lcd(0x80);
                         printf(escreve_lcd, "OU PRESSIONE");
                         comando_lcd(0xC0);
                         printf(escreve_lcd, "BOTAO INICIAR");
                         delay_ms (1000);
                         limpa_lcd();
                              
                              if(config_Bot==0){
                              limpa_lcd ();
                              cont_config=1;
                              cont_aux=0;
                              printf(escreve_lcd, "CONFIGURACAO");
                              comando_lcd(0xC0);
                              printf(escreve_lcd, "INCUBADORA");
                              delay_ms (3000);
                               
                           }
                             if(BOT_INI==0){
                               limpa_lcd ();
                               comando_lcd(0x80);
                               printf(escreve_lcd, "INCIANDO A ");
                               comando_lcd(0xC0);
                               printf(escreve_lcd, "INCUBADORA");
                               delay_ms (4000);
                               cont_aux=0;
                               cont_config=0;
                               UNLOCK=1;
                               cont=1;
                           
                           }
                              
                         }// fim do 3 la�o
                         
                         
                       
 } // fim do 1 la�o


} // FIM DA FUN��O CONFIGURA�AO







void main (void){

      // configura os tris
      set_tris_a(0b00001111); 
      set_tris_b(0b10010010);  
      set_tris_c(0b00000000);
      set_tris_d(0b00000000);  
      set_tris_e(0b00000100);
      
      // inicializa os ports
      porta=0x00;            // limpa porta
      portb=0x00;            // limpa portb
      portc=0x00;            // limpa portc
      portd=0x00;            // limpa portd
      porte=0x00;            // limpa porte
      
      setup_adc(ADC_CLOCK_DIV_32); //  configura o clock utilizado para o conversor ad e o pior clock ele usa a malha RC � lenta 
      setup_adc_ports( AN0_TO_AN2 ); // selecionaos pinos A0 a A2 como pinos de sinais analogicos 
      inicializa_lcd(); // fun��o responsavel por inicializar o modulo lcd
          
          
          comando_lcd(0x80); //  posiciona o cursor no endere�o 0x83
          printf(escreve_lcd,"INCUBADORA");
          comando_lcd(0xC0); //  posiciona o cursor no endere�o 0x83
          printf(escreve_lcd,"DE OVOS");
          delay_ms(3000);
          comando_lcd(0x80); //  posiciona o cursor no endere�o 0x83
          printf(escreve_lcd,"APERTE O BOTAO");
          comando_lcd(0xC0); //  posiciona o cursor no endere�o 0x83
          printf(escreve_lcd,"CONFIGURACAO");
          delay_ms(3000);
          
            // CONFIGURA?+O DO TIMER0
               setup_timer_0 (RTCC_INTERNAL | RTCC_8_BIT | RTCC_DIV_64);
               set_timer0(131);
               enable_interrupts(GLOBAL);//chave principal das interrup??es
               enable_interrupts(int_timer0); // interrup??o do TIMER 0
            
            
  while(true){
  
   
         
          if (cont==1){ // escreve a mensagem apenas uma vez no lcd  
          limpa_lcd();
          comando_lcd(0x80); //  posiciona o cursor no endere�o 0x83
          printf(escreve_lcd,"APERTE O BOTAO");
          comando_lcd(0xC0); //  posiciona o cursor no endere�o 0x83
          printf(escreve_lcd,"INI OU CONFIG");
          cont=0;
          }
         if( UNLOCK==1){  
          if (!BOT_INI){   // o botao do motor 1 foi pressionado?
            if (! (BTN_INI)){ // O bot�o UP j� foi tratato ?
            
               // n�o.
               if (filtro_INI != 0) // Fim do filtro do bot�o UP ?
                  filtro_INI--; // N�o, ent�o decrementa o filtro
                  else { 
                       limpa_lcd();
                               comando_lcd(0x80); //  posiciona o cursor no endere�o 0x83
                               printf(escreve_lcd,"AJUSTANDO");
                              
                               comando_lcd(0xC0); //  posiciona o cursor no endere�o 0x83
                               printf(escreve_lcd,"PARAMENTROS...");
                               delay_ms(2000);
                               cont_rotina++;
                       
                               while (cont_rotina==1 ){
                                     
                                     
                                     temperatura = ntc(); //  le o valor  de temperatura em graus celsius que a fun��o do ntc retorna
                                     umidade = dht11();
                                       if(horas == 3){ //  HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
                                       mexeovos();
                                       horas=0;
                                       }
   
                                    if(cont_ini==0 ){ // aqui fica a rotina princiapal mostranso os valorres de temperatura e umidade
                                     comando_lcd(0x80); //  posiciona o cursor no endere�o 0x83
                                    
                                     printf(escreve_lcd,"UMIDADE:00000 ");
                                  
                                     comando_lcd(0xC0); //  posiciona o cursor no endere�o 0x83
                                     printf(escreve_lcd,"TEMP. %f",temperatura);
                                    }
                                     if(cont_ini==1 ){
                                     comando_lcd(0x80);
                                     printf(escreve_lcd,"DESEJA PARAR A ");
                                     comando_lcd(0xC0); //  posiciona o cursor no endere�o 0x83
                                     printf(escreve_lcd,"INCUBACAO?");
                                     delay_ms(500);
                                     
                                    
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
                                     delay_ms(500);
                                     if (BOT_INI==0){
                                           if (cont_ini==0){
                                          limpa_lcd();
                                          printf(escreve_lcd,"AGURADE...");
                                          delay_ms(1000);
                                          cont_ini=1;
                                           }
                                     }
                                     if (BOT_INI==0){
                                       if (cont_ini==1){
                                          limpa_lcd();
                                          printf(escreve_lcd,"OK");
                                          comando_lcd(0xC0); //  posiciona o cursor no endere�o 0x83
                                          printf(escreve_lcd,"INCUBACAO");
                                          delay_ms(1000);
                                          cont_ini=0;
                                          
                                     }
                                     
                                     }
                                      if (config_bot==0){    // ESTE IF INTERROMPE A INCUBA�AO
                                       if (cont_ini==1){
                                             PIN_COOLER =0 ;
                                             RELE_LAMP = 0 ;
                                             limpa_lcd();
                                             printf(escreve_lcd,"INCUBACAO");
                                             comando_lcd(0xC0); //  posiciona o cursor no endere�o 0x83
                                             printf(escreve_lcd,"INTERROMPIDA");
                                             
                                            
                                          delay_ms(1000);
                                          cont_ini=0; // VARIAVEL QUE INDICA O Q DEVE SER MOSTRAADO NO LCD; 
                                          cont_rotina=0; // SINALIZA QUE A ROTINA DE INCUBA�AO EST� ENCERRADA
                                          cont=1;
                                     }
                                     
                                     }
                                     WHILE (config_bot==0){    // QUANDO O BOTAO CONFIGURA�AO � PRESSIONADO DURANTE A INCUBA�AO ELE MOSTRA OS VALORES SET.
                                      
                                            comando_lcd(0x80);
                                             printf(escreve_lcd, "SET TEMP:  %.1f        ", set_temp);
                                             comando_lcd(0xC0);
                                             printf(escreve_lcd, "SET UMI:   %.1f       ", set_umi);
                                             delay_ms(100);
                                            limpa_lcd();
                                     
                                     }
                                     
                                     
                                     
                               }// fim da rotina
                      
                          
                       
                      
                       
                       
                       BTN_INI = 1; 
                  }
                     
                  }
                  
         } 
          else{
              filtro_INI = t_filtro;
              BTN_INI = 0; 
      }       
          
          
          
          
          
          
  }// fim do unlock
  
  
  
 // if (!UNLOCK){ // BOTAO CONFIGURA��O N�O FUCIONA QUANDO PROGRAMA ESTIVER NA ROTINA PRINCIPAL
     if (!config_Bot){   // o botao do motor 1 foi pressionado?
            if (! (BTN_B1)){ // O bot�o UP j� foi tratato ?
            
               // n�o.
               if (filtro_b1 != 0) // Fim do filtro do bot�o UP ?
                  filtro_b1--; // N�o, ent�o decrementa o filtro
                  else { 
                      configuracao (); // ao clicar no bot�o de configura�ao chama a fun��o de configura��o, onde ser�o passados os parametros de temperatura e umidade
                      BTN_B1 = 1; 
                  }
            }        
         } 
          else{
              filtro_b1 = t_filtro;
               BTN_B1 = 0; 
      } 
     
  //}   // FIM DA FLAG UNlock
  
     
   
    
  
    
  }//FIM DA ROTINA PRINCIPAL
  
  
}// FIM DA VOID MAIN



































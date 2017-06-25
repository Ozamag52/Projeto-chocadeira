/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *      Rotina que envia um COMANDO para o LCD                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void comando_lcd(int caracter)
{
   rs = 0;               // seleciona o envio de um caracter
   portd = caracter;         // carrega o portc com o caracter
   enable = 1 ;            // gera pulso no enable
   delay_us(1);            // espera 1 microsegundos
   enable = 0;            // desce o pino de enable

   delay_us(40);            // espera mínimo 40 microsegundos
   
   return;               // retorna
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *      Rotina que envia um DADO a ser escrito no LCD              *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void escreve_lcd(int caracter)
{
   rs = 1;               // seleciona o envio de um comando
   portd = caracter;         // carrega o portc com o valor do comando
   enable = 1;            // gera pulso no enable
   delay_us(1);            // espera 3 microsegundos
   enable = 0;            // desce o pino de enable

   delay_us(40);            // espera mínimo 40 microsegundos
   
   return;               // retorna
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Função para limpar o LCD                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void limpa_lcd()
{
   comando_lcd(0x01);         // limpa lcd   
   delay_ms (2);
   return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *           Inicialização do Display de LCD                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void inicializa_lcd()
{
   comando_lcd(0x30);         // envia comando para inicializar display
   delay_ms(4);            // espera 4 milisengundos

   comando_lcd(0x30);         // envia comando para inicializar display
   delay_us(100);            // espera 100 microsengundos

   comando_lcd(0x30);         // envia comando para inicializar display

   comando_lcd(0x38);         // liga o display, sem cursor e sem blink

   limpa_lcd();            // limpa lcd

   comando_lcd(0x0c);         // display sem cursor

   comando_lcd(0x06);         // desloca cursor para a direita

   return;               // retorna
}   
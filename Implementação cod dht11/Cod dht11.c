
#include <18F4550.h>
#fuses HS,NOWDT,PUT,NOPROTECT
#use delay(clock = 8000000)

// Connection pin between PIC16F84A and DHT11 (RHT01) sensor
#BIT Data_Pin = 0x06.7                       // Pin mapped to PORTB.7
#BIT Data_Pin_Direction = 0x86.7             // Pin direction mapped to TRISB.7
char message2[] = "RH   = 00.0 %";
short Time_out;
unsigned int8, RH_byte1, RH_byte2, CheckSum ;

void start_signal(){
  Data_Pin_Direction = 0;              // Configure connection pin as output
  Data_Pin = 0;                        // Connection pin output low
  delay_ms(25);
  Data_Pin = 1;                        // Connection pin output high
  delay_us(30);
  Data_Pin_Direction = 1;              // Configure connection pin as input
}






short check_response(){
  delay_us(40);
  if(!Data_Pin){                     // Read and test if connection pin is low
    delay_us(80);
    if(Data_Pin){                    // Read and test if connection pin is high
      delay_us(50);
      return 1;}
    }
}





unsigned int8 Read_Data(){
  unsigned int8 i, k, _data = 0;     // k is used to count 1 bit reading duration
  if(Time_out)
    break;
  for(i = 0; i < 8; i++){
    k = 0;
    while(!Data_Pin){                          // Wait until pin goes high
      k++;
      if (k > 100) {Time_out = 1; break;}
      delay_us(1);}
    delay_us(30);
    if(!Data_Pin)
      bit_clear(_data, (7 - i));               // Clear bit (7 - i)
    else{
      bit_set(_data, (7 - i));                 // Set bit (7 - i)
      while(Data_Pin){                         // Wait until pin goes low
      k++;
      if (k > 100) {Time_out = 1; break;}
      delay_us(1);}
    }
  }
  return _data;
}






void main(){
 
           while(TRUE){
             delay_ms(1000);
             Time_out = 0;
             Start_signal();
                      if(check_response()){                    // If there is response from sensor
                        RH_byte1 = Read_Data();                 // read RH byte1
                        RH_byte2 = Read_Data();                 // read RH byte2
                        Checksum = Read_Data();                 // read checksum
                        if(Time_out){                           // If reading takes long time
                        /*  lcd_putc('\f');                       // LCD clear
                          lcd_gotoxy(5, 1);                     // Go to column 5 row 1
                          lcd_putc("Time out!");*/
                        }
                           else{
                               if(CheckSum == ((RH_Byte1 + RH_Byte2) & 0xFF)){
                                
                                message2[7]  = RH_Byte1/10 + 48;
                                message2[8]  = RH_Byte1%10 + 48;
                                message2[10] = RH_Byte2/10 + 48;
                              /*  lcd_putc('\f');                       // LCD clear
                                lcd_gotoxy(1, 2);                     // Go to column 1 row 2
                                printf(lcd_putc, message2);           // Display message2 */
                               }
                                   else {
                                    /* lcd_putc('\f');                     // LCD clear
                                     lcd_gotoxy(1, 1);                   // Go to column 1 row 1
                                     lcd_putc("Checksum Error!");*/
                                   }
                     
                           }
                      }
             else {
              /* lcd_putc('\f');             // LCD clear
               lcd_gotoxy(3, 1);           // Go to column 3 row 1
               lcd_putc("No response");
               lcd_gotoxy(1, 2);           // Go to column 1 row 2
               lcd_putc("from the sensor");*/
             }
  
         
           }
         
         
}

// LCD module connections
sbit LCD_RS at RD4_bit;
sbit LCD_EN at RD5_bit;
sbit LCD_D4 at RD0_bit;
sbit LCD_D5 at RD1_bit;
sbit LCD_D6 at RD2_bit;
sbit LCD_D7 at RD3_bit;

sbit LCD_RS_Direction at TRISD4_bit;
sbit LCD_EN_Direction at TRISD5_bit;
sbit LCD_D4_Direction at TRISD0_bit;
sbit LCD_D5_Direction at TRISD1_bit;
sbit LCD_D6_Direction at TRISD2_bit;
sbit LCD_D7_Direction at TRISD3_bit;
// End LCD module connections

// Variables :
char txt_panne[4];
char txt[9];
int v=0,v2=0,v3=0,count=0,count2=0,count3=0,count4=0,write=0;
int nombre_panne=0,panne=0,NB,T,O;
float data_capteur;
float Valeur;

void interrupt(){
     if(INTCON.T0IF == 1){
       NB--;
       if (NB==0){
        NB=15;
        TMR0=0;
        T+=1;
       }INTCON.T0IF = 0;     //FLAG DE TIMER 0
      }
      if((intcon.inte)&&(intcon.INTF)){                      //RB0
             v=1;
      }
      if((INTCON.RBIE)&&(INTCON.RBIF)){                //RB4 -> RB7
       if(PORTB.rb4==1)
         {
            v2=1;
         }
         if(PORTB.rb5==1)
         {
            v3=1;
         }
      }
       INTCON.INTF=0; //flag RB0=0
       INTCON.RBIF=0; // flag des Port RB1 -> RB7= 0

}
void sound(int O)
{
 while((T-O)<300 && panne==1){
 Sound_Play(200,250);  // 1 ere Attribue Frequence en HZ de volume et 2 �me attribue pour la duree
 data_capteur=ADC_Read(3);   // Read data from RA0.AN0
 Valeur=((data_capteur*100)/1023);
 if(Valeur>=30) panne=0;
 }
}
void main() {
        //config Port :
        Sound_Init(&PORTC, 3);
        TRISA.RA3=1;//entrer de capture d'humidite
        trisb.rb0=1;//entrer capteur de pluie
        trisb.rb4=1;//entrer interrepteur
        trisb.rb5=1;//boutton consulter
        trisc.rc1=0;//sortie moteur
        trisc.rc2=0;//sortie moteur
        //------------
        //config interrupt
        INTCON.GIE=1; // Global interrupt
        INTCON.INTE=1; //  interrupt source RB0
        INTCON.RBIE=1; //  interrupt source RB Port
        OPTION_REG.INTEDG=1; //Activer l'interruption sur front montant
         //Timer :
        INTCON.T0IE = 1;
        OPTION_REG = 0x07;                //Prediviseru = 7 -> * 256
        TMR0=0;  // de 0 jusqu' a 256 apres inttereption
        NB=15;
        //----------------
        ADC_Init();    // init Analogic to Digital conversion
        Lcd_Init();   // inti LCD
        Lcd_Cmd(_LCD_CLEAR);
        Lcd_Cmd(_LCD_CURSOR_OFF);
        Lcd_Out(1,6,"WELCOME TO");
        Lcd_Out(2,2,"SMART GARDEN 2A3");
        delay_ms(3000);
       Lcd_Cmd(_LCD_CLEAR);
       nombre_panne=EEPROM_read(0x00);
while(1){
   if(v3==1)
     {
         O=T;
         ByteTostr(nombre_panne,txt_panne);
         Lcd_Cmd(_LCD_CLEAR);
         while((T-O)<=60)
         {
             lcd_out(1,1,"P=");
             lcd_out(2,1,txt_panne);
         }
         Lcd_Cmd(_LCD_CLEAR);
         v3=0;
     }
     if(v && Button(&PORTB, 0, 1, 0)) v=0; // LOGIC State change from one To Zero
     if(v2 && Button(&PORTB, 4, 1, 0)) v2=0; // LOGIC State change from one  To Zero
     data_capteur=ADC_Read(3);   // Read data from RA0.AN0
     Valeur=((data_capteur*100)/1023);
     FloatToStr(Valeur, txt);
     Lcd_Out(4,1,"H=");
     Lcd_Out_cp(txt);
     Lcd_Out(4,16,"%");
     delay_ms(500);
    if((Valeur >=30.00 && Valeur<=50.00) && PORTB.RB4!=1 && v==0)
    {
          v=0;
          v2=0;
          panne=0;
          write=0;
    }
    if((Valeur>50.00) && (v==0 || v==1))
     {
          v2=1;
          panne=0;
          write=0;
     }
    if(Valeur <=29.00)
     {
             v=0;
             v2=0;
             panne=1;
             write++;
     }
     if(panne==0 && v==0 && v2==0)      // Motor is working
     {
        portc.rc1=1;
        portc.rc2=0;
     }
     else                  //Motor is not working
     {
        portc.rc1=0;
        portc.rc2=0;
     }
     if(panne==0 && v3==0)
     {
      PORTC.RC3=0;
        if(v==1 && v2==0)
          {
              count3=0;
              count2=0;
              count4=0;
              count++;
              if(count==1)
              {
              Lcd_Cmd(_LCD_CLEAR);
              }
             if(count>=1){
                 lcd_out(1,1,"Arrosage Manuelle");
                 delay_ms(500);
               }
          }
      if(v==0 && v2==0)
          {
            count3=0;
            count=0;
            count4=0;
            count2++;
            if(count2==1)
              {
              Lcd_Cmd(_LCD_CLEAR);
              }
            if(count2>=1)
              {
                 lcd_out(1,1,"Arrosage gazon");
                 delay_ms(500);
              }
          }
          if(v2==1 && (v==0 ||v==1))
          {
             count=0;
             count2=0;
             count4=0;
             count3++;
            if(count3==1)
              {
              Lcd_Cmd(_LCD_CLEAR);
              }
            if(count3>=1)
              {
                 lcd_out(1,1,"FIN Arrosage gazon");
                 delay_ms(500);
              }
          }
     }
     if(panne==1)
     {
             count=0;
             count2=0;
             count3=0;
             count4++;
            if(count4==1)
              {
              Lcd_Cmd(_LCD_CLEAR);
              }
            if(count4>=1)
              {
                 lcd_out(1,1,"Systeme En Panne");
                 delay_ms(500);
              }
     if(write==1) // Only one time
         {
           nombre_panne=EEPROM_read(0x00);
           nombre_panne+=1;
           EEPROM_write(0x00,nombre_panne);
         }
         PORTC.RC3=1;             // +VCC to RC3
         if(PORTC.RC3=1 && write==1) // only one time for 5 min
         {
             O=T;   // Old time ;
             sound(O);                          //Sounder function
         }
     }

}
}
    /*

Laboratório 1  - Portas de entrada e saida
Professor: Ramirez
Data: 22/12/2023
Objetivo e fazer um tipo de cadeado eletronico que registre a senha, erros e acertos

*/

// Conexoes do modulo LCD
  sbit LCD_RS at RC5_bit;
  sbit LCD_EN at RC4_bit;
  sbit LCD_D4 at RC3_bit;
  sbit LCD_D5 at RC2_bit;
  sbit LCD_D6 at RC1_bit;
  sbit LCD_D7 at RC0_bit;

  sbit LCD_RS_Direction at TRISC5_bit;
  sbit LCD_EN_Direction at TRISC4_bit;
  sbit LCD_D4_Direction at TRISC3_bit;
  sbit LCD_D5_Direction at TRISC2_bit;
  sbit LCD_D6_Direction at TRISC1_bit;
  sbit LCD_D7_Direction at TRISC0_bit;
// Fim das Conexoes do modulo LCD

#define SW0 PORTD.RD2 //Define SW0 no pino RD2
#define SW1 PORTD.RD3 //Define SW1 no pino RD3
#define SW2 PORTD.RD4 //Define SW1 no pino RD4

void Move_Delay() {                  //Funcao usada para criar um delay ao mover o texto
 Delay_ms(500);                      //Aqui pode ser mudada a velocidade do movimento
}

void Alerta_erro(int senha_correta){ //Funcao  usada para acender o led e tocar o buzzer em caso de 3 erros seguidos
 int bip = 0;

 if(senha_correta){    //Condicionante para verificar se a senha est? correta ou nao
   PORTB = 0;         // Ativa o buzzer e o led

   } else{

   while(bip <= 3){
   PORTB = 0b00000011; //PORTB recebe um valor em hexadecimal, entao ativa o buzzer e o led
   Delay_ms(5000);   // Aguarda por algum tempo para manter o buzzer ativo
   PORTB = 0b00000000;       // Desativa o buzzer e o led
   bip++;
   }
   }
}

void main() {

 char txt[] = "*";   // Preciso apenas de 1, pois posso "imprimir" ele em varios lugares.
 char txt1[] = "PIC16F887";
 char txt2[] = "         ";
 char txt3[] = "Erro";       //Texto de erros

 int passos = 0;      // come?a em 1 pois ? sempre o primeiro digito a ser selecionado
 int digito = 0;
 int i = 0;  // Indice de senha
 char k;

 int senha_correta = 1; // Inicializa a senha_correta como verdadeira
 int j = 0;

 int h = 0;      //Indice do display
 int click = 0; //Variavel para verificar se o botao SW0 foi pressionado

 int senha[4] = {0,0,0,0};     //Vetor para armezanar a senha que o  ?su?rio digitou
 int verifica[4] = {3,8,5,9}; //Vetor para verificar se a senha est? correta

 int tent =  0;           //Numero de tentativas falhas
 int tam = 0;            //Verirficador o tamanho da senha
 int num = 0;   //Valor que o usuario digitou

 int aux = 0;            //Variavel auxiliar para o verificador de click em SW0
 int aux2 = 0;          //Variavel auxiliar para o verificador de click em SW1
 int aux3 = 0;         //Variavel auxiliar para o verificador de click em SW2

 int cont = 0; //Variavel para selecao do caso espacifico
 int corrige = 4; //Variavel para indicar ao case como corrigir

 ANSEL = 0 ;  //Configua os pinos da porta A com entradas digitais
 ANSELH = 0;  //Configua os pinos da porta B com entradas digitais

 C1ON_bit = 0;
 C2ON_bit = 0;

 TRISB = 0;    // Configura todos pinos B com saidas
 TRISD = 1;   //Configura todos os pinos D como entradas
 PORTB = 0;   //Inicializa PORTB como zero, para evitar erros
 PORTD = 0;   //Inicializa PORTD como zero, para evitar erros


 Lcd_Init();                        // Inicializa o LCD.
 Lcd_Cmd(_LCD_CLEAR);               // Limpa o display antes das opera??es.
 Lcd_Cmd(_LCD_CURSOR_OFF);          // Desabilita o cursor do display LCD.

 Lcd_Out(1,7,txt);  // imprime primeiro asterisco
 Lcd_Out(1,8,txt);   // imprime segundo asterisco
 Lcd_Out(1,9,txt);   // imprime terceiro asterisco
 Lcd_Chr(1,10,48);  // imprime ultimo asterisco

 for(;;){

    //*********************************************************************************************************************
     switch (passos){
        case(0):
           /********************************************************************/
           if(SW0 == 1)  aux = 1; //Se o botao esta em nivel alto, atualiza o aux para um e mantem
           if(SW0 == 0 && aux == 1){
           aux = 0; //Se o botao passa para zero, e aux e um, atualiza aux para zero e realiza a acao
           num++;                        //Estabelece o intervalo de valores como de 0 a 9
           if(num == 10) num = 0;       // se o digito estiver em 9 e voc? alterar ele volta para 0.
           Lcd_Chr(1,10,48+num);         // imprime primeiro digito.
           }
           /*********************************************************************/
           if(SW1 == 1)  aux2 = 1; //Se o botao esta em navel alto, atualiza o aux para um e mantem
           if(SW1 == 0 && aux2 == 1){
           aux2 = 0;
           if(digito == 0){
           senha[0] = num;
           Lcd_Chr(1,9,48+senha[0]);  // desloca os numeros ja selecionados
           num = 0;
           Lcd_Chr(1,10,48);         // escreve 0 para recome?ar sele??o do proximo nume
           }
           if(digito == 1){
           senha[1] = num;
           Lcd_Chr(1,8,48+senha[0]);  // desloca os numeros ja selecionados
           Lcd_Chr(1,9,48+senha[1]);  // desloca os numeros ja selecionados
           num = 0;
           Lcd_Chr(1,10,48);         // escreve 0 para recome?ar sele??o do proximo nume
           }
           if(digito == 2){
           senha[2] = num;
           Lcd_Chr(1,7,48+senha[0]);  // desloca os numeros ja selecionados
           Lcd_Chr(1,8,48+senha[1]);  // desloca os numeros ja selecionados
           Lcd_Chr(1,9,48+senha[2]);  // desloca os numeros ja selecionados '
           num = 0;
           Lcd_Chr(1,10,48);         // escreve 0 para recome?ar sele??o do proximo nume
           }
           if(digito == 3){
           senha[3] = num;
           Lcd_Chr(1,7,48+senha[0]);  // desloca os numeros ja selecionados
           Lcd_Chr(1,8,48+senha[1]);  // desloca os numeros ja selecionados
           Lcd_Chr(1,9,48+senha[2]);  // desloca os numeros ja selecionados
           Lcd_Chr(1,10,48+senha[3]);  // desloca os numeros ja selecionados
           passos = 1;
           }
           digito++;
           //digito = 1;               // confirmei meu digito, ent?o posso passar para o proximo
           }
           if(SW2 == 1)  aux3 = 1; //Se o botao esta em nivel alto, atualiza o aux para um e mantem
           if(SW2 == 0 && aux3 == 1){
            if(digito == 0) digito = 0; //redundante.
            else{
            digito = digito - 1;
             Lcd_Chr(0,7 + digito ,48+senha[0]);
             Lcd_Chr(0,8 + digito ,48+senha[1]);
             Lcd_Chr(0,9 + digito ,48+senha[2]);
             Lcd_Chr(0,10 + digito ,48+senha[3]);
            }
           }
           break;
        case(1):
           if(senha[0] == verifica[0] && senha[1] == verifica[1] && senha[2] == verifica[2] && senha[3] == verifica[3]){
           Lcd_Out(0,5,"ACERTOU");
           }
           else{
           Lcd_Out(0,7,"ERROU");
           delay_ms(1000);
           tent++;
           if(tent > 3) passos = 2;
           else{
           passos = 0;
           Lcd_Cmd(_LCD_CLEAR);
           senha[0] = 0;
            senha[1] = 0;
             senha[2] = 0;
              senha[3] = 0;

              Lcd_Out(1,7,txt);  // imprime primeiro asterisco
 Lcd_Out(1,8,txt);   // imprime segundo asterisco
 Lcd_Out(1,9,txt);   // imprime terceiro asterisco
 Lcd_Chr(1,10,48);  // imprime ultimo asterisco

 digito = 0;
           }
           }

           break;
}
}
}
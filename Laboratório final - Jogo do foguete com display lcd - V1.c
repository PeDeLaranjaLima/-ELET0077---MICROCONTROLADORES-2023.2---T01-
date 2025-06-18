/* ************ Laboratorio final - Microcontroladores *********************************************

 Aluno: Gabriel Lima Dantas
 Professor: Ramirez Hidalgo
 Nome do aluno: Gabriel Lima Dantas
 Data: 12/04/2024
 Local: Laborat�rio de sistemas digitais - Departamento de engenharia el�trica

      O projeto consiste em um jogo, no qual um "foguete" deve se mover entre as das linhas do lcd para desviar
 de um "meteoro" que se move pelas linhas e colunas do lcd. Estes dois simbolos foram gerados pela funcao
 Lcd Custom Character, a movimentacao � feita usando um loop for, de maneira que o "meteoro" se mova por todas
 as colunas, a cada iteracao do loop, e mude de linha atraves da geracao de posicao aleatoria da funcao rand,
 a movimentacao do "foguete" e feita por meio da funcao interrupt, que usa a interrupccao externa da porta B
 para manipular o valor da linha na qual ele se encontra, outro detalhe � que o fogute se move apenas da linha
 1 para a 2 e vice-versa, sem alterar seu local entre as colunas.
      A condicao de vitoria do jogo e desviar com sucesso do "meteoro", ou seja, garantir que as posicoes relativas
 dos dois objetos nao coincida, de forma que a cada vitoria sejam acumulados pontos caso contrario, o jogador perde,
 e a pontuacao � reiniciada.

*/

// Conex�es do modulo LCD
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
// Fim das Conex�es do modulo LCD

//Assinaturas das funcoes
void interrupt(); //Funcao interrupcao externa, usada para atualizar a posicao do "foguete"
int  rand();      //Fumcao rand, para gerar um numero aleatorio, usada para mudar a posicao nas linhas do "meteoro"
void perdeu();    //Funcao perdeu, iniciada para indicar que o usuario perdeu o jogo
void atraso();    //Funcao atraso, usada para tornar o jogo cada vez mais rapido
//Fim das assinaturas das funcoes

//Variaveis globais usadas nas funcoes
unsigned int posicao = 1; //Posicao do "foguete"
int cnt = 0;              //Contador de vitorias

//Caractere especial de foguete
const char foguete[] = {0,16,8,31,14,16,0,0};

void foguete1(char pos_row, char pos_char) {
  char i;
    Lcd_Cmd(72);
    for (i = 0; i<=7; i++) Lcd_Chr_CP(foguete[i]);
    Lcd_Cmd(_LCD_RETURN_HOME);
    Lcd_Chr(pos_row, pos_char, 1);
}
//Fim de caractere especial de foguete

//Caractere especial de meteoro
const char meteoro[] = {0,21,14,31,14,21,0,0};

void meteoro2(char pos_row, char pos_char) {
  char i;
    Lcd_Cmd(80);
    for (i = 0; i<=7; i++) Lcd_Chr_CP(meteoro[i]);
    Lcd_Cmd(_LCD_RETURN_HOME);
    Lcd_Chr(pos_row, pos_char, 2);
}
//Fim de caractere especial de meteoro

//Caractere especial de explosao
const char explosao[] = {0,9,2,24,14,18,0,0};

void explosao3(char pos_row, char pos_char) {
  char i;
    Lcd_Cmd(88);
    for (i = 0; i<=7; i++) Lcd_Chr_CP(explosao[i]);
    Lcd_Cmd(_LCD_RETURN_HOME);
    Lcd_Chr(pos_row, pos_char, 3);
}
//Fim de caractere especial de explosao

void main(){  //Inicio da funcao main

 int pos = 0;    //Variavel que determina a posicao alatoria de linha que o "meteoro" estara ocupando a cada iteracao do loop for
 int coluna = 0; //Variavel que determina a posicao nas colunas que o "meteoro" estara ocupando a cada iteracao do loop for
 int linha = 1;  //Variavel que determina a posicao nas linhas que o "meteoro" estara ocupando a cada iteracao do loop for

 //Configura��o dos pinos do microcontrolador
 
 // Configura os pinos AN (anal�gicos) como digitais
 ANSEL = 0;
 ANSELH = 0;
 
 // Desabilita os comparadores
 C1ON_bit = 0;
 C2ON_bit = 0;

 TRISB0_bit = 1; //pino 0 da porta PORTB como entrada (interrupcao)

 // Porta C ligada ao LCD 16X2
 TRISC = 0;        //Toda a porta C ser� sa�da
 PORTC = 0;        //Toda a porta C igual a 0
 
 //Porta D ligada ao led
 TRISD = 0;
 PORTD = 0;
 
 OSCCON = 0x75; //Usa o oscilador interno do microcontrolador

 //Configura��o das interrup��es
 INTCON.INTF = 0; //Flag de interrup��o externa
 INTCON.GIE = 1; //Habilita��o geral das interrup��es
 INTCON.PEIE = 1; //Habilita��o dos perif�ricos
 INTCON.INTE = 1; // Habilita��o de interrup��o externa
 OPTION_REG.INTEDG = 1; //interrup��o externa por borda de subida

 Lcd_Init();                        // Inicializa LCD
 Lcd_Cmd(_LCD_CLEAR);               // Limpa o display
 Lcd_Cmd(_LCD_CURSOR_OFF);          // Apaga cursor
 foguete1(posicao, 1);              //Imprime o foguete na posicao inicial

  while(1) { //Inicio do laco infinito

   for(coluna = 13; coluna>= 1; coluna--){ //Loop de movimento dos meteoros

        pos = rand(); //Determina uma linha aleatoria para o "meteoro"
        
        if(pos % 2 > 0){   //Se pos for impar, o "meteoro" estara na linha 1
         linha = 1;
        }else{             //Se pos for par, o "meteoro" estara na linha 2
          linha = 2;
         }
         
        if (posicao % 2 > 0){ //Se posicao for impar, o "foguete" estara na linha 1
            posicao = 1;
            
          // Exibe o foguete na nova posi��o
          foguete1(posicao, 1);
          // Limpa a posi��o anterior do foguete
          Lcd_Out(2, 1, "  ");
       
          if(linha == 1 && coluna == 1) perdeu();  //Verifica se o "foguete" e o "meteoro" colidiram
            
        }else{                //Se posicao for par, o "foguete" estara na linha 2
            posicao = 2;
            
          // Exibe o foguete na nova posi��o
          foguete1(posicao, 1);
          // Limpa a posi��o anterior do foguete
          Lcd_Out(1, 1, "  ");
          if(linha == 2 && coluna == 1) perdeu();   //Verifica se o "foguete" e o "meteoro" colidiram
        }

        // Exibe o meteoro na nova posi��o
        meteoro2(linha, coluna);
        delay_ms(70);
        // Limpa a posi��o anterior do meteoro
        Lcd_Out(linha, coluna, "  ");

        // Introduz um atraso entre os movimentos do meteoro
        atraso(); //Ajuste de velocidade conforme o avan�o do jogo

        if (posicao % 2 > 0){ //Se posicao for par, o "foguete" estara na linha 1
            posicao = 1;

          // Exibe o foguete na nova posi��o
          foguete1(posicao, 1);
          // Limpa a posi��o anterior do foguete
          Lcd_Out(2, 1, "  ");

          if(linha == posicao && coluna == 1) perdeu(); //Verifica se o "foguete" e o "meteoro" colidiram

        }else{                //Se posicao for impar, o "foguete" estara na linha 2
            posicao = 2;

          // Exibe o foguete na nova posi��o
          foguete1(posicao, 1);
          // Limpa a posi��o anterior do foguete
          Lcd_Out(1, 1, "  ");
          
          if(linha == posicao && coluna == 1) perdeu(); //Verifica se o "foguete" e o "meteoro" colidiram
        
        }
       if(coluna == 1) { // Ao finalizar um ciclo sem impactos, conta-se uma vitoria
       
        cnt++;  // Incrementa a variavel de contagem de vitorias
       
        Lcd_Chr(1, 16, 48 + cnt%10);  //Imprime o digito das unidade
        Lcd_Chr(1, 15, 48 + cnt/10); //Imprime o digito das dezenas

       }
     } //Fim do loop de movimento dos meteoros
  } //Fim do la�o infinito
}  //Fim da funcao Main

void interrupt(){ //Inicio da rotina de interrup��o

  if (INTCON.INTF == 1) {  //Se o flag de interrupcao externa estiver ativo

    posicao++; // incrementa contador por 1
    INTCON.INTF = 0; // reinicia Flag de interrup��o externa
    
    PORTD = 0b11111111; //Acende o led
    delay_ms(50);       //Aguarda um tempo
    PORTD = 0b0000000;  //Apaga o led
    
  }
}

void perdeu(){ //Se os dois objetos colidirem, zera a pontuacao e imprime mensagens "personalizadas" de acordo com a posicao

 if(cnt <= 10){  //Se a pontuacao for menor que 10 imprime uma mensagem
  PORTD = 0b11111111; //Acende o led
  Lcd_Cmd(_LCD_CLEAR); //Limpa o display
  explosao3(posicao, 1); //Imprime uma "explosao" no locla da colisao
  Lcd_Out(1, 4, "Perdeu noob :/");
  delay_ms(300);  //Aguada um tempo
  PORTD = 0b0000000; //Apaga o led
  cnt = 0; //Zera os pontos
  Lcd_Cmd(_LCD_CLEAR); // Limpa o display
 }
 
 if(cnt > 10 && cnt <= 20){
  PORTD = 0b11111111; //Acende o led
  Lcd_Cmd(_LCD_CLEAR); // Limpa o display
  explosao3(posicao, 1); //Imprime uma "explosao" no locla da colisao
  Lcd_Out(1, 4, "Perdeu mas :|");
  Lcd_Out(2, 3, "pode melhorar");
  delay_ms(300); //Aguada um tempo
  PORTD = 0b0000000; //Apaga o led
  cnt = 0; //Zera os pontos
  Lcd_Cmd(_LCD_CLEAR); // Limpa o display
 }
 
 if(cnt > 20){
  PORTD = 0b11111111; //Acende o led
  Lcd_Cmd(_LCD_CLEAR); // Limpa o display
  explosao3(posicao, 1); //Imprime uma "explosao" no locla da colisao
  Lcd_Out(1, 4, "Um novo");
  Lcd_Out(2, 3, "record :)");
  delay_ms(300); //Aguada um tempo
  PORTD = 0b0000000; //Apaga o led
  cnt = 0; //Zera os pontos
  Lcd_Cmd(_LCD_CLEAR); // Limpa o display
 }
 
}

void atraso(){ //Funcao para mudar a velociade do "meteoro" de acordo com a pontuacao, aumentando assim a dificuldade do jogo

 if(cnt < 10){ //Se a pontuacao for menor que 10
   delay_ms(400); //Aguada um tempo longo para passar o meteoro de linha ou coluna
 }
 if(cnt >= 10 < 20){ //Se a pontuacao for maior ou igual a 10 e menor que 20
   delay_ms(300); //Aguada um tempo medio para passar o meteoro de linha ou coluna
 }
 if(cnt >= 20){ //Se a pontuacao for maior que 20
   delay_ms(200); //Aguada um tempo curto para passar o meteoro de linha ou coluna
 }
 
}
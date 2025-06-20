#define EN1 PORTD   //Constante para gerar a manipula��o da intensidade dos leds
#define print_digite 2 // quantidade de vezes que vai repetir "digite uma tecla"
#define delay_time 100 // distancia em ms da impress?o da frase acima.
#define F 70
#define A 65
#define E 69
#define D 68
#define P 80
#define L 76
#define Fosc 8000000 //Frequ�ncia do cristal Mhz

unsigned char efeito = 1;
unsigned char dado; //Vari?vel para mandar a mensagem
int cnt_aux = 0;         //variavel de controle para imprimir "digite uma tecla"
int Duty = 5;           //vari?vel que vai fazer a conta na interrup??o
bit control_F;
bit control_A;
bit control_D;

unsigned char valor = 24;          //variavel com o valor de 10% de duty cycle
// --- Rotina de Interrup��o ---

void TXCHR_USART(char dado1)
{
while(TXIF_bit == 0); //espera pelo flag de interrup??o de transmiss?o
TXIF_bit = 0;
TXREG = dado1; //espera que o flag de interrup??o de transmiss?o fique em 1 o
//qual indica que TXREG est? pronto para outra transmiss?o
}
/******************** Fun??o para transmitir uma string ************/
void TX_STRING_USART(const char *dado)
{
unsigned int i = 0;
while (dado[i] != '\0');
TXCHR_USART(dado[i++]);
}//fim da fun??o TX_STRING_USART()

/**************** Fun??o INCIAR_USART ****************************/
void INICIAR_USART(const unsigned long Baud_Rate)
{
SPBRG = (Fosc/(long)(16*Baud_Rate))-1; // baud rate para Fosc = 20Mhz e BRGH=1
SYNC_bit = 0; // habilita o modo ass?ncrono
SPEN_bit = 1; //habilita pinos da porta serial
CREN_bit = 1; //habilita a recep??o
TXIE_bit = 0; //desabilita a interrup??o de transmiss?o
RCIE_bit = 0; //desabilita a interrup??o de recep??o
TX9_bit = 0;  //transmiss?o de 8 bits de dados
RX9_bit = 0;  //recep??o de 8 bits de dados
TXEN_bit = 1; //habilita a transmiss?o
}

void interrupt()
{
 if(TMR2IF_bit) //interrup??o do Timer2
  {
    TMR2IF_bit = 0; // clear TMR2IF
    if(Duty > 0 && Duty < 10) EN1 = ~EN1;
    if(Duty == 10) EN1 = 1; 
    if(EN1 == 0) //EN1 em 1?
    {
     PR2 = (Duty * valor); //Duty de duty cycle
    } //fim if(EN1 == 1)
    else
    {
      if(Duty >= 1)
      PR2 = (10-Duty)* valor; //(10 - Duty) de duty  
    } //fim EN1 em 1?
  if(RCIF_bit) //Houve interrup??o na USART?
{ //Sim...
RCIF_bit = 0; //Limpa flag
if(FERR_bit || OERR_bit) //Ocorreu erro de framing ou overrun?
{ //Sim...
CREN_bit = 0x00; //Limpa status de recebimento
CREN_bit = 0x01;
} //fim de teste de erro
TXREG = RCREG; //transmitimos o que recebemos
dado = RCREG; //a variavel recebe o valor do dado recebido   
}
  } //fim interrup??o do Timer2
 }
void main() {
//const char message[] = ("Pressionar as teclas F, A, D, E, P ou L");
EN1 = 1;
ANSEL = 0; // Configurar pinos AN como digital
ANSELH = 0;
C1ON_bit = C2ON_bit = 0; // Desabilita os comparadores
TRISB = 0; //Porta B sa�da
PORTB = 0; //Leds apagados, conectados na porta B
TRISD = 0xFFFF; //Porta C sa�da
PORTD = 0;

OSCCON = 0X75; //Habilita o oscilador interno do pic, para susbstituir o cristal de 20Hz

BRGH_bit = 1; //EUSART funciona a alta velocidade e utiliza a equa??o mostrada para SPBRG.
INICIAR_USART(19200); //Inicializa com 19200 bps = 19,2 Kbps
Delay_ms(100); // Espera que o modulo USART se estabilise
//TX_STRING_USART(message); //transmitimos a variavel message[]
TXCHR_USART(10); //LF=LineFeed, passa para outra linha
TXCHR_USART(13); //CR=Carrier Return, retorno de carro. Cursor volta ao inicio da linha

/***********Esta configura��o deve ir depois da fun��o INICIAR_USART pq nesta estamos desabilitando a interrup��o de recep��o USART *******/
RCIF_bit = 0; //desabilitamos o flag de interrup��o de recep��o USART
RCIE_bit = 1; //Habilitamos a interrup��o de recep��o USART
GIE_bit = 1; //Habilitamos a interrup��o global
PEIE_bit = 1; //Habilitamos a interrup��o de perifericos
// Fim das configura��es de interrup��o de recep��o USART
//Configuracao da interrupcao do Timer2
TMR2IF_bit = 0;
PIE1.TMR2IE = 1; //habilita interrup??o do TIMER2
PR2 = valor; //Com este valor de PR2 temos 10% de duty cycle
T2CON = 0x4C;
dado = E;
// Fim das configura��es de interrup��o externa 

  while(1) 
  {
      if(cnt_aux < print_digite){
      //TX_STRING_USART("Digite alguma tecla:"); // Envia mensagem para o monitor serial
      Delay_ms(delay_time); 
      cnt_aux++;
      }
   switch(dado){
    case F: //Caso seja selecionado o modo para piscar de frente para tras
     //Executar apenas uma vez
     /*********************************************************************/
     if(control_F == 0){
     //TX_STRING_USART("Frente"); // Envia mensagem para o monitor serial
     control_F = 1;
     cnt_aux = 0;
     }
     /*********************************************************************/
     //Executar sempre
     /*********************************************************************/
     if(efeito >= 128)
     {
      efeito = 1;
      TRISD = ~efeito;
      Delay_ms(500);
      }
      else
      {
        efeito = efeito<<1;
        TRISD = ~efeito;
        Delay_ms(500);
       }
     break;
     case E:
     //Executar apenas uma vez
     /*********************************************************************/ 
     if(control_A == 0){
     //TX_STRING_USART("Piscando mais significativos"); 
     control_A = 1;
     cnt_aux = 0;
     }
     /*********************************************************************/ 
     //Executar sempre
     /*********************************************************************/ 
      TRISD = 0b00001111;
      Delay_ms(500);
      TRISD = 0b11111111;
      Delay_ms(500);
      /*********************************************************************/
      break;
    case A: //Caso seja selecionado o modo para piscar de tras para frente
     //Executar apenas uma vez
     /*********************************************************************/ 
     if(control_A == 0){
     //TX_STRING_USART("Atras"); // Envia mensagem para o monitor serial
     control_A = 1;
     cnt_aux = 0;
     }
     /*********************************************************************/
     //Executar sempre
     /*********************************************************************/
     if(efeito == 0)
      {
      efeito = 128;
      TRISD = ~efeito;
      Delay_ms(500);
      }
      else
      {
      efeito = efeito>>1;
      TRISD = ~efeito;
      Delay_ms(500);
      }
     /*********************************************************************/
     break;
    case D: //Caso seja selecionado o modo para piscar apenas os mais significativos
     //Executar apenas uma vez
     /*********************************************************************/
     if(control_D == 0){
     //TX_STRING_USART("Piscando menos significativo"); 
     control_D = 1;
     cnt_aux = 0;
     }
     /*********************************************************************/
     //Executar sempre
     /*********************************************************************/
     TRISD = 0b11110000;
     Delay_ms(500);
     TRISD = 0b11111111;
     Delay_ms(500);
     /*********************************************************************/
     break;
    case L: //Diminuir o brilho dos leds
     Duty--;  // Decrementa a vari?vel que representa o Duty-cicle.
     //TX_STRING_USART("Tecla Diminui duty ciclo pressionada"); // Envia mensagem para o monitor serial
     cnt_aux = 0;
     // Verifica??o para o Duty n?o sair da normalidade
     /*********************************************************************/
     if (Duty == 0){
        EN1 = 0;    //Se o Duty for de 0%, o sinal ser? contantemente LOW
        }
     if (Duty == 10){
        EN1 = 1;    //Se o Duty for de 100%, o sinal ser? contantemente HIGH
        } 
     if (Duty < 0){//Se o Duty estava em 0% e eu decrementei, ele vai para 100%.
        EN1 = 1;  // o sinal ser? contantemente HIGH 
        Duty = 10;  // Reinicia o valor de Duty para 10, para Duty de 100%
        }
        /*********************************************************************/
     if(Duty == 0){
        TX_STRING_USART("Duty cicle em 0%"); // Envia mensagem para o monitor serial
        }
        else if(Duty == 1){
        TX_STRING_USART("Duty cicle em 10%"); // Envia mensagem para o monitor serial
        }
        else if(Duty == 2){
        TX_STRING_USART("Duty cicle em 20%"); // Envia mensagem para o monitor serial
       }
        else if(Duty == 3){
        TX_STRING_USART("Duty cicle em 30%"); // Envia mensagem para o monitor serial
        }
        else if(Duty == 4){
        TX_STRING_USART("Duty cicle em 40%"); // Envia mensagem para o monitor serial
       }
        else if(Duty == 5){
        TX_STRING_USART("Duty cicle em 50%"); // Envia mensagem para o monitor serial
        }
        else if(Duty == 6){
        TX_STRING_USART("Duty cicle em 60%"); // Envia mensagem para o monitor serial
        }
        else if(Duty == 7){
        TX_STRING_USART("Duty cicle em 70%"); // Envia mensagem para o monitor serial
        }
        else if(Duty == 8){
        TX_STRING_USART("Duty cicle em 80%"); // Envia mensagem para o monitor serial
        }
        else if(Duty == 9){
        TX_STRING_USART("Duty cicle em 90%"); // Envia mensagem para o monitor serial
        }
        else{
        TX_STRING_USART("Duty cicle em 100%"); // Envia mensagem para o monitor serial
       }
     dado = 0;
    /*********************************************************************/
     break;
      case P: //Aumenta o brilho dos leds
        cnt_aux = 0;
        Duty++;  // incrementa a vari?vel que representa o Duty-cicle.
        // Verifica??o para o Duty n?o sair da normalidade
        /**********************************************************************/ 
        if (Duty == 0) {
           EN1 = 0;    //Se o Duty for de 0%, o sinal ser? contantemente LOW
        }
        if (Duty == 10) {
           EN1 = 1;    //Se o Duty for de 100%, o sinal ser? contantemente HIGH
        } 
        if (Duty < 0) { //Se o Duty estava em 0% e eu decrementei, ele vai para 100%.
           EN1 = 1;    // o sinal ser? contantemente HIGH 
           Duty = 10;  // Reinicia o valor de Duty para 10, para Duty de 100%
        }
       /**********************************************************************/
        if(Duty == 0){
        TX_STRING_USART("Duty cicle em 0%"); // Envia mensagem para o monitor serial
        }
        else if(Duty == 1){
        TX_STRING_USART("Duty cicle em 10%"); // Envia mensagem para o monitor serial
        }
        else if(Duty == 2){
        TX_STRING_USART("Duty cicle em 20%"); // Envia mensagem para o monitor serial
        }
        else if(Duty == 3){
        TX_STRING_USART("Duty cicle em 30%"); // Envia mensagem para o monitor serial
        }
        else if(Duty == 4){
        TX_STRING_USART("Duty cicle em 40%"); // Envia mensagem para o monitor serial
        }
        else if(Duty == 5){
        TX_STRING_USART("Duty cicle em 50%"); // Envia mensagem para o monitor serial
        }
        else if(Duty == 6){
        TX_STRING_USART("Duty cicle em 60%"); // Envia mensagem para o monitor serial
        }
        else if(Duty == 7){
        TX_STRING_USART("Duty cicle em 70%"); // Envia mensagem para o monitor serial
       }
       else if(Duty == 8){
        TX_STRING_USART("Duty cicle em 80%"); // Envia mensagem para o monitor serial
        }
        else if(Duty == 9){
        TX_STRING_USART("Duty cicle em 90%"); // Envia mensagem para o monitor serial
        }
        else{
        TX_STRING_USART("Duty cicle em 100%"); // Envia mensagem para o monitor serial
        }
        dado = 0; 
        break;
     }//fim do switch    
  }//fim do while
} //fim do main
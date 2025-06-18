/*
  Este laboratorio foi feto para usar seis teclas para controlar a dire??o e velocidade de dois motores DC.
  O funcionamento solicitado foi o seguinte:

  a) Quando se pressiona e solta a tecla1 os dois motores DC come?am a rodar para "frente" no sentido 1 indefinidamente
  e com a tecla6 controlamos as velocidades dos dois motores mudando o duty cycle de 10% em 10% , freq = 500Hz
  b) Quando se pressiona e solta a tecla2 os dois motores DC come?am a rodar para "atr?s" no sentido 2 indefinidamente
  e com a tecla6 controlamos as velocidades dos dois motores mudando o duty cycle de 10% em 10% , freq = 500Hz
  c) Quando se pressiona e solta a tecla3 o motor M1 para e o outro roda para frente indefinidamente
  e com a tecla6 controlamos as velocidades dos dois motores mudando o duty cycle de 10% em 10%, freq = 500Hz
  d) Quando se pressiona e solta a tecla4 o motor M2 para e o outro roda para frente indefinidamente
  e com a tecla6 controlamos as velocidades dos dois motores mudando o duty cycle de 10% em 10%, freq = 500Hz
  e) Quando se pressiona e solta a tecla5 os motores param

  E assim sucessivamente continua o ciclo de opera??o b,c,d,....

  Para isso foi usado o timer 2 para gerar o Duty cycle e a interrup??o deste
  PR2 = (Ta*Fosc)/(4*Pre*Pos)
  A frequencia do sinal eh de 500Hz, portanto o periodo ser? T= 2ms
  Encontramos um valor de DUTY CYCLE DE 10% sendo assim T(10%) = 0,2ms = 200us.
  O PR2 que representa o 10%(0,2ms) de 500Hz(2ms) ser? = 25 (aproximadamente) e
  a configura??o do timer2 ser?:
  T2CON=0b01001101; // T2CON = 0x4D. Postcaler 1:10, TIMER2 ligado e prescaler 1:4 (500Hz)
  A continua??o uma tabela mostrando os valores de PR2 para os diferentes
  duty cycles:
       Porcentagem Duty        Valor de PR2
            10%                     25
            20%                     50
            30%                     75
            40%                     100
            50%                     125
            60%                     150
            70%                     175
            80%                     200
            90%                     225
            100%                    250

  Utilizaremos uma tecla (tecla6) ligada a entrada de interrupcao externa para mudar o valor
  do duty cycle.
*/

//Definicao dos pinos que controlam o CI L293D
#define IN1     PORTD.RD0   //CONTROLA O MOTOR 1
#define IN2     PORTD.RD1   //CONTROLA O MOTOR 1
#define IN3     PORTD.RD2   //CONTROLA O MOTOR 2
#define IN4     PORTD.RD3   //CONTROLA O MOTOR 2
#define EN1     PORTD.RD4   //ENABLE CONTROLA O MOTOR 1 e 2 (ser? conectado junto com EN2)

/*
  Medidor de distancia usando o sensor de ultrasom HC-SR04 e o codigo MikroC para o PIC16F887.
  Os pinos Echo e Trigger do HC-SR04 s?o conectados aos pinos RD5 e RD6 respectivamente.
*/

// Os pinos Echo e Trigger sao conectados a RD5 e RD6 respectivamente
#define ECHO_PIN      PORTD.RD5           //Pino 5 da porta D recebe o nome de ECHO_PIN
#define TRIGGER_PIN   PORTD.RD6           //Pino 6 da porta D recebe o nome de TRIGGER_PIN

// --- Constantes Auxiliares ---
#define      n     5        //numero de pontos da media movel

//Define o buzzer e um led para gerar um alerta de proximidade
#define BUZZER PORTC.RC3  //Pino 3 da porta C recebe o nome de BUZZER
#define LED PORTC.RC2     //Pino 2 da porta C recebe o nome de LED

unsigned int j,i;
float tempo, x, distancia, media;
char *res;
unsigned char modo;
int       numbers[n];        //vetor com os valores para media movel

//Funcoes prototipo
unsigned int espera_sensor();
float obtem_tempo();
long media_movel();       //Funcao para filtro de media movel

//Defini??o dos pinos onde est?o conectadas as teclas
#define tecla1     PORTB.RB1
#define tecla2     PORTB.RB2
#define tecla3     PORTB.RB3
#define tecla4     PORTB.RB4
#define tecla5     PORTB.RB5
#define tecla6     PORTB.RB0     //tecla ligada ao pino 0 da porta B, interrup??o externa

//Defini��o do pino para simular o bot�o de dist�ncia
//#define distancia_cm PORTC.RC1

int Duty = 10;            //vari?vel que vai fazer a conta na interrup??o
unsigned char valor = 24;          //variavel com o valor de 10% de duty cycle

void interrupt() // Inicio da rotina de interrup??o
{
 if (INTCON.INTF == 1) {     // interrup??o externa ser? acrescentado o duty cycle
   Delay_ms(150);
   Duty--; // incrementa contador por 1
   INTCON.INTF = 0;; // reinicia Flag de interrup??o externa
 }
 if(TMR2IF_bit) //interrup??o do Timer2
  {
    TMR2IF_bit = 0; // clear TMR2IF
    if(Duty > 0 && Duty < 10) EN1 = ~EN1; //complemento do PORTD.RD4 . Conectamos EN1 e EN2 juntos
    if(EN1 == 1) //EN1 em 1?
    {
     PR2 = (Duty * valor); //Duty de duty cycle
    } //fim if(EN1 == 1)
    else
    {
      if(Duty >= 1)
      PR2 = (10-Duty)* valor; //(10 - Duty) de duty cycle
    } //fim EN1 em 1?
  } //fim interrup??o do Timer2
}

void main() {

     unsigned char estado1,estado2,estado3,estado4,estado5, modo = 0 ;   //Variaveis auxiliares usadas a cada estado, ou seja, modo do motor

     //Configura??o dos pinos do microcontrolador
     ANSEL = ANSELH = 0;  // Configura os pinos AN (anal?gicos) como digitais
     C1ON_bit = 0;        // Desabilita os comparadores
     C2ON_bit = 0;

     //Na porta B est?o ligadas as teclas por esse motivo se configuracao como entradas
     TRISB = 0B0111111 ;   //s? pino 7 da porta B como sa?da

     //Pinos da porta D como sa?das, j? que por ali ser?o enviados os sinais para o L293D
     TRISD = 0B0000000; // porta D como sa?da porem com o pino 6 como entrada
     PORTD = 0; //

     TRISD5_bit = 1; //pino Echo como entrada
     TRISD6_bit = 0; //pino Trigger como sa?da

     TRISC2_bit = 0; //Pino da saida do LED
     TRISC3_bit = 0; //Pino da saida do BUZZER
     //TRISC1_bit = 1; //Pino da entrada do bot�o de dist�ncia
     PORTC = 0;

     //Configura??o das interrup??es
     //Configuracao da Interrupcao externa
     INTCON.INTF = 0; //Flag de interrup??o externa
     INTCON.INTE = 1; // Habilita??o de interrup??o externa
     OPTION_REG.INTEDG = 1; //interrup??o externa por borda de subida

     //Configuracao da interrupcao do Timer2
     TMR2IF_bit = 0;
     PIE1.TMR2IE = 1; //habilita interrup??o do TIMER2
     PR2 = valor; //Com este valor de PR2 temos 10% de duty cycle
     //PR2 = (Ta * Fosc)/(4 * prescale * postscale)
     //T2CON=0b01001100; // T2CON = 0x4C. Postcaler 1:10, TIMER2 ligado e prescaler 1:1 (2KHz)

    //Configura??o do Timer 1
    //Devemos encontrar o tempo que demora em retornar a onda do som (em us)
    //Entao fazendo os calculos teremos o seguinte:
    //tick = Prescaler*4/Fosc
    // o tempo ser? igual a tempo = contagem * tick   (em us)
    //onde contagem ? o valor armazenado nos registradores TMR1H e TMR1L
    //tick ser? encontrado com um prescaler de 4.
    // tempo = ((TMR1H << 8 )| TMR1L) * (4*4/20MHz) = ((TMR1H << 8 )| TMR1L) * (0,8 us)

     //Configuracao da interrupcao global e de perifericos
     INTCON.GIE = 1; //Habilita??o geral das interrup??es
     INTCON.PEIE = 1; //Habilita??o dos perif?ricos

     //Configuramos o timer 2 e o ativamos
     T2CON=0b01001101;

       /*
      Os oito pulsos de ultrasom viajam atrav?s do ar se afastando do transmissor.
      Enquanto que, o pino de eco se p?e ALTO para come?ar a formar o come?o do sinal
      de retorno do eco.
    Caso esseS pulsos n?o sejam refletidos, o sinal de eco expirar? depois de 38 ms
    e voltar? ao nivel BAIXO. Por tanto, um pulso de 38 ms indica que no existe
    obstru??o dentro da faixa do sensor, j? que eh o tempo que tem estado em ALTO.
      */

      T1CON = 0x20; //prescaler de 1:4 ....
      
       IN1 = 0; //Motor 1 e 2 parados
       IN2 = 0;
       IN3 = 0;
       IN4 = 0;
       //distancia = 25;
       
     while(1)
     {
       // O pino trigger envia um pulso de 10 us
        TRIGGER_PIN = 0;
        delay_us(4);
        TRIGGER_PIN = 1;
        delay_us(10);
        TRIGGER_PIN = 0;
        
        // Le o pulso que vem do pino Echo
        if(espera_sensor()){
          tempo = obtem_tempo();  //obtem o numero de contas do timer1 e o tempo = contagem * 0,8us
          distancia = tempo/58.0;   //distancia dada em cm
          } else {
            //continua o funcionamento de verficacao
          }
          delay_ms(200);
          
          /*
          //C�digo para seguran�a, caso o sensor n�o funcione, ele serve de salvaguarda, ja que o experimento nao determina o uso do sensor
        if(distancia_cm == 0) distaux = 1;
        if(distancia_cm == 1 && distaux == 1)
        {

        if(distancia > 0) distancia = distancia - 10; //A cada intera��o com o bot�o, incrementa 5cm na proximidade com o objeto

        if(distancia == 0) distancia = 150; //Reinicia a distancia para um limite acima da condi��o de parada de qualquer bot�o

        }//Fim do bot�o de simulacao de distancia
          */
          
        switch(modo){
           case 1:
            if(distancia <= 20){ //tocar o buzzer, acende o led e para os motores

            BUZZER = 1; //Inicia o BUZZER para alertar sobre a proximidade
            LED = 1;    //Inicia o LED para alertar sobre a proximidade
            IN1 = 0;    //Motor 1 e 2 parados
            IN2 = 0;
            IN3 = 0;
            IN4 = 0;
            }
            else {

            BUZZER = 0; //Desativa o BUZZER ja que o obstaculo esta longe o suficiente
            LED = 0;    //Desativa o LED ja que o obstaculo esta longe o suficiente
            IN1 = 0;    //gira no sentido 1 ("para frente")
            IN2 = 1;
            IN3 = 0;
            IN4 = 1;
            }
            break;
           case 2:
             if(distancia <= 40){ //tocar o buzzer, acende o led e para os motores
             BUZZER = 1; //Inicia o BUZZER para alertar sobre a proximidade
             LED = 1;    //Inicia o LED para alertar sobre a proximidade
             IN1 = 0;    //Motor 1 e 2 parados
             IN2 = 0;
             IN3 = 0;
             IN4 = 0;
             }
             else {

             BUZZER = 0; //Desativa o BUZZER ja que o obstaculo esta longe o suficiente
             LED = 0;    //Desativa o LED ja que o obstaculo esta longe o suficiente

              IN1 = 1;  //gira no sentido 2 ("para atras")
              IN2 = 0;
              IN3 = 1;
              IN4 = 0;
             }
             break;
           case 3:
             if(distancia <= 50){ //tocar o buzzer, acende o led e para os motores

             BUZZER = 1; //Inicia o BUZZER para alertar sobre a proximidade
             LED = 1;    //Inicia o LED para alertar sobre a proximidade

             IN1 = 0; //Motor 1 e 2 parados
             IN2 = 0;
             IN3 = 0;
             IN4 = 0;
             }
             else {

             BUZZER = 0; //Desativa o BUZZER ja que o obstaculo esta longe o suficiente
             LED = 0;    //Desativa o LED ja que o obstaculo esta longe o suficiente

              IN1 = 0; //Motor 1 para e o outro vai em frente ("direita")
              IN2 = 0;
              IN3 = 1;
              IN4 = 0;
             }
             break;
           case 4:
             if(distancia <= 30){ //tocar o buzzer, acende o led e para os motores

             BUZZER = 1; //Inicia o BUZZER para alertar sobre a proximidade
             LED = 1;    //Inicia o LED para alertar sobre a proximidade

             IN1 = 0; //Motor 1 e 2 parados
             IN2 = 0;
             IN3 = 0;
             IN4 = 0;
             }
             else {

             BUZZER = 0; //Desativa o BUZZER ja que o obstaculo esta longe o suficiente
             LED = 0;    //Desativa o LED ja que o obstaculo esta longe o suficiente

             IN1 = 1; //Motor 2 para e o outro vai em frente ("esquerda")
             IN2 = 0;
             IN3 = 0;
             IN4 = 0;
             }
             break;
           case 5:
             if(distancia <= 100){ //tocar o buzzer, acende o led e para os motores

             BUZZER = 1; //Inicia o BUZZER para alertar sobre a proximidade
             LED = 1;    //Inicia o LED para alertar sobre a proximidade

             IN1 = 0; //Motor 1 e 2 parados
             IN2 = 0;
             IN3 = 0;
             IN4 = 0;
             }
             else {

             BUZZER = 0; //Desativa o BUZZER ja que o obstaculo esta longe o suficiente
             LED = 0;    //Desativa o LED ja que o obstaculo esta longe o suficiente

             IN1 = 0; //Motor 1 e 2 parados
             IN2 = 0;
             IN3 = 0;
             IN4 = 0;
             }
             break;
        }
        if(tecla1 == 0) estado1 = 1;
        if(tecla1 == 1 && estado1 == 1)
        {
        modo = 1;
        estado1 = 0;
        } //Final da tecla 1

        if(tecla2 == 0) estado2 = 1;
        if(tecla2 == 1 && estado2 == 1)
        {
        modo = 2;
        estado2 = 0;
        } //Final da tecla 2

        if(tecla3 == 0) estado3 = 1;
        if(tecla3 == 1 && estado3 == 1)
        {
        modo = 3;
        estado3 = 0;
        } //Final da tecla 3

        if(tecla4 == 0) estado4 = 1;
        if(tecla4 == 1 && estado4 == 1)
        {
        modo = 4;
        estado4 = 0;
        } //Final da tecla 4

        if(tecla5 == 0) estado5 = 1;
        if(tecla5 == 1 && estado5 == 1)
        {
        modo = 5;
        estado5 = 0;
        }

        //Verifica o Duty, para mudar a veocidade do carrinho esta localizado aqui para simular o local da tecla6
        if (Duty == 0)
        {
           EN1 = 1;
        }
        if (Duty == 10)
        {
           EN1 = 0;
        }
        if (Duty < 0)
        {
           EN1 = 0;
           Duty = 10;
        }
     } //fim do while(1)
}//fim do main

//Funcoes
/*
Neste projeto ? usado o Timer1 para medir as larguras do sinal(larguras da logica 1 e logica 0),
este e configurado para incrementar a cada 0,8 us por  tick .
Isto significa que a frequencia de clock do Timer1 ? 1,25MHz.
A funcao unsigned int espera_sensor() espera pela resposta do sensor, este retorna 1
se recebeu alguma resposta e 0 se o tempo eh out(time out = ultrapassou o tempo maximo de espera)
Aqui o time out ? 800 ticks => 640 us .
*/
unsigned int espera_sensor()
{
  j = 0;
  TMR1H  = TMR1L = 0;   // zera Timer1
  TMR1ON_bit = 1;           // habilita o modulo Timer1
  while(!ECHO_PIN && (j < 800))
    j = ( TMR1H << 8 ) | TMR1L;   // le Timer1 e armazena seu valor em i
  TMR1ON_bit = 0;           // desabilita o modulo Timer1
  if(j >= 800)
    return 0;
  else
    return 1;
}  // fim de funcao espera_sensor
/*
  Esta funcao mede a duracao do pulso enviado pelo sensor HC-SR04 (em microsegundos)
  Esta duracao ? armazenda  na variavel "i".
  Esta funcao retorna 0 se esta OK e 1 se est? fora da range ou erro de time out
  Nesta funcao o Timer1 tambem eh usado para medir o pulso (pulso em alto)
  gerado pelo sensor HC-SR04 (pulso Echo).
  Porque 29000?
  A distancia atual (em cm) ? igual ? largura do pulso Echo (em us) dividido por 58.
  Como a distancia maxima que o sensor pode medir ? 400 cm obtemos que o valor maximo
  do pulso Echo (em us) ser? : 400 x 58 = 23200 us, como cada tick ? de 0,8us o
  numero de contas que deve ser feita ? a seguinte contas = 23200/0,8 = 29000
*/
float obtem_tempo()
{
 j = 0;
 TMR1H = TMR1L = 0;
 TMR1ON_bit = 1;           // habilita o modulo Timer1
 while(ECHO_PIN &&(j < 29000)); //Enquanto o pino de eco estiver em 1 e a conta for menor que 29000
                                //permanece nessa instru??o at? que algum deles seja falso
 TMR1ON_bit = 0;           // desabilita o modulo Timer1
 j = ( TMR1H << 8 ) | TMR1L;   // le Timer1 e armazena seu valor em i
 media = media_movel();
 x = media * 0.8;  //tempo que demora em retornar a onda do som (em us)

 return x;
}

long media_movel()
{
    long acc = 0;          //acumulador para somar os pontos da media movel

   //desloca os elementos do vetor de media movel
   for(i = n-1; i>0; i--) numbers[i] = numbers[i-1];
   numbers[0] = j; //posicao inicial do vetor recebe a leitura original

   for(i=0; i<n; i++) acc += numbers[i]; //faz a somatoria do numero de pontos

   return acc/n;  //retorna a media movel

} //fim media_movel
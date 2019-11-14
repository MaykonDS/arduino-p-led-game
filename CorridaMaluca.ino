#include "Arduino.h"
#include <MyStack.h> // biblioteca criada para iniciar uma pilha para cada fileira de leds.
#include <Bipper.h> // biblioteca criada para os sons do buzzer.
#define INTERV 120 // valor fixo para o cálculo da dificuldade.
#define LEDINTERV 20 // valor de intervalo do led start

// Leds
int ledAzul[] = {A4,A5,2,3};
int ledBranco[] = {4,5,6,7};
int ledVermelho[] = {A0,A1,A2,A3};
// Array aonde suas posições irão ser os jogadores e a dificuldade/intervalo deles.
int jogadorDifficulty[] = {500,500,500};
// Botões
int btnStart = 8;
int btnAdd = 9;
int btnRemove = 10;
// Controladores
bool inGame;
int turn; 
long lastcheck = 0;
// Pilha
MyStack pilhaV(4), pilhaA(4), pilhaB(4);
// Buzzer
Bipper bip(11);


void setup() {
  // Velocidade da porta serial.
  Serial.begin(9600);

  // Definição de entrada e saída.
  for(int i=0; i<4;i++){
  pinMode(ledAzul[i], OUTPUT);
  pinMode(ledBranco[i], OUTPUT);
  pinMode(ledVermelho[i], OUTPUT);
  } //end for
  pinMode(btnStart, INPUT_PULLUP);
  pinMode(btnAdd, INPUT_PULLUP);
  pinMode(btnRemove, INPUT_PULLUP);

  // Inicia os controladores.
  inGame = 0;
  turn = 0;
  
}
/**
* Ficará em loop, inGame será o controlador para identificar se está em partida.
*/
void loop() {
    if (!inGame){
    waitForStart();
    }
    difficultyManager();
    placar();
    sequencial(ledVermelho, ledAzul, ledBranco, jogadorDifficulty[turn]);

    if(turn==2)
      turn=0;
    else
      turn++;
      
    didWin();
}
/**
 * Irá segurar até que o jogador aperte o botão Start.
 */
void waitForStart(){
  while (true) {
    Serial.println("Esperando apertar Start.");
    int start = digitalRead(btnStart);
    if (start==0) {
      show();
      Serial.println("Começou a Corrida!");
      bip.bipStart();
      inGame = 1;
      return;
    }
  }
}
/**
 * Bônus - utilizado para definir a dificuldade/intervalo do jogo.
 * Quanto mais pontos/leds o jogador acender mais rápido irá ficar.
 * Caso alguém remova um led de um player a dificuldade diminui.
 */
void difficultyManager(){
   switch(turn){
    case 0: // jogador vermelho
      jogadorDifficulty[turn] = 500 - (INTERV*pilhaV.size());
    break;
    case 1: // jogador azul
      jogadorDifficulty[turn] = 500 - (INTERV*pilhaA.size());
    break;
    case 2: // jogador branco
      jogadorDifficulty[turn] = 500 - (INTERV*pilhaB.size());
    break;
   }
   Serial.print("Turn: ");Serial.print(turn);Serial.print(" Dif. ");Serial.println(jogadorDifficulty[turn]);
}
/**
 * É o "main" do jogo, responsável por acender os leds da vez em sequência.
 * vermelho, azul e branco.
 * @params vermelho[], azul[], branco[] - array com a pinagem correspondente a cada cor.
 * @intervalo - irá definir o intervalo entre cada led.
 */
void sequencial(int vermelho[], int azul[],int branco[], int intervalo){
 // inicia as posições dos leds de acordo com as pilhas e os controladores(sequencia e didPush).
 int posV = pilhaV.size();
 int posA = pilhaA.size();
 int posB = pilhaB.size();
 int sequencia = 1;
 int didPush = false;
 
 while (true){ // enquanto não apertar um botão irá continuar fazendo a sequencia.
  switch(sequencia){
  // led vermelho
  case 1:
  digitalWrite(vermelho[posV],HIGH);
  didPush = timer(intervalo,sequencia,vermelho); // inicia o timer
  sequencia++;
  break;
  // led azul
  case 2:
  digitalWrite(azul[posA],HIGH);
  didPush = timer(intervalo,sequencia,azul); // inicia o timer
  sequencia++;
  break;
  // led branco
  case 3:
  digitalWrite(branco[posB],HIGH);
  didPush = timer(intervalo,sequencia,branco); // inicia o timer
  sequencia = 0;
  sequencia++;
  break;
  } //end switch
  // se algum botão foi apertado então irá verificar se ainda está apertado
  // e somente prosseguir se solta-lo.
  if(didPush){
    while(digitalRead(btnAdd)==LOW || digitalRead(btnRemove)==LOW){
      if (digitalRead(btnAdd)==HIGH && digitalRead(btnRemove)==HIGH)
      break;
  }
  break;
  // apaga os leds do momento para iniciar a sequencia novamente
  } else {
  //bip.bipIdle(sequencia);
  digitalWrite(vermelho[posV],LOW);
  digitalWrite(azul[posA],LOW);
  digitalWrite(branco[posB],LOW);

  bip.bipIdle(sequencia);
  delay(50);
  }
 }
}
/**
 * Responsável pelo intervalo entre cada led/sequencia e por verificar se um
 * botão foi pressionado durante o mesmo.
 * @param duration - duração do intervalo
 * @param sequencia - identificação de qual led está aceso para o checkButtons().
 * @param led[] - array com a pinagem dos leds para o checkButtons().
 */
bool timer(unsigned long duration, int sequencia, int led[])
{  
 unsigned long start = millis();
 // enquanto o intervalo for maior que o meu start(tempo em que começou o timer) menos o tempo atual...
 while (millis() - start <= duration) {
   bool didPress = checkButtons(sequencia, led);  // check the buttons
   if (didPress){
    return true;
   }
   }
  return false;
}
/**
 * Responsável por checar se o botão + ou - foi apertado no momento em que o led
 * está aceso.
 * @param sequencia - para saber qual led está aceso
 * @param led[] - array com as pinagens dos leds
 */
bool checkButtons(int sequencia, int led[]){
    boolean didPressAdd = digitalRead(btnAdd)==LOW; // Lê o botão +
    boolean didPressRemove = digitalRead(btnRemove)==LOW; // Lê o botão -
    // verifica se o botão + foi apertado e em qual led pela sequencia.
    if(didPressAdd){
      bip.bipAdd();
      didPressAdd = false;
      if (sequencia==1){
       pilhaV.push(led[pilhaV.size()]);
       return true;
    } else if (sequencia==2){
        pilhaA.push(led[pilhaA.size()]);
        return true;
    } else if (sequencia==3){
        pilhaB.push(led[pilhaB.size()]);
        return true;
    }
   // verifica se o botão - foi apertado e em qual led pela sequencia.
 } else if(didPressRemove){
      bip.bipRemove();
      didPressRemove = false;
      if (sequencia==1){
       digitalWrite(led[pilhaV.size()], LOW);
       pilhaV.pop();
       return true;
    } else if (sequencia==2){
       digitalWrite(led[pilhaA.size()], LOW);
       pilhaA.pop();
       return true;
    } else if (sequencia==3){
       digitalWrite(led[pilhaB.size()], LOW);
       pilhaB.pop();
       return true;
    }
 }
 return false;
}
/**
 * Função responsável por verificar a cada turno se algum jogador venceu
 */
void didWin(){
  if(pilhaV.isFull() || pilhaA.isFull() || pilhaB.isFull()){
    reset();
  } 
}
/**
* Função responsável por resetar o jogo quando algum jogador ganhar!
*/
void reset(){
  inGame = false;
  turn = 0;
  // apaga todos os leds.
  for (int i=0; i<sizeof(ledVermelho)/sizeof(char *);i++){
    digitalWrite(ledVermelho[i],LOW);
    digitalWrite(ledAzul[i],LOW);
    digitalWrite(ledBranco[i],LOW); 
  }
  // reseta as pilhas
  pilhaV.clear();
  pilhaA.clear();
  pilhaB.clear();
  bip.bipFinish();
}
/**
 * Irá mostrar no Monitor Serial o placar(Jogador da vez, pontuação e dificuldade)
 */
void placar(){
  Serial.println("---------------------------------------------");
    Serial.print("Jogador na vez: ");Serial.print(turn+1); Serial.print(" Dificuldade: "); Serial.println(jogadorDifficulty[turn]);
    Serial.print("Vermelho: "); Serial.println(pilhaV.size());
    Serial.print("Azul: "); Serial.println(pilhaA.size());
    Serial.print("Branco: "); Serial.println(pilhaB.size());
    Serial.println("---------------------------------------------");
}
/**
 * Quando iniciado, iniica uma animação dos leds.
 */
void show(){
  int fila = 0;
  while(true){
  if(fila%2==0){ // Par
  acenderEsquerdaDireita(fila, HIGH);
  delay(10);
  acenderEsquerdaDireita(fila, LOW);
  } else { // Impar
    acenderDireitaEsquerda(fila, HIGH);
    delay(10);
    acenderDireitaEsquerda(fila, LOW);
  }
  fila++;
  if(fila==4){
    fila=2;
    break;
  }
  }
  while(true){
    if(fila%2==0){ // Par
  acenderEsquerdaDireita(fila, HIGH);
  delay(10);
  acenderEsquerdaDireita(fila, LOW);
  } else { // Impar
    acenderDireitaEsquerda(fila, HIGH);
    delay(10);
    acenderDireitaEsquerda(fila, LOW);
  }
  fila--;
  if(fila==0){
    break;
  }
  }
}
/**
 * Acende os leds da Esquerda para a Direita
 */
void acenderEsquerdaDireita(int index, int turn){
  digitalWrite(ledVermelho[index],turn);
  delay(LEDINTERV);
  digitalWrite(ledAzul[index],turn);
  delay(LEDINTERV);
  digitalWrite(ledBranco[index],turn);
  delay(LEDINTERV);
}
/**
 * Acende os leds da Direita para a Esquerda
 */
void acenderDireitaEsquerda(int index, int turn){
  digitalWrite(ledBranco[index],turn);
  delay(LEDINTERV);
  digitalWrite(ledAzul[index],turn);
  delay(LEDINTERV);
  digitalWrite(ledVermelho[index],turn);
  delay(LEDINTERV);
}

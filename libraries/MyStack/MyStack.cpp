#include "MyStack.h"

// Constructors
MyStack::MyStack(int tam)
{
  tamanho = tam;
  top = -1;
  //Serial.println("MyStack - Pilha criada!");
}

// Pop - irá retirar um item caso não esteja vazia
int MyStack::pop()
{
  if (!isEmpty()){
  	apagarLed(stack[top]);
  	top--;
  } else {
  	//Serial.println("MyStack - ERRO - Pilha vazia!");
  }
  return stack[top];
}

// Push - irá adicionar um item caso não esteja cheia
void MyStack::push(int item)
{
  if (!isFull()){
  	top++;
  	stack[top] = item;
  	acenderLed(stack[top]);
  } else {
  	//Serial.println("MyStack - ERRO - Pilha cheia!");
  }
}

// IsEmpty - retorna se a pilha está vazia
boolean MyStack::isEmpty()
{
  return top==-1;
}

// IsFull - retorna se a pilha está cheia
boolean MyStack::isFull()
{
  return size()==(sizeof(stack)/sizeof(char *));
}

int MyStack::size()
{
  return top+1;
}
void MyStack::clear(){
	top=-1;
}
void MyStack::acenderLed(int led)
{
	digitalWrite(led, HIGH);
}

void MyStack::apagarLed(int led)
{
	digitalWrite(led, LOW);
}

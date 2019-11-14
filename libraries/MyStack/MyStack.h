#ifndef MYSTACK_H
#define MYSTACK_H

#include <Arduino.h>
class MyStack 
{
public:
  MyStack(int tam);
  //Métodos
  int pop();
  void push(int item);
  void clear();
  boolean isEmpty();
  boolean isFull();
  int size();

private:
  int top;
  int tamanho;
  int stack[4];
  void acenderLed(int led);
  void apagarLed(int led);
};


#endif

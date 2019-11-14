#ifndef BIPPER_H
#define BIPPER_H

#include <Arduino.h>
class Bipper
{
public:
  Bipper(int pin);
  //Métodos
  void bipAdd();
  void bipRemove();
  void bipStart();
  void bipFinish();
  void bipHard();
  void bipNormal();
  void bipIdle(int pos);
private:
int buzzer;

};


#endif

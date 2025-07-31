#include "moteur.h"
#include "config.h"
void init_moteur()
{
  pinMode(PIN_VANNE1, OUTPUT);
  pinMode(PIN_VANNE0, OUTPUT);
  digitalWrite(PIN_VANNE1, LOW);
  digitalWrite(PIN_VANNE0, LOW);
}

void vanneF()
{
  digitalWrite(PIN_VANNE1, HIGH);
  digitalWrite(PIN_VANNE0, LOW);
}

void vanneO()
{
  digitalWrite(PIN_VANNE1, LOW);
  digitalWrite(PIN_VANNE0, HIGH);
}

void vanneOff()
{
  digitalWrite(PIN_VANNE1, LOW);
  digitalWrite(PIN_VANNE0, LOW);
}
#include "timers.h"

void Timers::startDisplayTimer(void)
{
  /* Set the TCNT0 interrupt to occur every 10ms
    When System clock is 16MHz and prescaled by 8, a timer tick occurs every 500uS.
    Thus, 20 timer ticks = 10mS; OCR0A = 20.
  */

  /* Normal port operation on I/O pins | WGM mode = CTC */
  TCCR0A = (0 << COM0A1) | (0 << COM0A0) | (0 << COM0B1) | (0 << COM0B0) | (1 << WGM01) | (0 << WGM00);
  /* Clear Timer/Counter */
  TCNT0 = 0;
  /* Set OCR0A to generate interrupt every 10mS when clock prescaler set to div. by 8. */
  OCR0A = 20;
  /* Enable interrupt generation on OCIE0A match */
  TIMSK0 = (0 << OCIE0B) | (1 << OCIE0A) | (0 << TOIE0);
  /* Start timer */
  TCCR0B = (0 << FOC0A) | (0 << FOC0B) | (0 << WGM02) | (0 << CS02) | (0 << CS01) | (1 << CS00);
}

void Timers::stopDisplayTimer(void)
{
  /* Stop timer (Set all Clock Select (CS) bits to 0) */
  TCCR0B = (0 << FOC0A) | (0 << FOC0B) | (0 << WGM02) | (0 << CS02) | (0 << CS01) | (0 << CS00);
}

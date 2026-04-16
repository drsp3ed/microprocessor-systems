volatile bool pause = false; // button press indicator
volatile uint8_t i = 0; // counter
volatile unsigned long lastPressTime = 0;
// decimal values:                 0     1     2     3     4     5     6     7     8     9
volatile uint8_t seg_table[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
void setup() 
{
  // put your setup code here, to run once:
  DDRB &= ~(1 << PB0);
  PORTB |= (1 << PB0);
  PCICR |= (1 << PCIE0);
  PCMSK0 |= (1 << PCINT0);

  DDRD = 0b11111111;

  TCCR1A = 0;
  TCCR1B = 0;

  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12);

  TIMSK1 |= (1 << OCIE1A);
  OCR1A = 31249;

  sei();
}

ISR(TIMER1_COMPA_vect) 
{
  if (!pause) 
  {
    PORTD = seg_table[i];
    i++;
    if (i > 9) i = 0;
  }
}

ISR(PCINT0_vect) {
  if (!(PINB & (1 << PB0))) {
    if ((millis() - lastPressTime) > 50) {
      pause = !pause;
      lastPressTime = millis();
    }
  }
}

void loop() 
{
  // it honestly feels like a competitive programming problem lol. well good for me i guess
}

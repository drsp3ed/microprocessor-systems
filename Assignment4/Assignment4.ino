volatile uint8_t cnt = 0;

void setup() {
  // put your setup code here, to run once:
  DDRD &= ~(1 << 2);
  PORTD |= (1 << 2);

  DDRB |= (1 << 0);

  EICRA |= (1 << ISC01);
  EICRA &= ~(1 << ISC00);
  EIMSK |= (1 << INT0);

  TCCR1A = 0;
  TCCR1B = 0;

  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS11) | (1 << CS10);

  TIMSK1 |= (1 << OCIE1A);

  OCR1A = 249;

  sei();
}

ISR(INT0_vect) {
  PORTB |= (1 << 0);
  cnt = 10;
}

ISR(TIMER1_COMPA_vect) {
  if (cnt > 0) cnt--;
  if (cnt <= 0) {
    PORTB &= ~(1 << 0);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}

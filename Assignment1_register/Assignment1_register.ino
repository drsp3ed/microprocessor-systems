void setup() {
  // put your setup code here, to run once:
  DDRB |= (1 << 5);
}

void loop() {
  // put your main code here, to run repeatedly:
  PORTB |= (1 << 5);
  _delay_ms(500);
  PORTB &= ~(1 << 5);// 00010000
  _delay_ms(500);
}

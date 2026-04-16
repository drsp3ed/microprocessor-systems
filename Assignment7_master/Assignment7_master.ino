uint8_t nums[] = {85, 170, 255};

void SPI_init()
{
  DDRB |= (1 << PB3) | (1 << PB5) | (1 << PB2); // MOSI, SS, SCK
  DDRB &= ~(1 << PB4); // MISO

  SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

void SPI_send(uint8_t data)
{
  SPDR = data;

  while (!(SPSR & (1 << SPIF)));
}

void setup() {
  SPI_init();

}

void loop() {
  for (int i = 0; i < 3; i++)
  {
    PORTB &= ~(1 << PB2);
    SPI_send(nums[i]);
    PORTB |= (1 << PB2);
    delay(500);
  }
}

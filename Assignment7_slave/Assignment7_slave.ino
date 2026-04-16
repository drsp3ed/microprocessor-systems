void SPI_init_slave() {
    DDRB &= ~((1 << PB3) | (1 << PB5) | (1 << PB2));
    DDRB |= (1 << PB4);

    SPCR = (1 << SPE);
}

uint8_t SPI_receive() {
    while (!(SPSR & (1 << SPIF)));

    return SPDR;
}

void setup() {
    Serial.begin(9600);
    SPI_init_slave();
}

void loop() {
    uint8_t received = SPI_receive();
    Serial.println(received);
}
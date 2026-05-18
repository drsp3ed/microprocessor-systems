void SPI_init_slave() {
    DDRB &= ~((1 << PB3) | (1 << PB5) | (1 << PB2));
    DDRB |= (1 << PB4);

    SPCR = (1 << SPE);
}

uint8_t SPI_receive() {
    while (!(SPSR & (1 << SPIF)));
    return SPDR;
}

void UART_init(unsigned int baud) {
    unsigned int ubrr = F_CPU/16/baud - 1; // formula is UBRR = F_cpu / (16 * baud rate) - 1

    UBRR0H = (unsigned char)(ubrr >> 8); // upper 8 bits
    UBRR0L = (unsigned char)ubrr; // lower 8 bits is the actual ubrr

    UCSR0B = (1 << TXEN0); // TX enable; automatically sets UCSZ02 = 0 for the next step
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // with UCSZ02 = 0, 8 bits are sent; 1 stop bit. Also sets other bits to 0.
}

void UART_send(uint8_t data) {
    while (!(UCSR0A & (1 << UDRE0))); // flag. check till buffer empty and UART ready to read
    UDR0 = data; // load byte into UDR0. UART starts transmission.
}

void UART_printNumber(uint8_t num) { // convert number into a its ASCII alternative and send it via UART
    char buffer[4];
    itoa(num, buffer, 10);

    for (int i = 0; buffer[i] != '\0'; i++) {
        UART_send(buffer[i]); // send characters, not digits
    }

    UART_send('\n');
}

void setup() {
    UART_init(9600);
    SPI_init_slave();
}

void loop() {
    uint8_t received = SPI_receive();
    UART_printNumber(received);
}
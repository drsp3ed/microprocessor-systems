uint8_t cnt = 0;
const uint8_t addr = 0x00;

// NOTE: 0x1F = EECR; 0x20 = EEDR; 0x21 = EEARL; 0x22 = EEARH

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  asm volatile (
    "out 0x21, %[y]\n"
    "out 0x22, r1\n"
    "sbi 0x1F, 0\n"
    "in %[x], 0x20\n"
    : [x] "=r" (cnt)
    : [y] "r" (addr)
    :
  );
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Enter 's' to save cnt in EEPROM, 'c' to reset:");

  asm volatile (
    "mov r16, %[x]\n"
    "inc r16\n"
    "mov %[x], r16\n"
    : [x] "+r" (cnt)
    :
    : "r16"
  );
  Serial.print("The counter is: "); Serial.println(cnt);
  delay(1000);
  char option;
  if (Serial.available())
  {
    option = Serial.read();
    if (option == 's')
    {
      asm volatile (
      "mov r16, %[y]\n"
      "mov r17, %[x]\n"

      "continue:\n"
      "sbic 0x1F, 1\n"
      "rjmp continue\n"

      "out 0x21, r16\n"
      "out 0x22, r1\n"
      "out 0x20, r17\n"
      "sbi 0x1F, 2\n"
      "sbi 0x1F, 1\n"
      : [x] "+r" (cnt)
      : [y] "r" (addr)
      : "r16", "r17"
      );
    } else if (option == 'c') {
      asm volatile (
        "mov r16, %[y]\n"
        "mov r17, %[x]\n"

        "wait:\n"
        "sbic 0x1F, 1\n"
        "rjmp wait\n"

        "out 0x21, r16\n"
        "out 0x22, r1\n"
        "clr r17\n"
        "out 0x20, r17\n"
        "sbi 0x1F, 2\n"
        "sbi 0x1F, 1\n"
        "mov %[x], r17\n"
        : [x] "+r" (cnt)
        : [y] "r" (addr)
        : "r16", "r17"
        );
    } else {
      Serial.println("Please, enter either 's' or 'c'");
    }

    while(Serial.available() > 0) Serial.read();
  
  }
}

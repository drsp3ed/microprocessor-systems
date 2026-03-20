void setup() {
  Serial.begin(9600);
}

void loop() {
  uint8_t a, b;
  if (Serial.available())
  {
    a = Serial.parseInt();
    b = Serial.parseInt();

    uint8_t status;
    uint8_t result;

    asm volatile (
      "mov r16, %[x]\n"
      "mov r17, %[y]\n"
      "sub r16, r17\n"
      "in %[s], __SREG__\n"
      "mov %[r], r16"
      : [r] "=r" (result),
        [s] "=r" (status)
      : [x] "r" (a),
        [y] "r" (b)
      : "r16", "r17"
    );
    int zero = (status >> 1) & 1;
    int carry = (status >> 0) & 1;

    Serial.print("A = "); Serial.println(a);
    Serial.print("B = "); Serial.println(b);
    Serial.print("Result = "); Serial.println(result);
    Serial.print("Zero Flag = "); Serial.println(zero);
    Serial.print("Carry Flag = "); Serial.println(carry);
    Serial.println("-------------------------");

    delay(100);
  }
}
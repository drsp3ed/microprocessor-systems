#include <avr/io.h>
#include <util/delay.h>
#include <Arduino.h>

// i ABSOLUTELY HAD TO write comments for this one because i was getting lost way too much

extern "C" {
  void __attribute__((used, noinline, noreturn)) modeA_entry();
  void __attribute__((used, noinline, noreturn)) modeB_entry();
  void __attribute__((used, noinline, noreturn)) modeC_entry();
}

volatile uint8_t effect = 0; // action tracker
volatile uint8_t effectIdx = 0; // for jmp table
bool ledOn = false; // tracks what LED should be at this iteration. Not "volatile" cuz not changed in ASM.

void applyLED() {
  if (effect == 1) { // inverter - ON = OFF and vice versa
    if (ledOn) PORTB &= ~(1 << 5); else PORTB |= (1 << 5);
  } else if (effect == 2) { // brightness sim - PWM.
    if (ledOn) {
      PORTB |= (1 << 5);
      _delay_us(200);
      PORTB &= ~(1 << 5);
      _delay_us(800);
    } else {
      PORTB &= ~(1 << 5);
      _delay_us(1000);
    }
  } else {
    if (ledOn) PORTB |= (1 << 5); else PORTB &= ~(1 << 5);
  }
} // Decides what to do with the onboard LED (portb, bit 5) depending on current action and ledON flag

void tick(uint16_t ms) { // used instead of simple delays to put lotta processes within each 1ms
  for (uint16_t t = 0; t < ms; t++) {
    static bool prevBtn = false; // static = persistence
    bool btn = (PINB & (1 << 0)) == 0; // checks if the button is pressed

    if (btn && !prevBtn) { // falling edge detection
      asm volatile( // load the Z register depending on the action and execute ijmp
        "ldi r30, lo8(pm(JumpTable_%=)) \n\t"
        "ldi r31, hi8(pm(JumpTable_%=)) \n\t"
        "add r30, %[idx] \n\t"
        "adc r31, __zero_reg__ \n\t"
        "ijmp \n\t"
        "JumpTable_%=: \n\t" // set up the jump table
        "rjmp Action0_%= \n\t"
        "rjmp Action1_%= \n\t"
        "rjmp Action2_%= \n\t"
        "rjmp Action3_%= \n\t"
        "Action0_%=: ldi %[fx], 1 \n\t ldi %[idx], 1 \n\t rjmp Done_%= \n\t"
        "Action1_%=: ldi %[fx], 2 \n\t ldi %[idx], 2 \n\t rjmp Done_%= \n\t"
        "Action2_%=: ldi %[fx], 0 \n\t ldi %[idx], 3 \n\t rjmp Done_%= \n\t"
        "Action3_%=: ldi %[fx], 0 \n\t ldi %[idx], 0 \n\t rjmp Done_%= \n\t"
        "Done_%=: \n\t" // using the "%=" to avoid name collisions
        : [idx] "+r" (effectIdx), [fx] "+r" (effect)
        : : "r30", "r31"
      );
      _delay_ms(50);
    }

    prevBtn = btn;
    applyLED();
    if (effect != 2) _delay_ms(1); // this 1ms break is too long and would break the dimness effect
  }
}

extern "C" {
  void modeA_entry() // makes the LED to light up and down every 800ms (slow blink)
  { 
    while (1) 
    { 
      ledOn = 1;
      tick(800);
      ledOn = 0;
      tick(800);
    } 
  } 
  void modeB_entry() // makes the LED light up for 150ms then down for 150ms then up for 150 ms then down for 800 ms (double blink)
  { 
    while (1) 
    { 
      ledOn = 1;
      tick(150);
      ledOn = 0;
      tick(150);
      ledOn = 1;
      tick(150);
      ledOn = 0;
      tick(800); 
    } 
  }
  void modeC_entry() // makes the LED light up and down every 50ms (fast blink)
  { 
    while (1) 
    { 
      ledOn = 1;
      tick(50);
      ledOn = 0;
      tick(50);  
    }
  }
}

void setup() {
  // set PORTB, bit 5 as output (LED); PORTB, bit 0 as input (button)
  DDRB |= (1 << 5);
  DDRB &= ~(1 << 0);
  PORTB |= (1 << 0);

  // boot loop, waiting for 1st click
  asm volatile(
    "1: sbic %0, 0\n\t"
    "rjmp 1b\n\t"
    : : "I" (_SFR_IO_ADDR(PINB))
  );

  // after 1st click, light LED up for 200ms
  PORTB |= (1 << 5); 
  _delay_ms(200);
  PORTB &= ~(1 << 5);

  // set press count to 1 by default, increase every time the button is pressed
  uint8_t presses = 1;
  uint32_t startTime = millis();
  bool prevBtn = false;

  while (millis() - startTime < 2000) { // while loop to track 2 seconds (2000 ms)
    bool btn = (PINB & (1 << 0)) == 0;
    if (btn && !prevBtn) // rising edge button detection  
    {
      presses++;
      _delay_ms(50); // 50ms window
    }
    prevBtn = btn;
  }

  if (presses == 1) asm volatile("jmp modeA_entry");
  else if (presses == 2) asm volatile("jmp modeB_entry");
  else asm volatile("jmp modeC_entry");
}

void loop() {

}
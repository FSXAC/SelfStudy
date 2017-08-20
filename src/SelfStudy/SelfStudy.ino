// Shift register pins
#define SH_CP 2
#define ST_CP 1
#define DS 0

// Button input pins
#define BTN_A 3
#define BTN_B 4

// Global variables
volatile uint8_t minutes = 0;
volatile uint8_t seconds = 0;
volatile uint8_t counter = 0;

// Interrupt Service Routine (ISR)
// Pin change interrupt
ISR(PCINT0_vect) {
}

// Timer 1 interrupt
ISR(TIMER1_COMPA_vect) {
}

void setup() {
    pinMode(SH_CP, OUTPUT);
    pinMode(ST_CP, OUTPUT);
    pinMode(DS, OUTPUT);

    pinMode(BTN_A, INPUT_PULLUP);
    pinMode(BTN_B, INPUT_PULLUP);

    // Setup pin change interrupt for buttons
    setupPCInterrupt();

    // Setup timer 1 (synchronous mode) for timing
    setupTimer1();
    
    // Enable interrupts
    sei();
}

void loop() {
}

static inline void setupTimer1() {
    TCCR1 |= (1 << CTC1);       // Set clear timer on compare match

    // System clock frequency (CK): 1E6 Hz
    // CK / prescaler value of 16384: count 61 times for 1 second
    // 1 / (CK / 16384) = 16.384ms
    // 61 * 16.384ms = 999.424ms (close enough)
    // Set clock prescaler to 16384
    TCCR1 |= (1 << CS13) | (1 << CS12) | (1 << CS11) | (1 << CS10);
    OCR1C = 61;                 // Set compare match value to 61
    TIMSK |= (1 << OCIE1A)      // Enable timer interrupt
}

//static inline void setupTimer1Async() {          
//    PLLCSR |= (1 << PLLE);      // Enables PLLE
//    delayMicroseconds(100);     // Wait for PLL to stablize
//
//    // Poll PLOCK until it turns on
//    while (!(PLLCSR & (1 << PLOCK)));
//    
//    PLLCSR |= (1 << PCKE);      // Enables PCKE
//}

static inline void setupPCInterrupt() {
    GIMSK |= (1 << PCIE);       // Enable pin change interrupt (PCI)
    PCMSK |= (1 << PCINT3);     // Enable pin change interrupt on PCINT3
    PCMSK |= (1 << PCINT4);     // Enable pin change interrupt on PCINT4
}


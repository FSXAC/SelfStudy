// Shift register pins
#define SH_CP 2
#define ST_CP 1
#define DS 0

// Button input pins
#define BTN_A 3
#define BTN_B 4

// Other constants
#define SECONDS_IN_MIN 60

// Global variables
volatile uint8_t minutes = 0;
volatile uint8_t seconds = 0;
volatile bool secondsChanged = false;
bool workMode = true;

// Interrupt Service Routine (ISR)
// Pin change interrupt
ISR(PCINT0_vect) {
}

// Timer 1 interrupt
ISR(TIMER1_COMPA_vect) {
    if (seconds == SECONDS_IN_MIN - 1) {
        seconds = 0;
        minutes++;
    } else {
        seconds++;
    }
    secondsChanged = true;
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

    // Clear shift registers
    shiftOut(DS, SH_CP, MSBFIRST, 0x00);
    shiftLatch();
}

uint8_t workMinutes = 15;
uint8_t breakMinutes = 3;
uint16_t workTime = SECONDS_IN_MIN * workMinutes;
uint16_t breakTime = SECONDS_IN_MIN * breakMinutes;

bool playBreakTimeAnimation = false;
bool playWorkTimeAnimation = true;

void loop() {
    if (secondsChanged) {
        uint8_t outputLED = 0x00;
        uint8_t progress = 0x00;

        if (workMode) {

            // progress in integer = time elapsed (seconds) * 8 lights / total time
            progress = ((int)minutes * SECONDS_IN_MIN + seconds) * 8 / workTime;

            if (minutes == workMinutes) {
                minutes = 0;
                workMode = false;
                playBreakTimeAnimation = true;
            }

        } else {
            progress = ((int)minutes * SECONDS_IN_MIN + seconds) * 8 / breakTime;

            if (minutes == breakMinutes) {
                minutes = 0;
                workMode = true;
                playWorkTimeAnimation = true;
            }
        }


        // Shift into LEDs
        for (unsigned char i = 0; i < progress; i++) {
            outputLED |= 0x01 << i;
        }

        // Blink on the next LED to indicate progress
        if (seconds % 2) {
            outputLED |= 0x01 << progress;
        } else {
            outputLED &= ~(0x01 << progress);
        }

        // Output to shift registers
        shiftOut(DS, SH_CP, MSBFIRST, outputLED);
        shiftLatch();
        secondsChanged = false;
    }

    if (playBreakTimeAnimation) breaktimeAnimation();
    if (playWorkTimeAnimation) worktimeAnimation();
}


// Play a two second animation
void breaktimeAnimation() {
    for (uint8_t i = 0; i < 40; i++) {
        digitalWrite(DS, ((i / 8) % 2) ? HIGH : LOW);
        shiftClock();
        shiftLatch();
        delay(50);
    }
    playBreakTimeAnimation = false;
}

void worktimeAnimation() {
    for (uint8_t i = 0; i < 40; i++) {
        shiftOut(DS, SH_CP, MSBFIRST, ((i / 4) % 2) ? 0xFF : 0x00);
        shiftLatch();
        delay(50);
    }

    playWorkTimeAnimation = false;
}


static inline void setupTimer1() {
    TCCR1 |= (1 << CTC1);       // Set clear timer on compare match

    // System clock frequency (CK): 1E6 Hz
    // CK / prescaler value of 16384: count 61 times for 1 second
    // 1 / (CK / 16384) = 16.384ms
    // 61 * 16.384ms = 999.424ms (close enough)
    // Set clock prescaler to 16384
    TCCR1 |= (1 << CS13) | (1 << CS12) | (1 << CS11) | (1 << CS10);
    OCR1C = 3;                 // Set compare match value to 61
    TIMSK |= (1 << OCIE1A);     // Enable timer interrupt
}

static inline void setupPCInterrupt() {
    GIMSK |= (1 << PCIE);       // Enable pin change interrupt (PCI)
    PCMSK |= (1 << PCINT3);     // Enable pin change interrupt on PCINT3
    PCMSK |= (1 << PCINT4);     // Enable pin change interrupt on PCINT4
}

void shiftClock() {
    digitalWrite(SH_CP, HIGH);
    digitalWrite(SH_CP, LOW);
}

void shiftLatch() {
    digitalWrite(ST_CP, HIGH);
    digitalWrite(ST_CP, LOW);
}


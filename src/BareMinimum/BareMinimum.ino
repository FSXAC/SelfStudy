// ATTINY Bare minimum shift register control

#define SH_CP 2
#define ST_CP 1
#define DS 0

void setup() {
    pinMode(SH_CP, OUTPUT);
    pinMode(ST_CP, OUTPUT);
    pinMode(DS, OUTPUT);
}

int count = 0;
void loop() {
    digitalWrite(DS, (count == 0) ? HIGH : LOW);
    clock();
    latch();

    delay(200);
    count = (count == 5 ? 0 : count+1);
}

void clock() {
    digitalWrite(SH_CP, HIGH);
    digitalWrite(SH_CP, LOW);
}

void latch() {
    digitalWrite(ST_CP, HIGH);
    digitalWrite(ST_CP, LOW);
}


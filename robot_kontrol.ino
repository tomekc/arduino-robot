#include <Arduino.h>

// Definicja pinów przycisków
#define BTN_AXIS1_FWD A9
#define BTN_AXIS1_REV A8

// Piny silnika krokowego
#define STEP_AXIS1_DIR 2
#define STEP_AXIS1_STEP 3

// Krańcówki
#define LIMIT_AXIS1_BACK A0

class Button {
public:
    Button(int pin);
    bool Pressed();
private:
    int pin;
};

Button::Button(int p) {
    pin = p;
    pinMode(pin, INPUT_PULLUP);
}

bool Button::Pressed() {
    return digitalRead(this->pin) == false;
}

#define DIR_FORWARD  0
#define DIR_BACKWARD 1

class Stepper {
public:
    Stepper(int dirPin, int stepPin);
    void SetLimitPins(int fwd, int back);
    void Step(int delay);
    void SetDirection(int dir);
    bool LimiterEnabled();
private:
    int stepPin;
    int dirPin;
    int backLimitSwitchPin;
    int forwardLimitSwitchPin;
    int direction;
};

Stepper::Stepper(int dirPin, int stepPin) {
    this->stepPin = stepPin;
    this->dirPin = dirPin;
    this->backLimitSwitchPin = -1; // -1 oznacza brak krańcówki
    this->forwardLimitSwitchPin = -1;
    pinMode(dirPin, OUTPUT);
    pinMode(stepPin, OUTPUT);
    this->SetDirection(DIR_FORWARD);
}

void Stepper::SetDirection(int dir) {
    if (dir == DIR_FORWARD) {
        this->direction = dir;
        digitalWrite(this->dirPin, HIGH);
    } else if (dir == DIR_BACKWARD) {
        this->direction = dir;
        digitalWrite(this->dirPin, LOW);
    }
}

void Stepper::SetLimitPins(int fwd, int back) {
    this->backLimitSwitchPin = back;
    this->forwardLimitSwitchPin = fwd;
    pinMode(fwd, INPUT_PULLUP);
    pinMode(back, INPUT_PULLUP);
}

bool Stepper::LimiterEnabled() {
    return this->backLimitSwitchPin != -1 && this->forwardLimitSwitchPin != -1;
}

void Stepper::Step(int delay) {
    if (this->LimiterEnabled()) {
        bool backLimiter = digitalRead(this->backLimitSwitchPin);
        bool forwardLimiter = digitalRead(this->forwardLimitSwitchPin);
        if (this->direction == DIR_BACKWARD && backLimiter == false) {
            return;
        }
        if (this->direction == DIR_FORWARD && forwardLimiter == false) {
            return;
        }
    }
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(delay);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(delay);
}


// Tutaj deklarujemy obiekty przycisków, podając w argumencie konstruktora pin do którego jest podłączony
Button axis1aButton = Button(BTN_AXIS1_FWD);
Button axis1bButton = Button(BTN_AXIS1_REV);

// Tutaj definiujemy silnik krokowy, podajemy piny kierunku i kroku
Stepper axis1Stepper = Stepper(STEP_AXIS1_DIR, STEP_AXIS1_STEP);

// Odstęp czasowy w mikrosekundach kroków. Mniejsza wartość przyspiesza silniki, dobrać eksperymentalnie.
#define STEP_DELAY 2000

void setup() {
    // dla uproszczenia obie krańcówki są na jednym pinie
    axis1Stepper.SetLimitPins(LIMIT_AXIS1_BACK, LIMIT_AXIS1_BACK);
}

void loop() {
    // put your main code here, to run repeatedly:


    if (axis1aButton.Pressed() || axis1bButton.Pressed()) {
        if (axis1aButton.Pressed()) {
            axis1Stepper.SetDirection(DIR_FORWARD);
        }
        if (axis1bButton.Pressed()) {
            axis1Stepper.SetDirection(DIR_BACKWARD);
        }

        axis1Stepper.Step(STEP_DELAY);
    }


}
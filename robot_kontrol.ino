#include <Arduino.h>

// Definicja pinów przycisków

// Piny silnika krokowego

// Krańcówki

// Odstęp czasowy w mikrosekundach kroków. Mniejsza wartość przyspiesza silniki, dobrać eksperymentalnie.
#define STEP_DELAY 2000

// jaką wartość zwraca krańcówka gdy jest uderzona
// NC oznacza stan wysoki po wciśnięciu
#define LIMITER_HIT true

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
        if (this->direction == DIR_BACKWARD && backLimiter == LIMITER_HIT) {
            return;
        }
        if (this->direction == DIR_FORWARD && forwardLimiter == LIMITER_HIT) {
            return;
        }
    }
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(delay);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(delay);
}

class Controller {
public:
    Controller(Stepper *stepper, Button *forwardButton, Button *backButton) : stepper(stepper),
                                                                              forwardButton(forwardButton),
                                                                              backButton(backButton) {}

    void Process() {
        // albo jeden albo drugi
        if (backButton->Pressed() != forwardButton->Pressed()) {
            if (backButton->Pressed()) {
                stepper->SetDirection(DIR_BACKWARD);
            }
            if (forwardButton->Pressed()) {
                stepper->SetDirection(DIR_FORWARD);
            }
            stepper->Step(STEP_DELAY);
        }
    }
private:
    Stepper *stepper;
    Button *forwardButton;
    Button *backButton;
};


// Tutaj deklarujemy obiekty przycisków, podając w argumencie konstruktora pin do którego jest podłączony
// W nawiasach symbol pinu do którego jest podpięty przycisk, który po wciśnięciu zwiera do masy
Button axis1ButtonLeft = Button(A9);
Button axis1ButtonRight = Button(A8);

// Tutaj definiujemy silnik krokowe
// W nawiasach podajemy numery pinów (dir, step)
Stepper axis1Stepper = Stepper(2, 3);

Controller axis1 = Controller(&axis1Stepper, &axis1ButtonLeft, &axis1ButtonRight);

void setup() {

    axis1Stepper.SetLimitPins(A0, A0);
}

void loop() {

    axis1.Process();
    delay(10);
}

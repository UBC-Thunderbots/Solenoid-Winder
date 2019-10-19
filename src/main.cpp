#include "libraries.hpp"
#include "serialparser/serialparser.hpp"
#include "util.hpp"

// mm
float wire_width = 0.8118;
float spool_width = 25;

float rev_per_s = 1;
float running = false;
uint16_t n_passes = 0;
int8_t direction = 1;

namespace Pins {
int const M1STEP = 5;  // D5
int const M1DIR = 2;   // D2

int const M2STEP = 6;  // D6
int const M2DIR = 3;   // D3

int const ENABLE = 8;  // D8
}  // namespace Pins

// 1.8-degree stepper
long const STEPS_PER_REV = 200;
// 1.8-degree stepper;
// 2mm pitch T8 thread
float const STEPS_PER_MM = 200.f /* step/rev */ / 2.f /* mm/rev */;

auto lateral_stepper =
    AccelStepper{AccelStepper::DRIVER, Pins::M1STEP, Pins::M1DIR};
auto rotational_stepper =
    AccelStepper{AccelStepper::DRIVER, Pins::M2STEP, Pins::M2DIR};
auto both = MultiStepper{};

float revs_for_lateral_dist(float dist) { return (dist / wire_width); }

long steps_for_revs(float revs) { return (revs / STEPS_PER_REV); }

long steps_for_lateral_mm(float dist) { return dist / STEPS_PER_MM; }

void print_help_messages() {
    Serial.print(
        F("Commands:\n"
          "w {wire width in mm}\n"
          "s {spool width in mm}\n"
          "z {zpeed in rpm}\n"
          "G {n_passes}: START: do this many passes back and forth\n"));
}

void move_both(long rotational, long lateral) {
    long moves[] = {rotational, lateral};
    both.moveTo(moves);
}

void move_dist_synchronized(float dist) {
    move_both(steps_for_revs(revs_for_lateral_dist(dist)),
              steps_for_lateral_mm(dist));
}

void clear_positions() {
    // Clear positions so we are moving relative to physical position
    lateral_stepper.setCurrentPosition(0);
    rotational_stepper.setCurrentPosition(0);
}

void handle_serial_command() {
    auto res = serialparser::parse(&Serial);
    switch (res.cmd) {
        case serialparser::Command::Go:
            Serial.println(F("Going!!"));
            break;

        case serialparser::Command::Speed:
            Serial.print(F("Speed is now: "));
            Serial.println(res.args[0]);
            break;

        case serialparser::Command::SpoolWidth:
            Serial.print(F("SpoolWidth is now: "));
            Serial.println(res.args[0]);
            break;

        case serialparser::Command::WireWidth:
            Serial.print(F("WireWidth is now: "));
            Serial.println(res.args[0]);
            break;

        case serialparser::Command::None:
        default:
            Serial.println(F("Unknown command"));
            break;
    }
}

void setup() {
    Serial.begin(9600);
    pinMode(Pins::ENABLE, OUTPUT);
    digitalWrite(Pins::ENABLE, 0);  // active low
    print_help_messages();
    both.addStepper(rotational_stepper);
    both.addStepper(lateral_stepper);
    while (true) {
        if (running) {
            both.run();
            if (lateral_stepper.currentPosition() ==
                    lateral_stepper.targetPosition() &&
                rotational_stepper.currentPosition() ==
                    rotational_stepper.targetPosition()) {
                // reached target
                if (n_passes == 0) {
                    running = false;
                }
                clear_positions();
                direction = -direction;
                --n_passes;
                move_dist_synchronized(direction * spool_width);
            }
        }
        if (Serial.available() > 0) {
            handle_serial_command();
        }
    }
}

void loop() {}
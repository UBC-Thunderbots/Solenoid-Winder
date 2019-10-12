#include "libraries.hpp"

// mm
float wire_width = 0.8118;
float spool_width = 25;

float rev_per_s = 1;
float running = false;
uint16_t n_passes = 0;
int8_t direction = 1;

// 1.8-degree stepper
long const STEPS_PER_REV = 200;
// 1.8-degree stepper;
// 2mm pitch T8 thread
float const STEPS_PER_MM = 200.f /* step/rev */ / 2.f /* mm/rev */;

auto lateral_stepper = AccelStepper{};
auto rotational_stepper = AccelStepper{};
auto both = MultiStepper{};

float revs_for_lateral_dist(float dist) { return (dist / wire_width); }

long steps_for_revs(float revs) { return (revs / STEPS_PER_REV); }

long steps_for_lateral_mm(float dist) { return dist / STEPS_PER_MM; }

void print_help_messages() {
    Serial.println(
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
    char cmd = Serial.read();
    Serial.read();
    float n = Serial.parseFloat();
    switch (cmd) {
        case 'w':
            wire_width = n;
            break;
        case 's':
            spool_width = n;
            break;
        case 'z':
            rev_per_s = n;
            break;
        case 'G':
            n_passes = n;
            running = true;
            break;
        default:
            Serial.println(F("Unrecognized command"));
            break;
    }
}

void setup() {
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
        if (Serial.available()) {
            handle_serial_command();
        }
    }
}

void loop() {}
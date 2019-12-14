# Solenoid Winder

This is a project to produce a solenoid winding machine to wind the solenoids
for UBC ThunderBots. It uses a design inspired by a 3D printer z-axis to
guide the wire onto a rotating solenoid body.

We use a control board designed for a laser cutter or other small CNC machine
to drive the steppers. The particular one used is the
["Akozon 3D Printer Engraving Expansion Board Kit Controller CNC Shield V4"](https://www.amazon.ca/gp/product/B07DS2T64W/)
with the Arduino Nano clone that comes with it. It should be noted that this
board contains a bug: the microstepping jumpers are configured to pull down
the configuration pins on the drivers (which have internal pull-downs),
meaning that there is no way to set the MS1, MS2 or MS3 pins to high. Due to
this bug, the drivers are forced to full steps. We may hack the stepper
driver boards so they have a bodge wire to directly pull these pins up and
cut the pins going to the buggy mainboard.

The winder is controlled by serial commands. I suggest
[CompuPhase Termite](https://www.compuphase.com/software_termite.htm) on
Windows to control the winder.

## Usage

The solenoid winder board must be connected to a 12v source. We usually use
one of the ThunderBots lab power supplies, set to a current of 1.5A. This
*only* powers the stepper motors, meaning that you can use the serial command
line interface without the external power on.

Due to the habit of the rotational stepper motor to heat up when it is left
on, it is advised to turn off the power supply while not actively winding. We
may replace this stepper with one that doesn't do that.

Set up the spool holder at least 30cm away from the winder while in use in
order to avoid any possible inconsistencies caused by winding of the source
spool. The intended attachment mechanism for this is to clamp it to the table.

<!-- TODO: put in a photo -->

### CLI operation

Set up your spool width with `spool_width {width in mm}`.

Set the wire width: `wire_width {width in mm}`

Then, you can move the carriage either by slowly rotating the lead screw by
hand with the power off or by using `move_lateral {distance_in_mm}`. Note
that the positive direction is marked on the base of the winder and is
oriented away from the stepper on the lead screw.

## Development

This project is developed using [PlatformIO](https://platformio.org/).

To build from the command line: `platformio run`

To upload from the command line (note that you need to disconnect your
Termite window before uploading or it will fail to upload):
`platformio run -t upload`.

## Board documentation

![board docs photo](./SolenoidWinderPCBDiagram.png)
#include "../util.hpp"
#include "sysheaders.h"
#pragma once
namespace serialparser {

uint8_t const max_nargs = 4;

enum class Command {
    None,
    WireWidth,
    SpoolWidth,
    Speed,
    Go,
};

struct CommandDef {
    const char *name;
    Command cmd;
    uint8_t nargs;
};

#define BULLSHIT(s) const char *const PROGMEM s = #s

namespace {
BULLSHIT(wire_width);
BULLSHIT(spool_width);
BULLSHIT(speed);
BULLSHIT(go);
}  // namespace

CommandDef const PROGMEM allCommands[] = {
    {wire_width, Command::WireWidth, 1},
    {spool_width, Command::SpoolWidth, 1},
    {speed, Command::Speed, 1},
    {go, Command::Go, 1},
};

struct ParseResult {
    Command cmd;
    float args[max_nargs];
};

ParseResult parse(Stream *ser);

}  // namespace serialparser
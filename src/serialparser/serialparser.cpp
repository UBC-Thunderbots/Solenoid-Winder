#include "serialparser.hpp"
namespace serialparser {

ParseResult parse(Stream* ser) {
    auto avail = ser->available();
    char buf[16];
    // read in the name of the command
    auto nread = ser->readBytesUntil(' ', buf, sizeof(buf) - 1);
    buf[nread + 1] = '\0';
    ser->print("grabbed:");

    ParseResult res;

    for (auto&& cmd : allCommands) {
        ser->write(pgm_read_byte(cmd.name));
        if (strcmp_P(buf, cmd.name) != 0) {
            continue;
        }
        res.cmd = cmd.cmd;

        for (uint8_t j = 0; j < util::nmin(cmd.nargs, max_nargs); ++j) {
            // this will blow up if you put in anything invalid. and there's
            // no detection of such. This is awful.
            res.args[j] = ser->parseFloat(SKIP_WHITESPACE);
        }

        return res;
    }
    res.cmd = Command::None;
    return res;
}

}  // namespace serialparser
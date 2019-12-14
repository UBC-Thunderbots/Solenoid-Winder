#include "serialparser.hpp"
namespace serialparser {

class MyStream : public Stream {
   public:
    size_t readBytesUntil(const char *terminators, char *buffer,
                          size_t length) {
        if (length < 1) return 0;
        size_t index = 0;
        while (index < length) {
            int c = this->timedRead();
            if (c < 0) break;
            char temp_s[2] = {static_cast<char>(c), '\0'};
            // if we *do* find a terminator in the buffer, get out
            if (strstr(terminators, temp_s) != nullptr) break;
            *buffer++ = static_cast<char>(c);
            index++;
        }
        return index;  // return number of characters, not including null
                       // terminator
    }
};

ParseResult parse(Stream *ser) {
    char buf[24];
    // read in the name of the command
    auto nread = (static_cast<MyStream *>(ser))
                     ->readBytesUntil(" \n\r", buf, sizeof(buf) - 1);
    buf[nread] = '\0';

    ParseResult res;

    for (auto cmd : allCommands) {
        if (strcmp(buf, cmd.name) != 0) {
            continue;
        }
        res.cmd = cmd.cmd;

        for (uint8_t j = 0; j < util::nmin(cmd.nargs, max_nargs); ++j) {
            // this will blow up if you put in anything invalid. and there's
            // no detection of such. This is awful.
            res.args[j] = ser->parseFloat(SKIP_WHITESPACE);
        }

        // clear the receive buffer in case we left stuff in it
        while (ser->available() > 0) ser->read();
        return res;
    }
    res.cmd = Command::None;
    return res;
}

}  // namespace serialparser
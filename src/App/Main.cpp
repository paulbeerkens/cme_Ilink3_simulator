#include "CMESimulator.h"
#include <Logger/LogMacros.h>

int main ([[maybe_unused]] int argc, [[maybe_unused]] char** args) {
    CMESimulator cmeSimulator;
    if (!cmeSimulator.setup()) {
        LOGERROR("Failed to setup. This is fatal.");
        return 1;
    }

    if (!cmeSimulator.run ()) {
        LOGERROR("Failed to run simulator");
        return 1;
    }
    return 0;
}

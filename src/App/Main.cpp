#include "CMESimulator.h"
#include <Logger/LogMacros.h>
#include <webserver/WebServer.h>

int main ([[maybe_unused]] int argc, [[maybe_unused]] char** args) {
    if (!webserver::WebServer::get ().start (26000)) {
        LOGERROR ("Failed to start webserver");
    }

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

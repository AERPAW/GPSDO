#include "gpsdo.h"

namespace gpsdo {
    Device::Device() {
        // DEBUG ONLY
    }

    Device::~Device() {
        // Close the device in case it's still open
        close();
    }

    bool Device::isOpen() {
        // TODO
        return false;
    }

    void Device::close() {
        // If the device is already closed, do nothing
        if (!isOpen()) { return; }

        // TODO
    }

    bool Device::gnssLocked() {
        return true;
    }

    bool Device::tcxoLocked() {
        return false;
    }

    Device open(const std::string& gnssPort, const std::string& tcxoPort) {
        // TODO

        return Device();
    }
}
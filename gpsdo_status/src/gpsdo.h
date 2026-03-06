#pragma once
#include <string>
#include <mutex>

namespace gpsdo {
    /**
     * GPSDO Device.
     * This class is NOT thread-safe.
    */
    class Device {
    public:
        // DEBUG ONLY
        Device();

        // Destructor
        ~Device();

        /**
         * Check if the device is open.
         * @return True if the device is open, false otherwise.
        */
        bool isOpen();

        /**
         * Close the device.
        */
        void close();

        /**
         * Query whether or not the GNSS receiver is locked.
         * @return True if it is locked, false if not.
        */
        bool gnssLocked();

        /**
         * Query whether or not the TCXO is locked.
         * @return True if it is locked, false if not.
        */
        bool tcxoLocked();

    private:
    };

    /**
     * Open a GPSO device using the serial ports of the GNSS receiver and TCXO.
     * @param gnssPort Serial port for the GNSS receiver.
     * @param tcxoPort Serial port for the TCXO.
     * @return Newly opened GPSDO device.
    */
    Device open(const std::string& gnssPort, const std::string& tcxoPort);
}
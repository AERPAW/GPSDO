#include <stdio.h>
#include <stdexcept>
#include "cli/cli.h"
#include "ansi/ansi.h"
#include "gpsdo.h"
#include "version.h"

// ANSI escape code macros for human-readable output
#define ANSI_RED    ANSI_CSI ANSI_SGR_FG_BRIGHT_RED
#define ANSI_GREEN  ANSI_CSI ANSI_SGR_FG_BRIGHT_GREEN
#define ANSI_RESET  ANSI_CSI ANSI_SGR_RESET

/**
 * Show the help dialog for the software given its command line interface.
 * @param iface Command line interface for the software.
*/
void showHelp(const cli::Interface& iface) {
    // Show the software version
    printf("gpsdo_status v" GPSDO_STATUS_VERSION "\n\n");

    // Show all valid command line arguments
    for (auto& [ name, arg ] : iface.arguments) {
        // Find the alias corresponding to the argument
        auto it = std::find_if(iface.aliases.begin(), iface.aliases.end(), [&name](std::pair<char, std::string> pair){ return pair.second == name; });
        
        // If found, save it, otherwise set it to null
        char alias = 0;
        if (it != iface.aliases.end()) { alias = it->first; }
        
        

    }
}

int main(int argc, char** argv) {
    try {
        // Define the command-line interface
        cli::Interface iface;
        iface.arg("gnss-port", 'g', "",    "Serial port used for the GNSS module");
        iface.arg("tcxo-port", 't', "",    "Serial port used for the TCXO module");
        iface.arg("json",      'j', false, "Show the status in software-readbale JSON format");
        iface.arg("help",      'h', false, "Show the help screen");

        // Parse the command line arguments
        auto cmd = cli::parse(iface, argc, argv);

        // If the help screen was requested, show it and exit
        if (cmd["help"] || argc == 1) {
            // Show the help screen
            showHelp(iface);

            // Exit successfully
            return 0;
        }

        // Make sure two ports were given
        std::string gnssPort = cmd["gnss-port"];
        std::string tcxoPort = cmd["tcxo-port"];
        if (gnssPort.empty() || tcxoPort.empty()) { throw std::runtime_error("Both a GNSS and TCXO port must be provided"); }

        // Open the device
        auto dev = gpsdo::open(gnssPort, tcxoPort);

        // Query the status
        bool gnssLocked = dev.gnssLocked();
        bool tcxoLocked = dev.tcxoLocked();

        // Display the status in the requested format
        if (cmd["json"]) {
            // Encode the status in JSON
            printf("{ \"gnssLocked\": %s, \"tcxoLocked\": %s }\n", gnssLocked ? "true":"false", tcxoLocked ? "true":"false");
        }
        else {
            // Show the status in a human-readable format
            printf("GNSS: %s\n" ANSI_RESET, gnssLocked ? ANSI_GREEN "Locked" : ANSI_RED "UNLOCKED");
            printf("TCXO: %s\n" ANSI_RESET, tcxoLocked ? ANSI_GREEN "Locked" : ANSI_RED "UNLOCKED");
        }

        // Exit succesfully
        return 0;
    }
    catch (const std::exception& e) {
        // Display the error
        fprintf(stderr, "ERROR: %s\n", e.what());
        
        // Exit unsuccessfully
        return -1;
    }
}
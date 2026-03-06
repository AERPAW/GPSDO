#!/usr/bin/env bash
# This is for mobile node
set -e

DEV="/dev/ttyACM0"

echo "Reset receiver to defaults"
ubxtool -f $DEV -p RESET
sleep 3

# It shouldn't be on survey in mode
ubxtool -f $DEV -z CFG-TMODE-MODE,0

# PPS configuration
ubxtool -f $DEV -z CFG-TP-TP1_ENA,1
ubxtool -f $DEV -z CFG-TP-SYNC_GNSS_TP1,1
ubxtool -f $DEV -z CFG-TP-USE_LOCKED_TP1,1

ubxtool -f $DEV -z CFG-TP-PERIOD_TP1,1000000
ubxtool -f $DEV -z CFG-TP-PERIOD_LOCK_TP1,1000000
ubxtool -f $DEV -z CFG-TP-LEN_TP1,100000
ubxtool -f $DEV -z CFG-TP-LEN_LOCK_TP1,100000

# UART RTCM mode
ubxtool -f $DEV -z CFG-UART1-BAUDRATE,115200
ubxtool -f $DEV -z CFG-UART1INPROT-RTCM3X,1
ubxtool -f $DEV -z CFG-UART1INPROT-UBX,1

# Enable NMEA + UBX
ubxtool -f $DEV -z CFG-USBOUTPROT-NMEA,1
ubxtool -f $DEV -z CFG-USBOUTPROT-UBX,1
ubxtool -f $DEV -z CFG-UART1OUTPROT-NMEA,1
ubxtool -f $DEV -z CFG-MSGOUT-NMEA_ID_GGA_USB,1
ubxtool -f $DEV -z CFG-MSGOUT-NMEA_ID_RMC_USB,1
ubxtool -f $DEV -z CFG-MSGOUT-NMEA_ID_GSA_USB,1
ubxtool -f $DEV -z CFG-MSGOUT-NMEA_ID_GSV_USB,1

# Save to flash
ubxtool -f $DEV -p SAVE

echo "Configuration complete."

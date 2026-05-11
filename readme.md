# AERPAW GPSDO

This repository contains the firmware, host utilities and design files for the AERPAW GPSDO.

The firmware is available in `firmware/` and the status utility in `gpsdo_status/`. See the readme in each for more information.

Install `gpsdo_cli` on Linux:

```sh
curl -fsSL https://raw.githubusercontent.com/AERPAW/GPSDO/main/gpsdo_cli/gpsdo_cli.py | sudo install -m 0755 /dev/stdin /usr/local/bin/gpsdo_cli
```

If `gpsdo_cli` cannot access `/dev/ttyACM*`, add your user to the serial device group and log out/in:

```sh
sudo usermod -aG dialout "$USER"
```

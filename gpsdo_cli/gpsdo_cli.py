#!/usr/bin/env python3

from __future__ import annotations

import argparse
import json
import os
import select
import termios
import time
import tty
from pathlib import Path


BANNER = r"""
    _    _____ ____  ____   _    __        __
   / \  | ____|  _ \|  _ \ / \   \ \      / /
  / _ \ |  _| | |_) | |_) / _ \   \ \ /\ / /
 / ___ \| |___|  _ <|  __/ ___ \   \ V  V /
/_/   \_\_____|_| \_\_| /_/   \_\   \_/\_/    GPSDO Host Utility @2026
""".strip("\n")

BAUDS = {
    9600: termios.B9600,
    19200: termios.B19200,
    38400: termios.B38400,
    57600: termios.B57600,
    115200: termios.B115200,
    230400: termios.B230400,
}


def find_port() -> str | None:
    for tty_path in sorted(Path("/sys/class/tty").glob("ttyACM*")) + sorted(
        Path("/sys/class/tty").glob("ttyUSB*")
    ):
        device = tty_path / "device"
        if not device.exists():
            continue
        parents = [device]
        try:
            parents.extend(device.resolve().parents)
        except OSError:
            pass
        for parent in parents:
            interface = parent / "interface"
            try:
                text = interface.read_text(errors="ignore").strip()
            except OSError:
                continue
            if "OCXO Port" in text:
                return f"/dev/{tty_path.name}"
    return None


class Serial:
    def __init__(self, path: str, baud: int):
        self.path = path
        self.baud = baud
        self.fd = None
        self.saved = None

    def __enter__(self):
        flag = BAUDS[self.baud]
        self.fd = os.open(self.path, os.O_RDWR | os.O_NOCTTY | os.O_NONBLOCK)
        self.saved = termios.tcgetattr(self.fd)
        tty.setraw(self.fd, termios.TCSANOW)
        attrs = termios.tcgetattr(self.fd)
        attrs[4] = flag
        attrs[5] = flag
        attrs[2] |= termios.CLOCAL | termios.CREAD
        attrs[2] &= ~termios.PARENB
        attrs[2] &= ~termios.CSTOPB
        attrs[2] &= ~termios.CSIZE
        attrs[2] |= termios.CS8
        termios.tcsetattr(self.fd, termios.TCSANOW, attrs)
        return self

    def __exit__(self, exc_type, exc, tb):
        if self.fd is None:
            return
        try:
            if self.saved is not None:
                termios.tcsetattr(self.fd, termios.TCSANOW, self.saved)
        except termios.error:
            pass
        os.close(self.fd)

    def write(self, text: str):
        data = text.encode("ascii", errors="ignore")
        sent = 0
        while sent < len(data):
            _, writable, _ = select.select([], [self.fd], [], 0.2)
            if writable:
                sent += os.write(self.fd, data[sent:])

    def read(self, timeout: float, idle: float = 0.05) -> str:
        try:
            termios.tcflush(self.fd, termios.TCIFLUSH)
        except termios.error:
            pass
        end = time.monotonic() + timeout
        last = None
        data = bytearray()
        while True:
            now = time.monotonic()
            if now >= end:
                break
            wait = end - now
            if last is not None:
                left = idle - (now - last)
                if left <= 0:
                    break
                wait = min(wait, left)
            readable, _, _ = select.select([self.fd], [], [], wait)
            if not readable:
                continue
            try:
                chunk = os.read(self.fd, 4096)
            except BlockingIOError:
                continue
            if not chunk:
                break
            data.extend(chunk)
            last = time.monotonic()
        return data.decode("ascii", errors="ignore")


def query(port: str | None, baud: int, timeout: float) -> dict:
    data = {"port": port, "lock": None, "source": None, "holdover": None, "health": None, "note": None}
    if not port:
        data["note"] = "OCXO port not found"
        return data
    try:
        with Serial(port, baud) as serial:
            serial.write("SYNC?\r\n")
            text = serial.read(timeout)
    except Exception as exc:
        data["note"] = str(exc)
        return data

    for raw_line in text.splitlines():
        line = raw_line.strip()
        if not line or line == "SYNC?" or line.startswith("scpi >"):
            continue
        if ":" in line:
            key, value = [part.strip() for part in line.split(":", 1)]
        else:
            continue
        key = " ".join(key.split())
        if key == "1PPS SOURCE STATE":
            data["source"] = value
        elif key == "1PPS LOCK STATUS":
            if value == "1":
                data["lock"] = "yes"
            elif value == "0":
                data["lock"] = "no"
            else:
                data["lock"] = value
        elif key == "HOLDOVER STATE":
            data["holdover"] = value
        elif key == "HEALTH STATUS":
            data["health"] = value

    if (
        data["lock"] is None
        and data["source"] is None
        and data["holdover"] is None
        and data["health"] is None
    ):
        data["note"] = "SYNC? response was empty or unrecognized"
    return data


def format_brief(data: dict) -> str:
    line = (
        f"ocxo lock={data['lock'] or 'unknown'} "
        f"src={data['source'] or 'unknown'} "
        f"holdover={data['holdover'] or 'unknown'} "
        f"health={data['health'] or 'unknown'}"
    )
    if data["note"]:
        line += f" note={json.dumps(data['note'])}"
    return line


def format_banner(data: dict, timestamp: str) -> str:
    line = (
        f"OCXO  lock={data['lock'] or 'unknown':<7} "
        f"src={data['source'] or 'unknown':<10} "
        f"holdover={data['holdover'] or 'unknown':<8} "
        f"health={data['health'] or 'unknown'}"
    )
    if data["note"]:
        line += f"  note={data['note']}"
    return "\n".join([BANNER, f"STATUS {timestamp}", line])


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--brief", action="store_true")
    parser.add_argument("--json", action="store_true")
    parser.add_argument("--ocxo-port")
    parser.add_argument("--ocxo-baud", type=int, default=115200)
    parser.add_argument("--ocxo-timeout", type=float, default=0.6)
    args = parser.parse_args()

    timestamp = time.strftime("%Y-%m-%dT%H:%M:%S%z")
    data = query(args.ocxo_port or find_port(), args.ocxo_baud, args.ocxo_timeout)
    if args.json:
        print(json.dumps({"timestamp": timestamp, "ocxo": data}, sort_keys=True))
    elif args.brief:
        print(format_brief(data))
    else:
        print(format_banner(data, timestamp))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

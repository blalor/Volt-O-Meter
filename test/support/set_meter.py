#!/usr/bin/env python
# encoding: utf-8
"""
set_meter.py

Created by Brian Lalor on 2011-07-10.
Copyright (c) 2011 __MyCompanyName__. All rights reserved.
"""

import sys
import os

import serial
import struct

def calc_checksum(data):
    chksum = len(data)
    
    for x in data:
        chksum += ord(x)
    
    chksum = (0x100 - (chksum & 0xFF))
    
    return chksum


def build_packet(name, val):
    payload = struct.pack('<cB', name, val)
    return '\xff\x55' + struct.pack('<B', len(payload)) + payload + struct.pack('<B', calc_checksum(payload))


def main():
    # boost at 89% == 34v
    # pwm val | volt meter
    #       4 |  1
    #      12 |  2
    #      16 |  2.5
    #      19 |  3
    #      27 |  4
    #      32 |  4.8
    #      34 |  5
    #      41 |  6
    #      48 |  7
    #      56 |  8
    #      64 |  9
    #      71 | 10
    #      78 | 11
    #      85 | 12
    #      92 | 13
    #     100 | 14
    #     106 | 15
    #     114 | 16
    #     121 | 17
    #     128 | 18
    #     142 | 20
    #     178 | 25
    #     216 | 30
    #     255 | 34
    
    ser = serial.Serial(port = "/dev/tty.usbserial-FTE3GB1Y", baudrate = 9600)
    ser.write(build_packet('M', int(sys.argv[1])))
    ser.flush()
    ser.close()


if __name__ == '__main__':
    main()


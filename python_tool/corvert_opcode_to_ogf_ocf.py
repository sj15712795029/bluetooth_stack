#!/bin/python
# Author:Zhongjun_Yu
# Copyright: Wireless Link(https://shop220811498.taobao.com)

import os
import sys

def opcode_to_ogf_ocf(input_opcode):
    print "->opcode:",hex(input_opcode)
    print "->ogf:",hex(input_opcode >> 10)
    print "->ocf:",hex(input_opcode & 0x3f)
    
def main():
    print "----------------------------------------------\n"
    print "Input hex opcde,include \" \" ,for exampel:\"0c03\" "
    print "----------------------------------------------\n"
    input_opcode = int(input("Please input opcode:"),16)
    
    opcode_to_ogf_ocf(input_opcode)

if __name__ == "__main__":
    sys.exit(main())
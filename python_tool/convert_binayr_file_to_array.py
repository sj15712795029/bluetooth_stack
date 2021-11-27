#!/bin/python
# Author:Zhongjun_Yu
# Copyright: Wireless Link(https://shop220811498.taobao.com)

import os
import sys

list_data = []

def read_data_from_binary_file(filename, list_data):
    f = open(filename, 'rb')
    f.seek(0, 0)
    while True:
        t_byte = f.read(1)
        if len(t_byte) == 0:
            break
        else:
            list_data.append("0x%.2X" % ord(t_byte))

def write_data_to_text_file(filename, list_data,data_num_per_line):
    data_num_per_line_int = int(data_num_per_line)
    f_output = open(filename, 'w+')
    f_output.write('const uint8_t output_array[] = \n')
    f_output.write('{\n    ')
    if ((data_num_per_line_int <= 0) or data_num_per_line_int > len(list_data)):
        data_num_per_line_int = 16
        print('data_num_per_line out of range,use default value\n')
    for i in range(0,len(list_data)):
        if ( (i != 0) and(i % data_num_per_line_int == 0)):
            f_output.write('\n    ')
            f_output.write(list_data[i]+', ')
        elif (i + 1) == len(list_data):
            f_output.write(list_data[i])
        else:
            f_output.write(list_data[i]+', ')
    f_output.write('\n};')
    f_output.close()

def main():
    print ("----------------------------------------------\n")
    print ("Wireless Link covery bin to array tool runing...\"")
    print ("----------------------------------------------\n")
    input_f = input("Please input source bin file_name:")
    print ("Binary input file:",input_f);
    output_f = input("Please input dest C file name:")
    print ("C output file:",output_f);
    data_num_per_line = input("Please input a num whitch indicates how many data for one line:")
    print ("data_num_per_line:",data_num_per_line);
    read_data_from_binary_file(input_f, list_data)
    write_data_to_text_file(output_f, list_data,data_num_per_line )

if __name__ == "__main__":
    sys.exit(main())

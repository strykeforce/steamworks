#!/usr/bin/env python3

# This tool generates boiler_target_data.
# Thanks to Sam Breuer for prototyping this.

import argparse
import os
import os.path
from openpyxl import load_workbook

parser = argparse.ArgumentParser(description='Create shooter_data.h.')
parser.add_argument('--input', help='excel file to process')

args = parser.parse_args()
dir_name, file_name = os.path.split(args.input)
os.chdir(dir_name)

wookbook = load_workbook(file_name)
sheet = wookbook.active

print("""#pragma once
namespace steamworks {
enum ShooterSolutionParamaters {kAzimuth,kElevation,kSpeed,kHopper,};
const double shooter_data[][4] = {""")

for row in range(2, sheet.max_row + 1):
    print('{', end='')
    for col in range(2, sheet.max_column + 1):
        cell_name = chr(col + 64) + str(row)
        print(sheet[cell_name].value, ',', sep='', end='')
    print('},')
print('};')

print("const int shooter_range_offset = {};".format(sheet['A2'].value))
print("const int shooter_data_size = {};".format(sheet.max_row - 1))

print('} /* deadeye */')

#!/usr/bin/env python3

# This tool generates default_config.h from conf/default_config.toml.
# TODO: rewrite in pure python.
# TODO: strip comments from input file.

import argparse
import os
import os.path
import subprocess

parser = argparse.ArgumentParser(description='Create default_conf.h.')
parser.add_argument('--input', help='default config file to process')

args = parser.parse_args()
dir_name, file_name = os.path.split(args.input)
os.chdir(dir_name)
data = subprocess.check_output(args=["/usr/bin/xxd", "-i", file_name])

preamble = "#pragma once"

print(preamble)
print(data.decode("ascii"))

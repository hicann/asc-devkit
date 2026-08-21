#!/usr/bin/python3
# coding=utf-8

import argparse
import os

import numpy as np


def gen_golden_data_simple():
    total_length = 256
    data_type = np.float32
    fill_value = 1
    golden = np.full([1, total_length], fill_value, dtype=data_type)
    os.makedirs("output", exist_ok=True)
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data_simple()


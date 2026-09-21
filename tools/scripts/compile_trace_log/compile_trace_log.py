#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
import re
import argparse
import json
from pathlib import Path
import inspect
import sys
import os


def extract_info_lines(filename):
    matching_lines = []

    try:
        with open(filename, "r", encoding="utf-8") as file:
            for line in file:
                stripped_line = line.strip()
                if "[INFO] ASC(" in stripped_line:
                    matching_lines.append(stripped_line)
    except FileNotFoundError:
        frame = inspect.currentframe()
        print(f"ERROR: {frame.f_code.co_filename}:line {frame.f_lineno}: File '{filename}' not found.", file=sys.stderr)
        raise
    except PermissionError:
        frame = inspect.currentframe()
        print(
            f"ERROR: {frame.f_code.co_filename}:line {frame.f_lineno}: Permission denied when reading '{filename}'.",
            file=sys.stderr,
        )
        raise
    except UnicodeDecodeError as e:
        frame = inspect.currentframe()
        print(
            f"ERROR: {frame.f_code.co_filename}:line {frame.f_lineno}: "
            f"Failed to decode '{filename}' with UTF-8 encoding: {e}",
            file=sys.stderr,
        )
        raise

    if matching_lines:
        print(f"INFO: Found {len(matching_lines)} matching log rows：")
    else:
        frame = inspect.currentframe()
        raise RuntimeError(
            f"INFO: {frame.f_code.co_filename}:line {frame.f_lineno}: No log starting with [INFO] "
            "ASC was found.Please check if the log file is empty or if the ASCEND_GLOBAL_EVENT_ENABLE"
            " environment variable for controlling the compilation time stamp is not set to 1. "
        )

    return matching_lines


def save_info_lines(trace_events, output_file):
    # 构建最终 JSON 结构
    data = {"traceEvents": trace_events}

    # 写入 JSON 文件
    try:
        with open(output_file, "w", encoding="utf-8") as f:
            json.dump(data, f, indent=2, ensure_ascii=False)
            f.write("\n")
    except PermissionError:
        frame = inspect.currentframe()
        print(
            f"ERROR: {frame.f_code.co_filename}:line {frame.f_lineno}: "
            f"Permission denied when writing to file '{output_file}'.",
            file=sys.stderr,
        )
        raise
    except FileNotFoundError:
        frame = inspect.currentframe()
        print(
            f"ERROR: {frame.f_code.co_filename}:line {frame.f_lineno}: "
            f"The specified path does not exist (may be a directory path): '{output_file}'.",
            file=sys.stderr,
        )
        raise
    except IsADirectoryError:
        frame = inspect.currentframe()
        print(
            f"ERROR: {frame.f_code.co_filename}:line {frame.f_lineno}: "
            f"The output path is a directory, not a file: '{output_file}'.",
            file=sys.stderr,
        )
        raise
    except Exception as e:
        frame = inspect.currentframe()
        print(
            f"ERROR: {frame.f_code.co_filename}:line {frame.f_lineno}: "
            f"Unknown error: failed to save data to file '{output_file}', details: {e}",
            file=sys.stderr,
        )
        raise
    return


def build_traceEvents(len_pid, optype, trace_events, timestamp, pid, tid, tilingtype, compile_stage):
    for i in range(len_pid):
        num = int(i / 12) * 7  # 除12是因为，打12个时间点，只有7个tiling信息
        idx = i % 12
        if idx == 11:
            continue
        if idx == 0:
            name = optype[num] + "   compile op"
            trace_events.append(
                {
                    "optype": optype[num],
                    "name": name,
                    "cat": "compile_op",
                    "ph": "B",
                    "ts": timestamp[i],
                    "pid": pid[i],
                    "tid": tid[i],
                    "args": {"tiling_key": tilingtype[num]},
                }
            )
            last_i = i + 11
            trace_events.append(
                {
                    "optype": optype[num],
                    "name": name,
                    "cat": "compile_op",
                    "ph": "E",
                    "ts": timestamp[last_i],
                    "pid": pid[last_i],
                    "tid": tid[last_i],
                }
            )
        else:
            common_trace_event(
                trace_events, compile_stage[idx], optype[num], timestamp[i], pid[i], tid[i], tilingtype[num]
            )

    return trace_events


def common_trace_event(trace_events, compile_stage, optype, timestamp, pid, tid, tilingtype):
    name, stage = compile_stage.rsplit(" ", 1)
    if stage == "start":
        trace_events.append(
            {
                "optype": optype,
                "name": name,
                "cat": "compile_op",
                "ph": "B",
                "ts": timestamp,
                "pid": pid,
                "tid": tid,
                "args": {"tiling_key": tilingtype},
            }
        )
    if stage == "end":
        trace_events.append(
            {"optype": optype, "name": name, "cat": "compile_op", "ph": "E", "ts": timestamp, "pid": pid, "tid": tid}
        )

    return trace_events


def group_lines_by_first_number_flat(lines):
    grouped = {}
    pattern = re.compile(r"\bASC\(\s*(\d+)\s*,", re.IGNORECASE)

    for line in lines:
        match = pattern.search(line)
        if match:
            first_num = int(match.group(1))
        else:
            first_num = None  # 无法提取时用 None 表示

        if first_num not in grouped:
            grouped[first_num] = []
        grouped[first_num].append(line)

    # 按 first_number 从小到大排序（None 放最后）
    sorted_groups = sorted(grouped.items(), key=lambda x: x[0] if x[0] is not None else float("inf"))

    # 将所有分组的行按顺序拼接成一个 flat 列表
    result = []
    for _, group_lines in sorted_groups:
        result.extend(group_lines)  # 按顺序添加组内所有行

    with open("datalog.txt", "w", encoding="utf-8") as f:
        for item in result:
            f.write(f"{item}\n")

    # 调用函数
    txtlist = []
    extract_lines_with_condition(txtlist, result)

    return txtlist


def extract_lines_with_condition(txtlist, result):
    if os.path.exists("check_info.txt"):
        with open("check_info.txt", "w", encoding="utf-8") as f:
            f.write("")

    listline = []
    for line_num, line in enumerate(result):
        if "compile op start ," in line:
            listline.append(line_num)

    # 遍历相邻行号对，检查差值是否为12
    for i in range(len(listline) - 1):
        a = listline[i]
        b = listline[i + 1]

        if b - a == 12:
            txtlist.extend(line.strip() for line in result[a:b])
        else:
            with open("check_info.txt", "a", encoding="utf-8") as out_f:
                for content in result[a:b]:
                    out_f.write(content + "\n")
                out_f.write("=======================================================================\n")
    if os.path.exists("check_info.txt") and os.path.getsize("check_info.txt") > 0:
        print(
            "[WARNING]: Some operator log reads failed.\n"
            "Failed operator details are in 'check_info.txt'.\n"
            "Please check if the operators are compiled correctly "
            "by referring to the operator names and original log files."
        )

    return txtlist


def compile_trace(input_file, output_file):
    matching_lines_old = extract_info_lines(input_file)
    matching_lines = group_lines_by_first_number_flat(matching_lines_old)
    pid = []
    tid = []
    timestamp = []
    optype = []
    tilingtype = []
    for line in matching_lines:
        # 提取第一个数字（pid）
        numbers = re.findall(r"\b\d+\b", line)
        p = int(numbers[0])
        pid.append(p)
        # timestamp
        match = re.search(r"timestamp:\s*(\d+)ns", line)
        ts = float(match.group(1)) / 1000000
        timestamp.append(ts)
        # tid
        match = re.search(r"\[tid:\s*(\d+)\]", line)
        t = int(match.group(1))
        tid.append(t)
        # 提取第一个 <...> 内容（optype）
        first_lt = line.find("<")
        first_gt = line.find(">", first_lt)
        if first_lt != -1 and first_gt != -1:
            op = line[first_lt + 1 : first_gt]
            optype.append(op)
        # 提取第二个 <...> 内容（tilingtype）
        second_lt = line.find("<", first_gt + 1)
        second_gt = line.find(">", second_lt)
        if second_lt != -1 and second_gt != -1:
            til = line[second_lt + 1 : second_gt]
            tilingtype.append(til)
    compile_stage = [
        "compile op start",
        "preprocess start",
        "preprocess end",
        "generate tiling start",
        "generate tiling end",
        "generate kernel stub start",
        "generate kernel stub end",
        "compile kernel start",
        "compile kernel end",
        "link kernel start",
        "link kernel end",
        "compile op end",
    ]
    # 构建 traceEvents 列表
    trace_events = []
    len_pid = len(pid)
    build_traceEvents(len_pid, optype, trace_events, timestamp, pid, tid, tilingtype, compile_stage)
    # 构建最终 JSON 结构
    save_info_lines(trace_events, output_file)


# ==================== 使用示例 ====================
if __name__ == "__main__":
    # 创建命令行解析器
    parser = argparse.ArgumentParser(
        description=("Extract the line starting with [INFO] ASC from the log file and output it as a JSON file.")
    )

    # 添加命令行参数
    parser.add_argument("-i", "--input", type=str, required=True, help="Path to the input log file (e.g., out_log.txt)")

    parser.add_argument(
        "-o",
        "--output",
        type=str,
        default="out_log_trace_output.json",
        help="Path to the output JSON file (e.g., out_log_trace_output.json)",
    )

    # 解析参数
    args = parser.parse_args()

    # 调用主函数
    try:
        compile_trace(args.input, args.output)
        print(f"[SUCCESS]: JSON file generated: {args.output}")
    except Exception as e:
        print(f"{e}")
        exit(1)

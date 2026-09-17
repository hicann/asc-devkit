#!/usr/bin/python3
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
"""检查 asc_op_compiler / adapter 包内是否存在 import 环。

compile_op.py 从 3600+ 行拆开时，codegen 簇并入了 ascendc_compile_gen_code.py、
json 后处理并入了 ascendc_compile_gen_json.py，由此引入了包内第一批同层横向依赖：

    ascendc_compile_gen_code -> ascendc_compile_v220      (get_v220_kernel_type_mix_flag)
    ascendc_compile_gen_code -> ascendc_compile_base      (get_actual_kernel_type)
    ascendc_compile_gen_json -> ascendc_compile_gen_code  (_get_tiling_struct_size 等)

这些边当前无环，但很脆弱 —— 只要有人在 ascendc_compile_v220.py 里 import
ascendc_compile_gen_code，立刻就会成环，而 Python 的循环 import 报错往往指向
一个与真正原因无关的位置，很难排查。这个脚本把约束固化下来。

用法:
    python3 tools/build/asc_op_compile_base/check_import_cycle.py
退出码 0 表示无环。
"""
import ast
import glob
import os
import sys

PACKAGES = ("asc_op_compiler", "adapter")


def build_graph(pkg_dir):
    """{模块名: 它依赖的同包模块集合}"""
    graph = {}
    for path in glob.glob(os.path.join(pkg_dir, "*.py")):
        mod = os.path.basename(path)[:-3]
        deps = set()
        try:
            tree = ast.parse(open(path, encoding="utf-8").read())
        except SyntaxError as err:
            print(f"[SKIP] {path} 解析失败: {err}")
            continue
        for node in ast.walk(tree):
            if not isinstance(node, ast.ImportFrom) or not node.module:
                continue
            if node.level == 1:                       # from .xxx import ...
                deps.add(node.module.split(".")[0])
            elif "asc_op_compiler." in node.module:   # 绝对路径写法
                deps.add(node.module.split("asc_op_compiler.")[1].split(".")[0])
        graph[mod] = deps
    return graph


def find_cycles(graph):
    """DFS 三色标记找出所有环，返回可读的路径字符串。"""
    white, gray, black = 0, 1, 2
    state = {k: white for k in graph}
    cycles = []

    def visit(node, path):
        state[node] = gray
        for nxt in sorted(graph.get(node, ())):
            if nxt not in graph:
                continue
            if state[nxt] == gray:
                cycles.append(" -> ".join(path[path.index(nxt):] + [nxt]))
            elif state[nxt] == white:
                visit(nxt, path + [nxt])
        state[node] = black

    for node in sorted(graph):
        if state[node] == white:
            visit(node, [node])
    return sorted(set(cycles))


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    failed = False
    for pkg in PACKAGES:
        pkg_dir = os.path.join(here, pkg)
        if not os.path.isdir(pkg_dir):
            print(f"[SKIP] {pkg} 目录不存在")
            continue
        cycles = find_cycles(build_graph(pkg_dir))
        if cycles:
            failed = True
            print(f"[FAIL] {pkg} 存在 import 环:")
            for cycle in cycles:
                print(f"    {cycle}")
        else:
            print(f"[OK] {pkg} 无 import 环")
    if failed:
        print("\n新增的 import 让包内产生了环。"
              "通常的解法是把被共用的那部分下沉到更底层的模块"
              "（比如 ascendc_constants / ascendc_common_utility），而不是互相 import。")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())

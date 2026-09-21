#!/usr/bin/python3
# coding=utf-8

# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

from __future__ import annotations

import tempfile
import unittest
from pathlib import Path

from presmoke.readme_spec import parse_readme, split_commands


README = """# sample

## 编译运行

- 配置环境变量
  ```bash
  source /usr/local/Ascend/cann/set_env.sh
  ```
- 样例执行
  ```bash
  SCENARIO=0
  mkdir -p build && cd build; # comment
  cmake -DSCENARIO_NUM=${SCENARIO} -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;
  python3 ../scripts/gen_data.py ${SCENARIO}
  ./demo
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin
  ```

- 编译选项说明
| 参数 | 说明 | 可选值 | 默认值 |
| :--- | :--- | :--- | :--- |
| CMAKE_ASC_RUN_MODE | 运行模式 | npu, sim | npu |
| CMAKE_ASC_ARCHITECTURES | NPU硬件架构 | dav-3510 | dav-3510 |
"""


class ReadmeSpecTest(unittest.TestCase):
    def test_split_commands_keeps_env_and_classifies(self) -> None:
        commands = split_commands("SCENARIO=0\ncmake ..; make -j && ./demo")
        self.assertEqual([c.kind for c in commands], ["cmake", "make", "run"])
        self.assertEqual(commands[0].env["SCENARIO"], "0")
        self.assertEqual(commands[-1].env["SCENARIO"], "0")

    def test_parse_readme_extracts_sample_block_arch_and_modes(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp) / "examples"
            ex = root / "x"
            (ex / "scripts").mkdir(parents=True)
            (ex / "README.md").write_text(README, encoding="utf-8")
            (ex / "scripts" / "gen_data.py").write_text("", encoding="utf-8")
            (ex / "scripts" / "verify_result.py").write_text("", encoding="utf-8")
            spec = parse_readme(ex, root)
        self.assertEqual(spec.source, "readme")
        self.assertIn("dav-3510", spec.archs)
        self.assertEqual(spec.modes, ["npu", "sim"])
        self.assertTrue(any(c.kind == "verify" for c in spec.commands))

    def test_support_product_section_overrides_command_arch_defaults(self) -> None:
        text = """# sample

## 支持的产品

- Ascend 950PR/Ascend 950DT
- Atlas A3 训练系列产品/Atlas A3 推理系列产品
- Atlas A2 训练系列产品/Atlas A2 推理系列产品

## 编译运行

```bash
cmake -S . -B build -DCMAKE_ASC_ARCHITECTURES=dav-2201
cmake --build build -j
./demo
```
"""
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp) / "examples"
            ex = root / "x"
            ex.mkdir(parents=True)
            (ex / "README.md").write_text(text, encoding="utf-8")
            spec = parse_readme(ex, root)

        self.assertEqual(spec.archs, ["dav-2201", "dav-3510"])

    def test_support_product_section_restricts_arch_even_if_option_table_mentions_more(self) -> None:
        text = """# sample

## 支持的产品

- Atlas A3 训练系列产品/Atlas A3 推理系列产品
- Atlas A2 训练系列产品/Atlas A2 推理系列产品

## 编译运行

```bash
cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;
./demo
```

| 参数 | 说明 |
| :--- | :--- |
| CMAKE_ASC_ARCHITECTURES | dav-2201 对应 Atlas A2/A3，dav-3510 对应 Ascend 950PR/Ascend 950DT |
"""
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp) / "examples"
            ex = root / "x"
            ex.mkdir(parents=True)
            (ex / "README.md").write_text(text, encoding="utf-8")
            spec = parse_readme(ex, root)

        self.assertEqual(spec.archs, ["dav-2201"])

    def test_parse_readme_prefers_cmake_block_over_manual_compile_block(self) -> None:
        text = """# sample

## 编译运行

- 样例执行

  - 命令行方式

    ```bash
    bisheng main.cpp -o demo -L./ \\
      -lfoo
    ./demo
    ```

  - CMake方式

    ```bash
    mkdir -p build && cd build;
    cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;
    ./demo
    ```
"""
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp) / "examples"
            ex = root / "x"
            ex.mkdir(parents=True)
            (ex / "README.md").write_text(text, encoding="utf-8")
            (ex / "CMakeLists.txt").write_text("add_executable(demo main.cpp)", encoding="utf-8")
            spec = parse_readme(ex, root)
        self.assertIn("cmake", " ".join(c.raw for c in spec.commands))
        self.assertNotIn("-lfoo", " ".join(c.raw for c in spec.commands))
        self.assertEqual([c.kind for c in spec.commands][-3:], ["cmake", "make", "run"])

    def test_split_commands_joins_shell_line_continuations(self) -> None:
        commands = split_commands("bisheng main.cpp -o demo -L./ \\\n  -lfoo\n./demo")
        self.assertEqual(len(commands), 2)
        self.assertIn("-lfoo", commands[0].raw)
        self.assertNotIn("\\", commands[0].raw)


if __name__ == "__main__":
    unittest.main()

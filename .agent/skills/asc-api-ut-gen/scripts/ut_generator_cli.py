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

"""
UT 生成器 CLI - 命令行入口

该脚本是 asc-api-ut-gen skill 的核心入口，用于生成各类 AscendC API 的单元测试代码。

使用方式：
    # 通过 Skill 调用
    python ut_generator_cli.py --config <config_file>
    python ut_generator_cli.py --type aiv --api Add --chip ascend910b1

    # 输出配置文件模板
    python ut_generator_cli.py --template-config
"""

import argparse
import json
import os
import re
import sys
import time
from dataclasses import asdict
from typing import Dict, List, Any, Optional, TextIO

# 添加脚本目录到路径
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, SCRIPT_DIR)

from ut_generator import (
    ApiType,
    ChipArch,
    TestCase,
    UTConfig,
    create_generator,
    DTYPE_MAP,
    GENERATOR_DTYPE_MAP,
    ARCH_DIR_MAP,
    NPU_ARCH_MAP,
    API_TYPE_MAP,
    API_TYPE_CHOICES,
    API_RESTRICTIONS,
    AIV_GENERIC_SCALAR_TENSOR_DISPATCH_APIS,
    CHIP_ARCH_BY_NAME,
    REFERENCE_CONSTRAINT_FILES,
    ensure_reference_constraints_loaded,
    get_adv_profile_output_path,
)


def write_line(message: str = "", stream: Optional[TextIO] = None) -> None:
    """Write a line to the requested stream."""
    output_stream = stream if stream is not None else sys.stdout
    output_stream.write(f"{message}\n")


def parse_coverage_report(report_path: Optional[str]) -> Dict[str, Any]:
    """读取 cov_report HTML，返回当前 Lines / Functions 覆盖率。"""
    if not report_path:
        return {
            "status": "unavailable",
            "lines": None,
            "functions": None,
            "source": None,
            "reason": "未提供 coverage report 路径",
        }

    if not os.path.exists(report_path):
        return {
            "status": "unavailable",
            "lines": None,
            "functions": None,
            "source": report_path,
            "reason": f"coverage report 不存在: {report_path}",
        }

    with open(report_path, "r", encoding="utf-8", errors="ignore") as f:
        content = f.read()

    def normalize_html_text(fragment: str) -> str:
        text = re.sub(r"<[^>]+>", " ", fragment)
        return re.sub(r"\s+", " ", text).strip()

    def find_percent(label: str) -> Optional[float]:
        label_re = re.escape(label)
        header_pattern = re.compile(
            rf'<td\b(?=[^>]*class=["\'][^"\']*\bheaderItem\b[^"\']*["\'])[^>]*>'
            rf"\s*{label_re}:?\s*</td>\s*"
            rf'<td\b(?=[^>]*class=["\'][^"\']*\bheaderCovTableEntry(?:Lo|Med|Hi)?\b[^"\']*["\'])[^>]*>'
            rf"\s*([0-9]+(?:\.[0-9]+)?)\s*%",
            re.IGNORECASE | re.DOTALL,
        )
        match = header_pattern.search(content)
        if match:
            return float(match.group(1))

        for row in re.findall(r"<tr\b[^>]*>.*?</tr>", content, re.IGNORECASE | re.DOTALL):
            row_text = normalize_html_text(row)
            if not re.search(rf"(?:^|\s){label_re}\s*:", row_text, re.IGNORECASE):
                continue
            match = re.search(r"([0-9]+(?:\.[0-9]+)?)\s*%", row_text)
            if match:
                return float(match.group(1))

        return None

    lines = find_percent("Lines")
    functions = find_percent("Functions")
    if lines is None and functions is None:
        return {
            "status": "unavailable",
            "lines": None,
            "functions": None,
            "source": report_path,
            "reason": "未在 coverage report 中解析到 Lines / Functions 百分比",
        }

    return {"status": "available", "lines": lines, "functions": functions, "source": report_path, "reason": None}


def get_token_usage(args: argparse.Namespace) -> Dict[str, Any]:
    """获取 token 消耗。CLI 本身不消耗 LLM token，只记录外部传入值。"""
    prompt = args.prompt_tokens
    completion = args.completion_tokens
    total = args.total_tokens
    source = "cli_args" if any(value is not None for value in [prompt, completion, total]) else None
    reason = None

    def parse_env_int(value: Optional[str], name: str, invalid: List[str]) -> Optional[int]:
        if value is None:
            return None

        stripped = value.strip()
        if not stripped:
            return None

        try:
            return int(stripped)
        except ValueError:
            invalid.append(f"{name}={value!r}")
            return None

    if source is None:
        env_prompt = os.getenv("ASC_API_UT_PROMPT_TOKENS")
        env_completion = os.getenv("ASC_API_UT_COMPLETION_TOKENS")
        env_total = os.getenv("ASC_API_UT_TOTAL_TOKENS")
        if any(value is not None for value in [env_prompt, env_completion, env_total]):
            source = "env"
            invalid_env = []
            prompt = parse_env_int(env_prompt, "ASC_API_UT_PROMPT_TOKENS", invalid_env)
            completion = parse_env_int(env_completion, "ASC_API_UT_COMPLETION_TOKENS", invalid_env)
            total = parse_env_int(env_total, "ASC_API_UT_TOTAL_TOKENS", invalid_env)
            if invalid_env:
                reason = "忽略非法 token 环境变量: " + ", ".join(invalid_env)

    if total is None and prompt is not None and completion is not None:
        total = prompt + completion

    if source is None:
        return {
            "prompt_tokens": None,
            "completion_tokens": None,
            "total_tokens": None,
            "source": "not_provided",
            "reason": "本地 CLI 生成不消耗 LLM token，调用环境也未提供 token usage",
        }

    return {
        "prompt_tokens": prompt,
        "completion_tokens": completion,
        "total_tokens": total,
        "source": source,
        "reason": reason,
    }


def print_generation_report(report: Dict[str, Any], stream) -> None:
    """打印生成后的简要报告。"""
    token_usage = report["token_usage"]
    coverage = report["current_coverage"]

    def fmt(value):
        return "未提供" if value is None else value

    write_line("生成报告:", stream)
    write_line(
        "  Token 消耗: "
        f"prompt={fmt(token_usage['prompt_tokens'])}, "
        f"completion={fmt(token_usage['completion_tokens'])}, "
        f"total={fmt(token_usage['total_tokens'])}"
        f" ({token_usage['source']})",
        stream,
    )
    if token_usage.get("reason"):
        write_line(f"  Token 说明: {token_usage['reason']}", stream)

    write_line(f"  生成耗时: {report['elapsed_seconds']}s", stream)

    if coverage["status"] == "available":
        write_line(f"  当前覆盖率: Lines={coverage['lines']}%, Functions={coverage['functions']}%", stream)
        write_line(f"  覆盖率来源: {coverage['source']}", stream)
    else:
        write_line(f"  当前覆盖率: 未获取 ({coverage['reason']})", stream)


def ensure_parent_dir(path: str) -> None:
    """创建文件父目录；当前目录文件不需要创建目录。"""
    parent = os.path.dirname(path)
    if parent:
        os.makedirs(parent, exist_ok=True)


def create_default_config() -> Dict[str, Any]:
    """创建默认配置模板"""
    return {
        "api_type": "aiv",
        "api_name": "Add",
        "chip": "ascend910b1",
        "test_cases": [
            {
                "name": "Add_half_256",
                "data_size": 256,
                "dtype": "half",
                "input_count": 2,
                "has_mask": False,
                "additional_params": {},
            },
            {
                "name": "Add_float_256",
                "data_size": 256,
                "dtype": "float",
                "input_count": 2,
                "has_mask": False,
                "additional_params": {},
            },
        ],
        "kernel_params": {},
        "custom_includes": [],
        "output_dir": None,
    }


def default_kernel_params(api_type: str) -> Dict[str, int]:
    """Return default kernel params only for templates that consume them."""
    if api_type == ApiType.AIC.value:
        return {"m": 16, "k": 64, "n": 16}
    return {}


def default_input_count(api_type: str, api_name: str) -> int:
    """Return the safest CLI default input count for generic templates."""
    if api_type == ApiType.AIV.value and api_name.lower() in AIV_GENERIC_SCALAR_TENSOR_DISPATCH_APIS:
        return 1
    return 2


def validate_config(config: Dict[str, Any]) -> List[str]:
    """验证配置文件"""
    errors = []

    # 验证 API 类型
    valid_types = API_TYPE_CHOICES
    if config.get("api_type") not in valid_types:
        errors.append(f"无效的 api_type: {config.get('api_type')}, 有效值: {valid_types}")

    # 验证芯片架构
    valid_chips = list(CHIP_ARCH_BY_NAME.keys())
    if config.get("chip") not in valid_chips:
        errors.append(f"无效的 chip: {config.get('chip')}, 有效值: {valid_chips}")

    # 验证 API 名称
    if not config.get("api_name"):
        errors.append("api_name 不能为空")

    # 验证架构限制
    chip = config.get("chip")
    api_type = config.get("api_type")

    restriction = API_RESTRICTIONS.get(api_type, {})
    allowed_chips = restriction.get("allowed_chips", [])
    if allowed_chips and chip not in allowed_chips:
        message = restriction.get("message", f"{api_type} API 不支持当前芯片: {{chip}}")
        errors.append(message.format(chip=chip))

    # 验证数据类型
    valid_dtypes = list(GENERATOR_DTYPE_MAP.keys())
    for tc in config.get("test_cases", []):
        if tc.get("dtype") not in valid_dtypes:
            errors.append(
                f"当前通用生成器不支持直接生成 dtype: {tc.get('dtype')}, "
                f"可直接生成值: {valid_dtypes}; 其他内置类型需按目标 API 自定义初始化"
            )

    return errors


def parse_config(config: Dict[str, Any]) -> UTConfig:
    """解析配置文件，创建 UTConfig 对象"""
    test_cases = []
    for tc in config.get("test_cases", []):
        test_cases.append(
            TestCase(
                name=tc.get("name", f"{config['api_name']}_case"),
                data_size=tc.get("data_size", 256),
                dtype=tc.get("dtype", "half"),
                input_count=tc.get("input_count", 2),
                has_mask=tc.get("has_mask", False),
                additional_params=tc.get("additional_params", {}),
            )
        )

    return UTConfig(
        api_type=API_TYPE_MAP[config["api_type"]],
        api_name=config["api_name"],
        chip=CHIP_ARCH_BY_NAME[config["chip"]],
        test_cases=test_cases,
        kernel_params=config.get("kernel_params", {}),
        custom_includes=config.get("custom_includes", []),
        output_dir=config.get("output_dir"),
    )


def get_output_path(config: UTConfig) -> str:
    """获取输出文件路径"""
    if config.output_dir:
        return config.output_dir

    # 根据 API 类型和芯片架构计算默认路径
    base_dir = ARCH_DIR_MAP[config.chip]

    if config.api_type == ApiType.AIV:
        return f"tests/api/basic_api/{base_dir}/{base_dir}_aiv/test_operator_{config.api_name.lower()}.cpp"
    elif config.api_type == ApiType.AIC:
        return f"tests/api/basic_api/{base_dir}/{base_dir}_aic/test_operator_{config.api_name.lower()}.cpp"
    elif config.api_type == ApiType.C_API:
        npu_arch = NPU_ARCH_MAP[config.chip]
        return f"tests/api/c_api/npu_arch_{npu_arch}/vector_compute/test_{config.api_name.lower()}.cpp"
    elif config.api_type == ApiType.ADV:
        profile_output = get_adv_profile_output_path(config.api_name, config.kernel_params)
        if profile_output:
            return profile_output
        return f"tests/api/adv_api/{config.api_name.lower()}/test_operator_{config.api_name.lower()}.cpp"
    elif config.api_type == ApiType.SIMT:
        return f"tests/api/simt_api/{base_dir}_simt/test_operator_simt_{config.api_name.lower()}.cpp"
    elif config.api_type == ApiType.REG_COMPUTE:
        return f"tests/api/reg_compute_api/{base_dir}_reg_compute/test_operator_reg_{config.api_name.lower()}.cpp"
    elif config.api_type == ApiType.UTILS:
        return f"tests/api/utils/test_{config.api_name.lower()}.cpp"

    return f"tests/api/test_{config.api_name.lower()}.cpp"


def main():
    """主函数"""
    start_time = time.monotonic()
    parser = argparse.ArgumentParser(
        description="AscendC API UT 代码生成器 CLI",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例:
  # 从配置文件生成
  %(prog)s --config ut_config.json

  # 命令行参数生成
  %(prog)s --type aiv --api Add --chip ascend910b1

  # 输出配置模板
  %(prog)s --template-config

  # 列出支持的配置
  %(prog)s --list-supported
        """,
    )

    parser.add_argument("--config", "-c", type=str, help="配置文件路径 (JSON 格式)")

    parser.add_argument("--type", "-t", choices=API_TYPE_CHOICES, help="API 类型")

    parser.add_argument("--api", "-a", type=str, help="API 名称 (如: Add, Mmad, asc_add)")

    parser.add_argument("--chip", choices=list(CHIP_ARCH_BY_NAME.keys()), help="目标芯片架构")

    parser.add_argument("--output", "-o", type=str, default=None, help="输出文件路径")

    parser.add_argument("--output-dir", type=str, default=None, help="输出目录（自动生成文件名）")

    parser.add_argument("--data-size", "-d", type=int, default=256, help="测试数据大小 (默认: 256)")

    parser.add_argument(
        "--dtype",
        type=str,
        default="half",
        choices=list(GENERATOR_DTYPE_MAP.keys()),
        help="通用模板可直接初始化的数据类型 (默认: half)",
    )

    parser.add_argument("--test-count", type=int, default=3, help="生成测试用例数量 (默认: 3)")

    parser.add_argument("--template-config", action="store_true", help="输出配置文件模板并退出")

    parser.add_argument("--list-supported", action="store_true", help="列出支持的配置选项")

    parser.add_argument("--validate", action="store_true", help="仅验证配置，不生成代码")

    parser.add_argument(
        "--coverage-report", type=str, default=None, help="当前覆盖率报告 HTML 路径，用于报告 Lines / Functions 覆盖率"
    )

    parser.add_argument("--prompt-tokens", type=int, default=None, help="外部调用环境记录的 prompt token 数")

    parser.add_argument("--completion-tokens", type=int, default=None, help="外部调用环境记录的 completion token 数")

    parser.add_argument("--total-tokens", type=int, default=None, help="外部调用环境记录的 total token 数")

    parser.add_argument("--report-json", type=str, default=None, help="将生成报告写入指定 JSON 文件")

    args = parser.parse_args()

    # 输出配置模板
    if args.template_config:
        template = create_default_config()
        write_line(json.dumps(template, indent=2, ensure_ascii=False))
        return 0

    try:
        ensure_reference_constraints_loaded()
    except RuntimeError as exc:
        write_line(f"结构化 reference 加载失败: {exc}", sys.stderr)
        return 1

    # 列出支持的配置
    if args.list_supported:
        write_line("=== 结构化 reference 约束 ===")
        for name, path in REFERENCE_CONSTRAINT_FILES.items():
            write_line(f"  - {name}: {path}")

        write_line("=== 支持的 API 类型 ===")
        for t in API_TYPE_CHOICES:
            write_line(f"  - {t}")

        write_line("\n=== 支持的芯片架构 ===")
        for chip, arch_dir in ARCH_DIR_MAP.items():
            npu_arch = NPU_ARCH_MAP[chip]
            write_line(f"  - {chip.value}: impl={arch_dir}, NPU_ARCH={npu_arch}")

        write_line("\n=== 文档内置数据类型 ===")
        for dtype, info in DTYPE_MAP.items():
            init = "generic-init" if info.get("generic_ut_generation", False) else "api-specific-init"
            write_line(f"  - {dtype}: size={info['size']}, bits={info.get('bit_width', 'unknown')}, {init}")

        write_line("\n=== 通用 UT 生成可直接初始化的数据类型 ===")
        for dtype, info in GENERATOR_DTYPE_MAP.items():
            write_line(f"  - {dtype}: size={info['size']}")

        write_line("\n=== 架构限制 ===")
        for api_type, restriction in API_RESTRICTIONS.items():
            allowed = ", ".join(restriction.get("allowed_chips", []))
            write_line(f"  - {api_type}: {allowed}")

        return 0

    # 从配置文件或命令行参数创建配置
    if args.config:
        with open(args.config, "r", encoding="utf-8") as f:
            config_dict = json.load(f)
    elif args.type and args.api and args.chip:
        # 从命令行参数构建配置
        test_cases = []
        dtypes = [args.dtype]

        # 如果是 half，额外生成 float 测试
        if args.dtype == "half" and args.test_count > 1:
            dtypes.append("float")

        for i, dtype in enumerate(dtypes[: args.test_count]):
            test_cases.append(
                {
                    "name": f"{args.api}_{dtype}_{args.data_size * (i + 1)}",
                    "data_size": args.data_size * (i + 1),
                    "dtype": dtype,
                    "input_count": default_input_count(args.type, args.api),
                    "has_mask": False,
                    "additional_params": {},
                }
            )

        config_dict = {
            "api_type": args.type,
            "api_name": args.api,
            "chip": args.chip,
            "test_cases": test_cases,
            "kernel_params": default_kernel_params(args.type),
            "custom_includes": [],
            "output_dir": args.output_dir,
        }
    else:
        parser.error("必须提供 --config 或 (--type, --api, --chip)")

    # 验证配置
    errors = validate_config(config_dict)
    if errors:
        write_line("配置验证失败:", sys.stderr)
        for error in errors:
            write_line(f"  - {error}", sys.stderr)
        return 1

    if args.validate:
        write_line("配置验证通过")
        return 0

    # 解析配置
    try:
        config = parse_config(config_dict)
    except Exception as e:
        write_line(f"配置解析失败: {e}", sys.stderr)
        return 1

    # 创建生成器并生成代码
    try:
        generator = create_generator(config)
        code = generator.generate()
    except Exception as e:
        write_line(f"代码生成失败: {e}", sys.stderr)
        return 1

    # 输出结果
    output_path = args.output or get_output_path(config)

    if output_path == "-" or output_path is None:
        write_line(code)
        report_stream = sys.stderr
    else:
        ensure_parent_dir(output_path)
        with open(output_path, "w", encoding="utf-8") as f:
            f.write(code)
        write_line(f"UT 文件已生成: {output_path}")
        report_stream = sys.stdout

    report = {
        "api_type": config.api_type.value,
        "api_name": config.api_name,
        "chip": config.chip.value,
        "output_path": output_path,
        "elapsed_seconds": round(time.monotonic() - start_time, 3),
        "token_usage": get_token_usage(args),
        "current_coverage": parse_coverage_report(args.coverage_report),
    }

    print_generation_report(report, report_stream)

    if args.report_json:
        ensure_parent_dir(args.report_json)
        with open(args.report_json, "w", encoding="utf-8") as f:
            json.dump(report, f, indent=2, ensure_ascii=False)

    return 0


if __name__ == "__main__":
    sys.exit(main())

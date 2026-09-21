#!/usr/bin/env python3
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
"""生成基于固定延迟模型的 Cube 算力 Roofline 报告。

理论性能由硬件并行度结合矩阵shape计算：

  - 并行度（每 cycle 硬件处理的元素块 cube_m*cube_n*cube_k）由场景精度决定（见 PARALLELISM 表）
  - 峰值算力 PeakMacPerCycle = cube_m * cube_n * cube_k（MAC/cycle）
  - 计算 cycle = ceil(M/cube_m) * ceil(N/cube_n) * ceil(K/cube_k)
  - 理论总 cycle = 头开销(默认 0) + 计算 cycle
  - 理论耗时(us) = 理论总 cycle / 主频(MHz)
  - 理论性能(MAC/cycle) = 计算量 / 理论总 cycle，首指令开销取 0 时等于峰值算力

实测：measured_cycles 直接取自 CSV 的 Cycles 列（仿真 instr_exe.csv 的 MMAD cycles）；
      measured_throughput = M*N*K / measured_cycles（MAC/cycle）。

Roofline 图：
  - 横轴：计算量 M*N*K（MMAC）
  - 纵轴：性能（MAC/cycle）
  - 实线：理论性能曲线（考虑头开销），虚线：峰值算力上限，*号：实测点

示例:
  python3 generate_roofline.py --csv perf_data_xxx_scenario1/perf_result_scenario1.csv
  python3 generate_roofline.py perf_data_xxx_scenario1/perf_result_scenario1.csv --head-overhead 0
"""

import argparse
import csv
import math
import re
import sys
from pathlib import Path

# 头开销（首指令延迟），单位 cycle。本样例按首指令开销为 0 计算（理论总 cycle = 计算 cycle）。
DEFAULT_HEAD_OVERHEAD = 25

# 各场景的硬件并行度（每 cycle 处理的元素块 cube_m x cube_n x cube_k）与主频
# 来源：硬件规格（2201 主频 1800 MHz，3510 主频 1650 MHz）
# k_divisor：K 方向有效缩减因子。sparse（4:2 结构化稀疏）右矩阵稠密化为 [K/2, N]，
#            硬件实际只遍历 K/2 个分形，故 k_divisor=2；普通场景为 1（缺省）。
#            等效峰值算力 = cube_m * cube_n * cube_k * k_divisor（sparse 因半 cycle 完成稠密等效计算，算力翻倍）。
SCENARIO_CONFIG = {
    # scenario: {cube_m, cube_n, cube_k, freq, head, precision, [k_divisor]}
    # head：首指令头开销（cycle），dav-2201=21、dav-3510=25
    1: {"cube_m": 16, "cube_n": 16, "cube_k": 32, "freq": 1800, "head": 21, "precision": "b8"},
    2: {"cube_m": 16, "cube_n": 16, "cube_k": 16, "freq": 1800, "head": 21, "precision": "b16"},
    3: {"cube_m": 16, "cube_n": 16, "cube_k": 4, "freq": 1800, "head": 21, "precision": "b32"},
    4: {"cube_m": 16, "cube_n": 16, "cube_k": 32, "freq": 1800, "head": 21, "precision": "sparse b8", "k_divisor": 2},
    11: {"cube_m": 16, "cube_n": 16, "cube_k": 32, "freq": 1650, "head": 25, "precision": "b8"},
    12: {"cube_m": 16, "cube_n": 16, "cube_k": 16, "freq": 1650, "head": 25, "precision": "b16"},
    13: {"cube_m": 16, "cube_n": 16, "cube_k": 1, "freq": 1650, "head": 25, "precision": "b32"},
    14: {"cube_m": 16, "cube_n": 16, "cube_k": 32, "freq": 1650, "head": 25, "precision": "mxfp8"},
    15: {"cube_m": 16, "cube_n": 16, "cube_k": 64, "freq": 1650, "head": 25, "precision": "mxfp4"},
}


def get_scenario_from_csv(csv_file):
    """从 CSV 文件名或父目录名中解析场景编号。"""
    csv_path = Path(csv_file)
    for text in (csv_path.name, csv_path.parent.name):
        match = re.search(r"scenario(\d+)", text)
        if match:
            return int(match.group(1))
    return None


def read_csv_data(csv_file, freq_mhz):
    """读取 perf.sh 生成的 CSV，并过滤无效测试行。

    仿真环境下 CSV 含 MMAD_Dur(us) 与 Cycles 两列：
      - MMAD_Dur(us)：MMAD 指令持续时间（trace.json 的 dur）
      - Cycles：MMAD 指令周期数（instr_exe.csv 的 cycles），实测 cycle 直接取该列，不再由耗时折算。
    """
    data = []
    with open(csv_file, "r") as f:
        reader = csv.DictReader(f)
        time_column = None
        cycle_column = None
        if reader.fieldnames:
            for column in ("MMAD_Dur(us)", "MMAD_Dur", "AIC_Cube_Time(us)", "AIC_Cube_Time", "aic_cube_time(us)"):
                if column in reader.fieldnames:
                    time_column = column
                    break
            for column in ("Cycles", "Cycle"):
                if column in reader.fieldnames:
                    cycle_column = column
                    break
        if time_column is None:
            raise ValueError("CSV 文件缺少 MMAD_Dur(us) 列")

        for row in reader:
            time_val = row[time_column]
            if time_val in ("N/A", "NA", "ERROR", ""):
                continue
            time_us = float(time_val)
            m = int(row["M"])
            k = int(row["K"])
            n = int(row["N"])
            total_macs = float(m) * n * k
            # 实测 cycle 优先取 CSV 的 Cycles 列（仿真直接产出）；缺失时回退按耗时折算
            cycle_val = row.get(cycle_column, "") if cycle_column else ""
            if cycle_val not in ("N/A", "NA", "ERROR", "", None):
                measured_cycles = float(cycle_val)
            else:
                measured_cycles = time_us * freq_mhz
            measured_throughput = total_macs / measured_cycles if measured_cycles > 0 else 0.0

            data.append(
                {
                    "test_id": int(row["Test_ID"]),
                    "m": m,
                    "k": k,
                    "n": n,
                    "shape": row.get("Shape", f"{m}_{k}_{n}"),
                    "time_us": time_us,
                    "measured_cycles": measured_cycles,
                    "total_macs": total_macs,
                    "macs_m": total_macs / 1e6,  # MMAC
                    "measured_throughput": measured_throughput,
                }
            )
    return data


def calc_compute_cycles(m, n, k, cube_m, cube_n, cube_k, k_divisor=1):
    """理论计算 cycle = ceil(M/cube_m) * ceil(N/cube_n) * ceil(K / (cube_k * k_divisor))。

    k_divisor>1（如 sparse 的 4:2 稀疏）时，硬件 K 方向实际只遍历 K/k_divisor 个分形，
    计算 cycle 相应减少，等效算力 = cube_m*cube_n*cube_k*k_divisor。
    """
    return math.ceil(m / cube_m) * math.ceil(n / cube_n) * math.ceil(k / (cube_k * k_divisor))


def calc_theory_throughput(total_macs, peak_mac_per_cycle, head_overhead):
    """理论性能（MAC/cycle）：total_macs / (head + total_macs / peak)。

    total_macs 为逻辑等效计算量（M*N*K）；peak_mac_per_cycle 为等效峰值（含 k_divisor）。
    """
    compute_cycles = total_macs / peak_mac_per_cycle
    total_cycles = head_overhead + compute_cycles
    return total_macs / total_cycles if total_cycles > 0 else 0.0


def generate_roofline(data, output_file, cfg, head_overhead):
    """生成 ASCII 版本 Cube Roofline 报告。"""
    cube_m, cube_n, cube_k, freq = (cfg["cube_m"], cfg["cube_n"], cfg["cube_k"], cfg["freq"])
    k_divisor = cfg.get("k_divisor", 1)
    peak = cube_m * cube_n * cube_k * k_divisor  # MAC/cycle（sparse 含 k_divisor，等效算力翻倍）

    max_macs_m = max(item["macs_m"] for item in data)
    min_macs_m = 0.01

    theory_macs_m = []
    theory_throughputs = []
    num_points = 100
    for i in range(num_points):
        macs_m = min_macs_m + (max_macs_m - min_macs_m) * i / (num_points - 1)
        tp = calc_theory_throughput(macs_m * 1e6, peak, head_overhead)
        theory_macs_m.append(macs_m)
        theory_throughputs.append(tp)

    lines = []
    lines.append("=" * 80)
    lines.append("Cube Throughput Roofline Model (Fixed-Latency)")
    lines.append("=" * 80)
    lines.append("")
    lines.append("硬件并行度与模型参数：")
    lines.append(f"  精度: {cfg['precision']}")
    lines.append(f"  并行度 (cube_m x cube_n x cube_k): {cube_m} x {cube_n} x {cube_k}")
    lines.append(f"  峰值算力: {peak} MAC/cycle")
    lines.append(f"  主频: {freq} MHz")
    lines.append(f"  头开销（固定延迟）: {head_overhead} cycles = {head_overhead / freq:.4f} us")
    lines.append("")
    lines.append("理论公式：")
    lines.append(f"  计算cycle = ceil(M/{cube_m}) * ceil(N/{cube_n}) * ceil(K/{cube_k})")
    lines.append(f"  理论总cycle = {head_overhead} + 计算cycle")
    lines.append("  理论耗时(us) = 理论总cycle / 主频(MHz)")
    lines.append("  性能(MAC/cycle) = M*N*K / cycle")
    lines.append("")
    lines.append("图表说明：")
    lines.append("  X轴: 计算量 M*N*K (MMAC)")
    lines.append("  Y轴: 性能 (MAC/cycle)")
    lines.append("  实线: 理论性能曲线（考虑头开销）")
    lines.append("  虚线: 峰值算力上限（无延迟）")
    lines.append("  *号: 实际测试数据点")
    lines.append("")

    chart_width = 70
    chart_height = 25
    max_throughput = max(max(theory_throughputs), max(item["measured_throughput"] for item in data), peak)
    tp_per_line = max_throughput / chart_height
    macs_per_char = max_macs_m / chart_width if max_macs_m > 0 else 1.0

    grid = [[" " for _ in range(chart_width)] for _ in range(chart_height)]

    peak_y = int(chart_height - peak / tp_per_line)
    if 0 <= peak_y < chart_height:
        for x in range(chart_width):
            if x % 3 == 0:
                grid[peak_y][x] = ":"

    for i in range(len(theory_macs_m)):
        x_pos = int(theory_macs_m[i] / macs_per_char)
        y_pos = int(chart_height - theory_throughputs[i] / tp_per_line)
        x_pos = max(0, min(chart_width - 1, x_pos))
        y_pos = max(0, min(chart_height - 1, y_pos))
        if grid[y_pos][x_pos] == " ":
            grid[y_pos][x_pos] = "-"

    for item in data:
        x_pos = int(item["macs_m"] / macs_per_char)
        y_pos = int(chart_height - item["measured_throughput"] / tp_per_line)
        x_pos = max(0, min(chart_width - 1, x_pos))
        y_pos = max(0, min(chart_height - 1, y_pos))
        grid[y_pos][x_pos] = "*"

    lines.append("Throughput (MAC/cycle)")
    for i, row in enumerate(grid):
        tp_value = (chart_height - i) * tp_per_line
        if i == peak_y:
            label = f"{peak:7.0f} ::"
        elif i % 5 == 0:
            label = f"{tp_value:7.0f} |"
        else:
            label = "        |"
        lines.append(label + "".join(row))

    lines.append("        +" + "-" * chart_width)
    lines.append("         Compute M*N*K (MMAC = million MACs)")

    x_labels = []
    for i in range(0, chart_width + 1, 14):
        macs_m = i * macs_per_char
        x_labels.append(f"{macs_m:5.1f}")
    lines.append("        " + " ".join(x_labels))
    lines.append("")

    lines.append("图例说明：")
    lines.append(f"  :  峰值算力上限（{peak} MAC/cycle，无延迟）")
    lines.append(f"  -  理论性能曲线（考虑 {head_overhead} cycle 头开销）")
    lines.append("  *  实际测试数据点")
    lines.append("")

    lines.append("=" * 80)
    lines.append("实际测试数据详细分析")
    lines.append("=" * 80)

    for item in data:
        m, n, k = item["m"], item["n"], item["k"]
        compute_cycles = calc_compute_cycles(m, n, k, cube_m, cube_n, cube_k, k_divisor)
        theory_total_cycles = head_overhead + compute_cycles
        theory_time_us = theory_total_cycles / freq
        # 算力利用率：理论总 cycle / 实测 cycle（实测越接近理论越高）
        utilization = theory_total_cycles / item["measured_cycles"] * 100 if item["measured_cycles"] > 0 else 0.0
        theory_throughput = item["total_macs"] / theory_total_cycles if theory_total_cycles > 0 else 0.0

        lines.append(f"\nTest {item['test_id']}: Shape [{m}, {k}, {n}]")
        lines.append("-" * 80)
        lines.append(f"  计算量: {item['macs_m']:.4f} MMAC ({item['total_macs']:.0f} MAC)")
        lines.append("")
        lines.append("  实际测量:")
        lines.append(f"    时间: {item['time_us']:.4f} us = {item['measured_cycles']:.2f} cycles")
        lines.append(f"    性能: {item['measured_throughput']:.1f} MAC/cycle")
        lines.append("")
        lines.append("  理论计算:")
        lines.append(
            f"    计算cycle: ceil({m}/{cube_m})*ceil({n}/{cube_n})*ceil({k}/{cube_k * k_divisor}) = {compute_cycles}"
        )
        lines.append(f"    理论总cycle: {head_overhead} + {compute_cycles} = {theory_total_cycles}")
        lines.append(f"    理论耗时: {theory_time_us:.4f} us")
        lines.append(f"    理论性能: {theory_throughput:.1f} MAC/cycle")
        lines.append("")
        lines.append("  性能对比:")
        lines.append(f"    算力利用率（理论cycle/实测cycle）: {utilization:.1f}%")

    lines.append("")
    lines.append("=" * 80)
    lines.append("性能分析总结")
    lines.append("=" * 80)
    avg_util = 0.0
    cnt = 0
    for item in data:
        compute_cycles = calc_compute_cycles(item["m"], item["n"], item["k"], cube_m, cube_n, cube_k, k_divisor)
        theory_total_cycles = head_overhead + compute_cycles
        if item["measured_cycles"] > 0:
            avg_util += theory_total_cycles / item["measured_cycles"] * 100
            cnt += 1
    avg_util = avg_util / cnt if cnt > 0 else 0.0
    avg_throughput = sum(item["measured_throughput"] for item in data) / len(data)
    lines.append(f"  峰值算力: {peak} MAC/cycle")
    lines.append(f"  平均实测性能: {avg_throughput:.1f} MAC/cycle")
    lines.append(f"  平均算力利用率: {avg_util:.1f}%")
    lines.append("")
    lines.append("=" * 80)

    with open(output_file, "w") as f:
        f.write("\n".join(lines))
    return "\n".join(lines)


def generate_matplotlib_roofline(data, output_file, cfg, head_overhead):
    """生成 PNG/PDF 版本 Cube Roofline 图。"""
    try:
        import matplotlib.pyplot as plt
        import numpy as np

        cube_m, cube_n, cube_k, freq = (cfg["cube_m"], cfg["cube_n"], cfg["cube_k"], cfg["freq"])
        k_divisor = cfg.get("k_divisor", 1)
        peak = cube_m * cube_n * cube_k * k_divisor

        max_macs_m = max(item["macs_m"] for item in data)
        theory_macs = np.linspace(0.01 * 1e6, max_macs_m * 1.1 * 1e6, 100)
        theory_throughputs = [calc_theory_throughput(c, peak, head_overhead) for c in theory_macs]
        theory_macs_m = theory_macs / 1e6

        fig, ax1 = plt.subplots(figsize=(11, 7.5))
        ax1.axhline(
            y=peak, color="red", linestyle="--", linewidth=2, label=f"Peak compute (no latency): {peak} MAC/cycle"
        )
        ax1.plot(
            theory_macs_m,
            theory_throughputs,
            "b-",
            linewidth=3,
            label=f"Theoretical throughput (with {head_overhead}c head overhead)",
        )

        colors = ["#2ecc71", "#f39c12", "#e74c3c", "#9b59b6", "#3498db", "#1abc9c", "#e67e22"]
        markers = ["o", "s", "D", "^", "v", "<", ">"]
        for i, item in enumerate(data):
            ax1.scatter(
                item["macs_m"],
                item["measured_throughput"],
                c=colors[i % len(colors)],
                marker=markers[i % len(markers)],
                s=250,
                edgecolors="black",
                linewidths=2.5,
                zorder=5,
                label=f"Shape [{item['m']},{item['k']},{item['n']}]",
            )
            ax1.annotate(
                f"{item['measured_throughput']:.0f}",
                xy=(item["macs_m"], item["measured_throughput"]),
                xytext=(15, 15),
                textcoords="offset points",
                fontsize=11,
                fontweight="bold",
                bbox=dict(boxstyle="round,pad=0.5", facecolor="yellow", alpha=0.7),
            )

        ax1.set_xlabel("Compute M*N*K (MMAC = million MACs)", fontsize=14, fontweight="bold")
        ax1.set_ylabel("Throughput (MAC/cycle)", fontsize=14, fontweight="bold")
        ax1.set_title(
            f"Cube Throughput Roofline Model\n(precision: {cfg['precision']}, "
            f"parallelism {cube_m}x{cube_n}x{cube_k}, {freq} MHz)",
            fontsize=15,
            fontweight="bold",
        )
        ax1.grid(True, linestyle="--", alpha=0.4)
        ax1.legend(loc="center left", bbox_to_anchor=(1.02, 0.5), fontsize=10)
        ax1.set_xlim(0, max_macs_m * 1.15)
        ax1.set_ylim(0, peak * 1.2)

        formula_text = (
            "Model Parameters\n"
            f"Parallelism: {cube_m}x{cube_n}x{cube_k} = {peak} MAC/cycle    Frequency: {freq} MHz    "
            f"Head Overhead: {head_overhead} cycles\n"
            f"Formula: Cycles = {head_overhead} + ceil(M/{cube_m})*ceil(N/{cube_n})*ceil(K/{cube_k}); "
            "Throughput = M*N*K / Cycles"
        )
        fig.text(
            0.5,
            0.04,
            formula_text,
            ha="center",
            va="bottom",
            fontsize=11,
            bbox=dict(boxstyle="round,pad=0.6", facecolor="wheat", edgecolor="gray", alpha=0.9),
        )
        fig.subplots_adjust(left=0.10, right=0.72, bottom=0.24, top=0.86)

        plt.savefig(output_file, dpi=300, bbox_inches="tight")
        print(f"Matplotlib 图表已保存: {output_file}")
        png_file = output_file.replace(".pdf", ".png")
        plt.savefig(png_file, dpi=300, bbox_inches="tight")
        print(f"PNG 版本已保存: {png_file}")
        plt.close(fig)
        return True
    except ImportError:
        print("警告: matplotlib 未安装，跳过图表生成")
        return False
    except Exception as e:
        print(f"警告: matplotlib 生成失败 ({e})")
        return False


def find_latest_perf_data():
    """查找最新的 perf_data 目录中的 CSV 文件。"""
    perf_dirs = sorted(Path(".").glob("perf_data_*"), reverse=True)
    for perf_dir in perf_dirs:
        csv_files = sorted(perf_dir.glob("perf_result_scenario*.csv"), reverse=True)
        if csv_files:
            return str(csv_files[0])
    return None


def main():
    parser = argparse.ArgumentParser(
        description="生成 Cube 算力 Roofline 图（固定延迟模型，理论由硬件并行度推导）",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""示例用法:
  python3 generate_roofline.py --csv perf_data_xxx_scenario1/perf_result_scenario1.csv
  python3 generate_roofline.py perf_data_xxx_scenario1/perf_result_scenario1.csv
  python3 generate_roofline.py            # 自动查找最新数据

说明:
  理论峰值算力由场景精度对应的硬件并行度（cube_m*cube_n*cube_k）直接得出，无需手工传入；
  --head-overhead 默认 0 cycle，--frequency 默认按场景自动取（2201=1800，3510=1650）。
""",
    )
    parser.add_argument("--csv", "-c", type=str, help="CSV 文件路径（不指定则自动查找最新数据）")
    parser.add_argument(
        "--head-overhead",
        type=int,
        default=None,
        help="指令头开销（cycles），默认按场景自动取（dav-2201=21，dav-3510=25）",
    )
    parser.add_argument(
        "--frequency", type=int, default=None, help="主频（MHz），默认按场景自动取（2201=1800，3510=1650）"
    )
    parser.add_argument("--scenario", type=int, default=None, help="场景编号（不指定则从 CSV 文件名解析）")
    parser.add_argument("--output", "-o", type=str, help="输出文件前缀，默认按 perf_data 目录名")
    parser.add_argument("csv_path", nargs="?", help="CSV 文件路径，等价于 --csv")

    args = parser.parse_args()
    csv_file = args.csv or args.csv_path
    if not csv_file:
        csv_file = find_latest_perf_data()
        if not csv_file:
            print("错误: 未找到 perf_data 目录或 CSV 文件")
            parser.print_help()
            sys.exit(1)
        print(f"自动检测到最新数据: {csv_file}")

    if not Path(csv_file).exists():
        print(f"错误: CSV 文件不存在: {csv_file}")
        sys.exit(1)

    scenario = args.scenario if args.scenario is not None else get_scenario_from_csv(csv_file)
    if scenario not in SCENARIO_CONFIG:
        print(f"错误: 无法识别场景编号 {scenario}，无法确定硬件并行度")
        print(f"支持的场景: {sorted(SCENARIO_CONFIG.keys())}")
        sys.exit(1)

    cfg = dict(SCENARIO_CONFIG[scenario])
    if args.frequency is not None:
        cfg["freq"] = args.frequency
    # 头开销：命令行未指定时按场景取（dav-2201=21，dav-3510=25）
    head_overhead = args.head_overhead if args.head_overhead is not None else cfg.get("head", 0)

    if args.output:
        output_prefix = args.output
    else:
        output_prefix = f"{Path(csv_file).parent.name}_cube_roofline"

    print(f"读取数据: {csv_file}")
    data = read_csv_data(csv_file, cfg["freq"])
    if not data:
        print(f"错误: CSV 文件没有可用于绘图的有效性能数据: {csv_file}")
        sys.exit(1)
    print(f"找到 {len(data)} 条测试数据")
    print()

    print("=" * 80)
    print("Roofline 模型参数")
    print("=" * 80)
    print(f"  场景编号: {scenario}（精度: {cfg['precision']}）")
    print(f"  并行度 (cube_m x cube_n x cube_k): {cfg['cube_m']} x {cfg['cube_n']} x {cfg['cube_k']}")
    _kdiv = cfg.get("k_divisor", 1)
    if _kdiv > 1:
        print(f"  K 方向缩减因子 (k_divisor): {_kdiv}（4:2 稀疏，硬件实际遍历 K/{_kdiv}）")
    print(f"  峰值算力: {cfg['cube_m'] * cfg['cube_n'] * cfg['cube_k'] * _kdiv} MAC/cycle")
    print(f"  主频: {cfg['freq']} MHz")
    print(f"  头开销: {head_overhead} cycles")
    print()

    ascii_output = f"{output_prefix}.txt"
    print("生成 ASCII Roofline 图...")
    ascii_chart = generate_roofline(data, ascii_output, cfg, head_overhead)
    print(ascii_chart)

    matplotlib_output = f"{output_prefix}.pdf"
    print("\n生成 Matplotlib Roofline 图...")
    generate_matplotlib_roofline(data, matplotlib_output, cfg, head_overhead)

    print("\n" + "=" * 80)
    print("Roofline 图生成完成！")
    print("=" * 80)
    print(f"  输入文件: {csv_file}")
    print(f"  ASCII 版本: {ascii_output}")
    print(f"  PNG 版本: {matplotlib_output.replace('.pdf', '.png')}")


if __name__ == "__main__":
    main()

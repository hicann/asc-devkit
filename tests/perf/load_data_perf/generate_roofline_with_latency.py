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
"""生成基于固定延迟模型的 MTE1 Bandwidth Roofline 报告。

默认模型参数:
  - 理论带宽: 256 Byte/cycle
  - 带宽延迟: 30 cycles
  - L0 最大容量: 64 KB

示例:
  python3 generate_roofline_with_latency.py --csv perf_data_20260613_162817/perf_result_scenario1.csv
  python3 generate_roofline_with_latency.py perf_data_20260613_162817/perf_result_scenario1.csv
"""

import argparse
import csv
import re
import sys
from pathlib import Path


def read_csv_data(csv_file):
    """读取 perf.sh 生成的 CSV，并过滤无效测试行。"""
    data = []
    with open(csv_file, "r") as f:
        reader = csv.DictReader(f)
        time_column = None
        if reader.fieldnames:
            for column in ("AIC_MTE1_Time(us)", "AIC_FixPipe_Time(us)"):
                if column in reader.fieldnames:
                    time_column = column
                    break
        if time_column is None:
            raise ValueError("CSV 文件缺少 AIC_MTE1_Time(us) 或 AIC_FixPipe_Time(us) 列")
        for row in reader:
            if row[time_column] in ("N/A", "NA", "ERROR", "") or row["Bandwidth(GB/s)"] in ("N/A", "NA", "ERROR", ""):
                continue
            time_us = float(row[time_column])
            bandwidth_gbps = float(row["Bandwidth(GB/s)"])
            data_size = bandwidth_gbps * time_us * 1e3
            data.append(
                {
                    "test_id": int(row["Test_ID"]),
                    "m": int(row["M"]),
                    "k": int(row["K"]),
                    "n": int(row["N"]),
                    "shape": row["Shape"],
                    "time_us": time_us,
                    "cycle": float(row["Cycle"]),
                    "bandwidth_gbps": bandwidth_gbps,
                    "data_size": data_size,
                    "data_size_kb": data_size / 1024,
                }
            )
    return data


def calculate_theoretical_bandwidth(
    data_size_bytes, peak_bw_bytes_per_cycle=256, latency_cycles=30, frequency_mhz=1800
):
    """按 Time(cycles) = Latency + DataSize / PeakBW 计算理论值。"""
    transfer_cycles = data_size_bytes / peak_bw_bytes_per_cycle
    total_cycles = latency_cycles + transfer_cycles
    time_us = total_cycles / frequency_mhz
    bandwidth_gbps = data_size_bytes / time_us / 1e3

    return bandwidth_gbps, time_us, total_cycles


def calculate_data_size(m, k):
    """在缺少 CSV 数据量信息时使用的兼容兜底。"""
    return m * k * 2


def get_scenario_from_csv(csv_file):
    """从 CSV 文件名或父目录名中解析场景编号。"""
    csv_path = Path(csv_file)
    for text in (csv_path.name, csv_path.parent.name):
        match = re.search(r"scenario(\d+)", text)
        if match:
            return int(match.group(1))
    return None


def get_transfer_path_by_scenario(scenario):
    """根据 README 的场景表返回图表中的搬运路径。"""
    if scenario in (1, 3, 5, 11, 15):
        return "L1 → L0A"
    if scenario in (2, 4, 6, 7, 12, 16, 17):
        return "L1 → L0B"
    if scenario == 13:
        return "L1 → L0A + L0A_MX"
    if scenario == 14:
        return "L1 → L0B + L0B_MX"
    if scenario in (8, 18):
        return "L1 → BiasTable Buffer"
    if scenario in (9, 19):
        return "L1 → Fixpipe Buffer"
    return "L1 → L0"


def generate_roofline_with_latency(
    data, output_file, peak_bw_bytes_per_cycle=256, latency_cycles=30, frequency_mhz=1800, l0_max_size_kb=64
):
    """生成 ASCII 版本 Roofline 报告。"""

    # CSV 已按场景计算带宽；这里反推数据量，避免假设固定矩阵路径。
    for item in data:
        if "data_size" not in item:
            item["data_size"] = calculate_data_size(item["m"], item["k"])
            item["data_size_kb"] = item["data_size"] / 1024

    peak_bw_gbps = peak_bw_bytes_per_cycle * frequency_mhz / 1e3

    max_data_kb = l0_max_size_kb
    min_data_kb = 0.1

    theory_data_sizes = []
    theory_bandwidths = []

    num_points = 100
    for i in range(num_points):
        data_size_kb = min_data_kb + (max_data_kb - min_data_kb) * i / (num_points - 1)
        data_size = data_size_kb * 1024

        bw, time, cycles = calculate_theoretical_bandwidth(
            data_size, peak_bw_bytes_per_cycle, latency_cycles, frequency_mhz
        )
        theory_data_sizes.append(data_size_kb)
        theory_bandwidths.append(bw)

    lines = []
    lines.append("=" * 80)
    lines.append("MTE1 Bandwidth Roofline Model (L0 Size Limited)")
    lines.append("=" * 80)
    lines.append("")
    lines.append("硬件限制说明：")
    lines.append(f"  L0 Buffer 最大容量: {l0_max_size_kb} KB")
    lines.append(f"  横轴范围: 0 - {l0_max_size_kb} KB (限制在L0容量内)")
    lines.append("")
    lines.append("Roofline 模型参数：")
    lines.append(f"  峰值带宽: {peak_bw_bytes_per_cycle} Byte/cycle")
    lines.append(f"  主频: {frequency_mhz} MHz")
    lines.append(f"  峰值带宽上限: {peak_bw_gbps:.2f} GB/s")
    lines.append(f"  固定延迟: {latency_cycles} cycles = {latency_cycles / frequency_mhz:.4f} us")
    lines.append("")
    lines.append("理论公式：")
    lines.append("  Time(cycles) = Fixed_Latency + DataSize / PeakBW")
    lines.append(f"  Time(cycles) = {latency_cycles} + DataSize / {peak_bw_bytes_per_cycle}")
    lines.append("  Bandwidth(GB/s) = DataSize / Time(us) / 1e3")
    lines.append("")
    lines.append("图表说明：")
    lines.append(f"  X轴: Data Size (KB) - 数据搬运量（限制在{l0_max_size_kb}KB内）")
    lines.append("  Y轴: Bandwidth (GB/s) - 实际带宽")
    lines.append("  实线: 理论带宽曲线（考虑延迟）")
    lines.append("  虚线: 峰值带宽上限（无延迟）")
    lines.append("  *号: 实际测试数据点")
    lines.append("")

    chart_width = 70
    chart_height = 25

    # ASCII 图按理论曲线和实测点的最大带宽共同缩放。
    max_bandwidth = max(max(theory_bandwidths), max(item["bandwidth_gbps"] for item in data))
    bw_per_line = max_bandwidth / chart_height
    kb_per_char = max_data_kb / chart_width

    grid = [[" " for _ in range(chart_width)] for _ in range(chart_height)]

    peak_y = int(chart_height - peak_bw_gbps / bw_per_line)
    if 0 <= peak_y < chart_height:
        for x in range(chart_width):
            if x % 3 == 0:
                grid[peak_y][x] = ":"

    for i in range(len(theory_data_sizes)):
        data_kb = theory_data_sizes[i]
        bw_gbps = theory_bandwidths[i]

        x_pos = int(data_kb / kb_per_char)
        y_pos = int(chart_height - bw_gbps / bw_per_line)

        x_pos = max(0, min(chart_width - 1, x_pos))
        y_pos = max(0, min(chart_height - 1, y_pos))

        if grid[y_pos][x_pos] == " ":
            grid[y_pos][x_pos] = "-"

    for item in data:
        x_pos = int(item["data_size_kb"] / kb_per_char)
        y_pos = int(chart_height - item["bandwidth_gbps"] / bw_per_line)

        x_pos = max(0, min(chart_width - 1, x_pos))
        y_pos = max(0, min(chart_height - 1, y_pos))

        grid[y_pos][x_pos] = "*"

    lines.append("Bandwidth (GB/s)")
    for i, row in enumerate(grid):
        bw_value = (chart_height - i) * bw_per_line

        if i == peak_y:
            label = f"{peak_bw_gbps:6.1f} ::"
        elif i % 5 == 0:
            label = f"{bw_value:6.1f} |"
        else:
            label = "       |"

        line = label + "".join(row)
        lines.append(line)

    lines.append("       +" + "-" * chart_width)
    lines.append("        Data Size (KB)")

    x_labels = []
    for i in range(0, chart_width + 1, 14):
        data_kb = int(i * kb_per_char)
        x_labels.append(f"{data_kb:3d}")
    lines.append("       " + " ".join(x_labels))

    lines.append("")
    lines.append("图例说明：")
    lines.append(f"  :  峰值带宽上限（{peak_bw_gbps:.1f} GB/s，无延迟）")
    lines.append(f"  -  理论带宽曲线（考虑{latency_cycles} cycle延迟）")
    lines.append("  *  实际测试数据点")
    lines.append(f"  L0 Buffer限制: {l0_max_size_kb} KB")
    lines.append("")

    lines.append("=" * 80)
    lines.append("实际测试数据详细分析")
    lines.append("=" * 80)

    for item in data:
        data_kb = item["data_size_kb"]

        theory_bw, theory_time, theory_cycles = calculate_theoretical_bandwidth(
            item["data_size"], peak_bw_bytes_per_cycle, latency_cycles, frequency_mhz
        )

        bw_diff = item["bandwidth_gbps"] - theory_bw
        time_diff_us = item["time_us"] - theory_time
        time_diff_cycles = item["cycle"] - theory_cycles

        lines.append(f"\nTest {item['test_id']}: Shape [{item['m']}, {item['k']}, {item['n']}]")
        lines.append("-" * 80)
        lines.append(f"  数据量: {data_kb:.2f} KB ({item['data_size']} bytes)")
        lines.append("")
        lines.append("  实际测量:")
        lines.append(f"    时间: {item['time_us']:.4f} us = {item['cycle']:.2f} cycles")
        lines.append(f"    带宽: {item['bandwidth_gbps']:.3f} GB/s")
        lines.append("")
        lines.append("  理论计算:")
        lines.append(f"    时间: {theory_time:.4f} us = {theory_cycles:.2f} cycles")
        lines.append(f"         = {latency_cycles} + {item['data_size']}/{peak_bw_bytes_per_cycle:.0f}")
        lines.append(f"         = {latency_cycles} + {item['data_size'] / peak_bw_bytes_per_cycle:.2f}")
        lines.append(f"    带宽: {theory_bw:.3f} GB/s")
        lines.append("")
        lines.append("  性能对比:")
        lines.append(f"    时间差异: {time_diff_us:.4f} us = {time_diff_cycles:.2f} cycles")
        lines.append(f"    带宽差异: {bw_diff:.3f} GB/s ({(bw_diff / theory_bw) * 100:+.1f}%)")

        if abs(bw_diff) < 5:
            rating = "✓ 理论相符"
        elif bw_diff > 5:
            rating = "✓ 优于理论"
        else:
            rating = "⚠ 低于理论"
        lines.append(f"    评级: {rating}")

    lines.append("")
    lines.append("=" * 80)
    lines.append("性能分析总结")
    lines.append("=" * 80)

    avg_bandwidth = sum(item["bandwidth_gbps"] for item in data) / len(data)
    avg_theory_bw = sum(
        calculate_theoretical_bandwidth(item["data_size"], peak_bw_bytes_per_cycle, latency_cycles, frequency_mhz)[0]
        for item in data
    ) / len(data)

    lines.append(f"  平均实际带宽: {avg_bandwidth:.3f} GB/s")
    lines.append(f"  平均理论带宽: {avg_theory_bw:.3f} GB/s")
    avg_diff = avg_bandwidth - avg_theory_bw
    avg_diff_ratio = avg_diff / avg_theory_bw * 100
    lines.append(f"  平均差异: {avg_diff:.3f} GB/s ({avg_diff_ratio:+.1f}%)")
    lines.append("")

    lines.append("延迟影响分析：")
    lines.append("")
    lines.append(f"  L0 Buffer最大容量: {l0_max_size_kb} KB = {l0_max_size_kb * 1024} bytes")
    lines.append("")

    for item in data:
        transfer_cycles = item["data_size"] / peak_bw_bytes_per_cycle
        latency_ratio = latency_cycles / (latency_cycles + transfer_cycles) * 100

        l0_utilization = item["data_size_kb"] / l0_max_size_kb * 100

        lines.append(f"  Shape [{item['m']},{item['k']},{item['n']}]:")
        lines.append(f"    数据量: {item['data_size_kb']:.2f} KB ({l0_utilization:.1f}% of L0)")
        lines.append(f"    延迟占比: {latency_ratio:.1f}%")
        lines.append(f"    数据搬运: {transfer_cycles:.1f} cycles")
        lines.append(f"    总时间: {latency_cycles + transfer_cycles:.1f} cycles")

    lines.append("")

    l0_max_bytes = l0_max_size_kb * 1024
    l0_bw, l0_time, l0_cycles = calculate_theoretical_bandwidth(
        l0_max_bytes, peak_bw_bytes_per_cycle, latency_cycles, frequency_mhz
    )

    lines.append(f"  L0满载时（{l0_max_size_kb} KB）理论性能：")
    lines.append(f"    时间: {l0_time:.4f} us = {l0_cycles:.2f} cycles")
    lines.append(f"    带宽: {l0_bw:.3f} GB/s")
    lines.append(f"    延迟占比: {latency_cycles / l0_cycles * 100:.1f}%")

    lines.append("")
    lines.append("=" * 80)

    with open(output_file, "w") as f:
        f.write("\n".join(lines))

    return "\n".join(lines)


def generate_matplotlib_roofline_with_latency(
    data,
    output_file,
    peak_bw_bytes_per_cycle=256,
    latency_cycles=30,
    frequency_mhz=1800,
    l0_max_kb=64,
    transfer_path="L1 → L0",
):
    """生成 PNG/PDF 版本 Roofline 图。"""
    try:
        import matplotlib.pyplot as plt
        import numpy as np

        # 与 ASCII 报告保持一致：优先使用 CSV 反推的数据量。
        for item in data:
            if "data_size" not in item:
                item["data_size"] = calculate_data_size(item["m"], item["k"])
                item["data_size_kb"] = item["data_size"] / 1024

        peak_bw_gbps = peak_bw_bytes_per_cycle * frequency_mhz / 1e3

        theory_sizes = np.linspace(0.1 * 1024, l0_max_kb * 1024, 100)
        theory_bandwidths = []

        for size in theory_sizes:
            bw, time, cycles = calculate_theoretical_bandwidth(
                size, peak_bw_bytes_per_cycle, latency_cycles, frequency_mhz
            )
            theory_bandwidths.append(bw)

        theory_sizes_kb = theory_sizes / 1024

        fig, ax1 = plt.subplots(figsize=(11, 7.5))

        ax1.axhline(
            y=peak_bw_gbps,
            color="red",
            linestyle="--",
            linewidth=2,
            label=f"Peak BW (no latency): {peak_bw_gbps:.1f} GB/s",
        )
        ax1.plot(
            theory_sizes_kb,
            theory_bandwidths,
            "b-",
            linewidth=3,
            label=f"Theoretical BW (with {latency_cycles}c latency)",
        )
        ax1.axvline(x=l0_max_kb, color="purple", linestyle=":", linewidth=2, label=f"L0 Max Size: {l0_max_kb} KB")

        colors = ["#2ecc71", "#f39c12", "#e74c3c", "#9b59b6", "#3498db", "#1abc9c", "#e67e22"]
        markers = ["o", "s", "D", "^", "v", "<", ">"]

        for i, item in enumerate(data):
            color_idx = i % len(colors)
            marker_idx = i % len(markers)

            ax1.scatter(
                item["data_size_kb"],
                item["bandwidth_gbps"],
                c=colors[color_idx],
                marker=markers[marker_idx],
                s=250,
                edgecolors="black",
                linewidths=2.5,
                zorder=5,
                label=f"Shape [{item['m']},{item['k']},{item['n']}]",
            )

            ax1.annotate(
                f"{item['bandwidth_gbps']:.1f} GB/s",
                xy=(item["data_size_kb"], item["bandwidth_gbps"]),
                xytext=(15, 15),
                textcoords="offset points",
                fontsize=11,
                fontweight="bold",
                bbox=dict(boxstyle="round,pad=0.5", facecolor="yellow", alpha=0.7),
            )

        ax1.set_xlabel(f"Data Size (KB) [L0 Max: {l0_max_kb}KB]", fontsize=14, fontweight="bold")
        ax1.set_ylabel("Bandwidth (GB/s)", fontsize=14, fontweight="bold")
        ax1.set_title(
            f"MTE1 Bandwidth Roofline Model\n({transfer_path}, Limited by L0 Size)", fontsize=16, fontweight="bold"
        )

        ax1.grid(True, linestyle="--", alpha=0.4)
        ax1.legend(loc="center left", bbox_to_anchor=(1.02, 0.5), fontsize=10)

        ax1.set_xlim(0, l0_max_kb * 1.1)
        ax1.set_ylim(0, peak_bw_gbps * 1.2)

        ax1.fill_between(
            [l0_max_kb, l0_max_kb * 1.1], [0, 0], [peak_bw_gbps * 1.2, peak_bw_gbps * 1.2], color="purple", alpha=0.1
        )

        ax1.text(
            l0_max_kb * 1.05,
            peak_bw_gbps * 0.5,
            "L0 Limit",
            fontsize=11,
            ha="center",
            rotation=90,
            color="purple",
            fontweight="bold",
        )

        # 参数框放在图外底部，避免遮挡曲线和数据点。
        formula_text = (
            "Model Parameters\n"
            f"Peak BW: {peak_bw_bytes_per_cycle} Byte/cycle    "
            f"Frequency: {frequency_mhz} MHz    "
            f"Fixed Latency: {latency_cycles} cycles    "
            f"L0 Max Size: {l0_max_kb} KB\n"
            f"Formula: Time(cycles) = {latency_cycles} + DataSize(bytes) / {peak_bw_bytes_per_cycle}; "
            "Bandwidth(GB/s) = DataSize(bytes) / Time(us) / 1e3"
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

        fig.subplots_adjust(left=0.10, right=0.72, bottom=0.24, top=0.88)

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
    """查找最新的 perf_data 目录中的 CSV 文件"""
    perf_dirs = sorted(Path(".").glob("perf_data_*"), reverse=True)

    for perf_dir in perf_dirs:
        csv_files = sorted(perf_dir.glob("perf_result_scenario*.csv"), reverse=True)
        if csv_files:
            return str(csv_files[0])

    return None


def main():
    parser = argparse.ArgumentParser(
        description="生成 MTE1 Bandwidth Roofline 图（带延迟模型）",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""示例用法:
  # 指定 CSV 文件
  python3 generate_roofline_with_latency.py --csv perf_data_20260613_162817/perf_result_scenario1.csv

  # 或直接传入路径
  python3 generate_roofline_with_latency.py perf_data_20260613_162817/perf_result_scenario1.csv

  # 不指定参数，自动查找最新数据
  python3 generate_roofline_with_latency.py

输出文件:
  - mte1_bandwidth_roofline_with_latency.txt (ASCII详细分析)
  - mte1_bandwidth_roofline_with_latency.png (PNG图表)
  - mte1_bandwidth_roofline_with_latency.pdf (PDF矢量图)
""",
    )

    parser.add_argument("--csv", "-c", type=str, help="CSV 文件路径（如果不指定，自动查找最新数据）")
    parser.add_argument("--peak-bw", type=int, default=256, help="峰值带宽（Byte/cycle），默认: 256")
    parser.add_argument("--latency", type=int, default=30, help="固定延迟（cycles），默认: 30")
    parser.add_argument("--frequency", type=int, default=1800, help="主频（MHz），默认: 1800")
    parser.add_argument("--l0-size", type=int, default=64, help="L0最大容量（KB），默认: 64")
    parser.add_argument("--output", "-o", type=str, help="输出文件前缀，默认: mte1_bandwidth_roofline_with_latency")
    parser.add_argument("csv_path", nargs="?", help="CSV 文件路径，等价于 --csv")

    args = parser.parse_args()

    csv_file = args.csv or args.csv_path

    if not csv_file:
        csv_file = find_latest_perf_data()
        if not csv_file:
            print("错误: 未找到 perf_data 目录或 CSV 文件")
            print("提示: 请先运行性能测试脚本，或使用 --csv 参数指定文件")
            parser.print_help()
            sys.exit(1)
        print(f"自动检测到最新数据: {csv_file}")

    if not Path(csv_file).exists():
        print(f"错误: CSV 文件不存在: {csv_file}")
        sys.exit(1)

    peak_bw = args.peak_bw
    latency = args.latency
    frequency = args.frequency
    l0_size = args.l0_size
    scenario = get_scenario_from_csv(csv_file)
    transfer_path = get_transfer_path_by_scenario(scenario)

    if args.output:
        output_prefix = args.output
    else:
        csv_path = Path(csv_file)
        parent_dir = csv_path.parent.name
        output_prefix = f"{parent_dir}_roofline"

    print(f"读取数据: {csv_file}")
    data = read_csv_data(csv_file)
    if not data:
        print(f"错误: CSV 文件没有可用于绘图的有效性能数据: {csv_file}")
        sys.exit(1)
    print(f"找到 {len(data)} 条测试数据")
    print()

    print("=" * 80)
    print("Roofline 模型参数")
    print("=" * 80)
    print(f"  理论峰值带宽: {peak_bw} Byte/cycle")
    print(f"  主频: {frequency} MHz")
    print(f"  峰值带宽: {peak_bw} × {frequency} / 1e3 = {peak_bw * frequency / 1e3:.2f} GB/s")
    print(f"  固定延迟: {latency} cycles = {latency / frequency:.4f} us")
    print(f"  L0 最大容量: {l0_size} KB")
    if scenario is not None:
        print(f"  场景编号: {scenario}")
    print(f"  搬运路径: {transfer_path}")
    print()
    print("理论公式:")
    print(f"  Time(cycles) = {latency} + DataSize(bytes) / {peak_bw}")
    print("  Bandwidth(GB/s) = DataSize(bytes) / Time(us) / 1e3")
    print()

    ascii_output = f"{output_prefix}.txt"
    print("生成 ASCII Roofline 图...")

    ascii_chart = generate_roofline_with_latency(data, ascii_output, peak_bw, latency, frequency, l0_size)
    print(ascii_chart)

    matplotlib_output = f"{output_prefix}.pdf"
    print("\n生成 Matplotlib Roofline 图...")
    generate_matplotlib_roofline_with_latency(
        data, matplotlib_output, peak_bw, latency, frequency, l0_size, transfer_path
    )

    print("\n" + "=" * 80)
    print("Roofline 图生成完成！")
    print("=" * 80)
    print(f"  输入文件: {csv_file}")
    print(f"  ASCII 版本: {ascii_output}")
    print(f"  PNG 版本: {matplotlib_output.replace('.pdf', '.png')}")
    print()
    print("使用方法:")
    print(f"  查看 ASCII 报告: cat {ascii_output}")
    print(f"  查看图片: 打开 {matplotlib_output.replace('.pdf', '.png')}")
    print()
    print("自定义参数:")
    print("  如需调整理论参数，可使用:")
    print(
        f"  python3 {sys.argv[0]} --csv {csv_file} --peak-bw {peak_bw} "
        f"--latency {latency} --frequency {frequency} --l0-size {l0_size}"
    )


if __name__ == "__main__":
    main()

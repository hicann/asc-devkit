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
"""生成基于固定延迟模型的 Fixpipe 搬出带宽 Roofline 报告。

  - 并行度（每 cycle Fixpipe 搬出单元处理的 out 元素数）由场景决定（见 SCENARIO_CONFIG）
  - 搬出量 DataSize(bytes) = M * N * sizeof(目的类型)，统一按 dst 目的类型字节数计算
  - 峰值带宽(Byte/cycle) = 并行度 * sizeof(目的类型)；双目标模式拆两个子核并行，等效翻倍
  - 理论总 cycle = 头开销 + DataSize / 峰值带宽(Byte/cycle)（头开销固定延迟：dav-2201=20，dav-3510=26）
  - 理论带宽(GB/s) = DataSize / 理论耗时(us) / 1e3，随搬出量增大趋近峰值带宽

实测：measured_cycles = aic_fixpipe_time(us) * 主频(MHz)；
      measured_bandwidth = DataSize(bytes) / time_us / 1e3（GB/s）。

Roofline 图：
  - 横轴：搬出量 Dst Data Size（KB）
  - 纵轴：带宽（GB/s）
  - 实线：理论带宽曲线（考虑头开销），虚线：峰值带宽上限，*号：实测点

示例:
  python3 generate_roofline.py --csv perf_data_xxx_scenario1/perf_result_scenario1.csv
  python3 generate_roofline.py perf_data_xxx_scenario1/perf_result_scenario1.csv
"""

import argparse
import csv
import re
import sys
from pathlib import Path

# 各场景的 Fixpipe 搬出并行度（out elem/cycle）、目的类型字节数、头开销（cycle）与主频（MHz）
# 并行度来源：硬件规格（L0C 搬出每 cycle 处理 64 个 out 元素，float 不转换时受字节带宽上限约束为 32）
# dsize：目的类型字节数（half=2、int8_t=1、float=4），用于把元素数换算为搬出字节数
# 头开销固定延迟：dav-2201=20、dav-3510=26（搬出单元首指令延迟）
# dual_factor：双目标模式拆两个子核并行，等效并行度翻倍，故 dual_factor=2；普通为 1
# 峰值带宽(Byte/cycle) = parallel * dsize * dual_factor
SCENARIO_CONFIG = {
    # scenario: {parallel, dsize, head, freq, dst_type, dual_factor, desc}
    1: {
        "parallel": 64,
        "dsize": 2,
        "head": 20,
        "freq": 1800,
        "dst_type": "half",
        "dual_factor": 1,
        "desc": "L0C->L1 float->half",
    },
    2: {
        "parallel": 64,
        "dsize": 1,
        "head": 20,
        "freq": 1800,
        "dst_type": "int8_t",
        "dual_factor": 1,
        "desc": "L0C->L1 float->int8_t",
    },
    11: {
        "parallel": 64,
        "dsize": 2,
        "head": 26,
        "freq": 1650,
        "dst_type": "half",
        "dual_factor": 1,
        "desc": "L0C->L1 float->half",
    },
    12: {
        "parallel": 64,
        "dsize": 1,
        "head": 26,
        "freq": 1650,
        "dst_type": "int8_t",
        "dual_factor": 1,
        "desc": "L0C->L1 float->int8_t",
    },
    13: {
        "parallel": 32,
        "dsize": 4,
        "head": 26,
        "freq": 1650,
        "dst_type": "float",
        "dual_factor": 1,
        "desc": "L0C->UB float->float single-dst",
    },
    14: {
        "parallel": 32,
        "dsize": 4,
        "head": 26,
        "freq": 1650,
        "dst_type": "float",
        "dual_factor": 2,
        "desc": "L0C->UB float->float dual-dst(split-M)",
    },
}


def get_scenario_from_csv(csv_file):
    """从 CSV 文件名或父目录名中解析场景编号。"""
    csv_path = Path(csv_file)
    for text in (csv_path.name, csv_path.parent.name):
        match = re.search(r"scenario(\d+)", text)
        if match:
            return int(match.group(1))
    return None


def read_csv_data(csv_file, freq_mhz, dsize):
    """读取 perf.sh 生成的 CSV，并过滤无效测试行。"""
    data = []
    with open(csv_file, "r") as f:
        reader = csv.DictReader(f)
        time_column = None
        if reader.fieldnames:
            for column in ("AIC_FixPipe_Time(us)", "AIC_FixPipe_Time", "aic_fixpipe_time(us)"):
                if column in reader.fieldnames:
                    time_column = column
                    break
        if time_column is None:
            raise ValueError("CSV 文件缺少 AIC_FixPipe_Time(us) 列")

        for row in reader:
            time_val = row[time_column]
            if time_val in ("N/A", "NA", "ERROR", ""):
                continue
            time_us = float(time_val)
            m = int(row["M"])
            k = int(row["K"])
            n = int(row["N"])
            data_size = float(m) * n * dsize  # 搬出字节数
            measured_cycles = time_us * freq_mhz
            measured_bw = data_size / time_us / 1e3 if time_us > 0 else 0.0  # GB/s

            data.append(
                {
                    "test_id": int(row["Test_ID"]),
                    "m": m,
                    "k": k,
                    "n": n,
                    "shape": row.get("Shape", f"{m}_{k}_{n}"),
                    "time_us": time_us,
                    "measured_cycles": measured_cycles,
                    "data_size": data_size,
                    "data_size_kb": data_size / 1024,
                    "measured_bw": measured_bw,
                }
            )
    return data


def calc_theory_bandwidth(data_size, peak_bw_bytes_per_cycle, head_overhead, freq_mhz):
    """理论带宽（GB/s）：总cycle = head + DataSize / 峰值带宽；带宽 = DataSize / 耗时(us) / 1e3。"""
    transfer_cycles = data_size / peak_bw_bytes_per_cycle
    total_cycles = head_overhead + transfer_cycles
    time_us = total_cycles / freq_mhz
    bw = data_size / time_us / 1e3 if time_us > 0 else 0.0
    return bw, time_us, total_cycles


def generate_roofline(data, output_file, cfg):
    """生成 ASCII 版本 Fixpipe 搬出带宽 Roofline 报告。"""
    parallel, dsize, head, freq = (cfg["parallel"], cfg["dsize"], cfg["head"], cfg["freq"])
    dual_factor = cfg.get("dual_factor", 1)
    peak_bw_bpc = parallel * dsize * dual_factor  # 峰值带宽 Byte/cycle
    peak_bw_gbps = peak_bw_bpc * freq / 1e3  # 峰值带宽 GB/s

    max_kb = max(item["data_size_kb"] for item in data)
    min_kb = 0.01

    theory_kb = []
    theory_bw = []
    num_points = 100
    for i in range(num_points):
        kb = min_kb + (max_kb - min_kb) * i / (num_points - 1)
        bw, _, _ = calc_theory_bandwidth(kb * 1024, peak_bw_bpc, head, freq)
        theory_kb.append(kb)
        theory_bw.append(bw)

    lines = []
    lines.append("=" * 80)
    lines.append("Fixpipe Bandwidth Roofline Model (Fixed-Latency)")
    lines.append("=" * 80)
    lines.append("")
    lines.append("搬出带宽与模型参数：")
    lines.append(f"  通路: {cfg['desc']}")
    lines.append(f"  目的类型: {cfg['dst_type']}（{dsize} 字节）")
    lines.append(f"  并行度: {parallel} out elem/cycle" + (f" x {dual_factor}（双目标）" if dual_factor > 1 else ""))
    lines.append(f"  峰值带宽: {peak_bw_bpc} Byte/cycle = {peak_bw_gbps:.2f} GB/s")
    lines.append(f"  主频: {freq} MHz")
    lines.append(f"  头开销（固定延迟）: {head} cycles = {head / freq:.4f} us")
    lines.append("")
    lines.append("理论公式：")
    lines.append(f"  DataSize(bytes) = M * N * {dsize}")
    lines.append(f"  理论总cycle = {head} + DataSize / {peak_bw_bpc}")
    lines.append("  理论耗时(us) = 理论总cycle / 主频(MHz)")
    lines.append("  带宽(GB/s) = DataSize / 耗时(us) / 1e3")
    lines.append("")
    lines.append("图表说明：")
    lines.append("  X轴: 搬出量 Dst Data Size (KB)")
    lines.append("  Y轴: 带宽 (GB/s)")
    lines.append("  实线: 理论带宽曲线（考虑头开销）")
    lines.append("  虚线: 峰值带宽上限（无延迟）")
    lines.append("  *号: 实际测试数据点")
    lines.append("")

    chart_width = 70
    chart_height = 25
    max_bw = max(max(theory_bw), max(item["measured_bw"] for item in data), peak_bw_gbps)
    bw_per_line = max_bw / chart_height
    kb_per_char = max_kb / chart_width if max_kb > 0 else 1.0

    grid = [[" " for _ in range(chart_width)] for _ in range(chart_height)]

    peak_y = int(chart_height - peak_bw_gbps / bw_per_line)
    if 0 <= peak_y < chart_height:
        for x in range(chart_width):
            if x % 3 == 0:
                grid[peak_y][x] = ":"

    for i in range(len(theory_kb)):
        x_pos = int(theory_kb[i] / kb_per_char)
        y_pos = int(chart_height - theory_bw[i] / bw_per_line)
        x_pos = max(0, min(chart_width - 1, x_pos))
        y_pos = max(0, min(chart_height - 1, y_pos))
        if grid[y_pos][x_pos] == " ":
            grid[y_pos][x_pos] = "-"

    for item in data:
        x_pos = int(item["data_size_kb"] / kb_per_char)
        y_pos = int(chart_height - item["measured_bw"] / bw_per_line)
        x_pos = max(0, min(chart_width - 1, x_pos))
        y_pos = max(0, min(chart_height - 1, y_pos))
        grid[y_pos][x_pos] = "*"

    lines.append("Bandwidth (GB/s)")
    for i, row in enumerate(grid):
        bw_value = (chart_height - i) * bw_per_line
        if i == peak_y:
            label = f"{peak_bw_gbps:7.1f} ::"
        elif i % 5 == 0:
            label = f"{bw_value:7.1f} |"
        else:
            label = "        |"
        lines.append(label + "".join(row))

    lines.append("        +" + "-" * chart_width)
    lines.append("         Dst Data Size (KB)")

    x_labels = []
    for i in range(0, chart_width + 1, 14):
        kb = i * kb_per_char
        x_labels.append(f"{kb:6.1f}")
    lines.append("        " + " ".join(x_labels))
    lines.append("")

    lines.append("图例说明：")
    lines.append(f"  :  峰值带宽上限（{peak_bw_gbps:.1f} GB/s，无延迟）")
    lines.append(f"  -  理论带宽曲线（考虑 {head} cycle 头开销）")
    lines.append("  *  实际测试数据点")
    lines.append("")

    lines.append("=" * 80)
    lines.append("实际测试数据详细分析")
    lines.append("=" * 80)

    for item in data:
        m, n = item["m"], item["n"]
        theory_bw_v, theory_time_us, theory_total_cycles = calc_theory_bandwidth(
            item["data_size"], peak_bw_bpc, head, freq
        )
        utilization = item["measured_bw"] / theory_bw_v * 100 if theory_bw_v > 0 else 0.0

        lines.append(f"\nTest {item['test_id']}: Shape [{m}, {item['k']}, {n}]")
        lines.append("-" * 80)
        lines.append(f"  搬出量: {item['data_size_kb']:.3f} KB ({item['data_size']:.0f} bytes)")
        lines.append("")
        lines.append("  实际测量:")
        lines.append(f"    时间: {item['time_us']:.4f} us = {item['measured_cycles']:.2f} cycles")
        lines.append(f"    带宽: {item['measured_bw']:.3f} GB/s")
        lines.append("")
        lines.append("  理论计算:")
        lines.append(f"    理论总cycle: {head} + {item['data_size']:.0f}/{peak_bw_bpc} = {theory_total_cycles:.2f}")
        lines.append(f"    理论耗时: {theory_time_us:.4f} us")
        lines.append(f"    理论带宽: {theory_bw_v:.3f} GB/s")
        lines.append("")
        lines.append("  性能对比:")
        lines.append(f"    带宽利用率（实测带宽/理论带宽）: {utilization:.1f}%")

    lines.append("")
    lines.append("=" * 80)
    lines.append("性能分析总结")
    lines.append("=" * 80)
    avg_util = 0.0
    cnt = 0
    for item in data:
        theory_bw_v, _, _ = calc_theory_bandwidth(item["data_size"], peak_bw_bpc, head, freq)
        if theory_bw_v > 0:
            avg_util += item["measured_bw"] / theory_bw_v * 100
            cnt += 1
    avg_util = avg_util / cnt if cnt > 0 else 0.0
    avg_bw = sum(item["measured_bw"] for item in data) / len(data)
    lines.append(f"  峰值带宽: {peak_bw_gbps:.1f} GB/s")
    lines.append(f"  平均实测带宽: {avg_bw:.3f} GB/s")
    lines.append(f"  平均带宽利用率: {avg_util:.1f}%")
    lines.append("")
    lines.append("=" * 80)

    with open(output_file, "w") as f:
        f.write("\n".join(lines))
    return "\n".join(lines)


def generate_matplotlib_roofline(data, output_file, cfg):
    """生成 PNG/PDF 版本 Fixpipe 搬出带宽 Roofline 图。"""
    try:
        import matplotlib.pyplot as plt
        import numpy as np

        parallel, dsize, head, freq = (cfg["parallel"], cfg["dsize"], cfg["head"], cfg["freq"])
        dual_factor = cfg.get("dual_factor", 1)
        peak_bw_bpc = parallel * dsize * dual_factor
        peak_bw_gbps = peak_bw_bpc * freq / 1e3

        max_kb = max(item["data_size_kb"] for item in data)
        theory_sizes = np.linspace(0.01 * 1024, max_kb * 1.1 * 1024, 100)
        theory_bw = [calc_theory_bandwidth(s, peak_bw_bpc, head, freq)[0] for s in theory_sizes]
        theory_kb = theory_sizes / 1024

        fig, ax1 = plt.subplots(figsize=(11, 7.5))
        ax1.axhline(
            y=peak_bw_gbps,
            color="red",
            linestyle="--",
            linewidth=2,
            label=f"Peak BW (no latency): {peak_bw_gbps:.1f} GB/s",
        )
        ax1.plot(theory_kb, theory_bw, "b-", linewidth=3, label=f"Theoretical BW (with {head}c head overhead)")

        colors = ["#2ecc71", "#f39c12", "#e74c3c", "#9b59b6", "#3498db", "#1abc9c", "#e67e22"]
        markers = ["o", "s", "D", "^", "v", "<", ">"]
        for i, item in enumerate(data):
            ax1.scatter(
                item["data_size_kb"],
                item["measured_bw"],
                c=colors[i % len(colors)],
                marker=markers[i % len(markers)],
                s=250,
                edgecolors="black",
                linewidths=2.5,
                zorder=5,
                label=f"Shape [{item['m']},{item['k']},{item['n']}]",
            )
            ax1.annotate(
                f"{item['measured_bw']:.1f} GB/s",
                xy=(item["data_size_kb"], item["measured_bw"]),
                xytext=(15, 15),
                textcoords="offset points",
                fontsize=11,
                fontweight="bold",
                bbox=dict(boxstyle="round,pad=0.5", facecolor="yellow", alpha=0.7),
            )

        ax1.set_xlabel("Dst Data Size (KB)", fontsize=14, fontweight="bold")
        ax1.set_ylabel("Bandwidth (GB/s)", fontsize=14, fontweight="bold")
        ax1.set_title(
            f"Fixpipe Bandwidth Roofline Model\n({cfg['desc']}, peak {peak_bw_bpc} Byte/cycle, {freq} MHz)",
            fontsize=14,
            fontweight="bold",
        )
        ax1.grid(True, linestyle="--", alpha=0.4)
        ax1.legend(loc="center left", bbox_to_anchor=(1.02, 0.5), fontsize=10)
        ax1.set_xlim(0, max_kb * 1.15)
        ax1.set_ylim(0, peak_bw_gbps * 1.2)

        formula_text = (
            "Model Parameters\n"
            f"Peak BW: {peak_bw_bpc} Byte/cycle    Frequency: {freq} MHz    "
            f"Head Overhead: {head} cycles\n"
            f"Formula: Cycles = {head} + DataSize / {peak_bw_bpc}; Bandwidth = DataSize / Time(us) / 1e3"
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
        description="生成 Fixpipe 搬出带宽 Roofline 图（固定延迟模型，理论由搬出并行度推导）",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""示例用法:
  python3 generate_roofline.py --csv perf_data_xxx_scenario1/perf_result_scenario1.csv
  python3 generate_roofline.py            # 自动查找最新数据

说明:
  理论峰值带宽 = 搬出并行度 * 目的类型字节数（双目标翻倍），由场景直接得出，无需手工传入；
  头开销取固定延迟（dav-2201=20、dav-3510=26），可用 --head-overhead 覆盖。
""",
    )
    parser.add_argument("--csv", "-c", type=str, help="CSV 文件路径（不指定则自动查找最新数据）")
    parser.add_argument(
        "--head-overhead", type=int, default=None, help="头开销（cycles），默认按场景取（2201=20、3510=26）"
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
        print(f"错误: 无法识别场景编号 {scenario}，无法确定搬出并行度")
        print(f"支持的场景: {sorted(SCENARIO_CONFIG.keys())}")
        sys.exit(1)

    cfg = dict(SCENARIO_CONFIG[scenario])
    if args.head_overhead is not None:
        cfg["head"] = args.head_overhead

    if args.output:
        output_prefix = args.output
    else:
        output_prefix = f"{Path(csv_file).parent.name}_fixpipe_roofline"

    print(f"读取数据: {csv_file}")
    data = read_csv_data(csv_file, cfg["freq"], cfg["dsize"])
    if not data:
        print(f"错误: CSV 文件没有可用于绘图的有效性能数据: {csv_file}")
        sys.exit(1)
    print(f"找到 {len(data)} 条测试数据")
    print()

    _peak_bpc = cfg["parallel"] * cfg["dsize"] * cfg.get("dual_factor", 1)
    print("=" * 80)
    print("Roofline 模型参数")
    print("=" * 80)
    print(f"  场景编号: {scenario}（{cfg['desc']}）")
    print(f"  峰值带宽: {_peak_bpc} Byte/cycle")
    print(f"  主频: {cfg['freq']} MHz")
    print(f"  头开销: {cfg['head']} cycles")
    print()

    ascii_output = f"{output_prefix}.txt"
    print("生成 ASCII Roofline 图...")
    ascii_chart = generate_roofline(data, ascii_output, cfg)
    print(ascii_chart)

    matplotlib_output = f"{output_prefix}.pdf"
    print("\n生成 Matplotlib Roofline 图...")
    generate_matplotlib_roofline(data, matplotlib_output, cfg)

    print("\n" + "=" * 80)
    print("Roofline 图生成完成！")
    print("=" * 80)
    print(f"  输入文件: {csv_file}")
    print(f"  ASCII 版本: {ascii_output}")
    print(f"  PNG 版本: {matplotlib_output.replace('.pdf', '.png')}")


if __name__ == "__main__":
    main()

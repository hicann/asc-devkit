---
name: ascendc-api-kernel-debug
description: >-
  AscendC Kernel 调试接口选型和问题定位。用户需要在 SIMD、SIMT、SIMT VF
  或 SIMD VF 中使用 printf、AscendC::printf、assert、ascendc_assert、__trap、
  asc_dump、clock、asc_time_stamp，或遇到调试输出缺失、输出截断、断言失败、
  Trap、周期测量和时间戳未生成时，必须使用本 skill。覆盖接口签名、头文件、
  平台/编程模式限制、ASCENDC_DUMP、NDEBUG、ASCENDC_TIME_STAMP_ON、FIFO
  容量和调试后清理。触发：Kernel 调试、printf 没有输出、assert 失败、
  asc_dump 数据异常、clock 周期测量、asc_time_stamp 未生成。不覆盖
  asc_prof_start、asc_prof_stop、asc_mark_stamp、
  TRACE_START、TRACE_STOP。
---

# AscendC API Kernel Debug

面向 Kernel 侧调试接口的选型、最小改动和输出链路诊断。接口支持、产品矩阵和编译行为随 CANN/AscendC 版本变化，遇到版本或平台不明确时先查当前 API 文档、公开头文件和示例，不凭旧经验下结论。

## 工作流

1. **确认上下文**：记录目标产品或 `__NPU_ARCH__`、SIMD/SIMT/SIMT VF/SIMD VF、CPU 调试或 NPU 上板、直接调用或工程化算子。
2. **按目标选接口**：

   | 用户目标 | 首选接口 | 关键区别 |
   | --- | --- | --- |
   | 打印标量、索引、地址或字符串 | `printf` | 格式字符串 + 变参；控制线程和数据量 |
   | 检查不变量并让算子失败 | `assert`/`ascendc_assert` | 条件失败时输出并触发异常 |
   | 条件满足时立即中断 SIMT | `__trap` | 无参数、无返回值，只用于 SIMT 代码 |
   | 查看 GM/UB/L1/L0C/寄存器数据 | `asc_dump` 系列 | 按地址空间选择重载，检查元素数和对齐 |
   | 计算局部执行周期 | `clock` | 返回 `uint64_t`，通常使用 `end - start` |
   | 写出结构化时间戳记录 | `asc_time_stamp` | SIMD/NPU 调试，默认需显式编译开启 |

3. **查详细资料**：只读取与当前接口对应的 reference；若产品、API 变体或调用模式不明确，协同 `ascendc-docs-search` 查 `asc-devkit/docs/zh/api`、头文件和 examples。
4. **给最小补丁**：补正确头文件、调用和必要的编译/运行配置，保持算子计算逻辑不变。
5. **检查输出链路**：依次检查平台/模式支持、`ASCENDC_DUMP`、`NDEBUG`、`ASCENDC_TIME_STAMP_ON`、FIFO/每核上限、线程过滤和输出解析。
6. **验证并回收**：确认日志、dump 数据或异常符合预期；调试结束后移除调用或关闭调试开关，避免性能和产物污染。

## 快速边界

- `assert` 和 `ascendc_assert` 提供相同的 AscendC 断言能力。
- `__trap` 不是通用 SIMD 异常 API；先判断条件，再在 SIMT 代码中调用。
- `clock` 返回数值，不自动产生结构化 dump；`asc_time_stamp` 由编译开关控制并输出时间戳记录。
- `ASCENDC_DUMP=0` 会关闭依赖 Dump 的调试输出；`NDEBUG` 会使断言不生效。
- `asc_dump` 的 `dump_size` 必须不超过实际元素数；非 32 字节对齐时要考虑系统补齐和容量消耗。
- 调试接口会改变性能。不要用包含大量 `printf`/`asc_dump` 的测量结果代表原始 Kernel 性能。
- `DumpTensor`、`DumpAccChkPoint`、`PrintTimeStamp` 仅作为相邻能力说明，不是本 skill 的主接口。
- `asc_prof_start`、`asc_prof_stop`、`asc_mark_stamp`、`TRACE_START`、`TRACE_STOP` 明确不在本期范围内，应路由到 profiling 能力。
- 不把未提交的 Super Tensor/FIFO 内部协议、`ASCENDC_DUMP_SUPER_TENSOR` 或 type 11/12 当作公开调用契约。

## 常见症状

| 症状 | 先检查 | 继续动作 |
| --- | --- | --- |
| 完全没有输出 | 平台/模式、头文件、`ASCENDC_DUMP`、线程条件 | 对照 [diagnosis_workflow.md](references/diagnosis_workflow.md) |
| 输出不全 | SIMD/SIMT FIFO、每核上限、单条记录大小、打印线程数 | 减小输出或按文档增加对应配置 |
| 断言没有失败 | 条件是否为真、`NDEBUG`、`ASCENDC_DUMP=0` | 用最小失败用例验证开关和模式 |
| 断言失败但没有自定义消息 | CPU/VF 模式限制、FIFO 和单条消息大小 | 保留标准断言上下文，缩短自定义消息 |
| `asc_dump` 数据不可信 | 地址空间重载、`T`、`dump_size`、32B 补齐 | 先用小范围且已知长度的数据验证 |
| `asc_time_stamp` 没有输出 | `ASCENDC_TIME_STAMP_ON`、NPU 上板、入图限制 | 重新编译并检查每核累计量 |
| Kernel 失败或中断 | 区分 assert 与 `__trap`，再转运行时/崩溃 skill | 保留 plog 和返回码供后续诊断 |

## 输出格式

回答接口问题时，按以下顺序给出内容：

1. 目标上下文和假设。
2. 选择的接口、原因及不适用的相邻接口。
3. 头文件和最小调用示例。
4. 平台/编程模式限制、开关、FIFO 和性能影响。
5. 可复现验证命令或观察点。
6. 调试完成后的清理动作；若问题超出本 skill，给出明确路由。

## 参考资料

- [debug_api_quickref.md](references/debug_api_quickref.md)：六类接口的快速对比。
- [printf.md](references/printf.md)：格式化输出和 FIFO。
- [assert_and_trap.md](references/assert_and_trap.md)：断言和显式中断。
- [asc_dump.md](references/asc_dump.md)：地址空间、类型、对齐和容量。
- [clock_and_timestamp.md](references/clock_and_timestamp.md)：周期和结构化时间戳。
- [platform_matrix.md](references/platform_matrix.md)：公开资料中的平台/模式矩阵。
- [diagnosis_workflow.md](references/diagnosis_workflow.md)：输出缺失和异常排查。

## 脚本

静态扫描 Kernel 源文件：

```bash
python3 scripts/scan_debug_apis.py --json path/to/kernel.asc
python3 scripts/scan_debug_apis.py --fail-on-excluded path/to/kernel.asc
```

脚本只做文本级检查，不替代编译、NPU 运行和当前版本 API 文档核对。

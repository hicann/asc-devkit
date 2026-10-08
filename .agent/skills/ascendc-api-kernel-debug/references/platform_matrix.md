# 平台和编程模式矩阵

下表是基于当前本地 `asc-devkit/docs/zh/api/Utils-API/tuning_interface` 文档的摘要。它用于初步路由，不替代目标 CANN 版本的 API 文档、公开头文件和实际编译验证。

| 接口 | Ascend 950 | A3 | A2 | 310P/310B | 910/Kirin |
| --- | --- | --- | --- | --- | --- |
| `printf` | SIMD、SIMT、SIMT VF、SIMD VF | 主要为 SIMD | 主要为 SIMD | 当前 `printf` 文档不支持 | 不支持 |
| `assert`/`ascendc_assert` | SIMD、SIMT/VF 变体 | SIMD | SIMD | 310P SIMD 文档支持；310B 不支持 | 不支持 |
| `__trap` | SIMT/SIMT VF | 当前文档不支持 | 当前文档不支持 | 当前文档不支持 | 不支持 |
| `asc_dump` | SIMD，另有受限 VF/Buffer 变体 | SIMD | SIMD | 当前文档不支持 | 不支持 |
| `clock` | SIMD、SIMT | SIMD | SIMD | 当前文档不支持 | 不支持 |
| `asc_time_stamp` | SIMD、NPU 上板 | SIMD、NPU 上板 | SIMD、NPU 上板 | 当前文档不支持 | 不支持 |

## 使用规则

1. “主要为 SIMD”表示当前公开 tuning 文档没有为 A2/A3 承诺 SIMT 或 VF 变体，不代表所有其他开发路径都没有相关能力。
2. 目标架构未知时，先获取 `__NPU_ARCH__`/SoC 信息，再选择 API；不要用产品名称推断每个接口都支持。
3. `__trap` 的最新公开文档只列出 Ascend 950PR&950DT系列产品。即使头文件存在其他条件编译分支，也必须以目标版本公开文档为准。
4. SIMD VF 的 reserved UBUF、FIFO 和编译选项限制需要单独检查，不能套用普通 SIMD 的容量结论。
5. API 支持不等于当前调用场景可运行；还要检查 CPU/NPU、直接调用/入图、编译模式和版本。

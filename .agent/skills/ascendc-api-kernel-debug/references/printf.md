# printf 调试指南

## 头文件和调用形式

```cpp
#include "utils/debug/asc_printf.h"

AscendC::printf("block=%u value=%f\\n", AscendC::GetBlockIdx(), value);
```

在使用 `AscendC::printf` 的工程中，按当前示例补充 `kernel_operator.h`。SIMD VF 的格式字符串必须位于 UB 可访问空间；SIMT/VF 的具体签名和平台支持必须以当前 API 文档为准。

## 格式符

常用 NPU 域格式符包括：

| 格式符 | 用途 | 常见类型 |
| --- | --- | --- |
| `%d`、`%i`、`%ld`、`%lld` | 十进制整数 | `int8_t`、`int16_t`、`int32_t`、`int64_t` |
| `%u`、`%lu`、`%llu` | 无符号整数 | `uint8_t`、`uint16_t`、`uint32_t`、`uint64_t` |
| `%x`、`%lx`、`%llx` | 十六进制整数 | 有符号和无符号整型 |
| `%f`、`%F` | 浮点数 | `float`、`half`、`bfloat16_t` |
| `%s` | 字符串 | 字符串指针/字面量，按场景检查地址空间 |
| `%p` | 指针地址 | 指针 |

格式符数量、顺序和参数类型必须匹配。不要用未经核对的 C 标准 `printf` 经验推断 NPU 域的所有类型支持。

## 输出容量和性能

- SIMD 场景单次调用的打印总量默认约 30 KB；当前文档允许通过 `simd_printf_fifo_size_per_core` 配置，取值范围和调用方式随工程场景确认。
- SIMT 场景默认使用约 2 MB 的 FIFO；当前文档允许通过 `simt_printf_fifo_size` 配置，范围为 1 MB 到 64 MB。
- SIMD VF 中 `printf` 与 `assert`/`ascendc_assert`/`asc_dump` 共享预留 UB 空间，单条记录需要能完整放入；FIFO 太小会导致部分数据不输出。
- 多核、多线程重复打印会迅速放大数据量，也会改变执行时间。优先按 `GetBlockIdx()` 或线程 ID 保留一个代表线程。
- 调试完成后删除打印，或按工程要求关闭 `ASCENDC_DUMP`。

## 无输出排查

1. 检查目标平台和 SIMD/SIMT/VF 模式是否支持该形式。
2. 检查头文件和调用命名空间，确认没有误用 CPU-only `printf`。
3. 检查是否定义了 `ASCENDC_DUMP=0`。
4. 检查 FIFO 配置和每核/单次输出是否超限。
5. 检查打印语句所在分支、Block/线程过滤和 Kernel 是否提前失败。
6. 用一个无参数的短字符串最小化验证，再逐步添加格式参数。

不要使用 `printf` 代替功能校验；文本打印只能提供局部观测，不能保证数据搬运或同步逻辑正确。

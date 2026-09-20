# asc_dump 调试指南

## 头文件和参数

```cpp
#include "utils/debug/asc_dump.h"

asc_dump_gm<half>((__gm__ half *)src, 1, 32);
asc_dump_ubuf<half>((__ubuf__ half *)ubuf, 2, 32);
asc_dump_l1buf<half>((__cbuf__ half *)l1, 3, 32);
asc_dump_cbuf<float>((__cc__ float *)l0c, 4, 16);
```

模板参数 `T` 是数据类型；输入地址必须与地址空间匹配；`desc` 是用于区分输出来源的 `uint32_t` 自定义标识；`dump_size` 是要打印的元素个数。

## 地址空间选择

| 数据位置 | 优先接口 | 指针限定符 |
| --- | --- | --- |
| Global Memory | `asc_dump_gm` 或 `asc_dump` 重载 | `__gm__` |
| Unified Buffer | `asc_dump_ubuf` 或 `asc_dump` 重载 | `__ubuf__` |
| L1 Buffer | `asc_dump_l1buf` 或 `asc_dump` 重载 | `__cbuf__` |
| L0C Buffer | `asc_dump_cbuf` 或 `asc_dump` 重载 | `__cc__` |
| SIMD VF 寄存器 | `asc_dump_reg` 或寄存器参数的 `asc_dump` | 寄存器类型，平台受限 |

Ascend 950 的公开文档还描述 BiasTable Buffer 和 Fixpipe Buffer 的变体。只有在当前产品文档和编译头文件同时确认支持时才使用这些变体；Fixpipe 保存的可能是硬件参数位域，输出不一定等于上游原始数据。

## 必须检查的限制

1. `dump_size` 超出输入实际元素数会产生未定义问题；先确认地址和长度，再决定打印元素数。
2. 非 32 字节对齐的 dump 需要考虑末尾 padding 对容量的影响；padding 本身不一定会显示。
3. SIMD 场景单核调试输出默认约 30 KB，超限时可能没有输出；配置和上限必须按当前 CANN 文档确认。
4. SIMD VF 中 `asc_dump`、`printf` 和断言共享预留 UB 空间；单条记录过大或关闭 reserved UBUF 时，接口可能不可用。
5. `ASCENDC_DUMP=0` 可关闭 dump；该接口只用于调试，不应保留在生产性能路径。
6. 多核同时 dump 时，`desc` 应区分阶段，线程/Block 应适当过滤，避免输出不可读或超限。

## 输出不可信时的最小验证

1. 用长度明确、已知值且不跨边界的 GM/UB 小数组。
2. 只 dump 1 个或 32 个元素，确认模板类型与实际存储类型一致。
3. 先验证 `asc_dump_gm`/`asc_dump_ubuf`，再扩展到 L1/L0C/VF 特殊 Buffer。
4. 对照 `desc`、Block 和地址空间标签检查输出来源。
5. 若只是希望查看 LocalTensor/GlobalTensor 的常规内容，评估公开的 `DumpTensor`；本 skill 不把它作为 `asc_dump` 的内部实现协议。

## Super Tensor/FIFO 边界

当前工作树可能包含 Super Tensor、type 11/12 或 FIFO 传输实现，但它们不是本 reference 的公开调用契约。除非对应能力已经进入当前版本公开 API 文档，否则不要生成这些宏、TLV 字段或内部 transport 的使用代码。

# asc_mem_bar

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

Reg计算宏函数内不同流水线之间的同步指令。该同步指令指定源流水线和目的流水线，如下图所示，目的流水线将等待源流水线上所有指令完成才进行执行。
读写场景下，当读指令使用的寄存器和写指令使用的寄存器相同时，可以触发寄存器保序，指令将会按照代码顺序执行，不需要插入同步指令，而当使用的寄存器不同时，如果要确保读写指令执行，则需要插入同步指令。写写场景同理。

![SIMD_VF函数内流水线等待示意图.png](../../figures/simd_vf_pipeline_wait_diagram.png)

## 函数原型

```cpp
__simd_callee__ inline void asc_mem_bar(MEM_TYPE mem_type)
```

## 参数说明

**表1** 参数说明
| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| mem_type | 输入 | 同步流水线的类型，类型为MEM_TYPE，取值范围见表2 mem_type取值说明。 |

**表2** mem_type取值说明（源流水线/目的流水线表示的含义见表3 Reg计算流水线说明）
| 值  | 源流水线 | 目的流水线 |
| ------ | -------- | -------- |
| VV_ALL | VEC_ALL | VEC_ALL |
| VST_VLD | VEC_STORE | VEC_LOAD |
| VLD_VST | VEC_LOAD | VEC_STORE |
| VST_VST | VEC_STORE | VEC_STORE |
| VS_ALL | VEC_ALL | SCALAR_ALL |
| VST_LD | VEC_STORE | SCALAR_LOAD |
| VLD_ST | VEC_LOAD | SCALAR_STORE |
| VST_ST | VEC_STORE | SCALAR_STORE |
| SV_ALL | SCALAR_ALL | VEC_ALL |
| ST_VLD | SCALAR_STORE | VEC_LOAD |
| LD_VST | SCALAR_LOAD | VEC_STORE |
| ST_VST | SCALAR_STORE | VEC_STORE |
| SS_ALL | SCALAR_ALL | SCALAR_ALL |
| ST_LD | SCALAR_STORE | SCALAR_LOAD |
| LD_ST | SCALAR_LOAD | SCALAR_STORE |
| ST_ST | SCALAR_STORE | SCALAR_STORE |

**表3** Reg计算流水线说明
| 流水线 | 含义 |
| ------ | -------- |
| VEC_STORE | SIMD_VF函数内矢量写UB流水线。<br>对应寄存器到UB的搬运指令，例如[asc_store](../reg_store/asc_store.md)。 |
| VEC_LOAD | SIMD_VF函数内矢量读UB流水线。<br>对应UB到寄存器的搬运指令，例如[asc_load](../reg_load/asc_load.md)。 |
| SCALAR_STORE | SIMD_VF函数内标量写UB流水线。 |
| SCALAR_LOAD | SIMD_VF函数内标量读UB流水线。 |
| VEC_ALL | SIMD_VF函数内所有矢量读写UB流水线。 |
| SCALAR_ALL | SIMD_VF函数内所有标量读写UB流水线。 |

## 返回值说明

无

## 约束说明

- 读写依赖的场景下，如果读指令和写指令使用的寄存器相同，会触发寄存器保序，指令将会按照代码顺序执行，无需额外插入同步指令。
- 冗余的同步指令会导致性能下降，可以通过外提出循环或者循环切分避免多次调用同步指令。
- 当UB数据存在依赖时，才需要插入同步，判断是否有依赖取决于指令读写的内存是否有重叠。部分搬运指令读写内存的模式如下：
    - [asc_gather](../reg_load/asc_gather.md)/[asc_scatter](../reg_store/asc_scatter.md)/[asc_gather_datablock](../reg_load/asc_gather_datablock.md)等指令取决于index地址偏移；
    - [asc_loadalign_brc_elem](../reg_load/asc_loadalign_brc_elem.md)单条指令读32B数据后将第一个元素进行广播；

## 调用示例

如下示例中，for循环中后一次循环中读UB矢量搬运，和前一次循环中写UB矢量搬运，操作了同一块UB地址空间。因此后一次循环中读UB矢量搬运需要等待前一次循环中写UB矢量搬运执行完成后才能执行，需要插入VEC_LOAD等待VEC_STORE的同步VST_VLD。

```cpp
// ub_addr是外部输入的UB内存空间地址
__ubuf__ half* ub_addr;

vector_half dst;
vector_half src0;
vector_half src1;
vector_bool mask;
mask = asc_create_mask_b16(PAT_ALL);

uint16_t repeatTime = 8;
uint16_t repeatSize = 64;
for (uint16_t i = 1; i < repeatTime; i++) {
    asc_mem_bar(VST_VLD);
    asc_loadalign(src0, ub_addr);
    asc_loadalign(src1, ub_addr + i * repeatSize);
    asc_add(dst, src0, src1, mask);
    asc_storealign(ub_addr, dst, mask);
}

```

# asc\_dump

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->

## 功能说明

将对应内存上的数据打印出来，同时支持打印自定义的附加信息（仅支持uint32\_t类型的信息），比如打印当前行号等。

使用该接口需要包含`"utils/debug/asc_dump.h"`头文件。

> [!CAUTION]注意
>该功能主要用于**调试和性能分析**，通常在调测阶段使用，会对算子性能产生一定影响，**生产环境可以通过设置ASCENDC_DUMP=0的方式关闭**。
>默认情况下，该功能开启。

## 函数原型

```cpp
// GM内存上的数据打印
template<typename T>
__aicore__ inline void asc_dump_gm(__gm__ T* input, uint32_t desc, uint32_t dump_size)

// UB内存上的数据打印
template<typename T>
__aicore__ inline void asc_dump_ubuf(__ubuf__ T* input, uint32_t desc, uint32_t dump_size)

// L1内存上的数据打印
template<typename T>
__aicore__ inline void asc_dump_l1buf(__cbuf__ T* input, uint32_t desc, uint32_t dump_size)

// L0C内存上的数据打印
template<typename T>
__aicore__ inline void asc_dump_cbuf(__cc__ T* input, uint32_t desc, uint32_t dump_size)

// GM内存上的数据打印
template<typename T>
__aicore__ inline void asc_dump(__gm__ T* input, uint32_t desc, uint32_t dump_size)

// UB内存上的数据打印
template<typename T>
__aicore__ inline void asc_dump(__ubuf__ T* input, uint32_t desc, uint32_t dump_size)

// L1内存上的数据打印
template<typename T>
__aicore__ inline void asc_dump(__cbuf__ T* input, uint32_t desc, uint32_t dump_size)

// L0C内存上的数据打印
template<typename T>
__aicore__ inline void asc_dump(__cc__ T* input, uint32_t desc, uint32_t dump_size)
```

<!-- npu="950" id7 -->
以下接口为simd\_vf中所使用的asc\_dump接口，仅支持Ascend 950PR/Ascend 950DT。

```cpp
// 寄存器上的数据打印
template <typename T, typename U>
__simd_callee__ inline void asc_dump_reg(U& input, uint32_t desc, uint32_t dump_size)

// 寄存器上的数据打印
template <typename T, typename U>
__simd_callee__ inline void asc_dump(U& input, uint32_t desc, uint32_t dump_size)

// UB内存上的数据打印
template <typename T>
__simd_callee__ inline void asc_dump_ubuf(__ubuf__ T* input, uint32_t desc, uint32_t dump_size)

// UB内存上的数据打印
template <typename T>
__simd_callee__ inline void asc_dump(__ubuf__ T* input, uint32_t desc, uint32_t dump_size)
```
<!-- end id7 -->

## 参数说明

**表1** 参数说明
| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| T | 输入 | 待dump的数据类型。 |
| U | 输入 | 待dump的寄存器数据类型。 |
| input | 输入 | 所需打印的内存块的起始地址。 |
| desc | 输入 | 自定义信息。uint32_t类型，支持的数据范围是[0, 2^32 - 1]。 |
| dump_size | 输入 | 所需要打印的元素数量。 |

## 数据类型
<!-- npu="950" id10 -->
- 针对Ascend 950PR/Ascend 950DT ：bool、int8_t、uint8_t、hifloat8_t、fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t。
<!-- end id10 -->
<!-- npu="A3" id11 -->
- 针对Atlas A3 训练系列产品/Atlas A3 推理系列产品 ：bool、int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t。
<!-- end id11 -->
<!-- npu="910b" id12 -->
- 针对Atlas A2 训练系列产品/Atlas A2 推理系列产品 ：bool、int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t。
<!-- end id12 -->

## 返回值说明

无

## 约束说明

<!-- npu="950" id8 -->
-   针对Ascend 950PR/Ascend 950DT，在使用该接口打印L1 Tensor数据时，HDK版本需要至少升级到25.7.0以上。
<!-- end id8 -->
-   使用该接口时，在每个核上dump的数据总量不能大于30KB，请开发者自行控制打印的内容数据量，超出则不会打印。
-   在计算数据量时，若dump的总长度未对齐，需要考虑padding数据的影响。当进行非对齐dump时，如果实际dump的元素长度不满足32字节对齐，系统会自动在其末尾补充一定数量的padding数据（这部分数据并不会打印），以满足对齐要求。
-   SIMD场景下，单次调用本接口打印的数据总量不可超过打印大小限制，默认为30KB。使用时应注意，如果超出这个限制，则数据不会被打印。您可以通过acl.json中的`"simd_printf_fifo_size_per_core"`字段进行配置，配置范围最小为1KB，最大为64MB（可通过[aclInit](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/latest/API/runtimeapi/aclcppdevg_03_0022.html)接口调整）。当打印数据量较大时，建议增加缓存空间。pytorch调用和算子入图场景暂不支持该配置。

-   在`simd_vf`场景下，每个AIV核在单次`asc_vf_call`执行期间最多能使用2KB的UB空间；同一次`asc_vf_call`中的所有`simd_vf`的`asc_dump`和`printf`调用共享该预留空间。
-   每次调用`simd_vf`的`asc_dump`时，除实际dump数据外，还会固定占用72字节的管理信息；实际dump数据需要按32字节向上对齐。
-   `simd_vf`调测接口不会检查上述预留空间是否越界。超过限制可能越界写入预留空间，导致打印结果异常，并可能影响算子执行。

## 调用示例

```cpp
__gm__ half* src;
uint32_t desc = 1;
uint32_t dump_size = 32;
asc_dump_gm<half>(src, desc, dump_size);
```

SIMD VF调用示例：

```cpp
__simd_vf__ inline void SimdVfDumpReg()
{
    AscendC::Reg::RegTensor<float> src_reg;
    uint32_t desc = 1;
    uint32_t dump_size = 32;
    asc_dump<float>(src_reg, desc, dump_size);
}
```

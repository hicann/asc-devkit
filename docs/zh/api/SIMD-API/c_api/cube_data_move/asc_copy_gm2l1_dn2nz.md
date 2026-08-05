# asc_copy_gm2l1_dn2nz

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

将矩阵数据从Global Memory搬运到L1 Buffer，在此过程中执行DN->NZ/NCHW->NC1HWC0/NCHW->C1HWNC0操作。若D未对齐C0的大小，将会在L1 Buffer的最内层维度填充0值。GM中的数据以DN类型存储；L1中的数据以NZ形式存储。

DN->NZ的搬运形式如下图：

![ ](../figures/asc_copy_gm2l1_dn2nz.png)

## 函数原型

- 常规搬运

    ```cpp
    __aicore__ inline void asc_copy_gm2l1_dn2nz(__cbuf__ int8_t* dst, __gm__ int8_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz(__cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz(__cbuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz(__cbuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz(__cbuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz(__cbuf__ int16_t* dst, __gm__ int16_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz(__cbuf__ uint16_t* dst, __gm__ uint16_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,  uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz(__cbuf__ half* dst, __gm__ half* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz(__cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz(__cbuf__ int32_t* dst, __gm__ int32_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz(__cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz(__cbuf__ float* dst, __gm__ float* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    ```

- 同步搬运

    ```cpp
    __aicore__ inline void asc_copy_gm2l1_dn2nz_sync(__cbuf__ int8_t* dst, __gm__ int8_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz_sync(__cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz_sync(__cbuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz_sync(__cbuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz_sync(__cbuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz_sync(__cbuf__ int16_t* dst, __gm__ int16_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz_sync(__cbuf__ uint16_t* dst, __gm__ uint16_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,  uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz_sync(__cbuf__ half* dst, __gm__ half* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz_sync(__cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz_sync(__cbuf__ int32_t* dst, __gm__ int32_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz_sync(__cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz_sync(__cbuf__ float* dst, __gm__ float* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    ```

## 参数说明

**表1** 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| dst | 输出 | 目的操作数（矢量）的起始地址。 |
| src | 输入 | 源操作数（矢量）的起始地址。 |
| loop1_src_stride | 输入 | 搬运过程中最内层循环相邻迭代源操作数的数据块间的间隔，单位为字节。 |
| l2_cache_ctl | 输入 | 配置数据在L2 Cache中的管理策略。取值说明请参见[表2](#table2)。 |
| n_value | 输入 | 源操作数中DN排布中的N值。 |
| d_value | 输入 | 源操作数中DN排布中的D值。 |
| loop4_src_stride | 输入 | 搬运过程中最外层循环相邻迭代源操作数的数据块间的间隔，单位为字节。 |
| smallc0_en | 输入 | SmallC0模式开关：  <br>&bull; true：C0_SIZE会被pad至32个字节。 <br>&bull; false：C0_SIZE会被pad为4个channel。 |

**表2** l2_cache_ctl取值说明 <a id="table2"></a>

| 取值 | 模式 | 含义 |
|------|------|------|
| 0    | NORMAL模式 | 启用L2 Cache，并且将分配的Cache Line标记为高替换优先级。|
| 1    | LAST模式 | &bull; 启用L2 Cache，并且将分配的Cache Line标记为低替换优先级。<br>&bull; **LAST模式功能，暂不支持。**|
| 2    | PERSISTENT模式 | &bull; 启用L2 Cache。已存入L2 Cache中的数据可能被替换，若需确保特定GlobalTensor的数据始终保留在L2 Cache中，可采用驻留模式。<br>&bull; 注意，被标记为驻留模式的Cache Line只能被其他同样被标记为驻留模式的Cache Line替换。<br>&bull; **目前该驻留模式功能尚在开发中，暂不支持，计划于Ascend 950PR/Ascend 950DT上提供支持。**|
| 4    | DISABLE模式 | 不启用L2 Cache，每次都直接从GM中读取，并且保持已有Cache Line的状态不变。 |

## 返回值说明

无

## 流水类型

PIPE_MTE2

## 约束说明

- 各存储单元的空间大小和对齐要求请参考[存储单元说明](../general_description_and_constraints.md#存储单元说明)。
- 注意当且仅当D <= 4时，smallc0_en才可以被启用，否则它会失效。

## 调用示例

```cpp
//搬运关于最内层循环的步长为4个字节
constexpr uint64_t loop1_src_stride = 4;
//L2 Cache采用NORMAL模式
constexpr uint8_t l2_cache_ctl = 1;
//N值为3
constexpr uint16_t n_value = 3;
//D值为3
constexpr uint32_t d_value = 3;
//搬运关于最外层循环的步长为4个字节
constexpr uint64_t loop4_src_stride = 2;
//每一个C0数据块都会被pad至32个字节
constexpr bool smallc0_en = true;
// src表示源操作数的起始地址
__cbuf__ half dst[256];
asc_copy_gm2l1_dn2nz(dst, src, loop1_src_stride, l2_cache_ctl, n_value, d_value, loop4_src_stride, smallc0_en);
```

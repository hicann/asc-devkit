# asc_copy_gm2l1

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：不支持
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
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

将矩阵数据从Global Memory搬运到L1 Buffer中，并支持不同类型的pad模式。

## 函数原型

- 普通搬运模式

    ```cpp
    // 不进行pad操作
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // 以32B为单位，每1B插入31B的pad
    __aicore__ inline void asc_copy_gm2l1_pad1(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // 以32B为单位，每1B插入15B的pad
    __aicore__ inline void asc_copy_gm2l1_pad2(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // 以32B为单位，每2B插入14B的pad
    __aicore__ inline void asc_copy_gm2l1_pad3(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // 以32B为单位，每4B插入12B的pad
    __aicore__ inline void asc_copy_gm2l1_pad4(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // 以32B为单位，每8B插入8B的pad
    __aicore__ inline void asc_copy_gm2l1_pad5(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // 每32B删除最高位的28B,最低位的4B连续存储
    __aicore__ inline void asc_copy_gm2l1_pad6(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // 每32B删除最高位的24B,最低位的8B连续存储
    __aicore__ inline void asc_copy_gm2l1_pad7(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // 每32B删除最高位的16B,最低位的16B连续存储
    __aicore__ inline void asc_copy_gm2l1_pad8(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    ```

- 普通搬运模式（高维切分）

    ```cpp
    // 不进行pad操作
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    // 以32B为单位，每1B插入31B的pad
    __aicore__ inline void asc_copy_gm2l1_pad1(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    // 以32B为单位，每1B插入15B的pad
    __aicore__ inline void asc_copy_gm2l1_pad2(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    // 以32B为单位，每2B插入14B的pad
    __aicore__ inline void asc_copy_gm2l1_pad3(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    // 以32B为单位，每4B插入12B的pad
    __aicore__ inline void asc_copy_gm2l1_pad4(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    // 以32B为单位，每8B插入8B的pad
    __aicore__ inline void asc_copy_gm2l1_pad5(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    // 每32B删除最高位的28B,最低位的4B连续存储
    __aicore__ inline void asc_copy_gm2l1_pad6(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    // 每32B删除最高位的24B,最低位的8B连续存储
    __aicore__ inline void asc_copy_gm2l1_pad7(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    // 每32B删除最高位的16B,最低位的16B连续存储
    __aicore__ inline void asc_copy_gm2l1_pad8(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    ```

- 普通搬运模式（同步）

    ```cpp
    // 不进行pad操作
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // 以32B为单位，每1B插入31B的pad
    __aicore__ inline void asc_copy_gm2l1_pad1_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // 以32B为单位，每1B插入15B的pad
    __aicore__ inline void asc_copy_gm2l1_pad2_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // 以32B为单位，每2B插入14B的pad
    __aicore__ inline void asc_copy_gm2l1_pad3_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // 以32B为单位，每4B插入12B的pad
    __aicore__ inline void asc_copy_gm2l1_pad4_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // 以32B为单位，每8B插入8B的pad
    __aicore__ inline void asc_copy_gm2l1_pad5_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // 每32B删除最高位的28B,最低位的4B连续存储
    __aicore__ inline void asc_copy_gm2l1_pad6_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // 每32B删除最高位的24B,最低位的8B连续存储
    __aicore__ inline void asc_copy_gm2l1_pad7_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // 每32B删除最高位的16B,最低位的16B连续存储
    __aicore__ inline void asc_copy_gm2l1_pad8_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    ```

- 2D搬运模式

    ```cpp
    // 2D搬运bfloat16_t类型矩阵数据
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D搬运half类型矩阵数据
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ half* dst, __gm__ half* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D搬运float类型矩阵数据
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ float* dst, __gm__ float* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D搬运int32_t类型矩阵数据
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ int32_t* dst, __gm__ int32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D搬运int8_t类型矩阵数据
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ int8_t* dst, __gm__ int8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D搬运uint32_t类型矩阵数据
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D搬运uint8_t类型矩阵数据
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    ```

- 2D搬运模式（同步）

    ```cpp
    // 2D搬运bfloat16_t类型矩阵数据（同步）
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D搬运half类型矩阵数据（同步）
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ half* dst, __gm__ half* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D搬运float类型矩阵数据（同步）
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ float* dst, __gm__ float* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D搬运int32_t类型矩阵数据（同步）
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ int32_t* dst, __gm__ int32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D搬运int8_t类型矩阵数据（同步）
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ int8_t* dst, __gm__ int8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D搬运uint32_t类型矩阵数据（同步）
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D搬运uint8_t类型矩阵数据（同步）
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    ```

## 参数说明

### 普通搬运模式参数

| 参数名       | 输入/输出 | 描述                                                              |
|:----------|:------|:----------------------------------------------------------------|
| dst       | 输出    | 目的操作数（矢量）的起始地址。                                                 |
| src       | 输入    | 源操作数的起始地址。                                                      |
| size      | 输入    | 搬运数据大小（字节）。                                                     |
| n_burst   | 输入    | 待搬运的连续传输数据块个数。取值范围：[1, 4095]。                                   |
| len_burst | 输入    | 待搬运的每个连续传输数据块的长度，单位为DataBlock（32字节）。取值范围：[1, 65535]。            |
| src_gap   | 输入    | 源操作数相邻连续数据块的间隔（前面一个数据块的尾与后面一个数据块的头的间隔）。<br>单位为DataBlock（32字节）。  |
| dst_gap   | 输入    | 目的操作数相邻连续数据块的间隔（前面一个数据块的尾与后面一个数据块的头的间隔）。<br>单位为DataBlock（32字节）。 |

### 2D搬运模式参数

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| dst | 输出 | 目的操作数（矢量）的起始地址。 |
| src | 输入 | 源操作数（矢量）的起始地址。 |
| base_idx | 输入 | 分形矩阵ID，说明搬运起始位置为源操作数中的第几个分形（0为源操作数中的第一个分形矩阵）。取值范围：[0, 65535]。单位512B。默认为0。 |
| repeat | 输入 | 迭代次数。 |
| src_stride |输入| 相邻迭代间，源操作数前一个分形与后一个分形起始地址的间隔，单位：512B。取值范围：[0, 65535]。默认为0。 |
| dst_gap | 输入 | 相邻迭代间，目的操作数前一个分形结束地址与后一个分形起始地址的间隔，单位：512B。取值范围：[0, 65536]。默认为0。 |

## 返回值说明

无

## 流水类型

PIPE_MTE2

## 约束说明

- src的起始地址要求按照对应数据类型所占字节数对齐。
- dst的起始地址要求32字节对齐。
- 如果需要执行多条asc_copy_gm2l1指令，且asc_copy_gm2l1指令的目的地址存在重叠，需要插入同步指令，保证多个asc_copy_gm2l1指令的串行化，防止出现异常数据。

## 调用示例

```cpp
// 普通数据搬运模式
//total_length指参与搬运的数据总长度。
constexpr uint64_t total_length = 128;
__cbuf__ half dst[total_length];
__gm__ half src[total_length];
// 连续搬运的数据块数量
constexpr uint16_t n_burst = 1;
// 连续搬运的数据块长度，此处为512个half元素
constexpr uint16_t len_burst = 512 * sizeof(half) / 32;
// 源操作数做连续搬运
constexpr uint16_t src_gap = 0;
// 目的操作数连续排布
constexpr uint16_t dst_gap = 0;
asc_copy_gm2l1((__cbuf__ void*)dst, (__gm__ void*)src, n_burst, len_burst, src_gap, dst_gap);

// 2D数据搬运模式
//搬运的起始分块为1
constexpr uint16_t base_idx = 1;
//搬运的迭代次数为2
constexpr uint8_t repeat = 2;
//输入的搬运步长为0字节，输出的搬运步长为1024字节
constexpr uint16_t src_stride = 0;
constexpr uint16_t dst_gap = 1;
__gm__ half src[256];
__cbuf__ half dst[256];
asc_copy_gm2l1(dst, src, base_idx, repeat, src_stride, dst_gap);
```

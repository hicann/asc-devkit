# asc_copy_l12gm

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

将数据从L1 Buffer搬运到Global Memory。

## 函数原型

- 常规搬运

    ```cpp
    __aicore__ inline void asc_copy_l12gm(__gm__ void* dst, __cbuf__ void* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    ```
- 同步搬运

    ```cpp
    __aicore__ inline void asc_copy_l12gm_sync(__gm__ void* dst, __cbuf__ void* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    ```

## 参数说明


| 参数名 | 输入/输出 | 描述                                                              |
| :--- |:------|:----------------------------------------------------------------|
| dst | 输出    | 目的操作数的起始地址。|
| src | 输入    | 源操作数的起始地址。|
| n_burst | 输入    | 待搬运的连续传输数据块个数。取值范围：[1, 4095]。 |
| len_burst | 输入    | 待搬运的每个连续传输数据块的长度，单位为32个字节。取值范围：[1, 65535]。 |
| src_gap | 输入    | 相邻迭代间，源操作数前一个迭代第一个分形的结束地址到下一个迭代第一个分形起始地址的间隔。 |
| dst_gap | 输入    | 相邻迭代间，目的操作数前一个迭代第一个分形的结束地址到下一个迭代第一个分形起始地址的间隔。单位为32字节。 |

## 返回值说明

无

## 流水类型

PIPE_MTE1

## 约束说明

- src的起始地址要求按照对应数据类型所占字节数对齐。
- dst的起始地址要求32字节对齐。
- 如果需要执行多条asc_copy_l12gm指令，且asc_copy_l12gm指令的目的地址存在重叠，需要插入同步指令，保证多个asc_copy_l12gm指令的串行化，防止出现异常数据。

## 调用示例

```cpp
//待搬运的连续传输数据块为1
constexpr uint16_t n_burst = 1;
//待搬运的每个连续传输数据块的长度为32个字节
constexpr uint16_t len_burst = 1;
//相邻迭代间，源操作数前一个分形与后一个分形起始地址的间隔为32个字节
constexpr uint16_t src_gap = 0;
//相邻迭代间，目标操作数前一个分形与后一个分形起始地址的间隔为64个字节
constexpr uint16_t dst_gap = 1;
__cbuf__ half src[256];
__gm__ half dst[256];
asc_copy_l12gm(dst, src, n_burst, len_burst, src_gap, dst_gap);
```

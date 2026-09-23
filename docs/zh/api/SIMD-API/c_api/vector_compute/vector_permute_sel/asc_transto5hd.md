# asc_transto5hd

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`"c_api/vector_compute/compute/vector_permute_sel.h"`。

[数据格式](../../../../../guide/technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout.md)转换，一般用于将NCHW格式转换成NC1HWC0格式。特别的，也可以用于二维矩阵数据块的转置。本接口单次Repeat内可处理512Byte的数据（16个DataBlock），根据数据类型不同，支持不同shape的矩阵转置（比如数据类型为half时，单次Repeat可完成16*16大小的矩阵转置），同时还可以支持多次Repeat操作。

单次repeat内转换规则如下：

- 当输入数据类型位宽为16位时，每个DataBlock中包含16个数，指令内部会循环16次，每次循环都会分别从指定的16个DataBlock中的对应位置取值，组成一个新的DataBlock单元放入目的地址中。如下图所示，图中的srcList[0]-srcList[15]代表源操作数的16个DataBlock。![](../../figures/asc_transto5hd_b16.png "图1输入数据类型位宽为16位时的转换规则")

- 当数据类型位宽为32位时，每个DataBlock包含8个数，指令内部会循环8次，每次循环都会分别从指定的16个DataBlock中的对应位置取值，组成2个新的DataBlock放入目的地址中。如下图所示：![](../../figures/asc_transto5hd_b32.png "图2 输入数据类型位宽为32位时的转换规则")

- 当数据类型位宽为8位时，每个DataBlock包含32个数，指令内部会循环16次，每次循环都会分别从指定的16个DataBlock中的对应位置取值，组成半个DataBlock放入目的地址中，读取和存放是在DataBlock的高半部还是低半部由参数srcHighHalf和dstHighHalf决定。如下图所示：![](../../figures/asc_transto5hd_b8.png "图3 输入数据类型位宽为8位时的转换规则")

基于以上的转换规则，使用该接口进行NC1HWC0格式转换或者矩阵转置。NC1HWC0格式转换相对复杂，这里给出其具体的转换方法：

NCHW格式转换成NC1HWC0格式时，如果是数据类型的位宽为32位或者16位，则C0=16；如果数据类型的位宽为8位，则C0=32。下图以C0=16为例进行介绍：![](../../figures/asc_transto5hd_NCHW2NC1HWC0.png)

## 函数原型

```c
__aicore__ inline void asc_transto5hd_b32(ub_addr8_t dst,
                                          ub_addr8_t src,
                                          uint8_t repeat,
                                          uint16_t dst_stride,
                                          uint16_t src_stride)
__aicore__ inline void asc_transto5hd_b16(ub_addr8_t dst,
                                          ub_addr8_t src,
                                          uint8_t repeat,
                                          uint16_t dst_stride,
                                          uint16_t src_stride)
__aicore__ inline void asc_transto5hd_b8(ub_addr8_t dst,
                                         ub_addr8_t src,
                                         uint8_t repeat,
                                         uint16_t dst_stride,
                                         uint16_t src_stride,
                                         bool dst_high_half,
                                         bool src_high_half)
```

## 参数说明

**表1** 参数说明

|参数名|输入/输出|描述|
| ------------ | ------------ | ------------ |
|dst|输出|目的地址寄存器。|
|src|输入|源地址寄存器。|
|repeat|输入|重复迭代次数， repeat∈[0,255]。<br>注意事项：<br>&bull;当repeat为1时，目的操作数/源操作数的有效起始位置为dst/src序列输入的起始位置加上dst_stride/src_stride。如果要让目的操作数/源操作数的有效起始位置为dst/src序列输入的起始位置，需要将dst_stride/src_stride置为0。<br>&bull;当repeat大于1时，第一次repeat中目的操作数/源操作数的有效起始位置为dst/src序列输入的起始位置，第二次会加上dst_stride/src_stride。以此类推。|
|dst_stride|输入|相邻迭代间，目的操作数相同DataBlock地址Stride，单位：DataBlock。<br>相邻迭代间相同DataBlock的地址步长参数的详细说明请参考[repeatStride](../../general_description_and_constraints.md#repeatStride)。|
|src_stride|输入|相邻迭代间，源操作数相同DataBlock地址Stride，单位：DataBlock。<br>相邻迭代间相同DataBlock的地址步长参数的详细说明请参考[repeatStride](../../general_description_and_constraints.md#repeatStride)。|
|dst_high_half|输入|指定每个dst地址中的数据存储到DataBlock的高半部还是低半部，该配置只配置int8_t/uint8_t的数据类型。<br>支持的数据类型为bool，有以下两种取值：<br>&bull; true:表示存储于DataBlock的高半部。<br>&bull; false:表示存储于DataBlock的低半部。|
|src_high_half|输入|指定每个src地址中的数据存储到DataBlock的高半部还是低半部，该配置只配置int8_t/uint8_t的数据类型。<br>支持的数据类型为bool，有以下两种取值：<br>&bull; true:表示存储于DataBlock的高半部。<br>&bull;false:表示存储于DataBlock的低半部。|

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

- 操作数地址对齐约束请参考[存储单元说明](../../general_description_and_constraints.md#存储单元说明)。
- 操作数地址重叠约束请参考[通用地址重叠约束](../../general_description_and_constraints.md#通用地址重叠约束)。
- `dst`和`src`不能传入相同的地址寄存器起始位置，即目的地址寄存器对和源地址寄存器对不能重叠。
- `dst`和`src`仅支持`VA0`、`VA2`、`VA4`、`VA6`，且二者不能相同，配置方式请参考[asc_set_va_reg](./asc_set_va_reg.md)。每个地址寄存器存储8个地址，接口会将传入的起始寄存器与相邻寄存器组成一组，处理16个地址。
- 为获得更优性能，建议`dst`、`src`、`dst_high_half`和`src_high_half`传入编译期常量，例如直接传入`VA0`、`VA2`、`true`或`false`，或传入由这些常量初始化的局部变量。若传入运行时变量，编译器会保留地址寄存器或布尔参数的分支判断，影响性能。
- 进行NCHW格式到NC1HWC0格式的转换时，一般用法是将`src`和`dst`中的每个元素配置为每个HW平面的起点。
- 数据类型位宽为8位时，建议先固定`dst_high_half`和`src_high_half`，沿HW方向完成Repeat处理后，再修改`dst_high_half`和`src_high_half`，以获得更优性能。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR&950DT系列产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```

<!-- end id8 -->

一次运行会分别验证下面两个场景：

- 单次Repeat的矩阵转置。输入是一块按行连续存放的16×16矩阵，每行恰好占用一个16位数据类型的DataBlock。代码将每行的起始地址写入`src_list`，并将目的矩阵每行的起始地址写入`dst_list`；两组地址分别通过`asc_set_va_reg`关联到相邻的地址寄存器。调用`asc_transto5hd_b16`时设置`repeat=1`、`dst_stride=0`、`src_stride=0`，完成一次转置，最终逐元素检查输出第`col`行、第`row`列是否等于输入第`row`行、第`col`列。
- 多次Repeat配合外层循环完成NCHW到NC1HWC0格式转换。输入形状为N=2、C=32、H=W=16，按每16个通道划为一组，共有4组。外层循环每次为一组通道更新16个源地址和16个目的地址：源地址指向各通道的HW平面起点，目的地址指向目标布局中对应通道的起点。每组调用一次`asc_transto5hd_b16`，通过`repeat=16`遍历HW方向上的16段数据；`src_stride=1`使每次迭代读取源平面的下一个DataBlock，`dst_stride=16`使输出按NC1HWC0布局前进。最后按`[N, C1, H, W, C0]`的索引顺序逐元素核对结果。

```cpp
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {

constexpr uint32_t MATRIX_SIDE = 16;
constexpr uint32_t MATRIX_ELEMENTS = MATRIX_SIDE * MATRIX_SIDE;
constexpr uint32_t TOTAL_ELEMENTS = 2 * 32 * 16 * 16;
constexpr uint32_t OFFSET = 16 * 16 * 16;

__global__ __vector__ void transto5hd_kernel(__gm__ uint16_t* matrix_out,
                                              __gm__ uint16_t* matrix_in,
                                              __gm__ uint16_t* layout_out,
                                              __gm__ uint16_t* layout_in)
{
    asc_init();
    __ubuf__ uint16_t src[TOTAL_ELEMENTS];
    __ubuf__ uint16_t dst[TOTAL_ELEMENTS];
    __ubuf__ half* src_list[16];
    __ubuf__ half* dst_list[16];

    // 场景一：一次repeat完成16x16矩阵转置，stride为0时从地址列表起点读取。
    asc_copy_gm2ub_align(src, matrix_in, MATRIX_ELEMENTS * sizeof(uint16_t));
    asc_sync_mte2(0);
    for (int i = 0; i < 16; ++i) {
        src_list[i] = reinterpret_cast<__ubuf__ half*>(src + i * 16);
        dst_list[i] = reinterpret_cast<__ubuf__ half*>(dst + i * 16);
    }
    asc_set_va_reg(VA0, dst_list);
    asc_set_va_reg(VA1, dst_list + 8);
    asc_set_va_reg(VA2, src_list);
    asc_set_va_reg(VA3, src_list + 8);
    asc_transto5hd_b16(VA0, VA2, 1, 0, 0);
    asc_sync_vec(0);
    asc_copy_ub2gm_align(matrix_out, dst, MATRIX_ELEMENTS * sizeof(uint16_t));
    asc_sync_mte3(0);

    // 场景二：N=2、C=32、H=W=16，四组C0=16通道分别处理。
    asc_copy_gm2ub_align(src, layout_in, TOTAL_ELEMENTS * sizeof(uint16_t));
    asc_sync_mte2(0);
    for (int j = 0; j < 4; ++j) {
        for (int i = 0; i < 16; ++i) {
            src_list[i] = reinterpret_cast<__ubuf__ half*>(src + OFFSET * j + i * 16 * 16);
            dst_list[i] = reinterpret_cast<__ubuf__ half*>(dst + OFFSET * j + i * 16);
        }
        asc_set_va_reg(VA0, dst_list);
        asc_set_va_reg(VA1, dst_list + 8);
        asc_set_va_reg(VA2, src_list);
        asc_set_va_reg(VA3, src_list + 8);
        // 16次repeat沿HW方向推进，src_stride=1、dst_stride=16，单位均为DataBlock。
        asc_transto5hd_b16(VA0, VA2, 16, 16, 1);
    }
    asc_sync_vec(0);
    asc_copy_ub2gm_align(layout_out, dst, TOTAL_ELEMENTS * sizeof(uint16_t));
    asc_sync_mte3(0);
}

void check(aclError status, const char* operation)
{
    if (status != ACL_SUCCESS) {
        std::cerr << operation << " failed: " << status << std::endl;
        std::exit(1);
    }
}

} // namespace

int main()
{
    std::vector<uint16_t> matrix_in(MATRIX_ELEMENTS), matrix_out(MATRIX_ELEMENTS);
    std::vector<uint16_t> layout_in(TOTAL_ELEMENTS), layout_out(TOTAL_ELEMENTS);
    for (uint32_t i = 0; i < MATRIX_ELEMENTS; ++i) matrix_in[i] = i;
    for (uint32_t i = 0; i < TOTAL_ELEMENTS; ++i) layout_in[i] = i;

    check(aclInit(nullptr), "aclInit");
    check(aclrtSetDevice(0), "aclrtSetDevice");
    uint16_t *matrix_in_device = nullptr, *matrix_out_device = nullptr;
    uint16_t *layout_in_device = nullptr, *layout_out_device = nullptr;
    check(aclrtMalloc(reinterpret_cast<void**>(&matrix_in_device), MATRIX_ELEMENTS * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST), "aclrtMalloc matrix_in");
    check(aclrtMalloc(reinterpret_cast<void**>(&matrix_out_device), MATRIX_ELEMENTS * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST), "aclrtMalloc matrix_out");
    check(aclrtMalloc(reinterpret_cast<void**>(&layout_in_device), TOTAL_ELEMENTS * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST), "aclrtMalloc layout_in");
    check(aclrtMalloc(reinterpret_cast<void**>(&layout_out_device), TOTAL_ELEMENTS * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST), "aclrtMalloc layout_out");
    check(aclrtMemcpy(matrix_in_device, MATRIX_ELEMENTS * sizeof(uint16_t), matrix_in.data(), MATRIX_ELEMENTS * sizeof(uint16_t), ACL_MEMCPY_HOST_TO_DEVICE), "copy matrix_in");
    check(aclrtMemcpy(layout_in_device, TOTAL_ELEMENTS * sizeof(uint16_t), layout_in.data(), TOTAL_ELEMENTS * sizeof(uint16_t), ACL_MEMCPY_HOST_TO_DEVICE), "copy layout_in");
    transto5hd_kernel<<<1, 0>>>(matrix_out_device, matrix_in_device, layout_out_device, layout_in_device);
    check(aclrtSynchronizeDevice(), "aclrtSynchronizeDevice");
    check(aclrtMemcpy(matrix_out.data(), MATRIX_ELEMENTS * sizeof(uint16_t), matrix_out_device, MATRIX_ELEMENTS * sizeof(uint16_t), ACL_MEMCPY_DEVICE_TO_HOST), "copy matrix_out");
    check(aclrtMemcpy(layout_out.data(), TOTAL_ELEMENTS * sizeof(uint16_t), layout_out_device, TOTAL_ELEMENTS * sizeof(uint16_t), ACL_MEMCPY_DEVICE_TO_HOST), "copy layout_out");

    bool matrix_passed = true;
    for (uint32_t row = 0; row < 16; ++row) {
        for (uint32_t col = 0; col < 16; ++col) {
            matrix_passed &= matrix_out[col * 16 + row] == matrix_in[row * 16 + col];
        }
    }
    bool layout_passed = true;
    for (uint32_t n = 0; n < 2; ++n) {
        for (uint32_t c = 0; c < 32; ++c) {
            for (uint32_t hw = 0; hw < 256; ++hw) {
                uint32_t src_index = (n * 32 + c) * 256 + hw;
                uint32_t dst_index = ((n * 2 + c / 16) * 256 + hw) * 16 + c % 16;
                layout_passed &= layout_out[dst_index] == layout_in[src_index];
            }
        }
    }
    std::cout << "16x16 transpose: " << (matrix_passed ? "PASS" : "FAIL") << std::endl;
    std::cout << "NCHW to NC1HWC0: " << (layout_passed ? "PASS" : "FAIL") << std::endl;
    check(aclrtFree(matrix_in_device), "free matrix_in");
    check(aclrtFree(matrix_out_device), "free matrix_out");
    check(aclrtFree(layout_in_device), "free layout_in");
    check(aclrtFree(layout_out_device), "free layout_out");
    check(aclrtResetDevice(0), "aclrtResetDevice");
    check(aclFinalize(), "aclFinalize");
    return matrix_passed && layout_passed ? 0 : 1;
}
```

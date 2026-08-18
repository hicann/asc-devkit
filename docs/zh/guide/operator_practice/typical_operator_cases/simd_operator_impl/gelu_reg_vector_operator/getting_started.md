# 入门功能落地<a name="ZH-CN_TOPIC_000000_GELU_ENTRY"></a>

本节以GELU（高斯误差线性单元）激活函数为例，介绍基于Ascend C RegBase编程模型实现向量算子的基本方法，帮助开发者快速入门Reg矢量算子开发。

## 算子概述

GELU近似计算公式为：

$$
GELU(x) \approx \frac{x}{1 + e^{-1.595769 \cdot x - 0.071405 \cdot x^3}}
$$

- x：输入，shape为[256, 32]，数据类型为float；
- y：输出，shape为[256, 32]，数据类型为float；

## 算子实现

GELU算子采用RegBase编程方式实现——通过[asc_vf_call](../../../../../api/SIMD-API/basic_api/reg_vector_compute/vf_call/asc_vf_call.md)调用VF函数，在寄存器中完成逐步计算。与传统MemBase（基于[LocalTensor](../../../../../api/SIMD-API/basic_api/data_structures/LocalTensor/LocalTensor_intro.md) + Compute API）方式相比，RegBase允许中间计算结果暂存在寄存器而非Unified Buffer（UB）中，减少了UB读写次数，适合多步骤融合计算场景。

完整的数据通路为：GM → UB → 寄存器（逐步计算）→ UB → GM。

核心计算逻辑封装在VF函数中：

```cpp
// VF 函数：在寄存器中完成 GELU 逐步计算
__simd_vf__ inline static void GeluVfMethod2(
    __ubuf__ float* xAddr, __ubuf__ float* yAddr, uint32_t count, uint32_t loopNum)
{
    constexpr uint32_t oneRepeatSize = AscendC::GetVecLen() / sizeof(float);  // GetVecLen()返回一个repeat的字节长度（VL=256B），除以数据类型字节数得到每个repeat处理的元素数（float类型下为64个元素）
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::RegTensor<float> xReg;
    AscendC::Reg::RegTensor<float> yReg;
    AscendC::Reg::RegTensor<float> tmpReg;
    for (uint32_t i = 0; i < loopNum; ++i) {
        mask = AscendC::Reg::UpdateMask<float>(count);
        AscendC::Reg::LoadAlign(xReg, xAddr + i * oneRepeatSize);   // UB → 寄存器
        AscendC::Reg::Mul(yReg, xReg, xReg, mask);                  // x²
        AscendC::Reg::Mul(yReg, yReg, xReg, mask);                  // x³
        AscendC::Reg::Muls(yReg, yReg, COEFF_CUBIC, mask);          // -0.071405 * x³
        AscendC::Reg::Muls(tmpReg, xReg, COEFF_LINEAR, mask);       // -1.595769 * x
        AscendC::Reg::Add(yReg, tmpReg, yReg, mask);                // -1.595769x - 0.071405x³
        AscendC::Reg::Exp(yReg, yReg, mask);                        // exp(...)
        AscendC::Reg::Adds(yReg, yReg, 1.0f, mask);                 // 1 + exp(...)
        AscendC::Reg::Div(yReg, xReg, yReg, mask);                  // x / (1 + exp(...))
        AscendC::Reg::StoreAlign(yAddr + i * oneRepeatSize, yReg, mask); // 寄存器 → UB
    }
}
```

完整样例代码请参见[Gelu入门样例](../../../../../../../examples/01_simd_cpp_api/00_introduction/04_reg_compute/gelu)。

## 可优化方向分析

| 序号 | 可优化方向 | 当前实现的问题 | 预期优化收益 |
|------|-----------|--------------|------------|
| 1 | VF融合双发优化 | VF函数内GELU计算依赖路径较长，VF融合的双发特性未充分利用，IPC较低 | 利用VF融合双发特性，常规计算指令并行度可达512 bytes/cycle，向量化指令耗时可减少**55%**以上 |
| 2 | 循环展开优化 | VF函数内循环为简单for循环，编译器无法充分调度指令级并行 | 使用`#pragma unroll N`展开循环，提高指令发射并行度和IPC，在VF融合基础上向量指令耗时可进一步减少约**4.6%** |
| 3 | 流水线串行执行 | 搬入、计算、搬出三个阶段严格串行执行，各硬件单元（MTE2/V/MTE3）无法同时工作 | 采用多缓冲机制，使搬入、计算、搬出可并行执行，提升硬件利用率 |
| 4 | 核数固定 | 固定使用2个核，未根据实际可用核数动态分配 | 动态获取可用核数，充分利用多核并行能力 |

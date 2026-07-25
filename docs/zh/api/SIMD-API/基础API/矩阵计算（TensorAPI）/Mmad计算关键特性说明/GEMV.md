# GEMV

## 特性说明

GEMV的核心功能体现为：当矩阵A的维度M取值为1时，接口可以手动启用GEMV功能，该操作退化为1×K维度的行向量与K×N维度矩阵之间的乘法运算。

<!-- npu="950" id1 -->
针对Ascend 950PR/Ascend 950DT产品，GEMV模式默认关闭。如需启用GEMV模式，需要设置`MmadTrait::disableGemv`参数为`false`。
<!-- end id1 -->

## 特性约束

- 1×K矩阵A需满足512B地址对齐，K个数据连续存储。以`half`类型为例，当K=256时，软件侧可视作16×16分块，配置m=1后硬件解析为1×256向量，可通过`Copy`接口将256个`half`数据从L1 Buffer搬至L0A Buffer。

- C矩阵是一个1×N的向量，当1×N向量被划分为多个1×16子向量时，每个子向量在L0C Buffer中占用1024B。但实际有效数据仅占最低32B或64B。例如，当N=50时，共划分4个1×16子向量，占用4×512B=2048B，其中有效数据仅为4×32B=128B。

**图1**  GEMV模式矩阵乘示意图

![GEMV模式矩阵乘示意图](../../../../figures/mmad_gemv.png)

## 使用优势

M=1时开启GEMV模式，则矩阵乘法将M方向作为非对齐场景进行处理。GEMV模式相较于非对齐处理方式，搬运数据量更少，性能更好。下面以M=1、K=256、N=32，左右矩阵数据类型为`half`的矩阵乘示例说明。

- GEMV模式

    将A矩阵从L1 Buffer搬运到L0A Buffer时，1×256的向量被当作16×16的矩阵进行处理，调用`Copy`接口一次完成16×16分形大小的矩阵搬运。B矩阵的搬运以及矩阵乘计算跟基础场景相同，如下图所示。

    **图2**  GEMV模式M=1的矩阵乘计算示意图

    ![GEMV模式M=1的矩阵乘计算示意图](../../../../figures/mmad_gemv_compare.png)

- 非GEMV模式

    将A矩阵从L1 Buffer搬运到L0A Buffer时，1×256的向量被当作非对齐矩阵数据进行处理，将M方向对齐到32字节后进行搬运。调用`Copy`接口每次搬运16×16分形大小的矩阵，一共搬运K/16=16次，导致搬运数据量增加，性能相较于GEMV模式差，如下图所示。

    **图3**  非GEMV模式M不等于1的矩阵乘计算示意图

    ![非GEMV模式M不等于1的矩阵乘计算示意图](../../../../figures/mmad_nongemv_compare.png)

## 使用示例

本GEMV场景示例的规格如下：

| 矩阵 | 维度大小 | 数据类型 | 格式 |
| --- | --- | --- | --- |
| A | 1×4096 | half | ND |
| B | 4096×256 | half | NZ |
| C | 1×256 | float | ND |

通过[Copy](../Copy（GM到L1数据搬运）.md/../矩阵计算的搬入/Copy（GM到L1数据搬运）.md)接口将输入矩阵A从Global Memory搬运到L1 Buffer上并保持ND格式不变，然后在L1 Buffer上将A矩阵转换为NZ格式，再通过[Copy](../Copy（L1到L0A数据搬运）.md/../矩阵计算的搬入/Copy（L1到L0A数据搬运）.md)接口搬运到L0A Buffer，从而以GEMV模式执行矩阵乘。另外注意GEMV模式默认是关闭的，需要自定义[MmadTrait](../Mmad计算/Mmad.md)来开启。

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

struct MmadTraitCustom {
    using TraitType = MmadTrait;
    // 设置MmadTrait::disableGemv参数为false，开启GEMV。
    static constexpr const TraitType value = MmadTrait(0, false, false, false, MmadType::NORMAL);
};

__aicore__ inline void MmadGemvExample(__gm__ half* aGm, __gm__ half* bGm, __gm__ float* cGm)
{
    using AType = half;
    constexpr uint32_t m = 1;
    constexpr uint32_t k = 4096;
    constexpr uint32_t n = 256;
    // 数据类型half的C0_ELEMENT = C0_SIZE(32B) / sizeof(half) = 16，1×K向量按NZ格式重排为(K/C0_ELEMENT)×C0_ELEMENT大小的Tensor。
    constexpr uint32_t c0 = C0_ELEMENT<AType>;

    __cbuf__ AType l1ABuf[m * k];
    __ca__ AType l0ABuf[m * k];

    // A矩阵Global Memory->L1 Buffer：ND->ND，1×K行向量按ND连续搬入L1 Buffer。
    auto gmATensor = MakeTensor(MakeMemPtr(aGm), MakeFrameLayout<NDExtLayoutPtn>(m, k));
    auto l1ATensor = MakeTensor(MakeMemPtr(l1ABuf), MakeFrameLayout<NDExtLayoutPtn>(m, k));
    auto copyGm2L1 = MakeCopy(CopyGM2L1{}, CopyGM2L1TraitDefault{});
    Copy(copyGm2L1, l1ATensor, gmATensor);

    // 在L1 Buffer上把这块连续数据重新解释为(K/C0_ELEMENT)×C0_ELEMENT大小的NZ分形矩阵（地址不变，仅换Layout）。
    auto l1ANzTensor = MakeTensor(l1ATensor.Data(), MakeFrameLayout<NZLayoutPtn, AType>(k / c0, c0));
    // L1 Buffer->L0A Buffer：NZ->NZ非转置搬运，一次完成分形搬运。
    auto l0ATensor = MakeTensor(MakeMemPtr(l0ABuf), MakeFrameLayout<NZLayoutPtn, AType>(k / c0, c0));
    auto copyL12L0A = MakeCopy(CopyL12L0A{}, CopyL12L0ATraitDefault{});
    Copy(copyL12L0A, l0ATensor, l1ANzTensor);

    // B矩阵搬运与基础场景相同（此处省略，假设已得到l0B）。
    // ...

    // Mmad计算：使用配置GEMV模式的Trait。
    MmadParams params(m, n, k, 0, true);
    auto atom = MakeMmad(MmadOperation{}, MmadTraitCustom{}).with(params);
    Mmad(atom, l0C, l0ATensor, l0B);
}
```

# Mmad

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

头文件路径为：`tensor_api/tensor.h`。

`Mmad`接口用于完成L0A Buffer上左矩阵A和L0B Buffer上右矩阵B的矩阵乘加，结果写入L0C Buffer上结果矩阵C。默认模式为普通矩阵计算。流水类型：PIPE_M。数学表达式为：

```text
C = A * B
```

带bias输入时，bias作为结果矩阵C初始值参与计算，等价于：

```text
C = A * B + Bias
```

Mmad的矩阵乘加关系可参考下图：

**图1**  Mmad矩阵乘加公式

![Mmad矩阵乘加公式](../../../../figures/mmad_formula.png)

`Mmad`使用显式传入的`MmadAtom`配置矩阵计算功能。左矩阵A、右矩阵B、结果矩阵C的图示说明如下：

**图2**  Mmad矩阵布局

![Mmad矩阵布局](../../../../figures/mmad_matrix_layout.png)

## 矩阵计算说明

| 矩阵 | 物理位置 | 维度 | 数据格式 | 数据类型 |
| :--- | :--- | :--- | :--- | :--- |
| 左矩阵A | L0A Buffer | m × k | NZ | 参见[数据类型](#数据类型) |
| 右矩阵B | L0B Buffer | k × n | ZN | 参见[数据类型](#数据类型) |
| 结果矩阵C | L0C Buffer | m × n | NZ | 参见[数据类型](#数据类型) |

## 函数原型

- 执行不传bias的矩阵计算。

    ```cpp
    template <typename AtomType, typename DstTensor, typename FmTensor, typename FilterTensor>
    __aicore__ inline void Mmad(
        const MmadAtom<AtomType>& atomMmad, const DstTensor& dst, const FmTensor& fm, const FilterTensor& filter)
    ```

- 执行传入bias的矩阵计算。

    ```cpp
    template <typename AtomType, typename DstTensor, typename FmTensor, typename FilterTensor, typename BiasTensor,
        Std::enable_if_t<IsAttrTensorV<BiasTensor>, int> Enable>
    __aicore__ inline void Mmad(
        const MmadAtom<AtomType>& atomMmad, const DstTensor& dst, const FmTensor& fm, const FilterTensor& filter,
        const BiasTensor& bias)
    ```

- 构造默认矩阵计算原子对象。

    ```cpp
    template <typename MmadOperationType>
    __aicore__ inline constexpr auto MakeMmad(const MmadOperationType& mmadOperation)
    ```

- 构造指定Trait的矩阵计算原子对象。

    ```cpp
    template <typename MmadOperationType, typename MmadTraitType>
    __aicore__ inline constexpr auto MakeMmad(
        const MmadOperationType& mmadOperation, const MmadTraitType& mmadTrait)
    ```

## 参数说明

**表1**  `Mmad`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| atomMmad | 输入 | 矩阵计算原子对象。通过`MakeMmad(MmadOperation{})`或`MakeMmad(MmadOperation{}, MmadTraitDefault{})`可以构造默认原子对象。 |
| dst | 输出 | 结果矩阵C，存储位置为`Location::L0C`，数据格式为NZ。 |
| fm | 输入 | 左矩阵A，存储位置为`Location::L0A`，数据格式为NZ。 |
| filter | 输入 | 右矩阵B，存储位置为`Location::L0B`，数据格式为ZN。 |
| bias | 输入 | bias张量，存储位置为`Location::BIAS`或`Location::L0C`，数据格式为ND。 |

**表2**  `MakeMmad`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| mmadOperation | 输入 | 矩阵计算操作对象，通过`MmadOperation{}`构造。 |
| mmadTrait | 输入 | 矩阵计算Trait对象，用于指定矩阵计算静态特性。默认使用`MmadTraitDefault{}`。 |

**表3**  `MmadParams`参数说明

| 参数名 | 类型 | 默认值 | 描述 |
| :--- | :--- | :--- | :--- |
| m | `uint16_t` | `0` | 左矩阵A的高度，结果矩阵C的高度。 |
| n | `uint16_t` | `0` | 右矩阵B的宽度，结果矩阵C的宽度。 |
| k | `uint16_t` | `0` | 左矩阵A的宽度，右矩阵B的高度。 |
| unitFlag | `uint8_t` | `0` | 控制`Mmad`和后续矩阵数据搬出的细粒度并行。`0`表示不使能，`2`表示使能且执行后不复位单元标记位，`3`表示使能且执行后复位单元标记位。 |
| cmatrixInitVal | `bool` | `false` | 不传bias时，控制是否初始化结果矩阵C。`true`表示C矩阵默认初始化为0，`false`表示C矩阵不进行默认操作，通过设置`cmatrixSource`参数进行初始化。 |

**表4**  `MmadTrait`参数说明

| 参数名 | 类型 | 默认值 | 描述 |
| :--- | :--- | :--- | :--- |
| fmOffset | `int32_t` | `0` | 左矩阵offset，当前Tensor API实现中作为兼容参数保留。 |
| kDirectionAlign | `bool` | `false` | K方向对齐控制，当前Tensor API实现中作为兼容参数保留。 |
| cmatrixSource | `bool` | `false` | 配置C矩阵初始值是否来源于BT Buffer，带bias调用时该配置无效。`false`表示C矩阵不进行初始化操作，`true`表示使用BT Buffer的数据对C矩阵进行初始化操作。|
| disableGemv | `bool` | `true` | M=1场景下是否关闭GEMV模式。`false`表示开启GEMV，`true`表示关闭GEMV。 |
| mmadType | `MmadType` | `MmadType::NORMAL` | 矩阵计算类型。默认使用`MmadType::NORMAL`。使用`MmadType::MX`表示MX场景Mmad计算。 |

## 数据类型

支持如下左矩阵A、右矩阵B、结果矩阵C的数据类型组合：

| 左矩阵A | 右矩阵B | 结果矩阵C |
| :--- | :--- | :--- |
| `int8_t` | `int8_t` | `int32_t` |
| `half` | `half` | `float` |
| `float` | `float` | `float` |
| `bfloat16_t` | `bfloat16_t` | `float` |
| `fp8_e4m3fn_t` | `fp8_e4m3fn_t` | `float` |
| `fp8_e4m3fn_t` | `fp8_e5m2_t` | `float` |
| `fp8_e5m2_t` | `fp8_e4m3fn_t` | `float` |
| `fp8_e5m2_t` | `fp8_e5m2_t` | `float` |
| `hifloat8_t` | `hifloat8_t` | `float` |

传入bias的场景中，`int8_t * int8_t -> int32_t`的组合要求bias数据类型为`int32_t`，其余组合要求bias数据类型为`float`。

## 返回值说明

`Mmad`无返回值。`MakeMmad`返回`MmadAtom`对象。

## 约束说明

- `dst`必须位于L0C Buffer，`fm`必须位于L0A Buffer，`filter`必须位于L0B Buffer。
- 结果矩阵C起始地址需要满足64Byte地址对齐要求。
- 左矩阵A和右矩阵B起始地址需要满足512Byte地址对齐要求。
- 上述地址对齐要求属于硬件访问约束，编译器和运行时不对所有场景单独检查，用户需要保证入参满足约束。
- `m`、`n`、`k`需要与实际参与`Mmad`计算的数据尺寸一致。
- 开启`unitFlag`功能时，需要结果矩阵C搬出接口的`FixpipeParams::unitFlag`与`Mmad`计算的`MmadParams::unitFlag`参数配合设置。
- 连续两次`Mmad`沿K方向累加时，需要关注结果矩阵C的写读依赖。同步优化的阈值关系可参考[结果矩阵C初始化](#结果矩阵c初始化)中的图示。
- 当M、K、N不是16的倍数时，硬件仍以16×16分形块组织数据，尾块中的无效数据会占用分形块空间但不参与有效计算。有效数据与无效数据排布方式如下图所示：

**图3**  尾块有效数据排布

![尾块有效数据排布](../../../../figures/mmad_tail_valid_data.png)

## 关键特性说明

### 结果矩阵C初始化

不传bias时，`cmatrixInitVal`控制是否初始化结果矩阵C。通常第一次K方向累加时设置为`true`，后续K分块累加设置为`false`。如果多K分块累加场景中首次`Mmad`如果设为`false`，将导致C矩阵包含L0C Buffer残留数据，计算结果错误。

连续两次`Mmad`沿K方向累加时，通常需要关注中间结果的写读依赖。同步优化的阈值关系可参考下图：

**图4**  PipeBarrier阈值关系

![PipeBarrier阈值关系](../../../../figures/mmad_pipebarrier_threshold.png)

### UnitFlag

`unitFlag`用于控制`Mmad`与后续`Fixpipe`的细粒度并行。启用时，`MmadParams::unitFlag`和结果搬出的`FixpipeParams::unitFlag`需要配合设置，常见取值为`2`和`3`。

### GEMV模式

当M=1时，可通过`MmadTrait::disableGemv`控制是否开启GEMV模式。默认值为`true`，表示关闭GEMV。

### MX Mmad模式

MX Mmad模式没有单独的函数接口，而是通过`MmadTrait::mmadType`设置。执行MX Mmad时，需要将`MmadTrait::mmadType`设置为`MmadType::MX`，并通过`MakeMmad(MmadOperation{}, MmadTraitMX{})`构造矩阵计算原子对象。

```cpp
struct MmadTraitMX {
    using TraitType = MmadTrait;
    static constexpr const TraitType value = MmadTrait(0, false, false, true, MmadType::MX);
};
```

MX Mmad支持如下左矩阵A、右矩阵B、结果矩阵C的数据类型组合：

| 左矩阵A | 右矩阵B | 结果矩阵C |
| :--- | :--- | :--- |
| `fp4x2_e2m1_t` | `fp4x2_e2m1_t` | `float` |
| `fp4x2_e2m1_t` | `fp4x2_e1m2_t` | `float` |
| `fp4x2_e1m2_t` | `fp4x2_e2m1_t` | `float` |
| `fp4x2_e1m2_t` | `fp4x2_e1m2_t` | `float` |
| `fp8_e4m3fn_t` | `fp8_e4m3fn_t` | `float` |
| `fp8_e4m3fn_t` | `fp8_e5m2_t` | `float` |
| `fp8_e5m2_t` | `fp8_e4m3fn_t` | `float` |
| `fp8_e5m2_t` | `fp8_e5m2_t` | `float` |

MX Mmad通常需要在调用`Mmad`前完成左矩阵缩放数据ScaleA和右矩阵缩放数据ScaleB的搬运。ScaleA搬运可参考[L1到L0ScaleA数据搬运](../矩阵计算的搬入/Copy（L1到L0ScaleA数据搬运）.md)，ScaleB搬运可参考[L1到L0ScaleB数据搬运](../矩阵计算的搬入/Copy（L1到L0ScaleB数据搬运）.md)。矩阵缩放数据不作为`Mmad`函数参数传入，而是通过`Copy`接口提前搬运到L0ScaleA Buffer和L0ScaleB Buffer。

## 相关接口

- [UnitFlag](../Mmad计算关键特性说明/UnitFlag.md)
- [GEMV](../Mmad计算关键特性说明/GEMV.md)
- [HF32](../Mmad计算关键特性说明/HF32.md)
- [L1到L0ScaleA数据搬运](../矩阵计算的搬入/Copy（L1到L0ScaleA数据搬运）.md)
- [L1到L0ScaleB数据搬运](../矩阵计算的搬入/Copy（L1到L0ScaleB数据搬运）.md)
- [L0C到GM数据搬运](../矩阵计算的搬出/Copy（L0C到GM数据搬运）.md)
- [L0C到UB数据搬运](../矩阵计算的搬出/Copy（L0C到UB数据搬运）.md)

## 调用示例

- Mmad调用示例

    ```cpp
    #include "tensor_api/tensor.h"

    __aicore__ inline void NormalMmadExample()
    {
        using namespace AscendC::Te;

        constexpr uint16_t m = 16;
        constexpr uint16_t n = 16;
        constexpr uint16_t k = 16;

        __ca__ half l0aBuf[m * k];
        __cb__ half l0bBuf[k * n];
        __cc__ float l0cBuf[m * n];

        auto l0A = MakeTensor(MakeMemPtr(l0aBuf), MakeFrameLayout<NZLayoutPtn, half>(m, k));
        auto l0B = MakeTensor(MakeMemPtr(l0bBuf), MakeFrameLayout<ZNLayoutPtn, half>(k, n));
        auto l0C = MakeTensor(MakeMemPtr(l0cBuf), MakeFrameLayout<NZLayoutPtn, 16>(m, n));

        MmadParams params(m, n, k, 0, true);
        auto atom = MakeMmad(MmadOperation{}, MmadTraitDefault{}).with(params);
        Mmad(atom, l0C, l0A, l0B);
    }
    ```

- MX场景Mmad调用示例

    ```cpp
    #include "tensor_api/tensor.h"

    struct MmadTraitMX {
        using TraitType = AscendC::Te::MmadTrait;
        static constexpr const TraitType value = AscendC::Te::MmadTrait(0, false, false, true, AscendC::Te::MmadType::MX);
    };

    __aicore__ inline void MxMmadExample()
    {
        using namespace AscendC::Te;

        constexpr uint16_t m = 16;
        constexpr uint16_t n = 16;
        constexpr uint16_t k = 16;

        __ca__ fp8_e5m2_t l0aBuf[m * k];
        __cb__ fp8_e5m2_t l0bBuf[k * n];
        __cc__ float l0cBuf[m * n];

        auto l0A = MakeTensor(MakeMemPtr(l0aBuf), MakeFrameLayout<NZLayoutPtn, fp8_e5m2_t>(m, k));
        auto l0B = MakeTensor(MakeMemPtr(l0bBuf), MakeFrameLayout<ZNLayoutPtn, fp8_e5m2_t>(k, n));
        auto l0C = MakeTensor(MakeMemPtr(l0cBuf), MakeFrameLayout<NZLayoutPtn, 16>(m, n));

        MmadParams params(m, n, k, 0, true);
        auto atom = MakeMmad(MmadOperation{}, MmadTraitMX{}).with(params);
        Mmad(atom, l0C, l0A, l0B);
    }
    ```

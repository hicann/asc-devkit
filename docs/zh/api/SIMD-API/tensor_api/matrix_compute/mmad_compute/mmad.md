# mmad

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

`mmad`接口用于完成L0A Buffer上左矩阵A和L0B Buffer上右矩阵B的矩阵乘加，结果写入L0C Buffer上结果矩阵C。默认模式为普通矩阵计算。流水类型：PIPE_M。数学表达式为：

```text
C = A * B
```

带bias输入时，bias作为结果矩阵C初始值参与计算，等价于：

```text
C = A * B + Bias
```

mmad的矩阵乘加关系可参考下图：

**图1**  mmad矩阵乘加公式

![mmad矩阵乘加公式](../../../../figures/mmad_formula.png)

`mmad`使用显式传入的`mmad_atom`配置矩阵计算功能。左矩阵A、右矩阵B、结果矩阵C的图示说明如下：

**图2**  mmad矩阵布局

![mmad矩阵布局](../../../../figures/mmad_matrix_layout.png)

## 矩阵计算说明

**表1**  矩阵参数说明

| 矩阵 | 物理位置 | 维度 | 数据格式 | 数据类型 |
| :--- | :--- | :--- | :--- | :--- |
| 左矩阵A | L0A Buffer | m × k | NZ | 参见[数据类型](#数据类型) |
| 右矩阵B | L0B Buffer | k × n | ZN | 参见[数据类型](#数据类型) |
| 结果矩阵C | L0C Buffer | m × n | NZ | 参见[数据类型](#数据类型) |

## 函数原型

- 执行不传bias的矩阵计算。

    ```cpp
    template <typename Atom, typename CTensor, typename ATensor, typename BTensor>
    __aicore__ inline void mmad(
        const mmad_atom<Atom>& atom, const CTensor& c, const ATensor& a, const BTensor& b)
    ```

- 执行传入bias的矩阵计算。

    ```cpp
    template <typename Atom, typename CTensor, typename ATensor, typename BTensor, typename BiasTensor>
    __aicore__ inline void mmad(
        const mmad_atom<Atom>& atom, const CTensor& c, const ATensor& a, const BTensor& b,
        const BiasTensor& bias)
    ```

- 构造默认矩阵计算原子对象。

    ```cpp
    template <typename MmadOperation>
    __aicore__ inline constexpr auto make_mmad(const MmadOperation& operation)
    ```

- 构造指定Trait的矩阵计算原子对象。

    ```cpp
    template <typename MmadOperation, typename MmadTrait>
    __aicore__ inline constexpr auto make_mmad(
        const MmadOperation& operation, const MmadTrait& trait)
    ```

## 参数说明

**表2**  `mmad`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| atom | 输入 | 矩阵计算原子对象。通过`make_mmad(mmad_operation{})`或`make_mmad(mmad_operation{}, mmad_trait_default{})`可以构造默认原子对象。 |
| c | 输出 | 结果矩阵C，存储位置为`location::l0c`，数据格式为NZ。 |
| a | 输入 | 左矩阵A，存储位置为`location::l0a`，数据格式为NZ。 |
| b | 输入 | 右矩阵B，存储位置为`location::l0b`，数据格式为ZN。 |
| bias | 输入 | bias张量，存储位置为`location::bias`，数据格式为ND。 |

**表3**  `make_mmad`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| operation | 输入 | 矩阵计算操作对象，通过`mmad_operation{}`构造。 |
| trait | 输入 | 矩阵计算Trait对象，用于指定矩阵计算静态特性。默认使用`mmad_trait_default{}`。 |

```cpp
enum class unit_flag_mode : uint8_t { disable = 0, enable_keep = 2, enable_update = 3 };

struct mmad_params {
    __aicore__ constexpr mmad_params(){};

    __aicore__ constexpr mmad_params(uint16_t m, uint16_t n, uint16_t k,
        unit_flag_mode unit_flag, bool init_with_zero) :
        m(m), n(n), k(k), unit_flag(unit_flag), init_with_zero(init_with_zero) {};

    uint16_t m = 0;
    uint16_t n = 0;
    uint16_t k = 0;
    unit_flag_mode unit_flag = unit_flag_mode::disable;
    bool init_with_zero = false;
};
```

**表4**  `mmad_params`参数说明

| 参数名 | 类型 | 默认值 | 描述 |
| :--- | :--- | :--- | :--- |
| m | `uint16_t` | `0` | 左矩阵A的高度，结果矩阵C的高度。 |
| n | `uint16_t` | `0` | 右矩阵B的宽度，结果矩阵C的宽度。 |
| k | `uint16_t` | `0` | 左矩阵A的宽度，右矩阵B的高度。 |
| unit_flag | `unit_flag_mode` | `unit_flag_mode::disable` | 控制`mmad`和后续矩阵数据搬出的细粒度并行。可选值为`unit_flag_mode::disable`、`unit_flag_mode::enable_keep`和`unit_flag_mode::enable_update`。 |
| init_with_zero | `bool` | `false` | 不传bias时，控制是否初始化结果矩阵C。`true`表示C矩阵默认初始化为0，`false`表示C矩阵不进行默认操作，通过设置`init_with_btbuf`参数进行初始化。 |

```cpp
enum class mmad_type : uint8_t { normal = 0, mx = 1 };

struct mmad_trait {
    __aicore__ constexpr mmad_trait(){};

    __aicore__ constexpr mmad_trait(int32_t fm_offset, bool k_direction_align,
        bool init_with_btbuf, bool disable_gemv, asc::te::mmad_type mmad_type) :
        fm_offset(fm_offset), k_direction_align(k_direction_align), init_with_btbuf(init_with_btbuf),
        disable_gemv(disable_gemv), mmad_type(mmad_type) {};

    int32_t fm_offset = 0;
    bool k_direction_align = false;
    bool init_with_btbuf = false;
    bool disable_gemv = true;
    asc::te::mmad_type mmad_type = asc::te::mmad_type::normal;
};
```

**表5**  `mmad_trait`参数说明

| 参数名 | 类型 | 默认值 | 描述 |
| :--- | :--- | :--- | :--- |
| fm_offset | `int32_t` | `0` | 左矩阵offset，当前Tensor API实现中作为兼容参数保留。 |
| k_direction_align | `bool` | `false` | K方向对齐控制，当前Tensor API实现中作为兼容参数保留。 |
| init_with_btbuf | `bool` | `false` | 配置C矩阵初始值是否来源于BT Buffer，带bias调用时该配置无效。`false`表示C矩阵不进行初始化操作，`true`表示使用BT Buffer的数据对C矩阵进行初始化操作。|
| disable_gemv | `bool` | `true` | M=1场景下是否关闭GEMV模式。`false`表示开启GEMV，`true`表示关闭GEMV。 |
| mmad_type | `mmad_type` | `mmad_type::normal` | 矩阵计算类型。默认使用`mmad_type::normal`。使用`mmad_type::mx`表示MX场景mmad计算。 |

## 数据类型

支持如下左矩阵A、右矩阵B、结果矩阵C的数据类型组合：

**表6**  支持的数据类型组合

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

`mmad`无返回值。`make_mmad`返回`mmad_atom`对象。

## 约束说明

- `c`必须位于L0C Buffer，`a`必须位于L0A Buffer，`b`必须位于L0B Buffer。
- 结果矩阵C起始地址需要满足64Byte地址对齐要求。
- 左矩阵A和右矩阵B起始地址需要满足512Byte地址对齐要求。
- 上述地址对齐要求属于硬件访问约束，编译器和运行时不对所有场景单独检查，用户需要保证入参满足约束。
- `m`、`n`、`k`需要与实际参与`mmad`计算的数据尺寸一致。
- 开启`unit_flag`功能时，需要结果矩阵C搬出接口的`l0c_to_gm_params::unit_flag`或`l0c_to_ub_params::unit_flag`与`mmad`计算的`mmad_params::unit_flag`参数配合设置。
- 连续两次`mmad`沿K方向累加时，需要关注结果矩阵C的写读依赖。同步优化的阈值关系可参考[结果矩阵C初始化](#结果矩阵c初始化)中的图示。
- 当M、K、N不是16的倍数时，硬件仍以16×16分形块组织数据，尾块中的无效数据会占用分形块空间但不参与有效计算。有效数据与无效数据排布方式如下图所示：

**图3**  尾块有效数据排布

![尾块有效数据排布](../../../../figures/mmad_tail_valid_data.png)

## 关键特性说明

### 结果矩阵C初始化

不传bias时，`init_with_zero`控制是否初始化结果矩阵C。通常第一次K方向累加时设置为`true`，后续K分块累加设置为`false`。如果多K分块累加场景中首次`mmad`如果设为`false`，将导致C矩阵包含L0C Buffer残留数据，计算结果错误。

连续两次`mmad`沿K方向累加时，通常需要关注中间结果的写读依赖。同步优化的阈值关系可参考下图：

**图4**  PipeBarrier阈值关系

![PipeBarrier阈值关系](../../../../figures/mmad_pipebarrier_threshold.png)

### unit_flag

`unit_flag`用于控制`mmad`与后续`Fixpipe`的细粒度并行。启用时，`mmad_params::unit_flag`和结果搬出的`l0c_to_gm_params::unit_flag`或`l0c_to_ub_params::unit_flag`需要配合设置。

### GEMV模式

当M=1时，可通过`mmad_trait::disable_gemv`控制是否开启GEMV模式。默认值为`true`，表示关闭GEMV。

### MX mmad模式

MX mmad模式没有单独的函数接口，而是通过`mmad_trait::mmad_type`设置。执行MX mmad时，需要将`mmad_trait::mmad_type`设置为`mmad_type::mx`，并通过`make_mmad(mmad_operation{}, mmad_trait_mx{})`构造矩阵计算原子对象。

```cpp
struct mmad_trait_mx {
    using trait_type = mmad_trait;
    static constexpr const trait_type value = mmad_trait(0, false, false, true, mmad_type::mx);
};
```

MX mmad支持如下左矩阵A、右矩阵B、结果矩阵C的数据类型组合：

**表7**  MX模式支持的数据类型组合

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

MX mmad通常需要在调用`mmad`前完成左矩阵缩放数据ScaleA和右矩阵缩放数据ScaleB的搬运。ScaleA搬运可参考[L1到L0ScaleA数据搬运](../cube_compute_load/copy_l1_to_l0scalea.md)，ScaleB搬运可参考[L1到L0ScaleB数据搬运](../cube_compute_load/copy_l1_to_l0scaleb.md)。矩阵缩放数据不作为`mmad`函数参数传入，而是通过`copy`接口提前搬运到L0ScaleA Buffer和L0ScaleB Buffer。

## 相关接口

- [unit_flag](../mmad_compute_key_features/unit_flag.md)
- [GEMV](../mmad_compute_key_features/gemv.md)
- [HF32](../mmad_compute_key_features/hf32.md)
- [L1到L0ScaleA数据搬运](../cube_compute_load/copy_l1_to_l0scalea.md)
- [L1到L0ScaleB数据搬运](../cube_compute_load/copy_l1_to_l0scaleb.md)
- [L0C到GM数据搬运](../cube_compute_store/copy_l0c_to_gm.md)
- [L0C到UB数据搬运](../cube_compute_store/copy_l0c_to_ub.md)

## 调用示例

- mmad调用示例

    ```cpp
    #include "tensor_api/tensor.h"

    using namespace asc::te;

    __aicore__ inline void normal_mmad_example()
    {

        constexpr uint16_t m = 16;
        constexpr uint16_t n = 16;
        constexpr uint16_t k = 16;

        __ca__ half l0a_buf[m * k];
        __cb__ half l0b_buf[k * n];
        __cc__ float l0c_buf[m * n];

        auto l0a = make_tensor(make_mem_ptr(l0a_buf), make_frame_layout<nz_layout_ptn, half>(m, k));
        auto l0b = make_tensor(make_mem_ptr(l0b_buf), make_frame_layout<zn_layout_ptn, half>(k, n));
        auto l0c = make_tensor(make_mem_ptr(l0c_buf), make_frame_layout<nz_layout_ptn, 16>(m, n));

        mmad_params params(m, n, k, unit_flag_mode::disable, true);
        auto atom = make_mmad(mmad_operation{}, mmad_trait_default{}).with(params);
        mmad(atom, l0c, l0a, l0b);
    }
    ```

- MX场景mmad调用示例

    ```cpp
    #include "tensor_api/tensor.h"

    using namespace asc::te;

    struct mmad_trait_mx {
        using trait_type = asc::te::mmad_trait;
        static constexpr const trait_type value = asc::te::mmad_trait(0, false, false, true, asc::te::mmad_type::mx);
    };

    __aicore__ inline void mx_mmad_example()
    {

        constexpr uint16_t m = 16;
        constexpr uint16_t n = 16;
        constexpr uint16_t k = 16;

        __ca__ fp8_e5m2_t l0a_buf[m * k];
        __cb__ fp8_e5m2_t l0b_buf[k * n];
        __cc__ float l0c_buf[m * n];

        auto l0a = make_tensor(make_mem_ptr(l0a_buf), make_frame_layout<nz_layout_ptn, fp8_e5m2_t>(m, k));
        auto l0b = make_tensor(make_mem_ptr(l0b_buf), make_frame_layout<zn_layout_ptn, fp8_e5m2_t>(k, n));
        auto l0c = make_tensor(make_mem_ptr(l0c_buf), make_frame_layout<nz_layout_ptn, 16>(m, n));

        mmad_params params(m, n, k, unit_flag_mode::disable, true);
        auto atom = make_mmad(mmad_operation{}, mmad_trait_mx{}).with(params);
        mmad(atom, l0c, l0a, l0b);
    }
    ```

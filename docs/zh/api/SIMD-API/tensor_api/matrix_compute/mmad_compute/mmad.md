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

`mmad`接口用于完成L0A Buffer上左矩阵A和L0B Buffer上右矩阵B的矩阵乘加，结果写入L0C Buffer上结果矩阵C。默认模式为普通矩阵计算。未传入bias时，数学表达式取决于`mmad_params::init_with_zero`,当`init_with_zero`为`true`时，首次计算前将C初始化为0，结果等价于`C = A * B`；当`init_with_zero`为`false`时，保留C的已有内容并执行累加：

```text
C = A * B + C
```

带bias输入时，bias作为结果矩阵C的初始值参与计算，等价于：

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

矩阵A、B、C分别按照`m`、`n`和`k`描述有效逻辑尺寸。硬件以16 * 16分形块组织矩阵数据，非16倍数的维度会占用完整分形的物理空间，尾块中的无效数据不参与计算。

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

**表2**  模板参数说明

| 参数名 | 描述 |
| :--- | :--- |
| Atom | `mmad_atom`的模板参数，由`make_mmad`接口推导得到。 |
| CTensor | 结果矩阵C的Tensor类型。 |
| ATensor | 左矩阵A的Tensor类型。 |
| BTensor | 右矩阵B的Tensor类型。 |
| BiasTensor | bias张量类型。 |
| MmadOperation | 矩阵计算操作对象类型。 |
| MmadTrait | 矩阵计算Trait对象类型。 |

**表3**  `mmad`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| atom | 输入 | 矩阵计算原子对象，可由`make_mmad`接口构造。 |
| c | 输出 | 结果矩阵C，存储位置为`location::l0c`，数据格式为NZ。 |
| a | 输入 | 左矩阵A，存储位置为`location::l0a`，数据格式为NZ。 |
| b | 输入 | 右矩阵B，存储位置为`location::l0b`，数据格式为ZN。 |
| bias | 输入 | bias张量，存储位置为`location::bias`，数据格式为ND。 |

**表4**  `make_mmad`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| operation | 输入 | 矩阵计算操作对象，通过`mmad_operation{}`构造。 |
| trait | 输入 | 矩阵计算Trait对象，用于指定矩阵计算静态特性。默认使用`mmad_trait_default{}`。 |

### Operation说明

`mmad_operation`用于标识矩阵计算通路。其public成员函数`mmad`接收Trait和矩阵计算参数，并将调用分发到普通MMAD或MX MMAD实现。

```cpp
struct mmad_operation {
    template <typename Trait, const Trait& trait, typename... Args>
    __aicore__ inline static void mmad(const Args&... args);
};
```

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

**表5**  `mmad_params`参数说明

| 参数名 | 类型 | 默认值 | 描述 |
| :--- | :--- | :--- | :--- |
| m | `uint16_t` | `0` | 左矩阵A的高度，结果矩阵C的高度，取值范围为[0, 4095]。为0时接口不执行。 |
| n | `uint16_t` | `0` | 右矩阵B的宽度，结果矩阵C的宽度，取值范围为[0, 4095]。为0时接口不执行。 |
| k | `uint16_t` | `0` | 左矩阵A的宽度，右矩阵B的高度，取值范围为[0, 4095]。为0时接口不执行。 |
| unit_flag | `unit_flag_mode` | `unit_flag_mode::disable` | 控制`mmad`和后续矩阵数据搬出的细粒度并行。可选值为`unit_flag_mode::disable`、`unit_flag_mode::enable_keep`和`unit_flag_mode::enable_update`。 |
| init_with_zero | `bool` | `false` | 不传bias时，控制是否初始化结果矩阵C。`true`表示C矩阵初始化为0，结果为`C = A * B`；`false`表示保留C已有内容，结果为`C = A * B + C`，也可以通过Trait配置从BT Buffer初始化。带bias时，该参数不生效。 |

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

**表6**  `mmad_trait`参数说明

| 参数名 | 类型 | 默认值 | 描述 |
| :--- | :--- | :--- | :--- |
| fm_offset | `int32_t` | `0` | 左矩阵offset。当前Tensor API实现中作为兼容参数保留，不参与计算。 |
| k_direction_align | `bool` | `false` | float数据类型的K方向对齐控制。`false`时按8对齐，`true`时按16对齐；当前产品的具体支持范围以Basic API约束为准。 |
| init_with_btbuf | `bool` | `false` | 配置C矩阵初始值是否来源于BT Buffer。`false`表示不从BT Buffer初始化，`true`表示使用BT Buffer数据初始化C。带bias调用时，该配置由bias所在位置决定，不生效。 |
| disable_gemv | `bool` | `true` | M=1场景下是否关闭GEMV模式。`false`表示开启GEMV，`true`表示关闭GEMV。该参数仅在M=1时有意义。 |
| mmad_type | `mmad_type` | `mmad_type::normal` | 矩阵计算类型。默认使用`mmad_type::normal`。使用`mmad_type::mx`表示MX场景mmad计算。 |

## 数据类型

支持如下左矩阵A、右矩阵B、结果矩阵C的数据类型组合：

**表7**  支持的数据类型组合

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

- `mmad`无返回值。
- `make_mmad`返回`mmad_atom`对象。

## 约束说明

- `c`必须位于L0C Buffer，`a`必须位于L0A Buffer，`b`必须位于L0B Buffer。
- 结果矩阵C起始地址需要满足1024Byte地址对齐要求。
- 左矩阵A和右矩阵B起始地址需要满足512Byte地址对齐要求。
- 上述地址对齐要求属于硬件访问约束，编译器和运行时不对所有场景单独检查，用户需要保证入参满足约束。
- `m`、`n`、`k`的取值范围为[0, 4095]，需要与实际参与`mmad`计算的数据尺寸一致；任意一个参数为0时，接口视为NOP。
- 开启`unit_flag`功能时，需要结果矩阵C搬出接口的`l0c_to_gm_params::unit_flag`或`l0c_to_ub_params::unit_flag`与`mmad`计算的`mmad_params::unit_flag`参数配合设置。
- 连续两次`mmad`沿K方向累加时，需要关注结果矩阵C的写读依赖。同步优化的阈值关系可参考[结果矩阵C初始化](#结果矩阵c初始化)中的图示。
- 当M、K、N不是16的倍数时，硬件仍以16×16分形块组织数据，尾块中的无效数据会占用分形块空间但不参与有效计算。有效数据与无效数据排布方式如下图所示：

**图3**  尾块有效数据排布

![尾块有效数据排布](../../../../figures/mmad_tail_valid_data.png)

## 关键特性说明

### 结果矩阵C初始化

不传bias时，`init_with_zero`控制是否初始化结果矩阵C。通常第一次K方向累加时设置为`true`，后续K分块累加设置为`false`。如果多K分块累加场景中首次`mmad`如果设为`false`，将导致C矩阵包含L0C Buffer残留数据，计算结果错误。

### 同步优化说明

当矩阵计算沿K轴进行累加时，中间结果通常需要写入同一块L0C Buffer。理论上，在连续两次mmad接口调用之间，应插入asc_sync_pipe\(PIPE\_M\)，以确保前一次mmad的写操作已完成，再进行下一轮mmad的读操作，从而保证数据一致性。

然而，当矩阵维度m和n超过一定阈值时，由于计算量足够大，数据访问模式具备良好的局部性和流水线并行潜力，硬件调度器能够自动处理写-读依赖问题。此时无需插入asc_sync_pipe\(PIPE\_M\)，mmad指令可并行执行，从而显著提升计算性能。

阈值公式：
    
$$
(m / 16) * (n / 16) < 10
$$
当计算量小于阈值时，需要插入asc_sync_pipe\(PIPE\_M\)。

```cpp
if ((m / 16) * (n / 16) < 10) {
    asc_sync_pipe(PIPE_M);
}
```

当计算量大于阈值时，不需要插入asc_sync_pipe\(PIPE\_M\)。

### UnitFlag

`unit_flag`用于控制`mmad`与后续矩阵数据搬出的细粒度并行：

- `unit_flag_mode::disable`：不启用unit flag；
- `unit_flag_mode::enable_keep`：执行完成后保持单元标志状态，通常用于K方向累加的前N-1次计算；
- `unit_flag_mode::enable_update`：执行完成后更新单元标志状态，通常用于K方向累加的最后一次计算。

启用时，`mmad_params::unit_flag`和结果搬出的`l0c_to_gm_params::unit_flag`或`l0c_to_ub_params::unit_flag`需要配合设置。具体读写顺序和多次搬出场景请参考[UnitFlag特性说明](../../../basic_api/cube_compute_ISASI/mmad_compute_key_features/UnitFlag.md)。

### GEMV模式

当M=1时，可通过`mmad_trait::disable_gemv`控制是否开启GEMV模式。默认值为`true`，表示关闭GEMV。开启GEMV时，A矩阵按ND格式读取，不再按普通NZ格式读取；A矩阵起始地址仍需满足512Byte对齐要求。

### MmadMx模式

MmadMx模式没有单独的函数接口，而是通过`mmad_trait::mmad_type`设置。执行MmadMx时，需要将`mmad_trait::mmad_type`设置为`mmad_type::mx`，并通过`make_mmad(mmad_operation{}, mmad_trait_mx{})`构造矩阵计算原子对象。调用`mmad`前，需要先通过L1到L0ScaleA和L1到L0ScaleB的`copy`接口完成ScaleA、ScaleB搬运；Scale数据不作为`mmad`函数参数传入，而是由硬件从对应的L0Scale Buffer隐式读取。

```cpp
struct mmad_trait_mx {
    using trait_type = mmad_trait;
    static constexpr const trait_type value = mmad_trait(0, false, false, true, mmad_type::mx);
};
```

MmadMx支持如下左矩阵A、右矩阵B、结果矩阵C的数据类型组合：

**表8**  MmadMx模式支持的数据类型组合

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

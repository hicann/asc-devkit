# mmad

<!-- md-trans-meta sourceCommit=8d12fdc1392b422cc5d95088e74075f8f24a6135 translatedAt=2026-11-19T02:33:36.406Z pushedAt=2026-11-19T05:54:06.642Z nl=2 -->

## Supported Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Function Description

The header file path is `tensor_api/tensor.h`.

The `mmad` API performs cube multiply-accumulate between left cube A in the L0A Buffer and right cube B in the L0B Buffer, and writes the result to result cube C in the L0C Buffer. The default mode is normal cube computation. When no bias is passed in, the mathematical expression depends on `mmad_params::init_with_zero`. When `init_with_zero` is `true`, C is initialized to 0 before the first computation, and the result is equivalent to `C = A * B`; when `init_with_zero` is `false`, the existing content of C is retained and accumulation is performed:

```text
C = A * B + C
```

When a bias input is provided, the bias serves as the initial value of result cube C and participates in the computation, which is equivalent to:

```text
C = A * B + Bias
```

The cube multiply-accumulate relationship of mmad is shown in the following figure:

**Figure 1**  mmad cube multiply-accumulate formula

![mmad cube multiply-accumulate formula](../../../../figures/mmad_formula.png)

`mmad` uses the explicitly passed `mmad_atom` to configure the cube computation. The illustrations of left cube A, right cube B, and result cube C are as follows:

**Figure 2**  mmad cube layout

![mmad cube layout](../../../../figures/mmad_matrix_layout.png)

## Cube Computation Description

**Table 1** Cube parameter description

| Cube | Physical Location | Dimension | Data Format | Data Type |
| :--- | :--- | :--- | :--- | :--- |
| Left cube A | L0A Buffer | m × k | NZ | See [data type](#data-type). |
| Right cube B | L0B Buffer | k × n | ZN | See [data type](#data-type). |
| Result cube C | L0C Buffer | m × n | NZ | See [data type](#data-type). |

Cubes A, B, and C describe their effective logical sizes by `m`, `n`, and `k`, respectively. The hardware organizes cube data in 16 * 16 fractal blocks. Dimensions that are not multiples of 16 occupy the physical space of a complete fractal, and invalid data in the tail block does not participate in the computation.

## Prototype

- Performs cube computation without passing a bias.

    ```cpp
    template <typename Atom, typename CTensor, typename ATensor, typename BTensor>
    __aicore__ inline void mmad(
        const mmad_atom<Atom>& atom, const CTensor& c, const ATensor& a, const BTensor& b)
    ```

- Performs cube computation with a bias passed in.

    ```cpp
    template <typename Atom, typename CTensor, typename ATensor, typename BTensor, typename BiasTensor>
    __aicore__ inline void mmad(
        const mmad_atom<Atom>& atom, const CTensor& c, const ATensor& a, const BTensor& b,
        const BiasTensor& bias)
    ```

- Constructs a default cube computation atomic object.

    ```cpp
    template <typename MmadOperation>
    __aicore__ inline constexpr auto make_mmad(const MmadOperation& operation)
    ```

- Constructs a cube computation atomic object with a specified Trait.

    ```cpp
    template <typename MmadOperation, typename MmadTrait>
    __aicore__ inline constexpr auto make_mmad(
        const MmadOperation& operation, const MmadTrait& trait)
    ```

## Parameters

**Table 2** Template parameter description

| Parameter | Description |
| :--- | :--- |
| Atom | Template parameter of `mmad_atom`, deduced by the `make_mmad` API. |
| CTensor | Tensor type of result cube C. |
| ATensor | Tensor type of left cube A. |
| BTensor | Tensor type of right cube B. |
| BiasTensor | Bias tensor type. |
| MmadOperation | Cube computation operation object type. |
| MmadTrait | Cube computation Trait object type. |

**Table 3** `mmad` API parameter description

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| atom | Input | Cube computation atomic object, which can be constructed by the `make_mmad` API. |
| c | Output | Result cube C, with the storage location of `location::l0c` and the data format of NZ. |
| a | Input | Left cube A, with the storage location of `location::l0a` and the data format of NZ. |
| b | Input | Right cube B, with the storage location of `location::l0b` and the data format of ZN. |
| bias | Input | Bias tensor, with the storage location of `location::bias` and the data format of ND. |

**Table 4** `make_mmad` API parameter description

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| operation | Input | Cube computation operation object, constructed by `mmad_operation{}`. |
| trait | Input | Cube computation trait object, used to specify the static characteristics of cube computation. `mmad_trait_default{}` is used by default. |

### Operation Description

`mmad_operation` identifies the cube computation path. Its public member function `mmad` receives the Trait and cube computation parameters, and dispatches the call to the normal MMAD or MX MMAD implementation.

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

**Table 5** `mmad_params` parameter description

| Parameter | Type | Default Value | Description |
| :--- | :--- | :--- | :--- |
| m | `uint16_t` | `0` | Height of left cube A and height of result cube C, with a value range of [0, 4095]. When it is 0, the API does not execute. |
| n | `uint16_t` | `0` | Width of right cube B and width of result cube C, with a value range of [0, 4095]. When it is 0, the API does not execute. |
| k | `uint16_t` | `0` | Width of left cube A and height of right cube B, with a value range of [0, 4095]. When it is 0, the API does not execute. |
| unit_flag | `unit_flag_mode` | `unit_flag_mode::disable` | Controls the fine-grained parallelism between `mmad` and the subsequent cube data move-out. Optional values are `unit_flag_mode::disable`, `unit_flag_mode::enable_keep`, and `unit_flag_mode::enable_update`. |
| init_with_zero | `bool` | `false` | When bias is not passed, controls whether to initialize result cube C. `true` means that cube C is initialized to 0, and the result is `C = A * B`; `false` means that the existing content of C is retained, and the result is `C = A * B + C`, which can also be initialized from the BT Buffer through Trait configuration. When bias is passed, this parameter does not take effect. |

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

**Table 6** `mmad_trait` parameter description

| Parameter | Type | Default Value | Description |
| :--- | :--- | :--- | :--- |
| fm_offset | `int32_t` | `0` | Offset of the left cube. Retained as a compatibility parameter in the current Tensor API implementation and does not participate in computation. |
| k_direction_align | `bool` | `false` | K-direction alignment control for the float data type. When `false`, alignment is performed by 8; when `true`, alignment is performed by 16. The specific supported range of the current product is subject to the Basic API constraints. |
| init_with_btbuf | `bool` | `false` | Whether the initial value of cube C comes from the BT Buffer. `false` means that C is not initialized from the BT Buffer, and `true` means that C is initialized with BT Buffer data. When called with bias, this configuration is determined by the location of bias and does not take effect. |
| disable_gemv | `bool` | `true` | Whether to disable GEMV mode when M=1. `false` means that GEMV is enabled, and `true` means that GEMV is disabled. This parameter is meaningful only when M=1. |
| mmad_type | `mmad_type` | `mmad_type::normal` | Cube computation type. `mmad_type::normal` is used by default. `mmad_type::mx` indicates mmad computation in the MX scenario. |

## Data Type

The following data type combinations are supported for left cube A, right cube B, and result cube C:

**Table 7**  Supported data type combinations

| Left cube A | Right cube B | Result cube C |
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

In scenarios where bias is passed in, the `int8_t * int8_t -> int32_t` combination requires the bias data type to be `int32_t`, while the remaining combinations require the bias data type to be `float`.

## Return Value

- `mmad` has no return value.
- `make_mmad` returns an `mmad_atom` object.

## Constraints

- `c` must be located in the L0C Buffer, `a` must be located in the L0A Buffer, and `b` must be located in the L0B Buffer.
- The start address of result cube C must meet the 1024-byte address alignment requirement.
- The start addresses of left cube A and right cube B must meet the 512-byte address alignment requirement.
- The preceding address alignment requirement is a hardware access constraint. The compiler and runtime do not check all scenarios individually, so users must ensure that the input parameters satisfy the constraint.
- The value range of `m`, `n`, and `k` is [0, 4095], which must be consistent with the data size actually involved in the `mmad` computation. If any parameter is 0, the API is treated as a NOP.
- When the `unit_flag` feature is enabled, the `l0c_to_gm_params::unit_flag` or `l0c_to_ub_params::unit_flag` of the result cube C move-out API must be set in coordination with the `mmad_params::unit_flag` parameter of the `mmad` computation.
- When two consecutive `mmad` operations accumulate along the K direction, pay attention to the write-read dependency of result cube C. For the threshold relationship of synchronization optimization, see the figure in [result cube C initialization](#result-cube-c-initialization).
- When M, K, and N are not multiples of 16, the hardware still organizes data in 16×16 fractal blocks. Invalid data in the tail block occupies fractal block space but does not participate in valid computation. The layout of valid and invalid data is shown in the following figure:

**Figure 3**  Layout of valid data in the tail block

![Layout of valid data in the tail block](../../../../figures/mmad_tail_valid_data.png)

## Key Features

### Result Cube C Initialization

When no bias is passed, `init_with_zero` controls whether result cube C is initialized. It is usually set to `true` for the first K-directional accumulation and to `false` for subsequent K-block accumulations. In a multi-K-block accumulation scenario, if the first `mmad` is set to `false`, cube C will contain residual data from the L0C Buffer, leading to incorrect computation results.

### Synchronization Optimization

During cube computation, when accumulation is performed along the K axis, intermediate results usually need to be written to the same L0C Buffer. In theory, between two consecutive mmad API calls, sync_pipe\(PIPE\_M\) should be inserted to ensure that the write operation of the previous mmad has completed before the read operation of the next mmad begins, thereby guaranteeing data consistency.

However, when the cube dimensions m and n exceed a certain threshold, the computation amount is large enough that the data access mode has good locality and pipeline parallel potential, and the hardware scheduler can automatically handle the write-read dependency. In this case, there is no need to insert sync_pipe\(PIPE\_M\), and the mmad instructions can be executed in parallel, significantly improving computation performance.

Threshold formula:
    
$$
(m / 16) * (n / 16) < 10
$$
When the computation amount is less than the threshold, sync_pipe\(PIPE\_M\) needs to be inserted.

```cpp
if ((m / 16) * (n / 16) < 10) {
    sync_pipe(pipe_t::PIPE_M);
}
```

When the computation amount is greater than the threshold, there is no need to insert sync_pipe\(PIPE\_M\).

### UnitFlag

`unit_flag` controls the fine-grained parallelism between `mmad` and the subsequent cube data move-out:

- `unit_flag_mode::disable`: Disables the unit flag.
- `unit_flag_mode::enable_keep`: Keeps the unit flag state after execution completes, typically used for the first N-1 computations of K-direction accumulation.
- `unit_flag_mode::enable_update`: Updates the unit flag state after execution completes, typically used for the last computation of K-direction accumulation.

When enabled, `mmad_params::unit_flag` must be set in coordination with `l0c_to_gm_params::unit_flag` or `l0c_to_ub_params::unit_flag` of the result move-out. For the specific read/write order and multiple move-out scenarios, see [UnitFlag feature description](../../../basic_api/cube_compute_ISASI/mmad_compute_key_features/UnitFlag.md).

### GEMV mode

When M=1, `mmad_trait::disable_gemv` controls whether to enable GEMV mode. The default value is `true`, which means GEMV is disabled. When GEMV is enabled, cube A is read in ND format instead of the normal NZ format; the start address of cube A must still meet the 512-byte alignment requirement.

### MmadMx Mode

The MmadMx mode has no separate function API; instead, it is configured through `mmad_trait::mmad_type`. To execute MmadMx, set `mmad_trait::mmad_type` to `mmad_type::mx` and construct the cube computation atomic object through `make_mmad(mmad_operation{}, mmad_trait_mx{})`. Before calling `mmad`, the ScaleA and ScaleB data must first be moved through the `copy` APIs from L1 to L0ScaleA and from L1 to L0ScaleB. The Scale data is not passed as a parameter of the `mmad` function; instead, it is implicitly read by the hardware from the corresponding L0Scale Buffer.

```cpp
struct mmad_trait_mx {
    using trait_type = mmad_trait;
    static constexpr const trait_type value = mmad_trait(0, false, false, true, mmad_type::mx);
};
```

MmadMx supports the following data type combinations for left cube A, right cube B, and result cube C:

**Table 8** Data type combinations supported in MmadMx mode

| Left cube A | Right cube B | Result cube C |
| :--- | :--- | :--- |
| `fp4x2_e2m1_t` | `fp4x2_e2m1_t` | `float` |
| `fp4x2_e2m1_t` | `fp4x2_e1m2_t` | `float` |
| `fp4x2_e1m2_t` | `fp4x2_e2m1_t` | `float` |
| `fp4x2_e1m2_t` | `fp4x2_e1m2_t` | `float` |
| `fp8_e4m3fn_t` | `fp8_e4m3fn_t` | `float` |
| `fp8_e4m3fn_t` | `fp8_e5m2_t` | `float` |
| `fp8_e5m2_t` | `fp8_e4m3fn_t` | `float` |
| `fp8_e5m2_t` | `fp8_e5m2_t` | `float` |

## Example

- mmad example

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

- mmad example in the MX scenario

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

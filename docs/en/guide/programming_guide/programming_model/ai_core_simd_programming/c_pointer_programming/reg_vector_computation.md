# Reg Vector Computation Programming<a name="ZH-CN_TOPIC_0000002600000001"></a>

<!-- md-trans-meta sourceCommit=4818c840484faa8ff60b3b8c4e28054f3c9b01aa translatedAt=2026-11-09T11:06:06.832Z pushedAt=2026-11-19T06:32:19.965Z nl=2 -->

> 📌 **Note**: Reg vector programming is supported only when performing SIMD vector computation programming on [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md).

This document describes how to use the language extension C APIs in Ascend C for register-level **vector computation programming**. This capability is intended for developers who are already familiar with the [Ascend C programming model](../../programming_model_overview.md). This hardware-register-based programming approach is called Reg vector computation programming.

## Why Is Reg Vector Programming Required<a name="section_part1"></a>

### Bottlenecks of Memory Vector Computation

In previous SIMD vector programming, both the source operands and the destination operand of a vector computation unit were stored directly in the Unified Buffer (UB). The execution of each vector computation involves "reading from UB → computing in the execution unit → writing back to UB". When an operator consists of multiple vector computations connected in series (for example, `dst = (a + b) * c + d`), the intermediate result of each step must first be written back to UB and then read out again by the next instruction.

This mode has three typical bottlenecks:

- **UB read/write bandwidth is repeatedly occupied**: Intermediate results travel back and forth on UB, and the memory access pressure scales linearly as vector computations increase.
- **Severe UB Bank conflicts**: Repeated reads and writes to UB greatly increase the probability of UB Bank conflicts.
- **The critical path is lengthened by memory access**: When the execution time of a computation instruction is very short, the overall time is dominated by the UB read/write time.

The overhead of this "landing every step in UB" approach becomes the ceiling for optimization.

### Positioning of Reg Vector Computation

To break through the bottleneck of the Memory vector programming described above, [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md) adds a new level of cache in hardware, **SIMD Register File** (hereinafter referred to as registers); Reg vector programming is the programming approach targeting this level of registers. Its core idea is to keep a continuous segment of vector computation within the registers, interacting with UB only once when entering and exiting this segment of computation, so that intermediate results no longer need to be written back to UB.

The comparison between Reg vector computation and Memory vector computation is as follows:

| Dimension | Memory Vector Computation | Reg Vector Computation |
| --- | --- | --- |
| Data location | UB | SIMD Registers |
| Intermediate result | Must be written back to UB | Can be continuously consumed in registers |
| Usage cost | Lower | Higher (requires understanding of registers and related operations) |
| Applicable target | Fast implementation without pursuing extreme performance | Pursuing extreme performance while accepting higher code complexity |

**On hardware that opens register programming, the Memory vector computation API is also provided to maintain compatibility with previous-generation hardware. The Memory vector computation API internally implements software-encapsulated Reg vector computation.**

### Applicable Scenarios

Consider using Reg vector programming when all of the following conditions are met:

1. The target device supports Reg vector programming.
2. The core time consumption is concentrated in continuous vector computation.
3. Intermediate results are repeatedly read from and written to UB.
4. You pursue extreme performance gains and can accept higher code complexity.

## Hardware Principles<a name="section_part2"></a>

### SIMD Register File and Execution Units

AIV is the core used for vector computation in the AI Core. The hardware units involved in Reg vector computation within the core include:

- **Reg vector execution unit**: used to execute Reg vector computation. It reads data from registers, completes the computation, and writes the result back to registers.
- **DMA unit**: used to execute Reg vector movement. It is responsible for moving data between registers and UB.
- **Aux Scalar**: handles the scalar computation (for example, address compute) required by the Reg vector execution unit and the Reg movement unit.

Although the Reg vector execution unit, DMA unit, and Aux Scalar belong to different hardware execution units, they are all assigned to the PIPE_V pipeline during actual execution. This architecture brings two direct results:

1. When there are register dependencies, the hardware ensures data dependency correctness in instruction order, and users do not need to explicitly insert synchronization. However, explicit synchronization must be inserted for reads and writes to the same UB region across registers, because there is no automatic ordering constraint between the movement unit and the computation unit. For details, see [Pipeline Synchronization](#section_sync).
2. When there is no data dependency, the Reg vector execution unit and the DMA unit can **issue simultaneously and execute in parallel**.

**Figure 1** SIMD Reg vector execution relationship

<img src="../../../../figures/reg_exec.png" title="Reg execution unit" style="zoom:80%;"/>

### Memory Hierarchy

The memory hierarchy of the AIV is as follows:

**Figure 2** Memory hierarchy

![](../../../../figures/reg_mem.png "Reg memory hierarchy")

In the SIMD architecture described above, both the UB and the registers are storage spaces exclusively owned by each AIV core. Registers do not support directly loading data from the GM or directly writing data out to the GM. To read GM data into registers, the data must first be moved from the GM to the UB, and then loaded from the UB into the registers. To write register data out to the GM, the data must first be stored from the registers to the UB, and then moved from the UB to the GM.

**Registers used as a whole**: Registers cannot be accessed by index or offset to reach a specific element or bit. Based on their functions, they are mainly classified into five types:

1. **Vector computation register**: the main register that participates in vector computation. The width of a single register is defined as the Vector Length (hereinafter referred to as VL). It can exchange data with the UB through the DMA unit.
2. **Mask register**: used to control the valid elements participating in vector computation or to represent single-bit elements participating in computation. The width of a single register is defined as VL/8. It can exchange data with the UB through the DMA unit.
3. **Address register**: assists the Reg vector movement unit in completing data exchange between vector computation registers and the UB. It stores the offset of a UB address. The width of a single register is 32 bits. It cannot directly exchange data with the UB.
4. **Load into unaligned register**: assists the Reg vector movement unit in loading UB data into vector computation registers. It temporarily stores one DataBlock of data at the aligned UB start address that contains the source unaligned UB address data. The width of a single register is DataBlock. UB data can be loaded into the unaligned register through the DMA unit.
5. **Store from unaligned register**: assists the Reg vector movement unit in storing vector computation register data out to the UB. It temporarily stores the data of the corresponding vector register that contains the unaligned UB address to be written. The width of a single register is DataBlock. The data of the store from unaligned register can be stored out to the UB through the DMA unit.



## Programming Model<a name="section_part3"></a>

### Overall Structure

The register-based (Regbase) programming model extends the `data load-in` -> `Compute` -> `data store-out` programming model of Memory vector programming into a Reg vector computation programming model of `data load-in` -> `Load` -> `Compute` -> `Store` -> `data store-out`.

**Figure 3** Overall structure of the Regbase programming model

![](../../../../figures/reg_model.png "Overall structure of the Regbase programming model")

### VF Functions and Execution Domains

In the Reg vector computation programming model, the execution process of `Load` -> `Compute` -> `Store` is defined in the implementation as a Vector Function (hereinafter referred to as a VF function). To distinguish different execution domains, functions in different execution domains use different function labels, and the API call relationships are restricted at compile time:

| Label | Role | Caller | Callable Object |
| --- | --- | --- | --- |
| `__aicore__` | Device-side ordinary function | Kernel or other `__aicore__` functions | `__aicore__` functions, or VF functions called through `asc_vf_call` |
| `__simd_vf__` | VF function | `__aicore__` functions called through `asc_vf_call` | Reg vector APIs modified by `__simd_callee__` or VF internal subfunctions |
| `__simd_callee__` | VF internal subfunction | `__simd_vf__` or other `__simd_callee__` | `__simd_callee__` |

> 📌 **Note**: As mentioned in the [Hardware Principles](#section_part2) section, the Vector computation unit contains an Aux Scalar unit. As seen in the aforementioned [Abstract Hardware Architecture](../abstract_hardware_architecture.md) document, a Scalar computation unit (hereinafter referred to as Main Scalar) also exists within the AI Core. These two Scalar units are independent of each other and differ in capabilities. On the programming interface, VF functions are used to isolate the execution domains of the two. Scalar computation inside a VF function is executed on the Aux Scalar unit; scalar computation outside a VF function is executed on the Main Scalar unit. Similarly, scalar computation in subfunctions called inside a VF function must satisfy the capabilities of Aux Scalar; scalar computation in subfunctions called outside a VF function must conform to the capabilities of Main Scalar.
>
> To distinguish the different functional execution domains of the three types of functions above, VF functions are identified by `__simd_vf__`, subfunctions inside VF functions are identified by `__simd_callee__`, and subfunctions outside VF functions are identified by `__aicore__`. At compile time, execution domain verification can be performed based on the different function identifiers.
>
> All Reg vector APIs are identified by `__simd_callee__` and can only be called inside VF functions.

This execution domain isolation brings three benefits:

- **Compile-time verification**: Cross-domain erroneous calls (for example, directly calling a Reg vector API in `__aicore__`) are detected at the compilation stage.
- **Clear responsibilities**: `__aicore__` is responsible for outer logic such as GM data movement and address computation; `__simd_vf__` focuses on register-level computation.
- **Explicit optimization boundaries**: The compiler can perform independent optimization on VF functions (such as VF fusion) without affecting the outer operator structure.

**Figure 4** Function label call relationships

<img src="../../../../figures/reg_calculation_call_hierarchy.png" title="Call hierarchy" style="zoom: 28%;"/>

A complete implementation consists of two parts: the outer `__aicore__` logic and the inner `__simd_vf__` logic:

- **Outer `__aicore__` code**: completes preparatory work such as GM data interaction outside the core, and calls the VF function through `asc_vf_call`.
- **Inner `__simd_vf__` code**: completes the movement between UB and registers as well as vector computation on registers.



A typical skeleton for calling a VF function is as follows:

```cpp
__aicore__ inline void compute()
{
    // 1. Prepare input/output data.
    // ...

    // 2. Enter register-level VF function domain via asc_vf_call.
    asc_vf_call<add_vf>(dst_addr, src0_addr, src1_addr,
                        one_repeat_size, repeat_times);

    // 3. Finalize work.
    // ...
}
```

The inside of a VF function usually follows the following skeleton:

```text
for (i = 0; i < repeat_times; ++i) {
    1. Compute the UB address offset of the current round.
    2. Load data from UB to registers.
    3. Perform computation on registers.
    4. Store the result back to UB.
}
```

### Register Types and Allocation

Registers are classified into the following five types based on their functions. In actual programming code, registers are used directly as variables and are automatically allocated to hardware registers by the compiler.

| Register Type | Variable Type Definition | [NPU Architecture Version 3510](../../../language_extension/simd_builtin_keywords.md) |
| --- | --- | --- |
| Vector data register | `vector_*`, for example, vector_float | VL = 256B |
| Mask register | `vector_bool` | VL/8 = 32B |
| Address register | `addr_reg` | 32bit = 4B |
| Unaligned Transfer In Register | `vector_load_unalign` | DataBlock = 32B |
| Unaligned Transfer Out Register | `vector_store_unalign` | DataBlock = 32B |

#### Vector Data Register

In [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md), the width of a vector data register is VL = 256B, and the number of elements is interpreted based on the data type.


| Bit Width | Vector Data Type | Element Count |
| --------- | --------- | --------- |
| b8        | vector_hifloat8_t/vector_fp8_e4m3fn_t/vector_fp8_e5m2_t/vector_fp8_e8m0_t/vector_fp4x2_e1m2_t | 256 |
| b8 | vector_int8_t/vector_uint8_t/vector_int4x2_t/vector_fp4x2_e2m1_t | 256 |
| b16       | vector_half/vector_bfloat16_t/vector_int16_t/vector_uint16_t | 128 |
| b32       | vector_float/vector_int32_t/vector_uint32_t | 64 |
| b64       | vector_int64_t/vector_uint64_t | 32 |

Vector data registers are used in code as follows:


```cpp
// Allocate vector data register with float data type
vector_float vector_reg;

// Allocate vector data register with half data type
vector_half vector_reg;

// Load data from UB address src_addr to vector_float src0_reg
asc_load(src0_reg, src_addr);

// Compute addition with vector_float src0_reg, src1_reg, dst_reg
asc_add(dst_reg, src0_reg, src1_reg, mask_reg);
```



#### Mask Register

The width of `vector_bool` is `VL/8`. The minimum correspondence is that 1 bit of the mask corresponds to 1 byte of data. When the data bit width is greater than 1 byte, the LSB (least significant bit) in the N-bit position corresponding to each N-byte element (for example, 4 bytes for float) takes effect. The following table shows the relationship between the effective element index and the bits in `vector_bool` for different element widths of vector data. For example, for b32 data, bits 0/4/8/12/... of `vector_bool` take effect, corresponding to elements 0/1/2/3/... of b32 in the vector data register.

| RegTensor Single-Element Width | 0th Bit | 1st Bit | 2nd Bit | 3rd Bit | 4th Bit | 5th Bit | 6th Bit | 7th Bit | 8th Bit | 9th Bit | 10th Bit | 11th Bit | 12th Bit | ...  |
| ------------------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | -------- | -------- | -------- | ---- |
| b8                  | 0       | 1       | 2       | 3       | 4       | 5       | 6       | 7       | 8       | 9       | 10       | 11       | 12       | ...  |
| b16                 | 0       | -       | 1       | -       | 2       | -       | 3       | -       | 4       | -       | 5        | -        | 6        | ...  |
| b32                 | 0       | -       | -       | -       | 1       | -       | -       | -       | 2       | -       | -        | -        | 3        | ...  |
| b64                 | 0       | -       | -       | -       | -       | -       | -       | -       | -       | 1       | -        | -        | -        | ...  |



`vector_bool` can be used as the mask input of computation APIs, as a bit-level element to carry the results of comparison operations and participate in shift, logic, and interleave computations, and as the destination/source parameter of load-in/store-out APIs.

The creation and common usage of `vector_bool` are as follows:

```cpp
// Directly allocate mask register
vector_bool mask_reg;

// Create mask register with all elements valid
vector_bool mask_reg = asc_create_mask_b32(PAT_ALL);

// Create mask register based on remaining elements update
vector_bool mask = asc_update_mask_b32(remainCount);

// Use vector_bool mask_reg as mask to control valid elements in computation
asc_add(dst_reg, src0_reg, src1_reg, mask_reg);
```



When used as 1-bit elements in computation, mixed computation between vector_* and vector_bool, as well as computation between vector_bool and vector_bool, is supported.

```cpp
// Use vector_bool dst_mask_reg as x computation parameter with vector_* src0_vector_reg
asc_select(dst_mask_reg, src0_vector_reg, src1_vector_reg, mask_reg);

// Use vector_bool src0_mask_reg, src1_mask_reg, and dst_mask_reg as parameters for computation.
asc_and(dst_mask_reg, src0_mask_reg, src1_mask_reg, mask_reg);
```

#### Address Iteration Register

`addr_reg` is used to store the address offset for UB access. It converts the scalar address compute that originally needs to be repeatedly executed in the loop body into register-based address management, supporting up to four-dimensional offset accumulation. `AddrReg` is initialized through `asc_update_addr_reg_b*`, and the offset compute formula is: `offset = index0*stride0 + index1*stride1 + ...`.

```cpp
// Allocate address iteration register
addr_reg a_reg;
// Use as 2D offset accumulation for b32 elements
for(uint16_t i = 0;i < extent1; i++){
    for(uint16_t j = 0;j < extent2; j++){
        a_reg = asc_update_addr_reg_b32(i, stride0, j, stride1);
        asc_loadalign(src_reg, src_addr, a_reg);
    }
}
```

#### Unaligned Move-In Register

`vector_load_unalign` is used to implement the temporary cache for continuous data move-in from an unaligned UB address to a vector data register. First, initialize it through `asc_loadunalign_pre` to load the data containing the unaligned portion into `vector_load_unalign`, and then call `asc_loadunalign` to move the data. For the specific usage process, see the description of unaligned movement in the subsequent [Feature Description](#section_part4) section.

```
// Allocate load unalign register
vector_load_unalign ureg0;

// Initialize vector_load_unalign using the unaligned UB address src_addr
asc_loadunalign_pre(ureg0, src_addr);

// Load the data starting from the unaligned UB address src_addr into the vector data register src_reg
asc_loadunalign(src_reg, ureg0, src_addr);
```

#### Unaligned Move-Out Register

`vector_store_unalign` is used to implement the temporary cache for data store-out from a vector data register to an unaligned UB address. First, move the data through `asc_storeunalign`; the tail data remains in `vector_store_unalign`. Finally, call `asc_storeunalign_post` once to store the tail data in `vector_store_unalign` out to UB. For the specific usage process, see the description of unaligned movement in the subsequent [Feature Description](#section_part4) section.

```cpp
// Allocate store unalign register
vector_store_unalign ureg0;

// Store vreg0 out to the unaligned UB address dst_addr
asc_storeunalign(dst_addr, vreg0, ureg0, stride);

// Store tail data from store unalign register to UB
asc_storeunalign_post(dst_addr, ureg0, 0);
```

### Pipeline Synchronization<a name="section_sync"></a>

As mentioned above, the Reg vector execution unit and the DMA unit can work in parallel. The key to determining whether synchronization is required lies not in the code order, but in whether the necessary data dependencies have been established between hardware pipelines:

| Scenario | Synchronization Required | Reason |
| --- | --- | --- |
| Read-after-write within the same register | No | Register dependencies are guaranteed by hardware in instruction order |
| Different registers access the same UB address with read-after-write | Yes | There is no automatic ordering constraint between the movement and computation execution units |
| Different registers access the same UB address with write-after-write | Yes | The write order must be guaranteed |
| No address overlap and no register dependency | No | No data race exists |

Explicit synchronization is implemented through asc_mem_bar:

```cpp
// Insert source Store to destination Load synchronization when store and load UB addresses are the same
asc_store(ub_addr, vreg0);
asc_mem_bar(VST_LD);
asc_load(vreg1, ub_addr);
```

The synchronization direction includes multiple specified [enumeration values](../../../../../api/SIMD-API/c_api/reg_compute/reg_sync/asc_mem_bar.md), which are used to specify the two pipeline directions that need to be synchronized. As shown in the following figure, when **operating on the same UB address**, a read-write dependency requires inserting a `Load->Store` synchronization `asc_mem_bar(VLD_ST)`; a write-write dependency requires inserting a Store->Store synchronization `asc_mem_bar(VST_ST)`. Only when there is a register variable dependency computation between reading and writing the same UB address is the same data processed serially, and no synchronization needs to be inserted.

**Figure 5** Pipeline synchronization diagram

<img src="../../../../figures/sync_pipe_1.png" alt="Pipeline synchronization" width="920px" height="677px">

> 📌 **Note**: After `asc_vf_call` calls the VF function, the outer layer does not need to pay attention to the PIPE_V barrier; when the VF function exits, all internal Reg vector instructions have already completed execution.

### Reg Vector Computation APIs

The Reg vector computation C APIs (prefixed with `asc_` in the source code and belonging to the language extension C APIs) are a set of **language extension interfaces that pass through to hardware instructions**.

- Each C API basically corresponds to one hardware instruction at compile time.
- Data types are directly mapped to hardware register widths. For example, `vector_float` corresponds to a vector register of VL width.
- The interface form, parameter order, and behavior details are all organized as a language extension layer based on hardware capabilities.

The main naming conventions are as follows:

| Category | Typical API |
| -------- | ------------------------------------------------------------ |
| Data movement | `asc_load*` / `asc_store*` |
| Vector computation | `asc_add` / `asc_sub` / `asc_mul` / `asc_div` / `asc_select` / `asc_reduce_*`, etc. |
| Synchronization control | `asc_mem_bar` |

### Programming Example

The following is a VF function example called by compute, which implements `dst[i] = src0[i] + src1[i]`:

```cpp
// VF function: complete vector addition in register domain
__simd_vf__ inline void add_vf(__ubuf__ float* dst_addr,
                               __ubuf__ float* src0_addr,
                               __ubuf__ float* src1_addr,
                               uint32_t one_repeat_size,
                               uint16_t repeat_times)
{
    vector_float src0_reg;
    vector_float src1_reg;
    vector_float dst_reg;
    vector_bool  mask = asc_create_mask_b32(MASK_PATTERN_ALL);

    for (uint16_t i = 0; i < repeat_times; ++i) {
        uint32_t offset = i * one_repeat_size;
        asc_loadalign(src0_reg, src0_addr + offset);
        asc_loadalign(src1_reg, src1_addr + offset);
        asc_add(dst_reg, src0_reg, src1_reg, mask);
        asc_storealign(dst_addr + offset, dst_reg, mask);
    }
}

// Caller: prepare LocalTensor and UB address in __aicore__ domain
__aicore__ inline void compute(__ubuf__ float* dst_addr,
                               __ubuf__ float* src0_addr,
                               __ubuf__ float* src1_addr,
                               uint32_t count)
{
    constexpr uint32_t one_repeat_size = 64;            // VL / sizeof(float) = 256 / 4
    uint16_t repeat_times = count / one_repeat_size;

    asc_vf_call<add_vf>(dst_addr, src0_addr, src1_addr,
                        one_repeat_size, repeat_times);
}
```

## Features<a name="section_part4"></a>

The key features of Reg vector programming are divided into three categories by purpose:

| Category | Feature |
| --- | --- |
| Data movement | General-purpose movement, aligned movement, unaligned movement, PostUpdate, movement using address registers |
| Vector computation | Tail block processing, MaskMergeMode |
| VF function optimization | VF fusion, hardware loop, dual instruction issue |

### Data Movement

#### General Movement

[`asc_load`](../../../../../api/SIMD-API/c_api/reg_compute/load/asc_load.md) / [`asc_store`](../../../../../api/SIMD-API/c_api/reg_compute/store/asc_store.md) are general-purpose movement APIs that do not distinguish whether the UB address is aligned. They are used to move continuous VL data between UB and vector data registers, or to store out element data not exceeding one VL from a vector data register to UB.

```cpp
// Transfer 1 VL length data from UB address src_addr to vector data register src_reg
asc_load(src_reg, src_addr);

// Transfer 1 VL length data from vector data register to UB address dst_addr
asc_store(dst_addr, dst_reg);

// Transfer less than 1 VL length data (10 elements) from vector data register to UB address dst_addr
asc_store(dst_addr, dst_reg, 10);
```

General-purpose movement is suitable for code that needs to uniformly handle both aligned and unaligned scenarios. If the address is known to meet the 32B alignment requirement and higher performance is desired, the aligned movement APIs should be preferred.

> 📌 **Note**: General-purpose movement is a software-wrapped API, and its internal implementation relies on the related APIs and registers of unaligned movement.

#### Aligned Movement

[`asc_loadalign`](../../../../../api/SIMD-API/c_api/reg_compute/load/asc_loadalign.md) / [`asc_storealign`](../../../../../api/SIMD-API/c_api/reg_compute/store/asc_storealign.md) require the UB address to meet 32B alignment. Compared with general-purpose movement, aligned movement has lower performance overhead and is the preferred API in continuous aligned scenarios.

```cpp
for (uint16_t i = 0; i < repeat_times; ++i) {
    uint32_t offset = i * one_repeat_size;
    asc_loadalign(srcReg, src_addr + offset);
    // Computation on RegTensor
    ...
    asc_storealign(dst_addr + offset, dstReg, mask);
}
```

If the access pattern has a fixed stride or multi-dimensional indexing, the API forms with the `block_stride` and `repeat_stride` parameters can also be used.

#### Unaligned Movement

Unaligned movement refers to the scenario where the UB address is unaligned but data still needs to be read from and written to between UB and vector data registers continuously. When data is moved directly with an unaligned UB address, hardware performance drops sharply. To improve performance in this scenario, `vector_load_unalign` / `vector_store_unalign` are used as temporary buffers to save data that crosses alignment boundaries, and multiple loop iterations are used to splice the data into aligned-address reads and writes, reducing the number of direct accesses to unaligned UB addresses and thereby reducing the extra overhead of unaligned access.

**Unaligned move-in** is completed by combining [`asc_loadunalign_pre`](../../../../../api/SIMD-API/c_api/reg_compute/load/asc_loadunalign_pre.md) and [`asc_loadunalign`](../../../../../api/SIMD-API/c_api/reg_compute/load/asc_loadunalign.md):
For example, on [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md), when you want to load in 256 bytes of data starting at UB address 48 as follows:

**Figure 6** Expected unaligned move-in

<img src="../../../../figures/reg_read_1.png" alt="Reg unaligned read 1" width="300px" height="84px">

1. First initialize `vector_load_unalign` with the unaligned UB address. The UB address is truncated to the 32B-aligned address 32, and then one DataBlock of 32B data is loaded into `vector_load_unalign`.

   **Figure 7**  Initializing `vector_load_unalign`

   <img src="../../../../figures/reg_read_2.png" alt="Reg unaligned read 2" width="300px" height="58px">

2. Call `asc_loadunalign` to move data. After the original unaligned UB address is aligned upward to 32B, 240B of data is loaded in, which is concatenated with part of the data in `vector_load_unalign` to form the expected 256B read-in data, and then written to the vector data register. At the same time, the data in `vector_load_unalign` is updated to the DataBlock 32B corresponding to the unaligned UB address of the next read.

   **Figure 8**  Reading in the expected data

   <img src="../../../../figures/reg_read_4.png" alt="Reg unaligned read 4" width="600px" height="235px">

**Unaligned move-out** is completed by combining [`asc_storeunalign`](../../../../../api/SIMD-API/c_api/reg_compute/store/asc_storeunalign.md) and [`asc_storeunalign_post`](../../../../../api/SIMD-API/c_api/reg_compute/store/asc_storeunalign_post.md): `asc_storeunalign` is used for movement inside the loop, and after the loop ends, `asc_storeunalign_post` stores the tail data in `vector_store_unalign` out to the UB.
For example, on [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md), when you want to store out the following 512Bytes of data starting from UB address 48:

**Figure 9** Expected unaligned move-out

<img src="../../../../figures/reg_write_0.png" alt="Reg unaligned write 0" width="300px" height="84px">

1. First use `asc_storeunalign`. In the first loop, write the 240B vector data register data whose tail is aligned to 32B into UB, and write the remaining 16B into `vector_store_unalign`; 

   **Figure 10** Expected data written in the first pass

   <img src="../../../../figures/reg_write_1.png" alt="Reg unaligned write 1" width="300px" height="160px">

2. In the second loop, concatenate the 16B data temporarily stored in `vector_store_unalign` with the 240B in the vector data register to be written in this loop into 256B data, and then write it to UB. At the same time, update the remaining 16B of this loop into `vector_store_unalign`; 

   **Figure 11** Expected data written in the second pass

   <img src="../../../../figures/reg_write_2.png" alt="Reg unaligned write 2" width="600px" height="265px">

3. Call `asc_storeunalign_post` to flush the tail data that has not yet been written out from the `vector_store_unalign` buffer back to UB.

   **Figure 12** Remaining data written out

   <img src="../../../../figures/reg_write_3.png" alt="Reg unaligned write 3" width="600px" height="164px">

The recommended approach is to place initialization and post-processing outside the loop, keeping only continuous movement and computation inside the loop body:

```cpp
vector_load_unalign  load_ureg;
vector_store_unalign store_ureg;

asc_loadunalign_pre(load_ureg, src_addr);
for (uint16_t i = 0; i < repeat_times; ++i) {
    asc_loadunalign(src_reg, load_ureg, src_addr, stride);
    // Reg vector computation
    ...
    asc_storeunalign(dst_addr, dst_reg, store_ureg, stride);
}
asc_storeunalign_post(dst_addr, store_ureg, 0);
```

#### PostUpdate Mode

PostUpdate means that the movement API automatically updates the UB address pointer after completing the current access. Such movement APIs end with `*_postupdate`. For example, use [`asc_loadalign_postupdate`](../../../../../api/SIMD-API/c_api/reg_compute/load/asc_loadalign_postupdate.md) for aligned movement. When i = 0, data is loaded from the address src_addr, and after the movement the hardware updates src_addr = src_addr + offset. When i = 1, data is loaded from the updated src_addr, and after the movement the hardware updates src_addr = src_addr + stride again. This process repeats for subsequent iterations.

```cpp
for (uint16_t i = 0; i < repeat_times; ++i) {
    asc_loadalign_postupdate(srcReg, src_addr, stride);
}
```

The advantages of PostUpdate are as follows:

- It reduces the number of scalar instructions for explicitly computing `src_addr + i * stride` inside the loop.
- It binds the address update semantics to the movement action, making it easier for the compiler to identify consecutive access patterns.

#### Movement via Address Registers

When a loop contains a fixed stride or multi-dimensional index, you can use `addr_reg` to manage the address offset, allowing the hardware to automatically compute the offset and reduce scalar instruction consumption. For example, a two-level loop is as follows:

```cpp
addr_reg a_reg;
for (uint16_t i = 0; i < extent1; ++i) {
    for (uint16_t j = 0; j < extent2; ++j) {
        a_reg = asc_update_addr_reg_b32(i, stride0, j, stride1);
        asc_loadalign(src_reg, src_addr, a_reg);
    }
}
```

`addr_reg` supports up to four-dimensional addressing. The offset formula is `offset = index0*stride0 + index1*stride1 + index2*stride2 + index3*stride3`.

### Vector Computation

Reg vector computation APIs operate directly on vector data registers or mask registers, typically with a mask parameter to control the valid elements. They fall into three categories:

#### Vector Data Register Computation

Basic arithmetic APIs perform element-wise computation between registers:

```cpp
vector_float src0_reg;
vector_float src1_reg;
vector_float dst_reg;
vector_bool mask = asc_create_mask_b32(PAT_ALL);

// Vector addition
asc_add(dst_reg, src0_reg, src1_reg, mask);

// Vector multiplication
asc_mul(dst_reg, src0_reg, src1_reg, mask);

// Vector subtraction
asc_sub(dst_reg, src0_reg, src1_reg, mask);

// Vector division
asc_div(dst_reg, src0_reg, src1_reg, mask);
```

Scalar arithmetic APIs perform element-wise computation between a register and a scalar:

```cpp
float scalar_value = 2.0f;

// Vector plus scalar
asc_add_scalar(dst_reg, src0_reg, scalar_value, mask);

// Vector multiplied by scalar
asc_mul_scalar(dst_reg, src0_reg, scalar_value, mask);
```

#### Mask Register Computation

Logical APIs can operate on vector data registers or mask registers:

```cpp
vector_bool mask0;
vector_bool mask1;
vector_bool dst_mask;

// Logical AND
asc_and(dst_mask, mask0, mask1, mask);

// Logical OR
asc_or(dst_mask, mask0, mask1, mask);

// Logical NOT
asc_not(dst_mask, mask0, mask);
```

#### Comparison and Selection

Comparison APIs output the comparison result, and selection APIs select between two source operands based on the mask:

```cpp
vector_bool cmp_mask;

// Compare (output the comparison result mask)
asc_compare(cmp_mask, src0_reg, src1_reg, CMPMODE::EQ, mask);

// Select (select src0 or src1 based on cmp_mask)
asc_select(dst_reg, cmp_mask, src0_reg, src1_reg, SELMODE::SEL, mask);
```

These APIs often have more parameters or restrictions. It is recommended that you refer to the corresponding API page when using them.

#### Tail Block Processing

When the total amount of data to be computed is not an integer multiple of `VL`, the last iteration needs to process data that is less than one `VL`. Reg vector programming implements tail block processing through `asc_update_mask_b*`: each call generates the mask for the current round based on the remaining element count, and automatically subtracts the element count corresponding to one `VL` after generation. This approach reduces the operations of determining the tail block through `if` inside the loop, avoiding violation of the constraints of the hardware loop.

```cpp
__simd_vf__ inline void add_vf(__ubuf__ float* dst_addr,
                                  __ubuf__ float* src0_addr,
                                  __ubuf__ float* src1_addr,
                                  uint32_t count,
                                  uint32_t one_repeat_size,
                                  uint16_t repeat_times)
{
    vector_float src0_reg;
    vector_float src1_reg;
    vector_float dst_reg;
    uint32_t remainCount = count;

    for (uint16_t i = 0; i < repeat_times; ++i) {
        vector_bool mask = asc_update_mask_b32(remainCount);
        uint32_t offset = i * one_repeat_size;
        asc_loadalign(src0_reg, src0_addr + offset);
        asc_loadalign(src1_reg, src1_addr + offset);
        asc_add(dst_reg, src0_reg, src1_reg, mask);
        asc_storealign(dst_addr + offset, dst_reg, mask);
    }
}
```

### VF Function Optimization

#### Hardware Loop

The hardware loop is the foundation of VF loop performance optimization. If a loop in a VF satisfies the hardware constraints, the compiler can convert it into a hardware-level loop, reducing the counting, comparison, and jump overhead of a software loop. Loops that cannot be recognized as Hardware Loops degrade to Software Loops, which significantly reduces performance and affects subsequent optimizations such as VF fusion.

> Tip: The main coding specifications for Hardware Loop are as follows:
>
> - The loop iteration variable must be of the `uint16_t` type.
> - The loop starts from 0 with a step of 1.
> - The loop boundary is not modified during execution.
> - Avoid runtime jump control flow such as `if/else` in the loop body.
> - A VF supports up to four levels of nested loops.

For compile-time branches and tail-block scenarios, use the following approaches instead of `if`:

```cpp
// Compile-time branch: if constexpr has no runtime overhead
if constexpr (has_bias) {
    asc_add(dst_reg, src_reg, bias_reg, mask);
}

// Tail-block scenario: use for(1) instead of if
uint16_t tail = count % one_repeat_size;
uint16_t has_tail = !!tail;
for (uint16_t i = 0; i < has_tail; ++i) {
    // Tail-block processing content
}
```

Typical Hardware Loop usage:

```cpp
for (uint16_t i = 0; i < repeat_times; ++i) {
    asc_loadalign(src_reg, src_addr + i * one_repeat_size);
    asc_add(dst_reg, src_reg, bias_reg, mask);
    asc_storealign(dst_addr + i * one_repeat_size, dst_reg, mask);
}
```

#### VF Fusion

VF fusion is the most important optimization technique for Reg vector computation programming. It merges multiple computations and their loops into a single VF function. After fusion, the VF call and Hardware Loop startup overhead is reduced, redundant Load/Store and synchronization instructions between multiple VFs are eliminated, and intermediate results are kept in registers as much as possible for continuous consumption.

VF fusion can be divided into two levels:

- **VF shallow fusion**: merges the implementations of multiple VF functions into a single VF function, which eliminates the VF call overhead.
- **VF deep fusion**: on the basis of VF shallow fusion, merges multiple computation loops to reduce the Hardware Loop startup overhead, eliminate redundant Load/Store operations, and fully reuse registers.

To increase the opportunities for VF fusion, the following practices are recommended:

- Keep the same or equivalent loop structure across multiple VFs to increase the opportunities for VF deep fusion, and ensure that the loops comply with the Hardware Loop specification.
- Control register usage to avoid exceeding the register resource limit after fusion.

> 📌 **Note**: A larger VF fusion scope is not always better. An excessively large fusion granularity may cause register spilling, which in turn degrades performance. For more optimization techniques, see [VF Fusion Optimization](../../../../operator_practice/simd_operator_optimization/vector_compute/vf_optimization/vf_fusion_optimization.md) and [VF Loop Optimization](../../../../operator_practice/simd_operator_optimization/vector_compute/vf_optimization/vf_loop_optimization.md).

#### Dual Instruction Issue

Dual instruction issue relies on the hardware's ability to **issue and execute multiple mutually independent instructions simultaneously**. The DMA unit and the Reg vector execution unit can work in parallel when there is no data dependency. Some instructions of the Reg vector computation unit can execute in parallel when there is no dependency. Multiple aligned load-ins of the DMA unit, and aligned load-ins and aligned store-outs, can execute in parallel when there is no address dependency. One of the optimization goals of Reg vector programming is to provide the execution queue with enough independent instructions to avoid long dependency chains blocking hardware parallelism.

If the computation chain is too long, you can also moderately split the VF loop and, when necessary, write intermediate results back to UB to obtain better instruction scheduling space. At the same time, you need to control the number of registers within a VF: the number of `vector_*`, `vector_bool`, `vector_load_unalign`, and `vector_store_unalign` that can be used simultaneously in a VF is limited by hardware specifications. If the limit is exceeded, the compiler inserts swap-in/swap-out and synchronization instructions, which degrades performance instead.

>📌 **Note**: For optimization methods, see [Dual Instruction Issue Optimization](../../../../operator_practice/simd_operator_optimization/vector_compute/vf_optimization/dual_issue_optimization.md).



## Summary<a name="section_part7"></a>

Reg vector computation programming is based on the register-level programming capability exposed by the SIMD Register File newly added in [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md), enabling developers to directly organize register movement, computation, mask control, and synchronization operations. It uses the two execution domains `__aicore__` and `__simd_vf__` as its basic structure, where the `__aicore__` side is responsible for overall task organization, data splitting, and VF invocation, and the `__simd_vf__` side implements fine-grained vector computation through five types of registers and their supporting APIs.

In actual development, Reg vector computation requires not only attention to the APIs themselves but also code development that accounts for hardware execution characteristics. Prioritize aligned movement, keep loops compliant with the Hardware Loop recognition specification, and perform VF fusion within the range allowed by register resources. This usually reduces movement, synchronization, and loop startup overhead, thereby achieving more stable performance.

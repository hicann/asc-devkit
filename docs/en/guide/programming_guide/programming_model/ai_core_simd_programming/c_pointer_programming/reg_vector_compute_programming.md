# Reg Vector Computation Programming<a name="ZH-CN_TOPIC_0000002600000001"></a>

<!-- md-trans-meta sourceCommit=a68ae41756158f83bfb839f691526313df91370f translatedAt=2026-08-20T11:13:41.367Z pushedAt=2026-08-26T03:06:35.989Z -->

> 📌 **Note:** Reg vector programming is supported only when performing SIMD vector computation programming on Ascend 950PR/Ascend 950DT.

This document describes how to use the language extension C APIs in Ascend C for register-level **vector computation programming**. This capability is intended for developers who are already familiar with the [Ascend C programming model](../../programming_model_overview.md). This hardware-register-based programming approach is called Reg vector computation programming.

## Why Reg Vector Programming Is Needed<a name="section_part1"></a>

### Bottlenecks of Memory Vector Computation

In previous SIMD vector programming, both the source operands and destination operands of the vector computation unit directly reside in LocalMemory (Unified Buffer, hereinafter referred to as UB). The execution of each vector computation involves "read from UB → compute in the execution unit → write back to UB". When an operator consists of multiple vector computations connected in series (for example, `dst = (a + b) * c + d`), the intermediate result of each step must first be written back to UB and then read out again by the next instruction.

This mode has three typical bottlenecks:

- **UB read/write bandwidth is repeatedly occupied**: Intermediate results travel back and forth on UB, and the memory access pressure scales linearly as vector computations increase.

- **Severe UB Bank conflicts**: Repeated reads and writes to UB greatly increase the probability of UB Bank conflicts.

- **Critical path lengthened by memory access**: When the computation instruction execution time is very short, the overall time is dominated by UB read/write time.

The overhead of this "write back to UB at every step" approach becomes the ceiling of optimization.

### Positioning of Reg Vector Computation

To break through the bottlenecks of memory vector programming described above, Ascend 950PR/Ascend 950DT adds a new level of cache in hardware, the **SIMD Register File** (hereinafter referred to as registers). Reg vector programming is the programming approach targeting this level of registers. Its core idea is to keep a continuous segment of vector computation within the registers, interacting with the UB only once when entering and exiting this segment of computation, so that intermediate results no longer need to be written back to the UB.

The comparison between Reg vector computation and memory vector computation is as follows:

| Dimension | Memory Vector Computation | Reg Vector Computation |
| --- | --- | --- |
| Data location | UB | SIMD Registers |
| Intermediate results | Must be written back to the UB | Can be consumed continuously in registers |
| Usage cost | Lower | Higher (requires understanding of registers and related operations) |
| Applicable goals | Fast implementation without pursuing ultimate performance | Pursuing ultimate performance while accepting higher code complexity |

**On hardware that exposes register programming, the memory vector computation APIs are also provided to maintain compatibility with previous-generation hardware. The memory vector computation APIs internally implement software-encapsulated Reg vector computation.**

### Scenarios

Consider using Reg vector programming when all of the following conditions are met:

1. The target device supports Reg vector programming.

2. The core time is mainly spent on continuous vector computation.

3. Intermediate results are subject to obvious repeated read/write in the UB.

4. Ultimate performance gains are pursued while higher code complexity is accepted.

## Hardware Principles<a name="section_part2"></a>

### SIMD Register File and Execution Unit

AIV is the core within the AI Core for vector computation. The hardware units within the core that participate in Reg vector computation include:

- **Reg vector execution unit**: used to execute Reg vector computation, reading data from registers and writing results back to registers after computation.

- **DMA unit**: used to execute Reg vector data movement, responsible for moving data between registers and UB.

- **Aux Scalar**: handles the scalar computation (such as address computation) required by the Reg vector execution unit and the Reg data movement unit.

Although the Reg vector execution unit, the DMA unit, and Aux Scalar belong to different hardware execution units, they are all assigned to the PIPE_V pipeline during actual execution. This architecture has two direct consequences:

1. When there are register dependencies, the hardware guarantees data dependency correctness in instruction order, so users do not need to explicitly insert synchronization. However, cross-register reads and writes to the same UB region require explicit synchronization, because there is no automatic ordering constraint between the data movement unit and the computation unit. For details, see [Pipeline Synchronization](#section_sync).

2. When there is no data dependency, the Reg vector execution unit and the DMA unit can **issue simultaneously and execute in parallel**.

**Figure 1** SIMD Reg vector execution relationship

<img src="../../../../figures/reg_execution_unit.png" title="Reg execution unit" style="zoom:80%;"/>

### Memory Hierarchy

The memory hierarchy of the AIV is as follows:

**Figure 2** Memory hierarchy

![](../../../../figures/reg_memory_hierarchy.png "Reg memory hierarchy")

In the SIMD architecture shown above, both the UB and the registers are storage spaces exclusively owned by each AIV core. The registers do not support loading data directly from GM or writing data directly to GM. To read GM data into the registers, you must first move the data from GM to the UB, and then load the data from the UB into the registers. To write register data to GM, you must first move the data from the registers to the UB, and then move the data from the UB to GM.

**Registers are used as a whole**: A register cannot be accessed by index or offset to read a specific element or bit. Based on their functions, registers are classified into five types:

1. **Vector computation register**: main register involved in vector computation. The width of a single register is defined as Vector Length (VL). It can exchange data with the UB through the DMA unit.

2. **Mask register**: used to control the valid elements involved in vector computation or to represent single-bit elements participating in computation. The width of a single register is defined as VL/8. It can exchange data with the UB through the DMA unit.

3. **Address register**: assists the Reg vector data movement unit in exchanging data between vector computation registers and the UB. It stores the offset of a UB address. The width of a single register is 32 bits. It cannot directly exchange data with the UB.

4. **Unaligned copy-in register**: assists the Reg vector data movement unit in moving UB data to vector computation registers. It temporarily stores one DataBlock of data starting from an aligned UB address that contains the source unaligned UB address data. The width of a single register is DataBlock. UB data can be moved to the unaligned register through the DMA unit.

5. **Unaligned copy-out register**: assists the Reg vector data movement unit in moving vector computation register data out to the UB. It is used to temporarily store the data of the vector register corresponding to the unaligned UB address to be written out. The width of a single register is DataBlock. The unaligned register data can be moved out to the UB through the DMA unit.

## Programming Model<a name="section_part3"></a>

### Overall Structure

The register-based (Regbase) programming model extends the memory vector programming model of `Data copy-in` -> `Compute` -> `Data copy-out` into the Reg vector computation programming model of `Data copy-in` -> `Load` -> `Compute` -> `Store` -> `Data copy-out`.

**Figure 3** Overall structure of Regbase programming

![](../../../../figures/regbase_programming_model_overall_architecture.png "Overall structure of Regbase programming")

### VF Functions and Execution Domains

In the Reg vector computation programming model, the execution process of `Load` -> `Compute` -> `Store` is defined as a Vector Function (hereinafter referred to as VF function). To distinguish different execution domains, functions in different execution domains use different function tags, and the API call relationships are restricted at compile time:

| Tag | Role | Caller | Callable Object |
| --- | --- | --- | --- |
| `__aicore__` | Ordinary function on the device side | Kernel function or other `__aicore__` functions | `__aicore__` functions, or VF functions called through `asc_vf_call` |
| `__simd_vf__` | VF function | `__aicore__` functions called through `asc_vf_call` | Reg vector APIs modified by `__simd_callee__` or subfunctions inside the VF |
| `__simd_callee__` | Subfunction inside the VF | `__simd_vf__` or other `__simd_callee__` | `__simd_callee__` |

> 📌 **Tip**: As mentioned in the [Hardware Principles](#section_part2) section, the vector computation unit contains an Aux Scalar unit. As shown in [*Abstract Hardware Architecture*](../abstract_hardware_architecture.md), there is also a scalar computation unit (hereinafter referred to as Main Scalar) inside the AI Core. These two scalar units are independent of each other and differ in capabilities. On the programming interface, VF functions are used to isolate the execution domains of the two. Scalar computation inside a VF function is executed on the Aux Scalar unit; scalar computation outside a VF function is executed on the Main Scalar unit. Similarly, scalar computation in subfunctions called inside a VF function must satisfy the Aux Scalar capabilities; scalar computation in subfunctions called outside a VF function must conform to the Main Scalar capabilities.
>
> To distinguish the different functional execution domains of the three types of functions above, VF functions are identified by `__simd_vf__`, subfunctions inside VF functions are identified by `__simd_callee__`, and subfunctions outside VF functions are identified by `__aicore__`. At compile time, execution domain verification can be completed based on the different function tags.
>
> All Reg vector APIs are identified by `__simd_callee__` and can only be called inside VF functions.

This execution domain isolation brings three benefits:

- **Compile-time check**: Cross-domain erroneous calls (for example, directly calling a Reg vector API in `__aicore__`) are detected at the compilation stage.

- **Clear responsibilities**: `__aicore__` handles outer logic such as GM data movement and address computation, while `__simd_vf__` focuses on register-level computation.

- **Well-defined optimization boundaries**: The compiler can perform independent optimization on VF functions (such as VF fusion) without affecting the outer operator structure.

**Figure 4** Function tag call relationship

<img src="../../../../figures/reg_calculation_call_hierarchy.png" title="Call hierarchy" style="zoom: 28%;"/>

A complete implementation consists of two parts: the outer `__aicore__` logic and the inner `__simd_vf__` logic:

- **Outer `__aicore__` code**: completes preparations such as GM data interaction outside the core, and calls the VF function through `asc_vf_call`.

- **Inner `__simd_vf__` code**: completes data movement between the UB and registers as well as vector computation on registers.

A typical skeleton for calling a VF function is as follows:

```cpp
__aicore__ inline void compute()
{
    // 1. Prepare the input/output data.
    // ...

    // 2. Enter the register-level VF function domain through asc_vf_call.
    asc_vf_call<add_vf>(dst_addr, src0_addr, src1_addr,
                        one_repeat_size, repeat_times);

    // 3. Finish up.
    // ...
}
```

A VF function typically follows the skeleton below:

```text
for (i = 0; i < repeat_times; ++i) {
    1. Calculate the UB address offset for the current iteration.
    2. Load data from the UB into registers.
    3. Perform computation on the registers.
    4. Store the results back to the UB.
}
```

### Register Types and Allocation

Registers are classified into the following five types by function. In actual programming code, registers are used directly as variables and are automatically allocated to hardware registers by the compiler.

| Register Type | Variable Type Definition | Ascend 950PR/Ascend 950DT |
| --- | --- | --- |
| Vector data register | `vector_*`, for example, vector_float | VL = 256B |
| Mask register | `vector_bool` | VL/8 = 32B |
| Address register | `iter_reg` | 32bits = 8B |
| Unaligned copy-in register | `vector_load_unalign` | DataBlock = 32B |
| Unaligned copy-out register | `vector_store_unalign` | DataBlock = 32B |

#### Vector Data Register

On Ascend 950PR/Ascend 950DT, the vector data register width is VL = 256B, and the number of elements is interpreted according to the data type.

| Bit Width | Vector Data Type | Element Count |
| --------- | --------- | --------- |
| b8        | vector_hifloat8_t/vector_fp8_e4m3fn_t/vector_fp8_e5m2_t/vector_fp8_e8m0_t/vector_fp4x2_e1m2_t | 256 |
| b8 | vector_int8_t/vector_uint8_t/vector_int4x2_t/vector_fp4x2_e2m1_t | 256 |
| b16       | vector_half/vector_bfloat16_t/vector_int16_t/vector_uint16_t | 128 |
| b32       | vector_float/vector_int32_t/vector_uint32_t | 64 |
| b64       | vector_int64_t/vector_uint64_t | 32 |

The vector data register is used in code as follows:

```cpp
// Allocate a vector data register and define the data type as float.
vector_float vector_reg;

// Allocate a vector data register and define the data type as half.
vector_half vector_reg;

// Load the data at UB address src_addr into vector_float src0_reg.
asc_load(src0_reg, src_addr);

// Compute the addition of vector_float src0_reg and src1_reg, and store the result in dst_reg.
asc_add(dst_reg, src0_reg, src1_reg, mask_reg);
```

#### Mask Register

The width of `vector_bool` is `VL/8`. The minimum correspondence is 1 bit mask corresponding to 1 byte of data. When the data bit width is greater than 1 byte, each N-byte element (for example, float is 4 bytes) corresponds to the least significant bit (LSB) in the N-bit position. The following table shows the relationship between the effective element indexes and the bits in `vector_bool` for different element widths of vector data. For example, b32 data takes effect for bits 0/4/8/12/... of `vector_bool`, which correspond to elements 0/1/2/3/... of b32 in the vector data register.

| RegTensor Single-Element Width | 0th Bit | 1st Bit | 2nd Bit | 3rd Bit | 4th Bit | 5th Bit | 6th Bit | 7th Bit | 8th Bit | 9th Bit | 10th Bit | 11th Bit | 12th Bit | ...  |
| ------------------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | -------- | -------- | -------- | ---- |
| b8                  | 0       | 1       | 2       | 3       | 4       | 5       | 6       | 7       | 8       | 9       | 10       | 11       | 12       | ...  |
| b16                 | 0       | -       | 1       | -       | 2       | -       | 3       | -       | 4       | -       | 5        | -        | 6        | ...  |
| b32                 | 0       | -       | -       | -       | 1       | -       | -       | -       | 2       | -       | -        | -        | 3        | ...  |
| b64                 | 0       | -       | -       | -       | -       | -       | -       | -       | -       | 1       | -        | -        | -        | ...  |

`vector_bool` can be used not only as the mask input of computation APIs, but also as a bit-level element carrier for the results of comparison operations and for participating in shift, logical, and interleaving computations. It can also serve as the destination/source parameter of copy-in/copy-out APIs.

The creation and common usage of `vector_bool` are as follows:

```cpp
// Allocate a mask register directly
vector_bool mask_reg;

// Create a mask register with all elements valid.
vector_bool mask_reg = asc_create_mask_b32(PAT_ALL);

// Create a mask register based on the remaining element count.
vector_bool mask = asc_update_mask_b32(remainCount);

// vector_bool mask_reg serves as the control to compute the valid element mask.
asc_add(dst_reg, src0_reg, src1_reg, mask_reg);
```

When used as 1-bit elements in computation, mixed computation between vector_* and vector_bool, as well as computation between vector_bool and vector_bool, is supported.

```cpp
// vector_bool dst_mask_reg is used as the x computation parameter to compute with vector_* src0_vector_reg.
asc_select(dst_mask_reg, src0_vector_reg, src1_vector_reg, mask_reg);

// vector_bool src0_mask_reg, src1_mask_reg, and dst_mask_reg are used as parameters for computation.
asc_and(dst_mask_reg, src0_mask_reg, src1_mask_reg, mask_reg);
```

#### Address Iteration Register

`iter_reg` is used to store the address offset for UB access. It converts the scalar address calculation that originally needs to be repeatedly executed in the loop body into register-based address management, supporting up to four-dimensional offset accumulation. `AddrReg` is initialized through `asc_create_iter_reg_b*`, and the offset calculation formula is: `offset = index0*stride0 + index1*stride1 + ...`.

```cpp
// Allocate an address iteration register.
iter_reg a_reg;
// As a 2D offset accumulation for b32 elements
for(uint16_t i = 0;i < extent1; i++){
    for(uint16_t j = 0;j < extent2; j++){
        a_reg = asc_create_iter_reg_b32(i, stride0, j, stride1);
        asc_loadalign(src_reg, src_addr, a_reg);
    }
}
```

#### Unaligned Copy-In Register

`vector_load_unalign` is used to implement a temporary buffer for contiguous data movement from an unaligned UB address to a vector data register. First, initialize it through `asc_loadunalign_pre` to load the data containing unaligned content into `vector_load_unalign`, and then call `asc_loadunalign` to move the data. For the specific usage flow, see the description of unaligned data movement in [Feature Description](#section_part4).

```
// Allocate an unaligned copy-in register.
vector_load_unalign ureg0;

// Initialize vector_load_unalign using the unaligned UB address src_addr.
asc_loadunalign_pre(ureg0, src_addr);

// Move the data starting from the unaligned UB address src_addr to the vector data register src_reg.
asc_loadunalign(src_reg, ureg0, src_addr);
```

#### Unaligned Copy-Out Register

`vector_store_unalign` is used to implement a temporary buffer for moving data from a vector data register to an unaligned UB address. First, data is moved through `asc_storeunalign`, and the tail data remains in `vector_store_unalign`. Finally, `asc_storeunalign_post` is called once to move the tail data from `vector_store_unalign` to the UB. For the specific usage process, see the description of unaligned data movement in [Feature Description](#section_part4).

```cpp
// Allocate an unaligned copy-out register.
vector_store_unalign ureg0;

// Copy vreg0 to the unaligned UB address dst_addr.
asc_storeunalign(dst_addr, vreg0, ureg0, stride);

// Copy the tail data of the unaligned copy-out register to the UB.
asc_storeunalign_post(dst_addr, ureg0, 0);
```

### Pipeline Synchronization<a name="section_sync"></a>

As mentioned earlier, the Reg vector execution unit and the DMA unit can work in parallel. The key to determining whether synchronization is required lies not in the code order, but in whether the necessary data dependencies have been established between hardware pipelines:

| Scenario | Synchronization Required | Reason |
| --- | --- | --- |
| Read-after-write within the same register | No | Register dependencies are guaranteed by hardware in instruction order. |
| Different registers access the same UB address with read-after-write | Yes | There is no automatic ordering constraint between the data movement and computation execution units. |
| Different registers access the same UB address with write-after-write | Yes | The write order must be guaranteed. |
| No address overlap and no register dependency | No | No data race exists. |

Explicit synchronization is implemented through asc_mem_bar:

```cpp
// When the store data UB address and the load data UB address are the same, insert synchronization from the source store to the destination load.
asc_store(ub_addr, vreg0);
asc_mem_bar(VST_LD);
asc_load(vreg1, ub_addr);
```

The synchronization direction includes multiple specified [enumeration values](../../../../../api/SIMD-API/C-API/reg/reg_vector/asc_mem_bar.md), which are used to specify the two pipeline directions that need to be synchronized. As shown in the following figure, **when operating on the same UB address**, a read-write dependency requires inserting `Load->Store` synchronization `asc_mem_bar(VLD_ST)`; a write-write dependency requires inserting Store->Store synchronization `asc_mem_bar(VST_ST)`. The same data is processed serially and no synchronization is required only when there is a register variable dependency computation between reading and writing the same UB address.

**Figure 5** Pipeline synchronization diagram

![](../../../../figures/sync_pipeline.png "Pipeline synchronization")

> 📌 **Note**: After `asc_vf_call` calls a VF function, the outer layer does not need to pay attention to the PIPE_V barrier; when the VF function exits, all internal Reg vector instructions have completed execution.

### Reg Vector Computation APIs

The Reg vector computation C APIs (prefixed with `asc_` in the source code and belonging to the language extension C APIs) are a set of **language extension APIs that directly pass through to hardware instructions**.

- Each C API generally corresponds to one hardware instruction at compile time.

- Data types map directly to hardware register widths. For example, `vector_float` corresponds to a vector data register of one VL width.

- The API forms, parameter order, and behavior details are all organized as a language extension layer based on hardware capabilities.

The main naming conventions are as follows:

| Category | Typical API |
| -------- | ------------------------------------------------------------ |
| Data movement | `asc_load*`/`asc_store*` |
| Vector computation | `asc_add`/`asc_sub` / `asc_mul`/`asc_div` / `asc_select`/`asc_reduce_*`, etc. |
| Synchronization control | `asc_mem_bar` |

### Programming Example

The following is an example of a VF function called by compute, which implements `dst[i] = src0[i] + src1[i]`:

```cpp
// VF function: complete vector addition in the register domain.
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

// Caller: prepare the LocalTensor and UB address in the __aicore__ domain.
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

## Feature Description<a name="section_part4"></a>

The key features of Reg vector programming are classified into three categories by purpose:

| Category | Feature |
| --- | --- |
| Data movement | General data movement, aligned data movement, unaligned data movement, PostUpdate, data movement using address registers |
| Vector computation | Tail block processing, MaskMergeMode |
| VF function optimization | VF fusion, Hardware Loop, dual-instruction issue |

### Data Movement

#### General Data Movement

[`asc_load`](../../../../../api/SIMD-API/C-API/reg/reg_load/asc_load.md) / [`asc_store`](../../../../../api/SIMD-API/C-API/reg/reg_store/asc_store.md) are general data movement APIs that do not distinguish whether the UB address is aligned. They are used to move contiguous VL data between the UB and vector data registers, or to copy element data not exceeding one VL from a vector data register to the UB.

```cpp
// Move one VL of data from the UB address src_addr to the vector data register src_reg.
asc_load(src_reg, src_addr);

// Move one VL of data from the vector data register to the UB address dst_addr.
asc_store(dst_addr, dst_reg);

// Move 10 elements, which is less than one VL of data, from the vector data register to the UB address dst_addr.
asc_store(dst_addr, dst_reg, 10);
```

General data movement is suitable for code that needs to handle aligned and unaligned scenarios in a unified manner. If the address is known to meet the 32B alignment requirement and higher performance is desired, prefer the aligned data movement APIs.

> 📌 **Note**: General data movement is a software-encapsulated API whose internal implementation relies on the APIs and registers related to unaligned data movement.

#### Aligned Data Movement

[`asc_loadalign`](../../../../../api/SIMD-API/C-API/reg/reg_load/asc_load.md) / [`asc_storealign`](../../../../../api/SIMD-API/C-API/reg/reg_store/asc_store.md) require the UB address to be 32B-aligned. Compared with general data movement, aligned data movement incurs lower performance overhead and is the preferred API for contiguous aligned scenarios.

```cpp
for (uint16_t i = 0; i < repeat_times; ++i) {
    uint32_t offset = i * one_repeat_size;
    asc_loadalign(srcReg, src_addr + offset);
    // Computation on RegTensor
    ...
    asc_storealign(dst_addr + offset, dstReg, mask);
}
```

If the access pattern has a fixed stride or multi-dimensional index, you can also use the API forms with the `block_stride` and `repeat_stride` parameters.

#### Unaligned Data Movement

Unaligned data movement refers to the scenario where the UB address is unaligned but data still needs to be read from and written to the vector data register continuously. When data is moved directly with an unaligned UB address, hardware performance drops sharply. To improve performance in this scenario, `vector_load_unalign`/`vector_store_unalign` are used as temporary buffers to hold data that crosses the alignment boundary. Through multiple loop iterations, the data is assembled into aligned-address reads and writes, reducing the number of direct accesses to unaligned UB addresses and thereby reducing the extra overhead of unaligned access.

**Unaligned copy-in** is completed by combining [`asc_loadunalign_pre`](../../../../../api/SIMD-API/C-API/reg/reg_load/asc_loadunalign_pre.md) and [`asc_loadunalign`](../../../../../api/SIMD-API/C-API/reg/reg_load/asc_loadunalign.md):
For example, on Ascend 950PR/Ascend 950DT, when you want to copy in 256 bytes of data starting from UB address 48 as follows:

**Figure 6** Expected unaligned register load

![](../../../../figures/reg_unaligned_read1.png "Reg unaligned read 1")

1. First use an unaligned UB address to initialize `vector_load_unalign`. The UB address is truncated to the 32-byte-aligned address 32, and then a 32-byte DataBlock is moved to `vector_load_unalign`.

   **Figure 7**  Initializing `vector_load_unalign`

   ![](../../../../figures/reg_unaligned_read2.png "Reg unaligned read 2")

2. Call `asc_loadunalign` to move data. After the original unaligned UB address is aligned upward to 32B, 240B of the data is copied in, which is concatenated with part of the data in `vector_load_unalign` to form the expected 256B of data, and then written to the vector data register. At the same time, the data in `vector_load_unalign` is updated to the 32B DataBlock corresponding to the unaligned UB address of the next read.

   **Figure 8**  Reading the expected data

   ![](../../../../figures/reg_unaligned_read4.png "Reg unaligned read 4")

**Unaligned copy-out** is completed by combining [`asc_storeunalign`](../../../../../api/SIMD-API/C-API/reg/reg_store/asc_storeunalign.md) and [`asc_storeunalign_post`](../../../../../api/SIMD-API/C-API/reg/reg_store/asc_storeunalign_post.md): `asc_storeunalign` is used for data movement inside the loop, and after the loop ends, `asc_storeunalign_post` moves the tail data from `vector_store_unalign` to the UB.
For example, on Ascend 950PR/Ascend 950DT, when you want to store the following 512 bytes of data starting from UB address 48:

**Figure 9** Expected unaligned store

![](../../../../figures/reg_unaligned_write0.png "Reg unaligned write 0")

1. First use `asc_storeunalign`. In the first loop iteration: write the 240-byte data of the vector data register, whose tail is aligned to 32B, to the UB, and write the remaining 16B to `vector_store_unalign`; 

   **Figure 10** Expected data written in the first iteration

   ![](../../../../figures/reg_unaligned_write1.png "Reg unaligned write 1")

2. In the second iteration: concatenate the 16B data temporarily stored in `vector_store_unalign` with the 240B data in the vector data register to be written in this iteration to form 256B data, and then write it to UB. At the same time, update and write the remaining 16B of this iteration into `vector_store_unalign`; 

   **Figure 11** Expected data written in the second iteration

   ![](../../../../figures/reg_unaligned_write2.png "Reg unaligned write 2")

3. Call `asc_storeunalign_post` to write back to UB the tail data in the `vector_store_unalign` cache that has not yet been written out.

   **Figure 12** Writing out the remaining data

   ![](../../../../figures/reg_unaligned_write3.png "Reg unaligned write 3")

The recommended approach is to place initialization and post-processing outside the loop, keeping only contiguous data movement and computation inside the loop body:

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

PostUpdate means that the data movement API automatically updates the UB address pointer after completing the current access. Such APIs end with `*_postupdate`. For example, use [`asc_loadalign_postupdate`](../../../../../api/SIMD-API/C-API/reg/reg_load/asc_loadunalign_postupdate.md) for aligned data movement. When i = 0: data is copied from the address `src_addr`, and after the copy, the hardware updates `src_addr = src_addr + offset`. When i = 1: data is copied from the updated `src_addr`, and after the copy, the hardware updates `src_addr = src_addr + stride` again. This process repeats for subsequent iterations.

```cpp
for (uint16_t i = 0; i < repeat_times; ++i) {
    asc_loadalign_postupdate(srcReg, src_addr, stride);
}
```

The advantages of PostUpdate are as follows:

- It reduces the number of scalar instructions for explicitly computing `src_addr + i * stride` inside the loop.

- It binds the address update semantics to the data movement operation, making it easier for the compiler to identify consecutive access patterns.

#### Data Movement Using Address Registers

When a loop contains a fixed stride or multi-dimensional indexes, you can use `iter_reg` to manage the address offset. The hardware automatically computes the offset to reduce scalar instruction consumption. For example, a two-level loop is as follows:

```cpp
iter_reg a_reg;
for (uint16_t i = 0; i < extent1; ++i) {
    for (uint16_t j = 0; j < extent2; ++j) {
        a_reg = asc_create_iter_reg(i, stride0, j, stride1);
        asc_loadalign(src_reg, src_addr, a_reg);
    }
}
```

`iter_reg` supports up to four-dimensional addressing. The offset formula is `offset = index0*stride0 + index1*stride1 + index2*stride2 + index3*stride3`.

### Vector Computation

#### Tail Block Processing

When the total amount of data to be computed is not an integer multiple of `VL`, the last iteration needs to process data that is less than one VL. Reg vector programming implements tail block processing through `asc_update_mask_b*`: each call generates the mask for the current round based on the remaining element count, and automatically subtracts the element count corresponding to one VL after generation. This approach reduces the need to determine the tail block through if statements within the loop, avoiding violating the hardware loop constraints.

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

Hardware loop is the foundation of VF loop performance optimization. If a loop in a VF satisfies the hardware constraints, the compiler can convert it into a hardware-level loop, reducing the counting, comparison, and jump overhead of a software loop. Loops that cannot be recognized as hardware loops degrade to software loops, significantly reducing performance and affecting subsequent optimizations such as VF fusion.

> Note: The main coding specifications for hardware loops are as follows:
>
> - The loop iteration variable must be of the `uint16_t` type.
> - The loop starts from 0 with a step of 1.
> - The loop boundary must not be modified during execution.
> - Avoid runtime jump control flow such as `if/else` in the loop body.
> - A VF supports up to four levels of nested loops.

For compile-time branches and tail block scenarios, use the following methods instead of `if`:

```cpp
// Compile-time branch: if constexpr has no runtime overhead.
if constexpr (has_bias) {
    asc_add(dst_reg, src_reg, bias_reg, mask);
}

// Tail block scenario: use for(1) instead of if.
uint16_t tail = count % one_repeat_size;
uint16_t has_tail = !!tail;
for (uint16_t i = 0; i < has_tail; ++i) {
    // Tail block processing content
}
```

Typical hardware loop implementation:

```cpp
for (uint16_t i = 0; i < repeat_times; ++i) {
    asc_loadalign(src_reg, src_addr + i * one_repeat_size);
    asc_add(dst_reg, src_reg, bias_reg, mask);
    asc_storealign(dst_addr + i * one_repeat_size, dst_reg, mask);
}
```

#### VF Fusion

VF fusion is the most important optimization technique for Reg vector computation programming. It merges multiple computations and their loops into a single VF function. After fusion, the VF call and hardware loop startup overhead is reduced, and redundant Load/Store and synchronization instructions between multiple VFs are eliminated, so that intermediate results are kept in registers as much as possible for continuous consumption.

VF fusion can be divided into two levels:

- **Shallow VF fusion**: merges multiple VF function implementations into a single VF function, which eliminates the VF call overhead.

- **Deep VF fusion**: merges multiple computation loops on the basis of shallow VF fusion to reduce the hardware loop startup overhead, eliminating redundant Load/Store operations and fully reusing registers.

To increase the opportunities for VF fusion, follow these recommendations:

- Keep the same or equivalent loop structure across multiple VFs to increase the opportunities for VF deep fusion, and ensure that the loops comply with the hardware loop specifications.

- Control register usage to avoid exceeding the register resource limit after fusion.

> 📌 **Note**: VF fusion is not always better with a larger scope. An excessively large fusion granularity may cause register spilling and degrade performance. For more optimization methods, see [VF Fusion Optimization](../../../../operator_practice/simd_operator_optimization/vector_compute/vf_optimization/vf_fusion_optimization.md) and [VF Loop Optimization](../../../../operator_practice/simd_operator_optimization/vector_compute/vf_optimization/vf_loop_optimization.md).

#### Dual-Instruction Issue

Dual-instruction issue relies on the hardware capability to **issue and execute multiple mutually independent instructions simultaneously**. The DMA unit and the Reg vector execution unit can work in parallel when there is no data dependency. Some instructions of the Reg vector computation unit can execute in parallel when there is no dependency. Multiple aligned data copy-ins of the DMA unit, as well as aligned data copy-ins and aligned data copy-outs, can execute in parallel when there is no address dependency. One of the optimization goals of Reg vector programming is to provide enough independent instructions for the execution queue, avoiding long dependency chains that block hardware parallelism.

If the computation chain is too long, you can moderately split the VF loop and, if necessary, write back intermediate results to the UB to gain better instruction scheduling space. At the same time, you need to control the number of registers within a VF: the number of `vector_*`, `vector_bool`, `vector_load_unalign`, and `vector_store_unalign` that can be used simultaneously in a VF is limited by hardware specifications. If the limit is exceeded, the compiler inserts swap-in/swap-out and synchronization instructions, which degrades performance instead.

>📌 **Tip:** For optimization methods, see [Dual-Instruction Issue Optimization](../../../../operator_practice/simd_operator_optimization/vector_compute/vf_optimization/dual_instruction_issue_optimization.md).

## Summary<a name="section_part7"></a>

Reg vector computation programming is based on the register-level programming capability exposed by the SIMD register file newly added to Ascend 950PR/Ascend 950DT, enabling developers to directly organize register data movement, computation, mask control, and synchronization operations. It uses the two execution domains `__aicore__` and `__simd_vf__` as its basic structure, where the `__aicore__` side is responsible for overall task organization, data partitioning, and VF call, and the `__simd_vf__` side performs fine-grained vector computation through the five types of registers and their associated APIs.

In actual development, Reg vector computation requires attention not only to the APIs themselves but also to writing code that aligns with hardware execution characteristics. Prioritize aligned data movement, keep loops compliant with the hardware loop recognition specifications, and perform VF fusion within the limits of available register resources. These practices generally reduce data movement, synchronization, and loop startup overhead, thereby delivering more stable performance.
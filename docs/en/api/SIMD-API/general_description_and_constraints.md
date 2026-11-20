# General Description and Constraints<a name="ZH-CN_TOPIC_0000001714258422"></a>

<!-- md-trans-meta sourceCommit=22a41aa8a74d7a2c3f454ea4bd5f0f8fb27aee29 translatedAt=2026-11-19T02:49:07.356Z pushedAt=2026-11-19T03:06:17.184Z nl=2 -->

## Required Header Files and Library Files<a name="section1299152225418"></a>

> [!NOTE] Note
> The header file directories of Ascend C APIs are as follows:
> - Basic APIs: `${INSTALL_DIR}/asc/include/basic_api`
> - Advanced APIs: (Note that if the APIs included in the header files in the following directories are not declared in the documentation, they are indirectly called APIs and do not require developer attention.)
>   - `${INSTALL_DIR}/asc/include/adv_api`
>   - `${INSTALL_DIR}/asc/include/tiling`
> 
> Replace `${INSTALL_DIR}` with the file storage path after CANN software installation. For example, when installed as the root user, the default storage path is `/usr/local/Ascend/cann`.
> 
> The library files that Ascend C APIs depend on are described as follows:
> - Basic APIs: Not involved.
> - Advanced APIs: Because advanced APIs are used with the Host Tiling API, `libtiling_api.a` must be linked.

For developer convenience, both Ascend C basic APIs and advanced APIs support calling the corresponding APIs by including the kernel function (Kernel)\_operator.h file. Unless otherwise specified, including this header file satisfies the API call requirements. If an API document contains a special description, follow the specific description of the API.

```
#include "kernel_operator.h"
```

## Mapping Relationship Between Logical Locations and Physical Storage<a name="section1359919519819"></a>

The operands of Ascend C APIs are usually [GlobalTensor](basic_api/data_structures/GlobalTensor/GlobalTensor.md) and [LocalTensor](basic_api/data_structures/LocalTensor/LocalTensor.md). The storage locations of tensor data are expressed using logical locations ([TPosition](basic_api/aux_data_structures/TPosition.md)), thereby hiding the differences in hardware architecture. The TPosition types include VECIN, VECOUT, VECCALC, A1, A2, B1, B2, CO1, CO2, and so on. The mapping relationship between TPosition and physical memory is shown in the following table.

**Table 1** Mapping relationship between TPosition and physical memory

<a name="table07372185712"></a>

| TPosition | Physical Memory |
| --- | --- |
| GM | Global Memory |
| VECIN | UB |
| VECCALC | UB |
| VECOUT | UB |
| A1 | L1 Buffer |
| A2 | L0A Buffer |
| B1 | L1 Buffer |
| B2 | L0B Buffer |
| C1 | <!-- npu="950" id1 -->Ascend 950PR&950DT products, L1 Buffer.<br><!-- end id1 --><!-- npu="910" id2 -->Atlas training products, Unified Buffer (UB).<br><!-- end id2 --><!-- npu="310p" id3 -->Atlas inference products AI Core, UB.<br><!-- end id3 --><!-- npu="910b" id4 -->Atlas A2 products, L1 Buffer.<br><!-- end id4 --><!-- npu="A3" id5 -->Atlas A3 products, L1 Buffer.<br><!-- end id5 --><!-- npu="310b" id6 -->Atlas 200I/500 A2 inference products, UB.<!-- end id6 --> |
| C2 | <!-- npu="950" id7 -->Ascend 950PR&950DT products, BiasTable Buffer.<br><!-- end id7 --><!-- npu="910" id8 -->Atlas training products, L0C Buffer.<br><!-- end id8 --><!-- npu="310p" id9 -->Atlas inference products AI Core, L0C Buffer.<br><!-- end id9 --><!-- npu="910b" id10 -->Atlas A2 products, BiasTable Buffer.<br><!-- end id10 --><!-- npu="A3" id11 -->Atlas A3 products, BiasTable Buffer.<br><!-- end id11 --><!-- npu="310b" id12 -->Atlas 200I/500 A2 inference products, BiasTable Buffer.<!-- end id12 --> |
| CO1 | L0C Buffer |
| CO2 | <!-- npu="950" id13 -->Ascend 950PR&950DT products, Global Memory.<br><!-- end id13 --><!-- npu="910" id14 -->Atlas training products, UB.<br><!-- end id14 --><!-- npu="310p" id15 -->Atlas inference products AI Core, UB.<br><!-- end id15 --><!-- npu="910b" id16 -->Atlas A2 products, Global Memory.<br><!-- end id16 --><!-- npu="A3" id17 -->Atlas A3 products, Global Memory.<br><!-- end id17 --><!-- npu="310b" id18 -->Atlas 200I/500 A2 inference products, Global Memory.<!-- end id18 --> |
| TSCM | L1 Buffer |
| SPM | <!-- npu="950" id19 -->Ascend 950PR&950DT products, Global Memory.<br><!-- end id19 --><!-- npu="910" id20 -->Atlas training products, L1 Buffer.<br><!-- end id20 --><!-- npu="310p" id21 -->Atlas inference products AI Core, L1 Buffer.<br><!-- end id21 --><!-- npu="910b" id22 -->Atlas A2 products, Global Memory.<br><!-- end id22 --><!-- npu="A3" id23 -->Atlas A3 products, Global Memory.<!-- end id23 --> |
| C2PIPE2GM | <!-- npu="950" id24 -->Ascend 950PR&950DT products, Fixpipe Buffer.<br><!-- end id24 --><!-- npu="910b" id25 -->Atlas A2 products, Fixpipe Buffer.<br><!-- end id25 --><!-- npu="A3" id26 -->Atlas A3 products, Fixpipe Buffer.<!-- end id26 --> |

## General Address Alignment Constraints<a name="section796754519912"></a>

The storage units on the AI Core are used to store the source operands and destination operands of vector computation and matrix computation. The alignment requirements of various storage units are shown in [Table 2](#table16278354141117). Therefore, the start address alignment requirements of Ascend C API operands should be consistent with the alignment requirements of these storage units. **Note that if an API explicitly specifies the start address alignment requirement of the operand, the description in the specific API shall prevail.**

**Table 2** Alignment requirements of different storage units

<a name="table16278354141117"></a>

| Storage Unit | Alignment Requirement |
| --- | --- |
| Global Memory | No alignment requirement. |
| UB | 32-byte alignment. |
| L1 Buffer | 32-byte alignment. |
| L0A Buffer/L0B Buffer | 512-byte alignment. |
| L0C Buffer | 64-byte alignment. |
| BiasTable Buffer | 64-byte alignment. |
| Fixpipe Buffer | 64-byte alignment. |

## General Address Overlap Constraints<a name="section668772811100"></a>

When using the tensor high-dimensional split computation APIs of the basic APIs, to save address space, a developer can define a tensor that is used by both the source operand and the destination operand (that is, address overlap). The following constraints must be noted during use:

-   Within a single iteration: The source operand and the destination operand must overlap 100% completely. Partial overlap is not supported.
-   Between multiple iterations: Overlap between the destination operand of a preceding iteration and the source operand of a subsequent iteration is not supported. For example, the destination operand of the Nth iteration is the source operand of the (N+1)th iteration (as shown in the following figure). In this case, the Nth iteration may overwrite the value of the source operand, resulting in failure to obtain the expected result. In particular, for some binary computation APIs (Add, Sub, Mul, Max, Min, AddRelu, SubRelu), when the data type is half, int32\_t, or float, overlap between the destination operand of a preceding iteration and the source operand of a subsequent iteration is supported: only for the case where the destination operand overlaps with the second source operand, and src1RepStride or dstRepStride must be 0.
-   Between multiple iterations: Overlap between the destination operand of a preceding iteration and the destination operand of a subsequent iteration is not supported. For example, the DataBlock numbers written in the first loop are 0, 2, 4, 6, 8, 10, 12, and 14, and the numbers written in the second loop are 8, 10, 12, 14, 16, 18, 20, and 22. That is, the previous iteration overwrites the last four DataBlocks of the preceding iteration, resulting in failure to obtain the expected result.

**Figure 1** Address overlap example (not supported)<a name="fig12654416181216"></a>  
![](../figures/address_overlap_unsupported.png "Address overlap example (not supported)")

> [!NOTE] Note
>The general address overlap constraints described in this section apply to general cases. If the API reference contains additional special descriptions, the description in the specific API shall prevail.
>If an API does not describe the address overlap constraint, address overlap for tensor high-dimensional split computation is considered not supported, and the computation result may not satisfy the expectation when addresses overlap.

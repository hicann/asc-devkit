# General Description and Constraints<a name="ZH-CN_TOPIC_0000001714258422"></a>

<!-- md-trans-meta sourceCommit=84d4771c758afa4058a229c863ddc58cf59e3dd3 translatedAt=2026-09-08T05:25:10.875Z pushedAt=2026-09-08T13:17:42.548Z -->

## Required Header Files and Library Files<a name="section1299152225418"></a>

> [!NOTE]
> The header file directories for the Ascend C API are as follows:
> - Basic API: `${INSTALL_DIR}/include/ascendc/basic_api/API`
> - Advanced API: (Note: If an API contained in the header files in the following directories is not declared in the documentation, it is an indirectly invoked API and developers do not need to pay attention to it.)
>   - `${INSTALL_DIR}/include/ascendc/highlevel_api/lib`
>   - `${INSTALL_DIR}/include/tiling`
>
> Replace `${INSTALL_DIR}` with the file storage path after the CANN software is installed. For example, when installed as the root user, the default file storage path is `/usr/local/Ascend/cann`.
>
> The library files that the Ascend C API depends on are described as follows:
> - Basic API: Not involved.
> - Advanced API: Because the advanced API is paired with the Host Tiling API, `libtiling_api.a` needs to be linked.

For developer convenience, both the Ascend C basic API and advanced API support invoking the corresponding APIs by including the kernel\_operator.h file. Unless otherwise specified, including this header file is sufficient to meet the API invocation requirements. If an API document contains specific instructions, follow the specific instructions of that API.

```
#include "kernel_operator.h"
```

## Mapping Between Logical Positions and Physical Memory<a name="section1359919519819"></a>

The operands of Ascend C APIs are usually [GlobalTensor](basic_api/data_structures/GlobalTensor/GlobalTensor.md) and [LocalTensor](basic_api/data_structures/LocalTensor/LocalTensor.md). The storage location of tensor data is expressed by a logical position ([TPosition](basic_api/aux_data_structures/TPosition.md)), which hides the differences in hardware architecture. The TPosition types include VECIN, VECOUT, VECCALC, A1, A2, B1, B2, CO1, CO2, and so on. The mapping between TPosition and physical memory is shown in the following table.

**Table 1** Mapping between TPosition and physical memory

<a name="table07372185712"></a>
| TPosition | Physical Memory |
| --- | --- |
| GM | Global Memory |
| VECIN | Unified Buffer |
| VECCALC | Unified Buffer |
| VECOUT | Unified Buffer |
| A1 | L1 Buffer |
| A2 | L0A Buffer |
| B1 | L1 Buffer |
| B2 | L0B Buffer |
| C1 | <!-- npu="950" id1 -->Ascend 950PR/Ascend 950DT, L1 Buffer<br><!-- end id1 --><!-- npu="910" id2 -->Atlas training products, Unified Buffer<br><!-- end id2 --><!-- npu="310p" id3 -->Atlas inference products AI Core, Unified Buffer<br><!-- end id3 --><!-- npu="910b" id4 -->Atlas A2 training products/Atlas A2 inference products, L1 Buffer<br><!-- end id4 --><!-- npu="A3" id5 -->Atlas A3 training products/Atlas A3 inference products, L1 Buffer<br><!-- end id5 --><!-- npu="310b" id6 -->Atlas 200I/500 A2 inference product, Unified Buffer<!-- end id6 --> |
| C2 | <!-- npu="950" id7 -->Ascend 950PR/Ascend 950DT, BiasTable Buffer<br><!-- end id7 --><!-- npu="910" id8 -->Atlas training products, L0C Buffer<br><!-- end id8 --><!-- npu="310p" id9 -->Atlas inference products AI Core, L0C Buffer<br><!-- end id9 --><!-- npu="910b" id10 -->Atlas A2 training products/Atlas A2 inference products, BiasTable Buffer<br><!-- end id10 --><!-- npu="A3" id11 -->Atlas A3 training products/Atlas A3 inference products, BiasTable Buffer<br><!-- end id11 --><!-- npu="310b" id12 -->Atlas 200I/500 A2 inference product, BiasTable Buffer<!-- end id12 --> |
| CO1 | L0C Buffer |
| CO2 | <!-- npu="950" id13 -->Ascend 950PR/Ascend 950DT, Global Memory<br><!-- end id13 --><!-- npu="910" id14 -->Atlas training products, Unified Buffer<br><!-- end id14 --><!-- npu="310p" id15 -->Atlas inference products AI Core, Unified Buffer<br><!-- end id15 --><!-- npu="910b" id16 -->Atlas A2 training products/Atlas A2 inference products, Global Memory<br><!-- end id16 --><!-- npu="A3" id17 -->Atlas A3 training products/Atlas A3 inference products, Global Memory<br><!-- end id17 --><!-- npu="310b" id18 -->Atlas 200I/500 A2 inference product, Global Memory<!-- end id18 --> |
| TSCM | L1 Buffer |
| SPM | <!-- npu="950" id19 -->Ascend 950PR/Ascend 950DT, Global Memory<br><!-- end id19 --><!-- npu="910" id20 -->Atlas training products, L1 Buffer<br><!-- end id20 --><!-- npu="310p" id21 -->Atlas inference products AI Core, L1 Buffer<br><!-- end id21 --><!-- npu="910b" id22 -->Atlas A2 training products/Atlas A2 inference products, Global Memory<br><!-- end id22 --><!-- npu="A3" id23 -->Atlas A3 training products/Atlas A3 inference products, Global Memory<!-- end id23 --> |
| C2PIPE2GM | <!-- npu="950" id24 -->Ascend 950PR/Ascend 950DT, Fixpipe Buffer<br><!-- end id24 --><!-- npu="910b" id25 -->Atlas A2 training products/Atlas A2 inference products, Fixpipe Buffer<br><!-- end id25 --><!-- npu="A3" id26 -->Atlas A3 training products/Atlas A3 inference products, Fixpipe Buffer<!-- end id26 --> |

## General Address Alignment Constraints<a name="section796754519912"></a>

The storage units on the AI Core are used to store the source operands and destination operands of vector and matrix computations. The alignment requirements of each storage unit are shown in [Table 2](#table16278354141117). Therefore, the start address alignment requirements of Ascend C API operands should be consistent with the alignment requirements of these storage units. **Note that if the start address alignment requirement of an operand is explicitly stated in an API, the description in that specific API prevails.**

**Table 2**  Alignment requirements of different storage units

<a name="table16278354141117"></a>
| Storage Unit | Alignment Requirement |
| --- | --- |
| Global Memory | No alignment requirement |
| Unified Buffer | 32-byte alignment |
| L1 Buffer | 32-byte alignment |
| L0A Buffer/L0B Buffer | 512-byte alignment |
| L0C Buffer | 64-byte alignment |
| BiasTable Buffer | 64-byte alignment |
| Fixpipe Buffer | 64-byte alignment |

## General Address Overlap Constraints<a name="section668772811100"></a>

When using the high-dimensional tensor slicing computation APIs of the basic APIs, to save address space, developers can define a single tensor that is used by both the source operand and the destination operand (that is, address overlap). The following constraints must be observed when using this feature:

-   Within a single iteration: The source operand and the destination operand must overlap 100% completely. Partial overlap is not supported.
-   Across multiple iterations: Overlap between the destination operand of a preceding iteration and the source operand of a subsequent iteration is not supported. For example, the destination operand of the Nth iteration is the source operand of the (N+1)th iteration (as shown in the following figure). In this case, the Nth iteration may overwrite the value of the source operand, resulting in unexpected results. In particular, for some binary computation APIs (**Add**, **Sub**, **Mul**, **Max**, **Min**, **AddRelu**, **SubRelu**), when the data type is half, int32\_t, or float, overlap between the destination operand of a preceding iteration and the source operand of a subsequent iteration is supported: only for the case where the destination operand overlaps with the second source operand, and either **src1RepStride** or **dstRepStride** must be 0.

**Figure 1** Address overlap example (not supported)<a name="fig12654416181216"></a>  
![](../figures/address_overlap_example_(not_supported).png "Address overlap example (not supported)")

> [!NOTE]
> The general address overlap constraints described in this section apply to general cases. If the API reference provides additional special instructions, the instructions in the specific API prevail.
> If an API does not describe address overlap constraints, address overlap for high-dimensional tensor slicing computation is considered unsupported, and the computation result may not meet expectations when addresses overlap.

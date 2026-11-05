# GM and UB Data Movement<a name="ZH-CN_TOPIC_0000002573902833"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T14:01:21.243Z pushedAt=2026-10-26T07:41:44.265Z nl=2 -->

## Overview<a name="ZH-CN_TOPIC_0000002543422566"></a>

Data movement between Global Memory (GM) and Unified Buffer (UB) provides a variety of APIs that support rich data movement scenarios (see [Table 1](#zh-cn_topic_0000002565937657_table12546123753110)). To achieve efficient vector computation, the system supports moving data from GM to UB for the Vector computation unit to perform vector computation; after the computation is complete, the results are moved back from UB to GM, completing the data loop. By flexibly configuring struct parameters, developers can precisely control key attributes such as the number of data blocks, the length of each address, and the address interval, thereby implementing contiguous and non-contiguous data movement (high-dimensional slicing data movement), fully unleashing the hardware's data movement and computation collaboration capability, and significantly improving the execution efficiency and overall performance of vector operators.

**Table 1**  Overview of GM-to-UB data movement APIs

<a name="zh-cn_topic_0000002565937657_table12546123753110"></a>

| Data Path | Function | Description |
| :---: | :--- | :--- |
| GM->UB | Contiguous data movement (DataCopy) | Provides basic data movement capability. Data retains its original format and content during transmission, supporting contiguous data movement. |
| UB->GM | Contiguous data movement (DataCopy) | Provides basic data movement capability. Data retains its original format and content during transmission, supporting contiguous data movement. |
| GM->UB | High-dimensional slicing data movement (DataCopy) | Provides basic data movement capability. Data retains its original format and content during transmission, supporting contiguous and non-contiguous data movement. |
| UB->GM | High-dimensional slicing data movement (DataCopy) | Provides basic data movement capability. Data retains its original format and content during transmission, supporting contiguous and non-contiguous data movement. |
| GM->UB | Sliced data movement (DataCopy) | This API is a software simulation API that supports sliced data movement, extracting a subset of a multi-dimensional Tensor for movement. |
| UB->GM | Sliced data movement (DataCopy) | This API is a software simulation API that supports sliced data movement, extracting a subset of a multi-dimensional Tensor for movement. |
| GM->UB | Inline conversion ND2NZ movement (DataCopy) | This API is a software simulation API that supports ND-to-NZ format conversion during data movement from Global Memory to UB. |
| UB->GM | Inline conversion NZ2ND movement (DataCopy) | This API is a software simulation API that supports NZ-to-ND format conversion during data movement from UB to Global Memory. |
| GM->UB | Multi-dimensional data movement NDDMA (DataCopy) | A multi-dimensional data movement API. Compared with the basic data movement API, it allows more flexible configuration of the dimension information to be moved in and the corresponding Stride. |
| GM->UB | Unaligned data movement (DataCopyPad) | Supports unaligned data movement, allowing data to be padded when moved into UB. |
| UB->GM | Unaligned data movement (DataCopyPad) | Supports unaligned data movement. |

## DataCopy (Contiguous Data Movement Between GM and UB)<a name="ZH-CN_TOPIC_0000002382908021"></a>

Supports contiguous data movement between Global Memory and UB. The data retains its original format and content during transmission.

The supported data paths are as follows (represented by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- Global Memory -> UB
    - GM -> VECIN
- UB -> Global Memory
    - VECOUT -> GM
    <!-- npu="310p" id1 -->
    - CO2 -> GM (supported only on the Atlas inference products AI Core)
    <!-- end id1 -->

The moved data is used for [vector computation](../memory_vector_compute/memory_vector_compute.md). For the specific API, see [DataCopy (Contiguous Data Movement Between GM and UB)](../memory_vector_compute/data_move/DataCopy_GMAndUB_continuous.md).

**src** and **dst** are the source operand and destination operand, respectively; **count** is the number of elements to be moved contiguously.

- Global Memory -> UB

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const uint32_t count)
    ```

- UB -> Global Memory

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
    ```

## DataCopy (High-Dimensional Slicing Data Movement Between GM and UB)<a name="ZH-CN_TOPIC_00000023829080211"></a>

Supports high-dimensional slicing data movement between Global Memory and UB. The data retains its original format and content during transmission.

High-dimensional slicing data movement supports both non-contiguous and contiguous data movement by configuring movement parameters such as the number of data blocks, the length of a single data block, and the address offset.

The supported data paths are as follows (represented by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- Global Memory -> UB
    - GM -> VECIN
- UB -> Global Memory
    - VECOUT -> GM
    <!-- npu="310p" id2 -->
    - CO2 -> GM (supported only on the Atlas inference products AI Core)
    <!-- end id2 -->

The moved data is used for [vector computation](../memory_vector_compute/memory_vector_compute.md). For the specific API, see [DataCopy (High-Dimensional Slicing Data Movement Between GM and UB)](../memory_vector_compute/data_move/DataCopy_GMAndUB_highdim_split.md).

**src** and **dst** are the source operand and destination operand, respectively. **repeatParams** is the movement parameters of the **DataCopyParams** type, through which the size, count, and interval of the data blocks to be moved can be configured. Both non-contiguous and contiguous data movement are supported.

- Global Memory -> UB

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyParams& repeatParams)
    ```

- UB -> Global Memory

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
    ```

## DataCopy (Sliced Data Movement Between GM and UB)<a name="ZH-CN_TOPIC_0000002568770166"></a>

This API is a software simulation API designed from the perspective of ease of use. It supports sliced data movement and moves a subset of multi-dimensional Tensor data.

The specific supported data paths are as follows (represented by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- Global Memory -> UB
    - GM -> VECIN
- UB -> Global Memory
    - VECOUT -> GM
    <!-- npu="310p" id3 -->
    - CO2 -> GM (supported only on the Atlas inference products AI Core)
    <!-- end id3 -->

The moved data is used for [vector computation](../memory_vector_compute/memory_vector_compute.md). For the specific API, see [DataCopy (Sliced Data Movement Between GM and UB)](../memory_vector_compute/data_move/DataCopy_GMAndUB_slice.md).

**src** and **dst** are the source operand and destination operand, respectively; **dstSliceInfo** and **srcSliceInfo** are the slice information of the destination and source operands, respectively, of the type **SliceInfo**; and **dimValue** is the dimension information of the operands.

- Global Memory -> UB

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const SliceInfo dstSliceInfo[], const SliceInfo srcSliceInfo[], const uint32_t dimValue = 1)
    ```

- UB -> Global Memory

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const GlobalTensor<T> &dst, const LocalTensor<T> &src, const SliceInfo dstSliceInfo[], const SliceInfo srcSliceInfo[], const uint32_t dimValue = 1)
    ```

## DataCopy (GM -> UB Inline Conversion ND2NZ Data Movement)<a name="ZH-CN_TOPIC_0000002349187356"></a>

This API is a software simulation API designed from the perspective of ease of use. It supports ND-to-NZ format conversion during data movement from Global Memory to UB.

The specific supported data paths are as follows (represented by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- Global Memory -> UB
    - GM -> VECIN

The moved data is used for [vector computation](../memory_vector_compute/memory_vector_compute.md). For the specific API, see [DataCopy (GM -> UB inline conversion ND2NZ Data Movement)](../memory_vector_compute/data_move/DataCopy_GMToUB_ND2NZ.md).

**src** and **dst** are the source operand and destination operand, respectively. **intriParams** is the movement parameter of the **Nd2NzParams** type, which configures the ND-to-NZ format conversion information.

- The enableSmallC0 mode is not supported.

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams)
    ```

<!-- npu="950" id4 -->
- The enableSmallC0 mode is supported (only supported by Ascend 950PR&950DT products).

    ```cpp
    template <typename T, bool enableSmallC0 = false>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams)
    ```
<!-- end id4 -->

## DataCopy (UB -> GM inline conversion NZ2ND transfer)<a name="ZH-CN_TOPIC_0000002391805265"></a>

This API is a software simulation API designed from the perspective of ease of use. It supports NZ-to-ND format conversion during data movement from UB to Global Memory.

The specific supported data paths are as follows (expressed in [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- UB -> Global Memory
    - VECOUT -> GM
    <!-- npu="310p" id5 -->
    - CO2 -> GM (supported only on the Atlas inference products AI Core)
    <!-- end id5 -->

The moved data is used for [vector computation](../memory_vector_compute/memory_vector_compute.md). For the specific API, see [DataCopy (UB->GM inline conversion NZ2ND movement)](../memory_vector_compute/data_move/DataCopy_UBToGM_NZ2ND.md).

**src** and **dst** are the source operand and destination operand, respectively. **intriParams** is the movement parameter of the **Nz2NdParamsFull** type, which configures the NZ-to-ND format conversion information.

```cpp
template <typename T>
__aicore__ inline void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const Nz2NdParamsFull& intriParams)
```

## DataCopy (GM -> UB Multi-dimensional Data Movement NDDMA)<a name="ZH-CN_TOPIC_0000002544407954"></a>

The multi-dimensional data movement API, compared with the basic data movement API, allows more flexible configuration of the dimension information to be moved in and the corresponding Stride.

The supported data paths are as follows (represented by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- Global Memory -> UB
    - GM -> VECIN

The moved data is used for [vector computation](../memory_vector_compute/memory_vector_compute.md). For the specific API, see [DataCopy (GM -> UB Multi-dimensional Data Movement NDDMA)](../memory_vector_compute/data_move/DataCopy_GMToUB_NDDMA.md).

**src** and **dst** are the source operand and destination operand, respectively; **params** is the movement parameter of the **NdDmaParams** type, which configures the multi-dimensional movement information.

- Global Memory -> UB, supporting multi-dimensional data movement

    ```cpp
    template <typename T, uint8_t dim, const NdDmaConfig& config = kDefaultNdDmaConfig>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const NdDmaParams<T, dim>& params)
    ```

- NDDMA Cache refresh

    ```cpp
    __aicore__ inline void NdDmaDci()
    ```

## DataCopyPad (GM -> UB Unaligned Data Movement)<a name="ZH-CN_TOPIC_0000001894460401"></a>

This API moves data from Global Memory to UB in an unaligned manner, and can pad data as needed by the developer. When the length of each moved data block (**blockLen**) is not 32-byte aligned, each data block must be padded to 32-byte alignment.

<!-- npu="950" id6 -->
In particular, for Ascend 950PR&950DT products, the Compact mode is supported. In this mode, a single data block is allowed to be non-32-byte aligned, and all data blocks are merged into one contiguous data block, which is then padded on the right side to 32-byte alignment.
<!-- end id6 -->

The supported data paths are as follows (represented by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- Global Memory -> UB
    - GM -> VECIN
    - GM -> VECOUT
    <!-- npu="950" id7 -->
    - GM -> VECCALC (supported only on Ascend 950PR&950DT products)
    <!-- end id7 -->

The moved data is used for [vector computation](../memory_vector_compute/memory_vector_compute.md). For the specific API, see [DataCopyPad (GM -> UB Unaligned Data Movement)](../memory_vector_compute/data_move/DataCopyPad_GMToUB.md).

**src** and **dst** are the source operand and destination operand, respectively; **dataCopyParams** is the movement parameters of the **DataCopyExtParams** type; and **padParams** is the padding control parameter of the **DataCopyPadExtParams** type.

- Configuring the data movement mode is not supported.

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopyPad(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyExtParams& dataCopyParams, const DataCopyPadExtParams<T>& padParams)
    ```

<!-- npu="950" id8 -->
- Configuring the data movement mode is supported (only supported by Ascend 950PR&950DT products).

    ```cpp
    template <typename T, PaddingMode mode = PaddingMode::Normal>
    __aicore__ inline void DataCopyPad(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyExtParams& dataCopyParams, const DataCopyPadExtParams<T>& padParams)
    ```
<!-- end id8 -->

## DataCopyPad (UB -> GM unaligned data movement)<a name="ZH-CN_TOPIC_0000001894460502"></a>

This API moves data from the UB to Global Memory in an unaligned manner.

For scenarios that are not 32-byte aligned, dummy data is filled in to align to 32B when reading UB data, and the dummy data is discarded when moving into Global Memory, thereby implementing unaligned data movement from UB to Global Memory.

The specific supported data paths are as follows (represented by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- UB -> Global Memory
    - VECIN -> GM
    - VECOUT -> GM

The moved data is used for [vector computation](../memory_vector_compute/memory_vector_compute.md). For the specific API, see [DataCopyPad (UB -> GM unaligned data movement)](../memory_vector_compute/data_move/DataCopyPad_UBToGM.md).

**src** and **dst** are the source operand and destination operand, respectively; **dataCopyParams** is the movement parameter of the **DataCopyExtParams** type.

- Configuring the data movement mode **mode** is not supported.

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopyPad(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyExtParams& dataCopyParams)
    ```

<!-- npu="950" id9 -->
- Supports configuring the data movement mode **mode** (supported only on Ascend 950PR&950DT products).

    ```cpp
    template <typename T, PaddingMode mode = PaddingMode::Normal>
    __aicore__ inline void DataCopyPad(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyExtParams& dataCopyParams)
    ```
<!-- end id9 -->

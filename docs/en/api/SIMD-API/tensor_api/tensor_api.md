# Tensor API

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T02:06:54.039Z pushedAt=2026-11-19T05:54:06.594Z nl=2 -->

Tensor API is a set of fundamental programming APIs for Ascend C operator development. It uses Tensor and Layout to uniformly describe the storage location, data type, size, and arrangement rules of data, and performs movement, cube computation, and Vector Computation based on this information, reducing the workload of manually organizing hardware instruction parameters and data layout for developers.

Tensor API provides Tensor-based movement, cube computation, and Vector Computation capabilities. Developers can combine Tensor API and C API in the same kernel function (Kernel) to implement complete operator functionality.

## API Hierarchy

**Table 1** API hierarchy description

| Hierarchy | Function | Main Contents |
| :--- | :--- | :--- |
| Tensor+Layout | Describes the storage location, data type, size, and arrangement rules of data. | Tensor, Pointer, Layout, Shape, Stride, and Coord. |
| Arch | Provides Operations related to movement and cube computation, Traits for describing compile-time characteristics, Params for describing runtime parameters, and Vector Computation APIs. | Operation: `copy_gm_to_l1`, `mmad_operation`<br>Trait: `gm_to_l1_trait`<br>Params: `l0c_to_gm_params`<br>Vector Computation: `select`, `fill`. |
| Atom | Combines an Operation, a Trait, and optional Params into an atomic object for movement or cube computation. | `copy_atom`, `mmad_atom`. |
| Algorithm | Provides a unified entry point for Tensor-oriented movement and cube computation. | `copy`, `make_copy`, `mmad`, and `make_mmad`. |

When using the Tensor API, include the unified entry header file:

```cpp
#include "tensor_api/tensor.h"
```

## Capability

- [Layout](layout/layout_structure.md): Describes the construction of Layout, Shape, Stride, and Coord, the data layout description, and the related attribute query capabilities.
- [Tensor](tensor/tensor_structure.md): Describes the creation, access, and slicing capabilities of Tensor and Pointer.
- [Algorithm](algorithm.md): Describes the roles, composition relationships, and usage of Operation, Trait, and Atom in movement and cube computation.
- [Movement](data_move/data_move.md): Describes multiple movement paths, including bidirectional movement between Global Memory and Unified Buffer (UB), and movement from Global Memory to L1 Buffer.
- [Cube Computation](matrix_compute/matrix_compute.md): Describes cube data loading, cube multiply-accumulate, cube result unloading, and related key features.
- [Cube Data Padding](matrix_compute/cube_data_fill/cube_data_fill.md): Describes the L1 physical block initialization capability before cube computation.
- [reg Vector Computation](reg_vector_compute/reg_vector_compute.md): Describes vector computation load-in, vector computation load-out, register data load-in and load-out, Mask register computation, and type conversion.
- [Tool APIs](utils/tool_functions.md): Describes the usage of tool APIs such as compile-time integers, dimension-preserving markers, Layout Pattern query, type judgment, debug printing, and system variable query.
- [Special Registers](special_register/special_register.md): Describes the APIs for saturation control and squeeze state query.
- [Scalar Computation](scalar_compute/scalar_compute.md): Describes the APIs for scalar bitwise operations and scalar GM read/write.
- [Synchronization Control](sync/sync.md): Describes the APIs for functional inter-core synchronization, object-based pipeline locks, and scope lock protection.
- [Cache Control](cache_control/cache_control.md): Describes the APIs for Data Cache cleaning, prefetching, and L2 Cache management policies for scalar load/store paths.
- [Atomic Operations](atomic/atomic.md): Describes the APIs for DMA atomic mode configuration and Scalar atomic operations.
- [Constant Padding Value Configuration](data_move/set_copy_pad_val.md): Describes the APIs for configuring constant padding values for aligned GM-to-UB movement.
- [Deprecated APIs](deprecated_interface/deprecated_interface.md): Describes the Upper Camel Case style APIs retained for compatibility.

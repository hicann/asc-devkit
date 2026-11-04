# Operator Functional Design

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T13:55:10.778Z pushedAt=2026-11-03T07:05:13.406Z nl=2 -->

This document covers basic content and describes the functional design that must be completed before writing custom operator code, including converting computation semantics into engineering implementation and identifying the engineering files to be written.

This document uses the AddCustom operator as an example. The computation semantics of AddCustom are as follows:

```text
z = x + y
```

This document only describes the content that must be clarified during the functional design phase. Specific APIs and code writing methods are described in subsequent sections.

## Prerequisite Understanding: What Are the Components of the aclnn Engineering Operator Development Approach

An aclnn custom operator project typically requires three types of code.

The first type of code: **operator prototype definition**, which declares the interface information of the operator, including inputs, outputs, and attributes, as well as the supported dtype and format.

The second type of code: **Host-side Tiling implementation**, which prepares the runtime parameters before the kernel function is executed. Before the kernel function runs on the AI Core, it needs to know how much data this input contains in total, how to split the cores and how many Blocks to launch, how to further split within each Block, whether additional workspace is required, and whether launch configurations such as the scheduling mode need to be set.

The third type of code: **kernel function-side operator implementation**, which performs the actual computation on the AI Core. Based on the parameters passed in by Tiling, the kernel function side moves data from GM to the Unified Buffer (UB), completes the computation on the UB, and then writes the results back to GM.

The functional design described in this document is to determine, before writing these three types of code, what each type of code needs to express.

## Functional Design Around the Operator Prototype

Operator prototype design starts from operator semantics. Operator semantics describe what data the operator receives, what computation it performs, and what results it outputs. This step determines the input and output declarations in OpDef, and also determines which data the Tiling and kernel function need to be designed around.

AddCustom is an element-wise addition operator. Elements at the same position in inputs `x` and `y` are added, and the result is written to output `z`. The computation of each element is independent of the others, making it suitable for splitting into multiple Blocks along contiguous data segments for parallel processing.

```text
z[i] = x[i] + y[i]
```

Based on this semantics, you can first determine the prototype boundary exposed by the operator. AddCustom exposes two inputs `x` and `y` and one output `z`. These three Tensors already fully express the computation semantics of `z = x + y`, and each call requires all of them, so they are all mandatory parameters on the prototype boundary. AddCustom has no additional attributes, so the caller only needs to prepare two input Tensors and one output Tensor.

If the operator has configuration items such as scalars, dimensions, thresholds, and switches, you need to determine how they are carried during the functional design phase. Configuration items with small values that describe operator behavior are suitable as attributes; content that needs to be passed in as a Tensor, participates in dynamic graph data dependencies, or is large in size is suitable as an input. For a complete example of attribute identification and passing, see [Passing Attribute Information Through TilingData](./tiling_data_attributes.md).

In addition, you need to determine the scenarios supported by the operator:

- Data types and data formats.
- Determinism: whether the operator produces the same output for the same input.

Combining the inputs and outputs determined in the previous two steps with the dtype/format list yields the complete operator prototype definition. For the method of writing a complete operator prototype definition and API descriptions, see [Operator Prototype Definition](./operator_prototype_definition.md).

After completing the operator prototype design, the functional design can continue to break down the runtime parameters that the Host-side Tiling needs to prepare, and how the kernel function side completes the actual computation.

## Functional Design Around Host-Side Tiling

The focus of host-side Tiling design is to determine which runtime parameters need to be prepared before the kernel function is launched. Host-side Tiling is responsible for converting information such as the input shape and attributes into the parameters required by the kernel function at runtime, including the core-splitting strategy, intra-block splitting, workspace size, TilingKey, and the necessary scheduling mode. The AddCustom kernel function only needs to know the total data volume and the number of blocks into which the total data is divided, so TilingData is designed with two fields.

The first field is `totalLength`, which indicates the total number of elements in this computation. The kernel function side calculates the data volume processed by each block based on this value. The second field is `tileNum`, which indicates the number of blocks into which the total data is divided in the sample. The kernel function calculates the data volume processed in a single operation through `totalLength / tileNum`, and determines the number of loop iterations for each block together with `numBlocks`.

When writing the Tiling function, you need to convert these design conclusions into host-side computation steps. The AddCustom Tiling function needs to read the shape of the input `x` and calculate `totalLength`, set the `numBlocks` used for kernel function launch, write `tileNum`, and provide the workspace size. More complex operators may also need to set the TilingKey or scheduling mode based on the scenario.

The TilingData structure, Tiling function implementation, workspace configuration, `numBlocks` configuration, and related launch parameters are described in detail in the [Host-Side Tiling Implementation](./host_tiling_implementation.md) section.

## Functional Design Around the Kernel Function Implementation

The focus of the kernel function implementation design is to determine which computation API on the AI Core is used to implement the operator semantics. The semantics of AddCustom is element-wise addition, which corresponds to the `Add` basic API in Ascend C.

```text
z[i] = x[i] + y[i]
        ↓
AscendC::Add(zLocal, xLocal, yLocal, length)
```

Here, `xLocal` and `yLocal` represent the input data that has been moved into the UB, and `zLocal` represents the output data in the UB. `length` represents the number of elements involved in the current round of computation, which is determined by the kernel function based on the Tiling information passed from the Host side.

From the data flow perspective, AddCustom adopts the basic move-in, compute, and write-back process:

```text
GM(x, y) -> UB(xLocal, yLocal) -> Add -> UB(zLocal) -> GM(z)
```

At the functional design stage, you only need to clarify this computation mapping and the basic data flow. Code organization details such as the kernel function entry parameters and TilingData reading will be elaborated in the [Kernel Function Operator Implementation](./kernel_operator_implementation.md) chapter.

## Extending the Kernel Function Implementation for Different Scenarios

After completing the basic kernel function design, you need to determine which scenarios the current implementation can cover. If you later need to support more data types, a larger shape range, or runtime configurations, you must add extension strategies on top of the basic implementation. During the functional design phase, you need to first determine the extension dimensions and trigger conditions, and then describe the specific implementation of TilingKey, kernel function templates, or differentiated compilation configurations through the [multi-branch strategy](./multi_branch_strategy.md).

### Extending the Kernel Function Implementation by Data Type

When different dtypes have different computation paths, precision requirements, or intermediate buffers, the scope of the kernel function implementation must be evaluated by data type. Taking AddCustom as an example, if both `float32` and `float16` are to be supported later, the instruction support, data transfer alignment, UB usage, and Tile size must be evaluated separately.

```text
float32: Elements occupy more space, so fewer elements can be processed at a time with the same UB capacity
float16: Elements occupy less space, but the alignment requirements of data transfer and computation APIs must still be met
```

Operators with long intermediate computation chains, such as reduction, accumulation, or multiply-add, may need to use `float32` to store intermediate results to improve precision. However, if the final output is still `float16`, the final result must still be within the representable range of `float16`; increasing precision cannot expand the data range of the final output.

The key point of this design is to identify, during the functional design phase, whether dtype differences will change the kernel function computation path. If different paths need to be selected by dtype later, the corresponding flag can be set in the Host-side Tiling, and the kernel function side then selects the matching implementation.

### Extending the Kernel Function Implementation by Shape Size

When the data volume differs significantly, the splitting strategy suitable for different shapes may also differ. A small shape may be moved into the Unified Buffer (UB) at once to complete the computation, while a large shape usually needs to be processed in blocks and tiles through loops.

Taking AddCustom as an example, the shape boundary approach can be defined in the functional design phase: when the total number of elements is small, use fewer tiles to reduce loop overhead; when the total number of elements is large, increase the number of tiles so that each block processes data through multiple rounds of moving in, computing, and writing back. The specific threshold needs to be determined based on the UB capacity, alignment requirements, tail block handling, and performance verification results. The input shape of the accompanying basic sample is `[8, 2048]`, with 8 blocks fixed and `tileNum` set to 8. In this case, each block processes 2048 elements, and a single pass also processes 2048 elements, so each block loops only once. This sample covers only a fixed specification; when extending the shape range, the splitting parameters need to be recalculated, and the tail core and tail tile handling for non-divisible cases must be added.

### Extending the Kernel Function Implementation Based on Runtime Configuration

Runtime configuration may also affect the kernel function implementation path. A typical scenario is deterministic configuration: when the output must be completely stable for the same input, the kernel function needs to fix the computation order; when pursuing performance, certain reduction operators may adopt a more flexible parallel order.

AddCustom is element-wise addition, where elements have no dependencies on each other. The basic implementation uses fixed data tiling and computation order, which is a deterministic implementation. For operators with cross-element relationships such as reduction and sorting, you need to record the determinism requirement during the functional design phase and determine whether different kernel function paths need to be prepared.

### Summary of Multi-Branch Strategy Selection

For the selection criteria of TilingKey and kernel function templates, see [multi-branch strategy](./multi_branch_strategy.md).

## Next Steps

  - [Operator Prototype Definition](./operator_prototype_definition.md): inputs, outputs, attributes, and dtype/format.
  - [Host-side Tiling Implementation](./host_tiling_implementation.md): TilingData definition and Tiling function writing.
  - [Kernel Function (Kernel) Operator Implementation](./kernel_operator_implementation.md): data movement, computation APIs, and the kernel function programming process.
  - [Multi-branch Strategy](./multi_branch_strategy.md): methods for selecting TilingKey and different kernel function branches.
  - [Passing Attribute Information Through TilingData](./tiling_data_attributes.md): introduction to attribute reading and passing.

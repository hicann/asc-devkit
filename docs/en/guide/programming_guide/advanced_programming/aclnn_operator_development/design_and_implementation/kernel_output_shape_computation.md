# Output Shape Dependency Calculation

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T13:51:10.487Z pushedAt=2026-11-03T07:05:13.405Z nl=2 -->

This section is extended content that describes how to implement output shapes that depend on the kernel computation result. For operators such as NonZero, the actual output shape cannot be determined until the kernel computation is complete. Such outputs need to be marked by calling `OutputShapeDependOnCompute()` in the prototype definition, and the kernel writes the actual shape to the shape output address appended by the framework in a fixed format.

## Marking the Output in the Operator Prototype

`OutputShapeDependOnCompute()` can only be used for outputs. The following example indicates that the shape of output `y` needs to wait for the Kernel computation result:

```cpp
this->Output("y")
    .ParamType(REQUIRED)
    .DataType({ge::DT_UINT32})
    .Format({ge::FORMAT_ND})
    .OutputShapeDependOnCompute();
```

When an operator has multiple outputs, add this marker only to the outputs that actually depend on the computation. For details about how to declare inputs and outputs, see [Operator Prototype Definition](./operator_prototype_definition.md).

## Adding a Shape Output Parameter to the Kernel Entry

The framework appends a `GM_ADDR` parameter after all regular outputs and before `workspace` and `tiling` to store the output shape that depends on computation. The kernel entry maintains the following order:

```text
Input -> Regular output -> Shape output -> Workspace -> Tiling
```

Example:

```cpp
extern "C" __global__ __aicore__ void non_zero_custom(
    GM_ADDR x, GM_ADDR y, GM_ADDR shapeOut,
    GM_ADDR workspace, GM_ADDR tiling)
{
    // Compute y and write the actual shape of y to shapeOut.
}
```

Even if multiple outputs depend on computation, only one shape output address is added. The shape information of each output is arranged consecutively in this address in the order of the prototype outputs.

## Shape Information Layout Format

The shape output is organized as a `uint64_t` array. Each output to be refreshed occupies a fixed 9 `uint64_t` elements:

| Offset | Content |
|---|---|
| `0` | rank, that is, the actual number of dimensions |
| `1` to `8` | Size of each dimension, up to 8 dimensions |

If there are `n` outputs that depend on computation, the total space is `n * 9` `uint64_t` elements. The `i`-th output starts from `i * 9`, and the order is consistent with the output declaration order in the operator prototype.

![shape information layout diagram](../../../../figures/pipe_task_run_33.png)

For the scenario where the output Tensor data type is `uint64_t`, bit 31 of the rank value must be set to 1, indicating that subsequent dimensions are parsed as `uint64_t`:

```cpp
constexpr uint64_t UINT64_SHAPE_FLAG = 0x0000000080000000ULL;
uint64_t encodedRank = UINT64_SHAPE_FLAG | rank;
```

When the output Tensor is `uint32_t`, this flag is not set, and the rank is written directly.

## Writing a Single Output Shape

Assume that the data type of output `y` is `uint32_t`, and the actual shape obtained after the kernel computation is `[32, 64]`:

```cpp
extern "C" __global__ __aicore__ void non_zero_custom(
    GM_ADDR x, GM_ADDR y, GM_ADDR shapeOut,
    GM_ADDR workspace, GM_ADDR tiling)
{
    // The operator computation logic is omitted here.

    constexpr uint32_t SHAPE_SLOT_SIZE = 9;
    AscendC::GlobalTensor<uint64_t> shapeGlobal;
    shapeGlobal.SetGlobalBuffer(
        reinterpret_cast<__gm__ uint64_t*>(shapeOut), SHAPE_SLOT_SIZE);

    shapeGlobal.SetValue(0, 2);   // rank
    shapeGlobal.SetValue(1, 32);  // dim[0]
    shapeGlobal.SetValue(2, 64);  // dim[1]
}
```

Indexes 3 to 8 are reserved slots for this output and do not store data of other outputs.

When the data type of the output tensor is `uint64_t` and the actual shape is `[1, 64, 128, 128]`, the type flag needs to be set:

```cpp
constexpr uint32_t SHAPE_SLOT_SIZE = 9;
constexpr uint64_t UINT64_SHAPE_FLAG = 0x0000000080000000ULL;
AscendC::GlobalTensor<uint64_t> shapeGlobal;
shapeGlobal.SetGlobalBuffer(
    reinterpret_cast<__gm__ uint64_t*>(shapeOut), SHAPE_SLOT_SIZE);

shapeGlobal.SetValue(0, UINT64_SHAPE_FLAG | 4ULL);
shapeGlobal.SetValue(1, 1);
shapeGlobal.SetValue(2, 64);
shapeGlobal.SetValue(3, 128);
shapeGlobal.SetValue(4, 128);
```

## Writing Multiple Output Shapes

Assume that both outputs depend on computation, both have the data type `uint64_t`, and their actual shapes are `[16, 32]` and `[1, 16, 16, 32]` respectively:

```cpp
constexpr uint32_t SHAPE_SLOT_SIZE = 9;
constexpr uint32_t OUTPUT_COUNT = 2;
constexpr uint32_t SHAPEOUT_SIZE = SHAPE_SLOT_SIZE * OUTPUT_COUNT;
constexpr uint64_t UINT64_SHAPE_FLAG = 0x0000000080000000ULL;

AscendC::GlobalTensor<uint64_t> shapeGlobal;
shapeGlobal.SetGlobalBuffer(
    reinterpret_cast<__gm__ uint64_t*>(shapeOut), SHAPEOUT_SIZE);

// Output 0: [16, 32]
shapeGlobal.SetValue(0, UINT64_SHAPE_FLAG | 2ULL);
shapeGlobal.SetValue(1, 16);
shapeGlobal.SetValue(2, 32);

// Output 1: [1, 16, 16, 32], with a starting offset of 9.
shapeGlobal.SetValue(9, UINT64_SHAPE_FLAG | 4ULL);
shapeGlobal.SetValue(10, 1);
shapeGlobal.SetValue(11, 16);
shapeGlobal.SetValue(12, 16);
shapeGlobal.SetValue(13, 32);
```

When the operator also contains regular outputs that do not depend on computation, the shape output area allocates slots only for the outputs for which `OutputShapeDependOnCompute()` is called, but the slot order still follows the order of these outputs in the prototype.

## Related Documents

- [Kernel-side Operator Implementation](./kernel_operator_implementation.md): Kernel entry format and parameter order.
- [Operator Prototype Definition](./operator_prototype_definition.md): Input/output declaration and shape derivation relationship.
- [OutputShapeDependOnCompute](../../../../../api/Utils-API/prototype_register_management/OpParamDef/OutputShapeDependOnCompute.md): API definition and project version constraints.

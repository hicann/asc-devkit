# binary_partition

<!-- md-trans-meta sourceCommit=2737c34bbaf046b0c4e915117e38247d0f621f63 translatedAt=2026-09-07T07:03:58.481Z pushedAt=2026-09-07T09:11:27.141Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

The `binary_partition` API divides a parent group into two subgroups based on a label (0 or 1). Threads with the same label are assigned to the same group.

## Function Prototype

```c++
coalesced_group binary_partition(const coalesced_group& g, bool pred)
```

```c++
template <unsigned int Size, typename ParentT>
coalesced_group binary_partition(const thread_block_tile<Size, ParentT>& g, bool pred)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| g | Input | Parent group to be divided. The type can be `coalesced_group` or `thread_block_tile`. |
| pred | Input | Label used to divide the subgroups. |

## Return Value

Returns the divided `coalesced_group` object.

## Constraints

None

## Example

- SIMT programming scenario:

    ```c++
    using namespace cooperative_groups;
    __global__ void simt_kernel(int *inputArr)
    {
        auto block = this_thread_block();
        auto tile32 = tiled_partition<32>(block);

        // inputArr contains random integers.
        int elem = inputArr[block.thread_rank()];
        // Divide tile32 into two subgroups based on whether elem&1 is true.
        auto subtile = binary_partition(tile32, (elem & 1));
        ...
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```c++
    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel(...)
    {
        ...
        auto block = this_thread_block();
        auto tile32 = tiled_partition<32>(block);

        // inputArr contains random integers.
        int elem = inputArr[block.thread_rank()];
        // Divide tile32 into two subgroups based on whether elem&1 is true.
        auto subtile = binary_partition(tile32, (elem & 1));
        ...
    }
    ```

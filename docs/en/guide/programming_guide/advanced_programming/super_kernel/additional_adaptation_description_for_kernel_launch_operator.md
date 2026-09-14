# Additional Adaptation Description for Kernel Launch Operators

<!-- md-trans-meta sourceCommit=bca43370d7874612de792971005975332093ee83 translatedAt=2026-08-26T12:10:51.514Z pushedAt=2026-09-03T09:23:37.495Z -->

For Ascend C operators developed using the `<<<>>>` method, in addition to following the general constraints in [Operator Adaptation Description](operator_adaptation_description.md), you need to add a SuperKernel entrypoint function on the operator kernel entry side. This document describes the specific adaptation method.

>[!NOTE] Note
>Kernel launch operators currently support entering SuperKernel only on the npugraph_ex backend, and do not support GE graph mode.

## Differences Between SuperKernel and Regular Kernel

### Entrypoint Function of a Regular Kernel

- Function signature: `__global__ __vector__ void func(parameter list)`
- Parameters are passed directly as function parameters.
- It can be called directly.

### SuperKernel Entrypoint Function

- Function signature: `__sk__ __vector__ void func(const ArgsStruct *args, sk::SkSystemArgs *sysArgs)`
- Parameters are passed through a structure pointer. You need to define a parameter structure and encapsulate the input parameters.
- You can optionally add the `sk::SkSystemArgs *sysArgs` parameter to obtain system information (such as block num).
- It needs to be bound to the regular kernel function through the SK_BIND macro.

## SuperKernel Adaptation Rules

### SK Sub-function Writing Rules

#### Rule 1: Function Signature Format

```cpp
__sk__ __vector__ void function_name_sk(const ArgsStruct *args, sk::SkSystemArgs *sysArgs)
```

> **Note**: Only `__sk__` is a mandatory constraint and must appear in the function signature.

#### Rule 2: Keeping the Kernel Type Identifier Consistent with That of the Original Global Function

- If the original global function uses `__cube__`, the SK sub-function must also use `__cube__`.
- If the original global function uses `__vector__`, the SK sub-function must also use `__vector__`.
- If the original global function uses `__mix__(c, v)`, the SK sub-function must also use `__mix__(c, v)`.

**Example**:

```cpp
// The original global function is of the cube type.
__global__ __cube__ void func(GM_ADDR x, GM_ADDR y);
// The SK sub-function also requires the cube identifier.
__sk__ __cube__ void func_sk(const ArgsStruct *args, sk::SkSystemArgs *sysArgs);

SK_BIND(func, 4, func_sk);

// The original global function is of the vector type.
__global__ __vector__ void func_vec(GM_ADDR x, GM_ADDR y);
// The SK sub-function also requires the vector identifier.
__sk__ __vector__ void func_vec_sk(const ArgsStruct *args, sk::SkSystemArgs *sysArgs);

SK_BIND(func_vec, 4, func_vec_sk);

// The original global function is of the mix type.
__global__ __mix__(1, 2) void func_mix(GM_ADDR x, GM_ADDR y);
// The SK sub-function requires the mix identifier.
__sk__ __mix__(1, 2) void func_mix_sk(const ArgsStruct *args, sk::SkSystemArgs *sysArgs);

SK_BIND(func_mix, 4, func_mix_sk);

// The original global function is of the mix(1, 0) type.
__global__ __mix__(1, 0) void func_mix_10(GM_ADDR x, GM_ADDR y);
// The SK sub-function requires the cube identifier.
__sk__ __mix__(1, 0) void func_mix_10_sk(const ArgsStruct *args, sk::SkSystemArgs *sysArgs);

SK_BIND(func_mix_10, 4, func_mix_10_sk);

// The original global function is of the mix(0, 1) type.
__global__ __mix__(0, 1) void func_mix_01(GM_ADDR x, GM_ADDR y);
// The SK sub-function requires the vector identifier.
__sk__ __mix__(0, 1) void func_mix_01_sk(const ArgsStruct *args, sk::SkSystemArgs *sysArgs);

SK_BIND(func_mix_01, 4, func_mix_01_sk);
```

#### Rule 3: Defining the Parameter Structure

You need to define a structure based on the actual parameters of the original global function to encapsulate all input parameters:

```cpp
// Example: Assume the original global function is
// __global__ __vector__ void my_kernel(GM_ADDR x, GM_ADDR y, uint32_t length, int16_t flag)
// The corresponding parameter structure is defined as follows:
struct MyArgs {
    GM_ADDR x;                      // First parameter
    GM_ADDR y;                      // Second parameter
    uint32_t length;                // Third parameter
    alignas(4) int16_t flag;        // Fourth parameter (smaller than 4 bytes, so alignas(4) is required)
};
```

> **Note**:
> - The structure must be defined based on the parameters of the actual operator kernel function (global function). Do not directly copy the field names in the example.
> - The structure fields must be arranged in the same order as the original function parameters.
> - For types smaller than 4 bytes (such as `int16_t`, `int8_t`, `uint16_t`, and `uint8_t`), you must use `alignas(4)` to ensure memory alignment (as required by the ABI).
> - Examples: `alignas(4) int16_t val;`, `alignas(4) uint8_t flag;`

#### Rule 4: Optionally Adding the SkSystemArgs Parameter

If the original operator uses the `AscendC::GetBlockNum()` API, in the SK scenario you can add the `sk::SkSystemArgs *sysArgs` parameter to obtain the expected block num:

```cpp
// Scenario 1: The operator uses AscendC::GetBlockNum() and needs to add the sysArgs parameter.
template<uint32_t splitNum>
__sk__ __vector__ void func_sk(const ArgsStruct *args, sk::SkSystemArgs *sysArgs)
{
    // Obtain the block num from the system parameters (replacing AscendC::GetBlockNum()).
    uint32_t blockNum = sysArgs->skNumBlocks;
    // Or obtain it through the API.
    uint32_t blockNum = sysArgs->SkGetNumBlocks();
    // Code that uses blockNum later
    ...
}

// Scenario 2: The operator does not use AscendC::GetBlockNum(), so the sysArgs parameter is not required.
template<uint32_t splitNum>
__sk__ __vector__ void func_sk(const ArgsStruct *args)
{
    // Perform computation directly with the parameters, without accessing system information.
    GM_ADDR x = args->x;
    GM_ADDR y = args->y;
    // Subsequent computation code.
    ...
}
```

> **Note:**
> - The `sk::SkSystemArgs *sysArgs` parameter is optional. If the operator does not use block num information, or if the functionality provided by `sk::SkSystemArgs` (currently only block num retrieval is supported) is not needed, you do not need to pass the `sysArgs` parameter to the function entrypoint, which avoids unnecessary code modifications.
> - You need to modify the `AscendC::GetBlockNum()` calls in the original code according to the actual situation:
>   - Directly access the member variable through `sysArgs->skNumBlocks`.
>   - Or obtain it through the `sysArgs->SkGetNumBlocks()` API.
> - The logical difference from `AscendC::GetBlockNum()` is that in the MIX 1:2 scenario, `skBlockNum` in the vector core is `blockNum * 2`.

#### Rule 5: Manually Calling the DestroyWithoutPipeAll API When TPipe Is Used

To optimize synchronization between SK functions, if a TPipe object is used, call the `DestroyWithoutPipeAll()` API to proactively destroy the TPipe (without calling PipeBarrierAll) before its natural destruction.

```cpp
template<uint32_t splitNum>
__sk__ __vector__ void func_sk(const ArgsStruct *args, sk::SkSystemArgs *sysArgs)
{
    ...
    // Scenario where TPipe is used
    TPipe pipe;
    ...
    // Manually call the API to destroy the TPipe instead of relying on the default destructor.
    pipe.DestroyWithoutPipeAll();
}
```

#### Rule 6: Logic Consistent with the Global Function

The internal logic implementation of the SK sub-function must be completely consistent with the original global function, including all subsequent logic such as data initialization, computation processing, and result output. Ensure that the SK sub-function is functionally equivalent to the original global function.

### SK_BIND Binding Rules

#### Rule 7: Binding with the SK_BIND Macro

```cpp
SK_BIND(original_func, 4, sk_func<0>, sk_func<1>, sk_func<2>, sk_func<3>);
```

**Parameter description**:

| Parameter | Description |
| --- | --- |
| `original_func` | Original global function. If it is a template function, it needs to be manually instantiated. |
| `4` | Mask parameter (uint64_t type), used to configure SuperKernel features. |
| `sk_func<0>` ~ `sk_func<3>` | Up to 4 SK sub-functions. If they are template functions, they need to be manually instantiated. |

**Mask parameter values**:
1/2/4/8 respectively represent the results of setting different bit positions. For example, 8 is the result of setting the 4th bit.

| Value | Meaning |
| --- | --- |
| 1 | early start wait flag |
| 2 | early start set flag |
| 4 | disable_dcci |
| 8 | disable_batchmode_check |

> **Note**:
> - SK sub-functions use template parameters to instantiate multiple symbols (recommended, as the code is more concise).
> - Template parameters are only used to instantiate different symbols and do not affect the function logic.

## Complete Adaptation Example

### Original Regular Kernel

```cpp
// Regular kernel definition
__global__ __vector__ void add_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z, uint32_t totalLength)
{
    KernelAdd op;
    op.Init(x, y, z, totalLength);
    op.Process();
}
```

### SuperKernel Adaptation Version

```cpp
// Keep the original regular kernel (for non-SuperKernel scenarios).
__global__ __vector__ void add_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z, uint32_t totalLength)
{
    KernelAdd op;
    op.Init(x, y, z, totalLength);
    op.Process();
}

// Rule 3: Define the parameter structure (based on the actual parameters of the original global function).
struct AddCustomArgs {
    GM_ADDR x;                      // Corresponds to the first parameter of add_custom
    GM_ADDR y;                      // Corresponds to the second parameter of add_custom
    GM_ADDR z;                      // Corresponds to the third parameter of add_custom
    uint32_t totalLength;           // Corresponds to the fourth parameter of add_custom
};

// Define an SK sub-function with a template parameter.
// The template parameter is used only to instantiate different symbols and does not affect the function logic
template<uint32_t splitNum>
__sk__ __vector__ void add_custom_sk(const AddCustomArgs *args, sk::SkSystemArgs *sysArgs /*Optionally add the sysArgs parameter*/)
{
    // Obtain parameters from the structure.
    GM_ADDR x = args->x;
    GM_ADDR y = args->y;
    GM_ADDR z = args->z;
    uint32_t totalLength = args->totalLength;
    // Rule 6: The logic is consistent with the global function.
    KernelAdd op;
    op.Init(x, y, z, totalLength);
    op.Process();
}

// Rule 7: Use SK_BIND for binding.
// Instantiate four different symbols by specifying template parameters.
SK_BIND(add_custom, 4, add_custom_sk<0>, add_custom_sk<1>, add_custom_sk<2>, add_custom_sk<3>);
```

## Adaptation Procedure Summary

**Step 1: Keep the original global function.**

The original `__global__ __vector__` function remains unchanged and continues to be used for non-SuperKernel scenarios.

**Step 2: Create the SK sub-function.**

1. Copy the code of the original global function.
2. Define the parameter structure based on the actual parameters of the original global function (use `alignas(4)` for types smaller than 4 bytes).
3. Change the function signature to `__sk__ __vector__ void func_sk(const ArgsStruct *args, sk::SkSystemArgs *sysArgs)` (add the corresponding identifier based on the kernel type of the original function).
4. If the operator does not use `AscendC::GetBlockNum()`, the `sk::SkSystemArgs *sysArgs` parameter can be omitted.
5. Use template parameters to instantiate multiple symbols (template parameters are used only to instantiate different symbols and do not affect the function logic).
6. Obtain parameters from the structure.
7. If `AscendC::GetBlockNum()` is used, change it to use `sysArgs->skNumBlocks` or `sysArgs->SkGetNumBlocks()`.
8. When TPipe is used, manually call the `DestroyWithoutPipeAll` API to destroy TPipe.
9. Keep the internal logic of the function exactly the same as the original global function.

**Step 3: Add SK_BIND binding.**

1. Use the `SK_BIND` macro to bind the original global function with the SK sub-function.
2. Support up to 4 SK sub-functions.
3. The second parameter is a mask (of type uint64_t), used to configure SuperKernel features:
   - `8` indicates **disable batchmode check**.
   - `4` indicates **disable dcci**.
   - `2` indicates **early start set flag**.
   - `1` indicates **early start wait flag**.

## Precautions

### Precautions for Parameter Structure Definition

- The structure must be defined based on the parameters of the actual operator kernel function (global function), and do not directly copy the field names in the example.
- The structure fields must be arranged in the same order as the original function parameters.
- For types smaller than 4 bytes (such as `int16_t`, `int8_t`, `uint16_t`, and `uint8_t`), `alignas(4)` must be used to ensure memory alignment (as required by the ABI).
- Example: `alignas(4) int16_t val;` and `alignas(4) uint8_t flag;`.

### Precautions for Using System Parameters

- `sk::SkSystemArgs *sysArgs` is an optional parameter. It needs to be added only when the original operator uses the `AscendC::GetBlockNum()` API.
- If the original operator does not use the `AscendC::GetBlockNum()` API, there is no need to pass the `sysArgs` parameter at the function entry, which avoids unnecessary code modifications.
- If the original operator uses the `AscendC::GetBlockNum()` API, you can add the `sk::SkSystemArgs *sysArgs` parameter to obtain the expected block num in the SK scenario:
  - Directly use `sysArgs->skNumBlocks` to access the member variable.
  - Or use the `sysArgs->SkGetNumBlocks()` API to obtain it.
- You need to modify the original code based on the actual situation and replace the call to `AscendC::GetBlockNum()` with one of the two methods above.
- The logical difference from `AscendC::GetBlockNum()` is that in the MIX 1:2 scenario, `skBlockNum` in the vector core is `blockNum * 2`.

### Binding Restrictions

- `SK_BIND` supports binding up to 4 SK sub-functions.
- The template parameters of SK sub-functions increment from 0 (`<0>` to `<3>`).
- You can bind 4 or fewer functions based on actual requirements.

### Mask Binding Parameter Description

The second parameter of the `SK_BIND` macro is a mask (of the uint64_t type), which is used to configure SuperKernel features:

| Value | Meaning |
| --- | --- |
| 8 | disable batchmode check |
| 4 | disable dcci |
| 2 | early start set flag |
| 1 | early start wait flag |

## FAQs

**Q1: Why do I need to define a parameter structure?**

A: SuperKernel uses a structure to pass parameters. You need to define a structure based on the actual parameters of the original global function to encapsulate all input parameters, which enables unified parameter management and sharing across splits. Note that the structure must be defined according to the parameters of your actual operator kernel function (global function). Do not directly copy the field names from the example.

**Q2: Why do types smaller than 4 bytes require alignas(4)?**

A: This is required by the Application Binary Interface (ABI) to ensure memory alignment and avoid access exceptions or performance issues.

**Q3: Can the logic of the SK sub-function differ from the original function?**

A: No. The SK sub-function must keep all logic after parameter retrieval consistent with the original global function. Otherwise, the computation result may be incorrect.

**Q4: What if I only need one or two SK sub-functions?**

A: You can specify only the required number of functions in `SK_BIND`, for example:

```cpp
SK_BIND(add_custom, 4, add_custom_sk<0>, add_custom_sk<1>);
```

**Q5: Is the sk::SkSystemArgs parameter mandatory?**

A: No, it is not mandatory. The `sk::SkSystemArgs *sysArgs` parameter is optional and is added only when the operator needs to use this feature. Currently, it only supports obtaining the block num.

**Q6: Does the original global function still need to be retained?**

A: Yes. The original global function continues to be used in non-SuperKernel scenarios, so both deployment modes can be supported simultaneously.

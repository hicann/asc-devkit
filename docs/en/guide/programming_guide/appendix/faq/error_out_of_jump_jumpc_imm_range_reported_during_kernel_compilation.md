# Kernel Compilation Error "error: out of jump/jumpc imm range"<a name="ZH-CN_TOPIC_0000002033872222"></a>

<!-- md-trans-meta sourceCommit=d7f8cd8648f37bc3a4868cb030f666b2f7fd6d4d translatedAt=2026-08-26T11:24:13.384Z pushedAt=2026-08-31T10:51:17.254Z -->

## Symptoms<a name="section151611254194612"></a>

When developing operators using the engineering-based operator development method based on a custom operator project, the operator compilation fails with the following error:

```
[ERROR] [ascendxxxx] PowerCustom_88a695f03edfbc0af76b9eaae9e4556c error: out of jump/jumpc imm range
```

## Root Cause<a name="section417961104715"></a>

This compilation error occurs because the operator kernel code is too large, causing the offset value of the jump instruction to exceed the limited range (the data range of int16_t) during compilation. You can add the compilation option "-mllvm -cce-aicore-jump-expand=true" to use indirect jumps to avoid this issue and allow the compiler to compile normally.

## Procedure<a name="section166318242419"></a>

1.  In the CMakeLists on the kernel side, use `add_ops_compile_options` to add the compilation option "-mllvm -cce-aicore-jump-expand=true" for the operator that reports the error. The following is an example:

    ```
    add_ops_compile_options(PowerCustom OPTIONS -mllvm -cce-aicore-jump-expand=true)
    ```

    For details about how to use `add_ops_compile_options`, see [Supporting Custom Compile Options](../../advanced_programming/aclnn_operator_development/operator_package_compilation/operator_project_compilation.md#section7321165972311).

2.  Recompile the operator. The compilation succeeds without errors.


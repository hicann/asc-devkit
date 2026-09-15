# Disabling ASCENDC_DUMP

<!-- md-trans-meta sourceCommit=777f0bb39da01f06996080e108a6f7865b3e6c07 translatedAt=2026-08-27T11:40:18.286Z -->

## Description

Debugging APIs (such as the on-board printing and anomaly detection APIs described in this chapter) affect operator performance after being enabled. They are typically used during the debugging phase and should be disabled in production environments. The **ASCENDC_DUMP** macro definition is the switch that controls the printing of debugging APIs.

## Example

- CMake compilation option control method
    - Kernel direct invocation project
        Modify the `ascendc_compile_definitions` command in the CMakeLists.txt file to add the `ASCENDC_DUMP=0` macro definition, thereby disabling the ASCENDC_DUMP switch. The following is an example:
        ```plain
        // Disable the print function of all operators.
        ascendc_compile_definitions({kernel_target_name} PRIVATE
            ASCENDC_DUMP=0
        )
        ```

    - Custom operator project
        Modify `add_ops_compile_options` or `npu_op_kernel_options` in the CMakeLists.txt file under the op_kernel directory of the operator project, and add the compilation option `-DASCENDC_DUMP=0` on the first line to disable the ASCENDC_DUMP switch. The following is an example:
        ```plain
        // Use add_ops_compile_options to disable the print function of all operators.
        ascendc_compile_definitions(ALL OPTIONS -DASCENDC_DUMP=0)
        // Use npu_op_kernel_options to disable the print function of all operators.
        ascendc_compile_definitions(ascendc_kernels ALL OPTIONS -DASENDC_DUMP=0)
        ```

    - Macro definition control method
Add #define ASCENDC_DUMP 0 at the beginning of the kernel code file. The following is an example:
        ```cpp
        #define ASCENDC_DUMP 0 // Add before including the header file. including the header file.
        #include "..." // Include the required header file.the required header file.
        ```

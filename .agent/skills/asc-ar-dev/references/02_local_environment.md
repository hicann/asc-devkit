# Environment And Repository Resolution

Use this file when paths, builds, tools, or installation layout matter.

## Required Inputs

- `DEVKIT_PATH`: the asc-devkit repository path, derived from the current workspace or this skill directory when the skill is used inside asc-devkit, or provided by the user when another repository is the target.
- `CANN_PATH`: the CANN package installation path provided by the user, embedded in `TASK_GOAL`, or resolved from environment variables.
- `SOC_ARCH`: target SoC architecture provided by the user or embedded in `TASK_GOAL`.
- `TASK_GOAL`: task goal in the format of `references/06_task_goal_example.md`.

Never substitute a hard-coded path for any input value.

## Devkit Path Discovery

When this skill is used from the asc-devkit repository, do not ask the user for the asc-devkit path first. Derive `DEVKIT_PATH` by walking upward from the current workspace or this skill directory until a repository root is found.

A candidate root should normally contain:

- `CMakeLists.txt`
- `include/`
- `impl/`
- `tools/`
- `.agent/`

Record the derived `DEVKIT_PATH` in the execution notes before using repository examples, docs, builds, tests, patches, or source inspection. If no valid root is found, ask for `DEVKIT_PATH` instead of guessing.

If the user explicitly provides another target repository, validate that path against the task-relevant files before using it.

## Missing Input Prompt

If the request appears to need this skill and the input bundle is missing, first extract `DEVKIT_PATH`, `CANN_PATH`, and `SOC_ARCH` from `TASK_GOAL` if they are present under explicit keys or clear natural-language wording. Resolve `DEVKIT_PATH` from the current workspace or skill path when possible. Ask only for values still missing:

```md
请提供本次 AscendC 任务的输入件：

- DEVKIT_PATH: <AscendC devkit 仓路径；若使用当前 asc-devkit 仓或任务目标中已包含可省略>
- CANN_PATH: <CANN 包安装路径；若任务目标中已包含或环境变量可解析可省略>
- SOC_ARCH: <soc 架构；若任务目标中已包含可省略，例如 dav-2201>
- TASK_GOAL: <按 references/06_task_goal_example.md 的四段格式填写>
```

## CANN Path Discovery

Resolve `CANN_PATH` at task time:

1. If `ASCEND_HOME_PATH` is set and non-empty, use it as the first candidate.
2. Otherwise, if `ASCEND_TOOLKIT_HOME` is set and non-empty, use it as the second candidate.
3. If `ASCEND_TOOLKIT_HOME` names a `toolkit` subdirectory and the parent contains a CANN `tools/` tree, normalize `CANN_PATH` to the parent directory.
4. Validate the candidate against the task. Common probes:
   - `${CANN_PATH}/tools/bisheng_compiler/bin/bisheng`
   - `${CANN_PATH}/tools/show_kernel_debug_data`
   - `${CANN_PATH}/python/site-packages/show_kernel_debug_data`
   - task-specific include, library, OPP, compiler, or toolkit paths
5. If no environment variable is set or validation fails, ask the user to provide the CANN package installation path. Do not guess.

## Useful Repository Documentation

- DumpTensor API:
  `${DEVKIT_PATH}/docs/zh/api/context/DumpTensor.md`
- NPU board debugging guide:
  `${DEVKIT_PATH}/docs/zh/guide/programming_guide/debug_and_tuning/functional_debug/npu_board_debug.md`
- `show_kernel_debug_data` guide:
  `${DEVKIT_PATH}/docs/zh/guide/programming_guide/appendix/show_kernel_debug_data_tool.md`

## Build Shortcut

- For `.asc` test cases, prefer direct compilation with `${CANN_PATH}/tools/bisheng_compiler/bin/bisheng` after the CANN path is resolved.
- Select the target chip with `--npu-arch=<arch>` such as `--npu-arch=dav-2201`; for simple compile or smoke validation, no extra build system is required.
- For repository regression, inspect current `build.sh`, `tests/test_parts.sh`, and relevant `tests/**/CMakeLists.txt` before choosing a test target.

## Repository Code Areas Worth Reusing

- Basic API and high-level API declarations:
  `${DEVKIT_PATH}/include`
- Basic API and high-level API implementations:
  `${DEVKIT_PATH}/impl`
- Compile-engineering tools:
  `${DEVKIT_PATH}/tools`
- Build or auxiliary scripts:
  `${DEVKIT_PATH}/scripts`
- CMake and toolchain integration:
  `${DEVKIT_PATH}/cmake`
- Bootstrapping and basic launch:
  `${DEVKIT_PATH}/examples/01_simd_cpp_api/00_introduction`
- Debug utilities:
  `${DEVKIT_PATH}/examples/01_simd_cpp_api/01_utilities`
- Compatibility and memory-path samples:
  `${DEVKIT_PATH}/examples/01_simd_cpp_api/05_compatibility_guide`
- Compile and metadata pipeline:
  `${DEVKIT_PATH}/tools/build/asc_op_compile_base/asc_op_compiler`

## Official References

- Doc structure overview:
  `https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_map_10_0004.html`
- Host and Device model:
  `https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_10_00028.html`
- Call-layer reference:
  `https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_10_10071.html`

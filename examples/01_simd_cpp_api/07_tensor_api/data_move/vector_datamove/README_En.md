# Vector Datamove Sample

## Overview

This sample demonstrates the four AIV-side Tensor API vector copy paths: `GM->UB`, `UB->GM`,
`UB->UB`, and `UB->L1`, as well as constant left and right padding for `GM->UB`. Scenarios 1 through
4 use a `[32, 32]` `int8_t` tensor. Scenario 5 pads a `[32, 24]` input to a `[32, 32]` output. Every
scenario validates the complete output.

## Supported Products

- Ascend 950PR/Ascend 950DT

## Scenarios

| SCENARIO_NUM | Primary path | Layout | API form |
| --- | --- | --- | --- |
| 1 | GM->UB | ND | `copy(dst, src)` with automatic path inference |
| 2 | UB->GM | DN | `make_copy(..., trait)` followed by `atom.call(dst, src)` |
| 3 | UB->UB | NZ | Explicit atom with the coordinate/shape copy overload |
| 4 | UB->L1 | ZN | Explicit `copy(atom, dst, src)` with AIV execution completion validation |
| 5 | GM->UB | ND | Three left-padding and five right-padding elements, with `asc_set_copy_pad_val` configuring the value |

All five kernels are declared with `__global__ __vector__`. Auxiliary copies only prepare input or return
the result to GM. On dav-3510, L1->UB is effective only on AIC, so a pure AIV kernel cannot read L1
back. Scenario 4 verifies that UB->L1 compiles, is issued, and completes synchronization, then returns
the source UB to GM as a kernel completion check.

Scenario 5 uses `gm_to_ub_params{3, 5, true}` to configure three left-padding and five right-padding
elements per row. `asc_set_copy_pad_val(PADDING_VALUE)` sets each padding element to `-9`.

## Build and Run

After setting up the CANN environment, run the following commands from the sample directory. This
example runs scenario 1 in simulator mode:

```bash
source ${install_path}/set_env.sh
cmake -S . -B build \
    -DCMAKE_ASC_RUN_MODE=sim \
    -DCMAKE_ASC_ARCHITECTURES=dav-3510 \
    -DSCENARIO_NUM=1
cmake --build build -j
cd build
python3 ../scripts/gen_data.py -scenarioNum=1
./demo
```

The data-generation `scenarioNum` must match the build-time `SCENARIO_NUM`.

A successful run prints:

```text
test pass!
```

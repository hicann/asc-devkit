---
okftype: reference.concept
description: "Describes the purpose and timing of system initialization: restores control state that may be retained across tasks to the C API-specified default values when an operator starts execution."
---
# System Initialization

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.596566+00:00 pushedAt=2026-11-16T02:10:57.885Z nl=2 -->

System initialization restores control states that may persist across tasks on the NPU to the default values defined by the C API when an operator starts executing. It is recommended to call `asc_init` at the kernel entry, before data movement and computation, to prevent atomic operations, masks, or data movement loop configurations left by preceding tasks from affecting the current task.

`asc_init` initializes hardware control states and does not clear user data in GM, L1, L0, UB, or registers. For Ascend 950PR&950DT products, the API initializes atomic operations, vector masks, CTRL states, and loop configurations for data movement between GM and UB. For details, see the API documentation.

## API List

- **[asc_init](asc_init.md)**

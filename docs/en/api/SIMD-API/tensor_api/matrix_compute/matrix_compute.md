# Cube Computation (Tensor API)

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T02:29:49.029Z pushedAt=2026-11-19T05:54:06.637Z nl=2 -->

Cube computation consists of data move-in, `mmad` computation, and result move-out. At each stage, the corresponding Arch path is selected based on the storage location and Layout of the Tensor, and the Trait and runtime Params are configured through the Atom.

- **[Overview](overview.md)**
- **[Cube Computation Fractal Introduction](cube_compute_fractal_intro.md)**
- **[Cube Computation Move-In](cube_compute_load/cube_compute_load.md)**
- **[mmad Computation](mmad_compute/mmad_compute.md)**
- **[mmad Computation Key Features](mmad_compute_key_features/mmad_compute_key_features.md)**
- **[mmad Computation Configuration](mmad_compute_config/mmad_compute_config.md)**
- **[mmad Computation Auxiliary Configuration APIs](mmad_compute_aux_config.md)**
- **[Cube Data Fill](cube_data_fill/cube_data_fill.md)**
- **[Cube Computation Move-Out](cube_compute_store/cube_compute_store.md)**
- **[Cube Move-Out Key Features](cube_store_key_features/cube_store_key_features.md)**

For the general combination method of Operation, Trait, and Atom, see [Algorithm](../algorithm.md).

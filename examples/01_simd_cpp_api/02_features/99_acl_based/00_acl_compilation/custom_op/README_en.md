# Custom Operator Project Build, Packaging, and Deployment Example

## Overview

This example uses a simple custom operator to demonstrate the process of building, packaging into a custom operator package, and deploying to the CANN environment.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |
| Atlas 200I/500 A2 Inference Products | >= CANN 9.0.0 |
| Atlas Inference Series Products | >= CANN 9.0.0 |

> Notice: This example involves multiple operator examples. Refer to the actual product models supported by each operator example.

## Directory Structure

```
├── custom_op
│   ├── CMakeLists.txt               // Top-level build project file
│   ├── framework                    // Framework adaptation plugin directory
│   │   ├── CMakeLists.txt           // Framework adaptation build project file
│   │   ├── onnx_plugin              // ONNX framework adaptation plugin
│   │   └── tf_plugin                // TensorFlow framework adaptation plugin
│   ├── op_host                      // Host-side operator implementation directory
│   │   ├── CMakeLists.txt           // Host-side build project file
│   │   ├── add_custom               // Add operator host implementation
│   │   ├── add_custom_template      // Add template operator host implementation
│   │   ├── add_custom_tiling_sink   // Add Tiling sink operator host implementation
│   │   └── leaky_relu_custom        // LeakyRelu operator host implementation
│   ├── op_kernel                    // Kernel-side operator implementation directory
│   │   ├── CMakeLists.txt           // Kernel-side build project file
│   │   ├── add_custom               // Add operator kernel implementation
│   │   ├── add_custom_template      // Add template operator kernel implementation
│   │   ├── add_custom_tiling_sink   // Add Tiling sink operator kernel implementation
│   │   └── leaky_relu_custom        // LeakyRelu operator kernel implementation
│   └── README.md                    // Example documentation
```

## Example Description

The Add computation formula is:

```
z = x + y
```

AddCustomTilingSink, AddCustomTemplate, and Add share the same kernel function. Specifically:
  - AddCustomTemplate demonstrates Tiling template programming. The added template parameters include input data types, shapes, and so on. Based on the template parameters, the example implementation logic is simplified or unified. Developers can define the required information in the template parameters, such as input/output data types and other extended parameters.
  - AddCustomTilingSink demonstrates the Tiling sink scenario. The Tiling function is registered to execute on both the host and device via `DEVICE_IMPL_OP_OPTILING`.

The LeakyRelu computation formula is:

$$
y=
\begin{cases}
x, \quad x\geq 0\\
a*x, \quad x<0
\end{cases}
$$
where a is a scalar value.

## Example Specification Description

- AddCustom
  <table border="2" align="center">
  <caption>Table 1: AddCustom Example Specification Description</caption>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">AddCustom</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[8, 2048]</td><td align="center">float16</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[8, 2048]</td><td align="center">float16</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[8, 2048]</td><td align="center">float16</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">add_custom</td></tr>
  </table>

- AddCustomTilingSink

  <table border="2" align="center">
  <caption>Table 2: AddCustomTilingSink Example Specification Description</caption>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">AddCustomTilingSink</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[8, 2048]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[8, 2048]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[8, 2048]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">add_custom_tiling_sink</td></tr>
  </table>

- AddCustomTemplate

  <table border="2" align="center">
  <caption>Table 3: AddCustomTemplate Example Specification Description</caption>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">AddCustomTemplate</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[8, 2048]</td><td align="center">float16/float</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[8, 2048]</td><td align="center">float16/float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[8, 2048]</td><td align="center">float16/float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">add_custom_template</td></tr>
  <tr><td rowspan="6" align="center">Template Parameters</td><td colspan="4" align="center">template&lt;typename D_T_X, typename D_T_Y, typename D_T_Z, int TILE_NUM, int IS_SPLIT&gt;</td>
      <tr><td>D_T_X</td><td colspan="1">typename</td><td colspan="2">Data type (half, float)</td></tr>
      <tr><td>D_T_Y</td><td colspan="1">typename</td><td colspan="2">Data type (half, float)</td></tr>
      <tr><td>D_T_Z</td><td colspan="1">typename</td><td colspan="2">Data type (half, float)</td></tr>
      <tr><td>TILE_NUM</td><td colspan="1">int</td><td colspan="2">Number of tiles</td></tr>
      <tr><td>IS_SPLIT</td><td colspan="1">int</td><td colspan="2">Whether to split</td></tr>
  </tr>
  </table>

- LeakyRelu

  <table border="2" align="center">
  <caption>Table 4: LeakyRelu Example Specification Description</caption>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">LeakyRelu</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[8, 200, 1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">negative_slope</td><td align="center">0.0</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">y</td><td align="center">[8, 200, 1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">leaky_relu_custom</td></tr>
  </table>

## Code Implementation

- Add</br>
  - Kernel implementation:</br>
    Implemented based on the vector computation interface `Add` provided by Ascend C.</br>

  - Tiling implementation:</br>
    TilingData parameter design: The `AddCustomTilingData` parameters are essentially parameters related to parallel data splitting. This example uses 2 tiling parameters: `totalLength` and `tileNum`. `totalLength` refers to the amount of data to be computed, and `tileNum` refers to the total number of data blocks per kernel.</br>

- AddCustomTemplate</br>
  - Kernel implementation:</br>
    Same as Add.
  - Tiling template design:</br>
    This example uses 5 template parameters. `D_T_X`, `D_T_Y`, and `D_T_Z` refer to the data types of input x, input y, and output z respectively. `TILE_NUM` refers to the total number of data blocks per kernel. `IS_SPLIT` indicates whether data block computation is enabled. When `IS_SPLIT` is 0, `TILE_NUM` is ineffective. The template parameter combination replaces the traditional TilingKey.

  - TilingData parameter design:</br>
    This example uses 1 tiling parameter. `totalLength` refers to the total data size to be computed across all kernels.</br>


- AddCustomTilingSink</br>
  - Kernel implementation:</br>
    The functionality is the same as Add. The kernel uses the `KERNEL_TASK_TYPE_DEFAULT` interface to force the example to run in an AIC/AIV mixed scenario, meeting the Tiling sink operator conditions. All Tiling function logic is implemented separately in `add_custom_tiling_sink_tiling.cpp`, and the sinked Tiling function is registered via the `DEVICE_IMPL_OP_OPTILING` interface.</br>

- LeakyRelu</br>
  - Kernel implementation:</br>
    Implemented by combining basic API interfaces, using `Maxs`, `Mins`, `Muls`, and `Add` to complete the LeakyRelu computation.</br>

  - Tiling implementation:</br>
    TilingData parameter design: The `LeakyReluCustomTilingData` parameters are essentially parameters related to parallel data splitting. This example uses 3 tiling parameters: `totalLength`, `tileNum`, and `negativeSlope`. `totalLength` and `tileNum` are similar to the Add example. `negativeSlope` represents the negative half-axis slope coefficient of LeakyRelu and is passed to the kernel side as a computation parameter.</br>


## Build and Run

Run the following steps in the root directory of this example to build, package, and deploy the custom example package.

- Configure environment variables

  Configure environment variables based on the [installation method](../../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Build, package, and deploy the example

  ```bash
  mkdir -p build && cd build
  cmake .. && make -j binary package
  ./custom_opp_*.run
  ```

  The following output indicates successful execution.

  ```text
  SUCCESS
  ```

## Cached Build Acceleration (Optional)

This example supports using ccache to accelerate repeated builds, providing two modes: standalone cache and distributed cache.

### Prerequisites

- `ccache` is installed. `ccache >= 4.6.1` is recommended.

  ```bash
  apt install ccache
  ```

- For the distributed cache scenario, confirm that the current version supports Redis storage.

  ```bash
  ccache --version
  # Expected: Features includes redis-storage
  ```

### Standalone Cache

After enabling `-DENABLE_CCACHE=ON`, `cmake` attaches `ccache` to the build process. When the source code, build commands, compiler, and build directory path remain the same, repeated builds can directly reuse the local cache results.

Enable via cmake parameters without modifying CMakeLists.txt:

```bash
mkdir -p build && cd build
cmake -DENABLE_CCACHE=ON .. && make -j binary package
```

Verify whether the local cache is effective as follows:

```bash
ccache -Cz
# First build: write to cache
rm -rf build && mkdir -p build && cd build
cmake -DENABLE_CCACHE=ON .. && make -j binary package
cd ..
ccache -z
# Second build: cache hit
rm -rf build && mkdir -p build && cd build
cmake -DENABLE_CCACHE=ON .. && make -j binary package
```

After each build, check the cache hit status via `ccache --show-stats -v`. The local cache hit rate `Local storage Hits` should increase significantly on the second build.

To clear the local cache, use the following methods:

```bash
# Clear statistics only, without deleting cache content
ccache -z

# Clear local cache content
ccache -C

# Clear both local cache content and statistics
ccache -Cz
```

### Distributed Cache (ccache + Redis)

Suitable for multi-machine shared cache scenarios: Machine A builds and pushes the results to Redis. Machine B, with the same source code, same build options, and same toolchain version, can hit the cache from Redis, reducing repeated compilation.

In the distributed scenario, `ccache` uses each machine's local cache as the first-level cache and Redis as the shared second-level cache. When Machine A builds for the first time, it invokes the actual compiler and writes the results to both the local cache and Redis. When Machine B builds again with the same source code, same build commands, same compiler, and same build directory path, it can hit the shared cache directly from Redis, reducing repeated compilation. If the compiler paths differ between the two machines but the content is the same, set `compiler_check=content`.


For more `ccache` configuration and cache behavior details, refer to the [ccache official documentation](https://ccache.dev/documentation.html).

Network requirements:

- Machine A: first-build machine, writes cache to Redis, IP is `<A_IP>`
- Machine B: second-build machine, verifies shared cache hit from Redis, IP is `<B_IP>`
- Machine C: Redis server, stores shared cache data, IP is `<C_IP>`
- Machines A, B, and C must be on the same network. Both machines A and B must be able to access `C_IP:6379`.

It is recommended that Machine A and Machine B use the same source code content, the same build commands, the same compiler version, and maintain consistent source code paths and build directory paths. Otherwise, cache misses may occur.

**1. Machine C: Deploy Redis Service**
```bash
apt install redis-server
# Start the Redis service
redis-server --daemonize yes --bind 0.0.0.0 --port 6379 --requirepass <PASSWORD>
# Verify Redis connectivity
redis-cli -h <C_IP> -p 6379 -a <PASSWORD> ping
```

> Note: The above configuration is for controlled test environments only. For shared or production environments, enable access control, authentication, and network isolation.

**2. Machine A / Machine B: Configure ccache**
```bash
apt install redis-tools
# Verify Redis connectivity
redis-cli -h <C_IP> -p 6379 -a <PASSWORD> ping
# Configure Redis as secondary storage with password authentication
# Format: redis://default:<PASSWORD>@<C_IP>:6379
ccache --set-config=secondary_storage=redis://default:<PASSWORD>@<C_IP>:6379
# Configure compiler content check to avoid cache misses due to path differences
ccache --set-config=compiler_check=content
```

**3. Machine A / Machine B: Execute Build**
```bash
ccache -Cz
rm -rf build && mkdir -p build && cd build
cmake -DENABLE_CCACHE=ON .. && make -j binary package
```

Machine A build writes to the local cache and Redis. Machine B has an empty local cache and reads the shared cache from Redis. After Machine A builds, execute the build on Machine B and compare the two `ccache --show-stats -v` results. The remote cache hit rate `Remote storage Hits` on Machine B should increase significantly.

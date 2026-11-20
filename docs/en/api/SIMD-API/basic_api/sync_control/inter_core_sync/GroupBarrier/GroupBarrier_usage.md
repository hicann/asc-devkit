# GroupBarrier Usage Guide<a name="ZH-CN_TOPIC_0000001960225008"></a>

<!-- md-trans-meta sourceCommit=22a41aa8a74d7a2c3f454ea4bd5f0f8fb27aee29 translatedAt=2026-11-19T02:59:46.808Z pushedAt=2026-11-19T05:58:49.117Z nl=2 -->

When a dependency exists between two AIV tasks in the same [CubeResGroupHandle](../../inter_core_communication/CubeResGroupHandle/CubeResGroupHandle.md), GroupBarrier can be used to control synchronization. Suppose that after one group of AIVs A completes task x, another group of AIVs B can begin subsequent operations. In this case, AIV group A is called the Arrive group, and AIV group B is called the Wait group.

The steps for using GroupBarrier-based group synchronization are as follows:

1.  Create a GroupBarrier.
2.  The waited-on AIVs call Arrive, and the waiting AIVs call Wait.

1.  Create a GroupBarrier.

    ```
    constexpr int32_t ARRIVE_NUM = 2; // Count of AIVs in the Arrive group
    constexpr int32_t WAIT_NUM = 6; // Count of AIVs in the Wait group
    // Create a GroupBarrier. The user manages and clears this part of the workspace.
    AscendC::GroupBarrier<AscendC::PipeMode::MTE3_MODE> barA(workspace, ARRIVE_NUM, WAIT_NUM);
    ```

2.  The waited-on AIVs call Arrive, and the waiting AIVs call Wait.

    ```
    auto id = AscendC::GetBlockIdx();
    if (id > 0 && id < ARRIVE_NUM) {
      // Various Vector computation logic, user-implemented
      barA.Arrive(id);
    } else(id >= ARRIVE_NUM && id < ARRIVE_NUM + WAIT_NUM){
      barA.Wait(id - ARRIVE_NUM);
      // Various Vector computation logic, user-implemented
    }
    ```


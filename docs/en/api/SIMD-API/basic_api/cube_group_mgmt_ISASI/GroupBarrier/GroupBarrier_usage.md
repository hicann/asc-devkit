# GroupBarrier Usage<a name="ZH-CN_TOPIC_0000001960225008"></a>

<!-- md-trans-meta sourceCommit=7f211052f814e6d340cc6bf2dfc44c8393b5424f translatedAt=2026-11-02T04:59:11.974Z pushedAt=2026-11-18T01:25:06.851Z nl=2 -->

When a dependency exists between two AIV tasks in the same [CubeResGroupHandle](../CubeResGroupHandle/CubeResGroupHandle.md), GroupBarrier can be used to control synchronization. Suppose that after one group of AIVs (group A) completes task x, another group of AIVs (group B) can start subsequent operations. Group A is called the Arrive group, and group B is called the Wait group.

The steps for group synchronization based on GroupBarrier are as follows:

1.  Create a GroupBarrier.
2.  The AIVs being waited for call Arrive, and the AIVs that need to wait call Wait.

1.  Create a GroupBarrier.

    ```
    constexpr int32_t ARRIVE_NUM = 2; // Number of AIVs in the Arrive group.
    constexpr int32_t WAIT_NUM = 6; // Number of AIVs in the Wait group.
    // Create a GroupBarrier. You manage and clear this workspace yourself.
    AscendC::GroupBarrier<AscendC::PipeMode::MTE3_MODE> barA(workspace, ARRIVE_NUM, WAIT_NUM);
    ```

2.  The AIVs being waited for call Arrive, and the AIVs that need to wait call Wait.

    ```
    auto id = AscendC::GetBlockIdx();
    if (id > 0 && id < ARRIVE_NUM) {
      // Various Vector computation logic, implemented by you.
      barA.Arrive(id);
    } else(id >= ARRIVE_NUM && id < ARRIVE_NUM + WAIT_NUM){
      barA.Wait(id - ARRIVE_NUM);
      // Various Vector computation logic, implemented by you.
    }
    ```


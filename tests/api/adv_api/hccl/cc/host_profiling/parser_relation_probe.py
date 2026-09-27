# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
import sys
from types import SimpleNamespace
from unittest.mock import patch

sys.path.insert(0, sys.argv[1])
from mscalculate.hccl.kfc_calculator import KfcCalculator


def check(rows):
    model = SimpleNamespace(_project_path="unused")
    with patch("mscalculate.hccl.kfc_calculator.Mc2CommInfoViewModel") as factory:
        factory.return_value.__enter__.return_value.get_kfc_stream.return_value = rows
        logical, mapping = KfcCalculator.get_mc2_comm_info_data(model)
    tasks = [SimpleNamespace(stream_id=i, task_id=i + 100) for i in range(7, 16)]
    grouped = KfcCalculator._group_tasks_by_comm_stream(tasks, mapping)
    return logical, mapping, grouped


def row(kfc, queues, parent):
    return SimpleNamespace(
        group_name="hash", rank_size=2, rank_id=0, usr_rank_id=parent, aicpu_kfc_stream_id=kfc, comm_stream_ids=queues
    )


asc = [row(59, ",".join(map(str, range(7, 15))), 0), row(59, "15", 0)]
expected = check(asc)
for combined in (
    [row(0, ",".join(map(str, range(7, 16))), 8)] + asc,
    asc + [row(0, ",".join(map(str, range(7, 16))), 8)],
    asc + asc,
):
    actual = check(combined)
    assert actual[0][59] == expected[0][59]
    assert actual[1] == expected[1]
    assert [(t.stream_id, t.task_id) for t in actual[2]["hash"]] == [
        (t.stream_id, t.task_id) for t in expected[2]["hash"]
    ]
    assert len(actual[2]["hash"]) == 9
print("PASS: installed parser relation mapping/task grouping; KFC=0+59 both orders; 8+1 split; parent fallback")

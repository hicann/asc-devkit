/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

export function loadGitTimestamps(runGit) {
  const shallow = runGit(['rev-parse', '--is-shallow-repository']) === 'true'
  const shallowBoundaries = shallow
    ? new Set(runGit(['rev-list', '--max-parents=0', 'HEAD']).split('\n').filter(Boolean))
    : new Set()
  const history = runGit([
    '-c',
    'core.quotePath=false',
    'log',
    '--format=@@%H:%ct',
    '--name-only',
    '--',
    'docs',
  ])
  const timestamps = new Map()
  let timestamp = 0
  let skipCommit = false

  for (const line of history.split('\n')) {
    if (line.startsWith('@@')) {
      const separator = line.indexOf(':', 2)
      const commit = separator >= 0 ? line.slice(2, separator) : ''
      timestamp = Number(separator >= 0 ? line.slice(separator + 1) : 0) * 1000
      skipCommit = shallowBoundaries.has(commit)
    }
    else if (line && timestamp > 0 && !skipCommit && !timestamps.has(line)) {
      timestamps.set(line, timestamp)
    }
  }
  return timestamps
}

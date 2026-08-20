/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/


const { spawnSync } = require('node:child_process')
const { resolve, join } = require('node:path')
const { cpSync, existsSync, rmSync } = require('node:fs')

const scriptsDir = resolve(__dirname)
const root = resolve(scriptsDir, '..')
const repoRoot = resolve(root, '..', '..', '..')

function runMdparser(inputDir, outputDir, label) {
  console.log(`Converting ${label}...`)
  const input = resolve(repoRoot, 'docs', 'zh', inputDir)
  const output = resolve(root, 'docs', outputDir)
  // The generated tree is ignored by Git, so remove pages left behind by
  // source renames or deletions before producing the next build input.
  rmSync(output, { recursive: true, force: true })
  const scriptsDirUnix = scriptsDir.replace(/\\/g, '/')
  const inputUnix = input.replace(/\\/g, '/')
  const outputUnix = output.replace(/\\/g, '/')

  const args = [
    '-c',
    `import sys; sys.path.insert(0, '${scriptsDirUnix}'); from mdparser.cli import main; main()`,
    '-s',
    inputUnix,
    '-o', outputUnix,
    '-r',
  ]

  const result = spawnSync('python3', args, {
    cwd: root,
    stdio: 'inherit',
    encoding: 'utf-8',
  })

  if (result.status !== 0) {
    console.error(`Failed to convert ${label}`)
    process.exit(result.status || 1)
  }
}

runMdparser('api', 'api', 'API docs')
runMdparser('guide', 'guide', 'Guide docs')

const srcFigures = resolve(repoRoot, 'docs', 'zh', 'figures')
const dstFigures = resolve(root, 'docs', 'figures')
if (existsSync(srcFigures)) {
  console.log(`Copying figures/...`)
  cpSync(srcFigures, dstFigures, { recursive: true })
  console.log(`  ${srcFigures} -> ${dstFigures}`)
}

console.log('Conversion complete')

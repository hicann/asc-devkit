/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

const fs = require('fs')
const path = require('path')
const crypto = require('crypto')

const root = path.resolve(__dirname, '..')
const repoRoot = path.resolve(root, '..', '..', '..')
const apiDir = path.resolve(repoRoot, 'docs', 'zh', 'api')
const outDir = path.resolve(root, 'docs', 'public', 'api-source')

function walkDir(dir, entries) {
  const list = fs.readdirSync(dir)
  for (const name of list) {
    const full = path.join(dir, name)
    const stat = fs.statSync(full)
    if (stat.isDirectory()) {
      walkDir(full, entries)
    } else if (stat.isFile() && name.endsWith('.md')) {
      entries.push(full)
    }
  }
}

function hashPath(str) {
  return crypto.createHash('md5').update(str).digest('hex')
}

if (!fs.existsSync(apiDir)) {
  console.error('api dir not found:', apiDir)
  process.exit(1)
}

fs.mkdirSync(outDir, { recursive: true })

const files = []
walkDir(apiDir, files)

const flatDir = path.join(outDir, 'files')
fs.mkdirSync(flatDir, { recursive: true })

const manifest = {}
for (const src of files) {
  const rel = path.relative(apiDir, src).replace(/\\/g, '/')
  const hash = hashPath(rel)
  manifest[rel] = hash
  const dest = path.join(flatDir, hash + '.md')
  fs.copyFileSync(src, dest)
}

fs.writeFileSync(path.join(outDir, 'manifest.json'), JSON.stringify(manifest, null, 2), 'utf-8')

const bundle = {}
for (const src of files) {
  const rel = path.relative(apiDir, src).replace(/\\/g, '/')
  bundle[rel] = fs.readFileSync(src, 'utf-8')
}
fs.writeFileSync(path.join(outDir, 'bundle.json'), JSON.stringify(bundle), 'utf-8')

const bundleSize = (fs.statSync(path.join(outDir, 'bundle.json')).size / (1024 * 1024)).toFixed(1)
console.log(`copied ${files.length} .md files to docs/public/api-source/files/`)
console.log(`generated bundle.json (${bundleSize} MB) with ${Object.keys(bundle).length} entries`)

const contribFiles = [
  'contributing/asc_adv_api_contributing.md',
  'contributing/asc_basic_api_contributing.md',
  'contributing/asc_c_api_contributing.md',
  'asc_how_to_choose_api.md',
  'quick_start.md',
]

const contribSrcDir = path.resolve(repoRoot, 'docs', 'zh')
const contribDstDir = path.resolve(root, 'docs')
fs.mkdirSync(contribDstDir, { recursive: true })

let copiedCount = 0
for (const f of contribFiles) {
  const src = path.join(contribSrcDir, f)
  if (fs.existsSync(src)) {
    const dst = path.join(contribDstDir, f)
    fs.mkdirSync(path.dirname(dst), { recursive: true })
    fs.copyFileSync(src, dst)
    copiedCount++
  }
}

const contribEnDir = path.resolve(repoRoot, 'docs', 'en')
const contribEnDst = path.resolve(root, 'docs', 'en')
fs.mkdirSync(contribEnDst, { recursive: true })

for (const f of contribFiles) {
  const src = path.join(contribEnDir, f)
  if (fs.existsSync(src)) {
    const dst = path.join(contribEnDst, f)
    fs.mkdirSync(path.dirname(dst), { recursive: true })
    fs.copyFileSync(src, dst)
    copiedCount++
  }
}

console.log(`copied ${copiedCount} contrib docs to vitepress source`)

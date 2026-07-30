/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

import { existsSync, statSync } from 'node:fs'
import { dirname, extname, isAbsolute, relative, resolve } from 'node:path'

const REPOSITORY_URL = 'https://gitcode.com/cann/asc-devkit'
const HREF_RE = /(\bhref\s*=\s*)(["'])([^"']*)\2/gi
const IMAGE_EXTENSIONS = new Set([
  '.bmp', '.gif', '.ico', '.jpeg', '.jpg', '.png', '.svg', '.webp',
])

function isInside(parent, target) {
  const pathFromParent = relative(parent, target)
  return pathFromParent === '' || (!pathFromParent.startsWith('..') && !isAbsolute(pathFromParent))
}

function splitHref(href) {
  const queryIndex = href.indexOf('?')
  const hashIndex = href.indexOf('#')
  const suffixIndex = [queryIndex, hashIndex]
    .filter(index => index >= 0)
    .reduce((first, index) => Math.min(first, index), href.length)
  return [href.slice(0, suffixIndex), href.slice(suffixIndex)]
}

function encodeRepositoryPath(path) {
  return path
    .replace(/\\/g, '/')
    .split('/')
    .filter(Boolean)
    .map(segment => encodeURIComponent(segment))
    .join('/')
}

export function rewriteRepositoryLinks(html, sourceFile, repoRoot) {
  if (!sourceFile || !repoRoot) return html

  const codeBlocks = []
  html = html.replace(/<(code|pre)\b[^>]*>[\s\S]*?<\/\1>/gi, (m) => {
    codeBlocks.push(m)
    return `\x00CB${codeBlocks.length - 1}\x00`
  })

  html = html.replace(HREF_RE, (original, attribute, quote, href) => {
    const rewrittenHref = rewriteRepositoryHref(href, sourceFile, repoRoot)
    if (rewrittenHref === href) return original
    return `${attribute}${quote}${rewrittenHref}${quote}`
  })

  html = html.replace(/\x00CB(\d+)\x00/g, (_, i) => codeBlocks[+i])

  return html
}

export function rewriteRepositoryHref(href, sourceFile, repoRoot) {
  if (!sourceFile || !repoRoot) return href

  const normalizedRepoRoot = resolve(repoRoot)
  const docsRoot = resolve(normalizedRepoRoot, 'docs')

  if (
    !href ||
    href.startsWith('/') ||
    href.startsWith('#') ||
    /^[a-z][a-z0-9+.-]*:/i.test(href)
  ) {
    return href
  }

  const [pathPart, suffix] = splitHref(href)
  let target
  try {
    target = resolve(dirname(sourceFile), decodeURI(pathPart))
  } catch {
    return href
  }

  if (!isInside(normalizedRepoRoot, target) || isInside(docsRoot, target)) {
    return href
  }
  if (!existsSync(target) || IMAGE_EXTENSIONS.has(extname(target).toLowerCase())) {
    return href
  }

  let linkType
  try {
    linkType = statSync(target).isDirectory() ? 'tree' : 'blob'
  } catch {
    return href
  }

  const repositoryPath = encodeRepositoryPath(relative(normalizedRepoRoot, target))
  const separator = repositoryPath ? '/' : ''
  return `${REPOSITORY_URL}/${linkType}/master${separator}${repositoryPath}${suffix}`
}

export function installRepositoryLinkRewrite(md, getSourceFile, repoRoot) {
  const originalLinkOpen = md.renderer.rules.link_open

  md.renderer.rules.link_open = (tokens, idx, options, env, self) => {
    const token = tokens[idx]
    const hrefIndex = token.attrIndex('href')
    if (hrefIndex >= 0) {
      const href = token.attrs[hrefIndex][1]
      token.attrs[hrefIndex][1] = rewriteRepositoryHref(href, getSourceFile(env), repoRoot)
    }

    if (originalLinkOpen) {
      return originalLinkOpen(tokens, idx, options, env, self)
    }
    return self.renderToken(tokens, idx, options)
  }
}

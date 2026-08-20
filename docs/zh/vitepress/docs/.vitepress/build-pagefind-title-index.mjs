/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

import { readFile, writeFile } from 'node:fs/promises'
import { isAbsolute, join, relative, resolve, sep } from 'node:path'
import { fileURLToPath } from 'node:url'
import { gunzipSync } from 'node:zlib'

import { load } from 'cheerio'
import { createIndex } from 'pagefind'

import {
  findKernelLaunch,
  KERNEL_LAUNCH_TOKEN,
  kernelLaunchMatches,
} from './search-syntax.mjs'

const CJK_PHRASE_REGEX = /[\u4e00-\u9fff\u3400-\u4dbf\uf900-\ufaff]{2,}/g
const IDENTIFIER_REGEX = /[A-Za-z_][A-Za-z0-9_]*(?:_[A-Za-z0-9_]+)+/g
const FORMAT_CHARACTER_REGEX = /[\u200b-\u200d\u2060\ufeff]/g
const MAX_IDENTIFIER_ROUTES = 8
const MAX_SPECIAL_SYNTAX_ROUTES = 20
const PAGE_RANK_DAMPING = 0.85
const PAGE_RANK_ITERATIONS = 60
const PAGE_RANK_TOLERANCE = 1e-12
const PAGE_RANK_WEIGHT = 0.5
const MAX_EXCERPT_LENGTH = 240

function cjkAlias(value) {
  return `cjk${[...value].map(character => character.codePointAt(0).toString(16)).join('')}x`
}

function cjkAliases(values) {
  const aliases = new Set()
  for (const value of values) {
    for (const phrase of value.match(CJK_PHRASE_REGEX) || []) {
      aliases.add(cjkAlias(phrase))
      for (let index = 0; index < phrase.length - 1; index++) {
        aliases.add(cjkAlias(phrase.slice(index, index + 2)))
      }
    }
  }
  return [...aliases]
}

function routeSegments(route) {
  return route.split('/').filter(Boolean).map((segment) => {
    try {
      return decodeURIComponent(segment)
    }
    catch {
      return segment
    }
  })
}

function compactIdentifier(value) {
  return String(value || '').normalize('NFKC').toLocaleLowerCase().replace(/[^a-z0-9]+/g, '')
}

function identifierMatches(content) {
  const normalized = String(content || '').replace(FORMAT_CHARACTER_REGEX, '')
  return (normalized.match(IDENTIFIER_REGEX) || [])
    .map(value => value.normalize('NFKC').toLocaleLowerCase())
    .filter((value) => {
      const compact = compactIdentifier(value)
      return compact.length >= 3 && compact.length <= 128 && value.length <= 160
    })
}

function searchTokenMatches(content) {
  return [
    ...identifierMatches(content),
    ...kernelLaunchMatches(content).map(() => KERNEL_LAUNCH_TOKEN),
  ]
}

function extractIdentifiers(content) {
  return [...new Set(searchTokenMatches(content))]
}

function normalizeRoute(route) {
  let normalized = String(route || '').split('#')[0].split('?')[0]
  try {
    normalized = decodeURIComponent(normalized)
  }
  catch {}
  normalized = normalized.replace(/\.(?:html?|md)$/, '').replace(/\/index$/, '').replace(/\/$/, '')
  return normalized || '/'
}

export function sourcePathsForRoute(intermediateHtmlDirectory, route) {
  const relativeRoute = normalizeRoute(route).replace(/^\/+/, '')
  const relativeCandidates = relativeRoute
    ? [`${relativeRoute}.html`, join(relativeRoute, 'index.html')]
    : ['index.html']
  return relativeCandidates.map(candidate => resolve(intermediateHtmlDirectory, candidate)).filter((candidate) => {
    const relativePath = relative(intermediateHtmlDirectory, candidate)
    return relativePath !== '..' && !relativePath.startsWith(`..${sep}`) && !isAbsolute(relativePath)
  })
}

function createIdentifierSignal() {
  return {
    heading: 0,
    catalogDefinition: 0,
    definition: 0,
    list: 0,
    table: 0,
    prose: 0,
    code: 0,
    reference: 0,
    first: 1,
    excerpt: '',
    excerptScore: -1,
    sectionId: '',
    sectionTitle: '',
  }
}

function identifierExcerpt(content, identifier) {
  const text = String(content || '')
    .replace(FORMAT_CHARACTER_REGEX, '')
    .replace(/\s+/g, ' ')
    .trim()
  const position = identifier === KERNEL_LAUNCH_TOKEN
    ? findKernelLaunch(text)?.index ?? -1
    : text.toLocaleLowerCase().indexOf(identifier)
  if (position < 0) return ''
  if (text.length <= MAX_EXCERPT_LENGTH) return text

  const prefixLength = position <= 72 ? position : 48
  const start = Math.max(0, position - prefixLength)
  const availableLength = MAX_EXCERPT_LENGTH - (start > 0 ? 3 : 0)
  const excerpt = text.slice(start, start + availableLength).trim()
  return `${start > 0 ? '...' : ''}${excerpt}`
}

function contextScore(field, excerpt, identifier, sectionTitle = '') {
  const fieldScore = {
    catalogDefinition: 700,
    definition: 650,
    list: 600,
    table: 550,
    prose: 500,
    heading: 200,
    code: 50,
  }[field] || 0
  const position = identifier === KERNEL_LAUNCH_TOKEN
    ? findKernelLaunch(excerpt)?.index ?? -1
    : excerpt.toLocaleLowerCase().indexOf(identifier)
  const sectionOwnsToken = identifier === KERNEL_LAUNCH_TOKEN
    ? Boolean(findKernelLaunch(sectionTitle))
    : sectionTitle.toLocaleLowerCase().includes(identifier)
  const hasExplanation = excerpt.length >= identifier.length + 12
  return fieldScore
    + (sectionOwnsToken ? 800 : 0)
    + (hasExplanation ? 100 : 0)
    + (position >= 0 && position <= 2 ? 40 : 0)
    + Math.min(excerpt.length, MAX_EXCERPT_LENGTH) / MAX_EXCERPT_LENGTH
}

function addIdentifierContext(signals, content, field, section = null) {
  for (const identifier of new Set(searchTokenMatches(content))) {
    if (!signals.has(identifier)) signals.set(identifier, createIdentifierSignal())
    const signal = signals.get(identifier)
    const excerpt = identifierExcerpt(content, identifier)
    const score = contextScore(field, excerpt, identifier, section?.title || '')
    if (excerpt && score > signal.excerptScore) {
      signal.excerpt = excerpt
      signal.excerptScore = score
      signal.sectionId = section?.id || ''
      signal.sectionTitle = section?.title || ''
    }
  }
}

function addIdentifierSignals(signals, content, field, section = null) {
  for (const identifier of searchTokenMatches(content)) {
    if (!signals.has(identifier)) signals.set(identifier, createIdentifierSignal())
    signals.get(identifier)[field]++
  }
  addIdentifierContext(signals, content, field, section)
}

function addDefinitionSignal(signals, content, field = 'definition', section = null) {
  const text = String(content || '').replace(FORMAT_CHARACTER_REGEX, '').trim()
  const identifier = searchTokenMatches(text)[0]
  if (!identifier) return
  const match = identifier === KERNEL_LAUNCH_TOKEN ? findKernelLaunch(text) : null
  const start = match?.index ?? text.toLocaleLowerCase().indexOf(identifier)
  const suffix = text.slice(start + (match?.value.length || identifier.length)).trimStart()
  const isStandalone = match
    ? text === match.value
    : compactIdentifier(text) === compactIdentifier(identifier)
  const isLeadingDefinition = start === 0 && /^[:\uFF1A\-\u2013\u2014]/.test(suffix)
  if (!isStandalone && !isLeadingDefinition) return
  if (!signals.has(identifier)) signals.set(identifier, createIdentifierSignal())
  signals.get(identifier)[field]++
  addIdentifierContext(signals, text, field, section)
}

function markedListDefinition($, element, fallbackSection) {
  const firstParagraph = $(element).children('p').first()
  if (!firstParagraph.length) return null
  const marker = firstParagraph.find('strong,code').first()
  if (!marker.length) return null
  const label = marker.text().replace(/\s+/g, ' ').trim()
  const identifiers = [...new Set(searchTokenMatches(label))]
    .filter(identifier => compactIdentifier(identifier) === compactIdentifier(label))
  if (identifiers.length === 0) return null
  const anchor = firstParagraph.find('a[id]').first().attr('id') || ''
  return {
    identifiers,
    section: anchor ? { id: anchor, title: label } : fallbackSection,
  }
}

function asciiWords(value) {
  return String(value || '')
    .normalize('NFKC')
    .replace(/([a-z0-9])([A-Z])/g, '$1 $2')
    .toLocaleLowerCase()
    .match(/[a-z]+[0-9]*|[0-9]+/g) || []
}

function identityTokens(values) {
  const tokens = new Set()
  for (const value of values) {
    const words = asciiWords(value)
    for (const word of words) tokens.add(word)
    for (let start = 0; start < words.length; start++) {
      let acronym = ''
      for (let end = start; end < Math.min(words.length, start + 6); end++) {
        acronym += words[end][0]
        if (acronym.length >= 2) tokens.add(acronym)
      }
    }
  }
  return tokens
}

function tokenMatchesIdentity(token, tokens) {
  if (tokens.has(token)) return true
  if (token.length < 3) return false
  return [...tokens].some(candidate => candidate.length > token.length
    && (candidate.startsWith(token) || candidate.endsWith(token)))
}

function identifierIdentityCoverage(identifier, tokens) {
  const parts = asciiWords(identifier)
  if (parts.length === 0) return 0
  return parts.filter(part => tokenMatchesIdentity(part, tokens)).length / parts.length
}

function bestContextScore(signal) {
  if (signal.heading > 0) return 1
  if (signal.catalogDefinition > 0) return 0.98
  if (signal.definition > 0) return 0.95
  if (signal.list > 0) return 0.85
  if (signal.table > 0) return 0.75
  if (signal.prose > 0) return 0.5
  if (signal.code > 0) return 0.1
  return 0.05
}

function identifierRelevance(identifier, record, analysis) {
  const signal = analysis.signals.get(identifier) || createIdentifierSignal()
  const key = compactIdentifier(identifier)
  const titleKey = compactIdentifier(record.meta.title)
  const leaf = routeSegments(record.url).at(-1) || ''
  const leafKey = compactIdentifier(leaf)
  const exactOwner = (key.length > 0 && (titleKey === key || leafKey === key))
    || (identifier === KERNEL_LAUNCH_TOKEN && signal.heading > 0)
  const partialOwner = key.length >= 4 && (titleKey.includes(key) || leafKey.includes(key))
  const coverage = identifierIdentityCoverage(identifier, analysis.identityTokens)
  const nonCodeCount = signal.heading + signal.catalogDefinition + signal.definition
    + signal.list + signal.table + signal.prose
  const early = Math.max(0, 1 - signal.first)
  const catalog = signal.list + signal.table > 0
    ? Math.min(1, Math.log1p(analysis.structuredIdentifierCount) / Math.log(64))
    : 0
  const ownerScore = exactOwner ? 4 : partialOwner ? 3 : signal.catalogDefinition > 0 ? 2.5 : coverage * 2
  return ownerScore
    + bestContextScore(signal)
    + early * 0.25
    + catalog * 0.2
    + Math.min(nonCodeCount, 2) * 0.1
}

function extractExcerpt($, article) {
  const candidates = []
  article.find('p').slice(0, 40).each((index, element) => {
    if ($(element).parents('pre,table').length > 0) return
    const paragraph = $(element).clone()
    paragraph.find('pre,code,table,ul,ol').remove()
    const content = paragraph.text().replace(/\s+/g, ' ').trim()
    if (content.length >= 20) candidates.push({ content, index })
  })
  if (candidates.length === 0) {
    article.find('li').slice(0, 40).each((index, element) => {
      const item = $(element).clone()
      item.find('pre,code,table,ul,ol').remove()
      const content = item.text().replace(/\s+/g, ' ').trim()
      if (content.length >= 4) candidates.push({ content, index })
    })
  }
  return candidates
    .sort((left, right) => Math.min(right.content.length, MAX_EXCERPT_LENGTH) - right.index * 2
      - (Math.min(left.content.length, MAX_EXCERPT_LENGTH) - left.index * 2))[0]
    ?.content.slice(0, MAX_EXCERPT_LENGTH) || ''
}

function existingSections($, article) {
  const sections = new WeakMap()
  const seen = new Map()
  let currentSection = null
  article.find('h2,h3,h4,h5,h6,pre,tr,th,td,li,p').each((_, element) => {
    if (/^h[2-6]$/.test(element.tagName)) {
      const title = $(element).text().replace(/\s+/g, ' ').trim()
      const baseSlug = title.replace(/\s+/g, '-')
      const count = seen.get(baseSlug) || 0
      const generatedId = count === 0 ? baseSlug : `${baseSlug}-${count}`
      seen.set(baseSlug, count + 1)
      currentSection = { id: $(element).attr('id') || generatedId, title }
    }
    sections.set(element, currentSection)
  })
  return sections
}

export function isCatalogOnlyArticle($, article) {
  const listItems = article.find('li')
  if (listItems.length === 0 || article.find('a[href]').length === 0) return false
  if (article.find('pre,table,blockquote,dl').length > 0) return false

  const standaloneParagraphs = article.find('p').filter((_, element) => (
    $(element).parents('li').length === 0
  ))
  if (standaloneParagraphs.length > 0) return false

  return listItems.toArray().every(element => $(element).find('a[href]').length > 0)
}

async function analyzeRecord(intermediateHtmlDirectory, record) {
  const signals = new Map()
  const fallback = {
    catalogOnly: false,
    signals,
    identityTokens: identityTokens([record.meta.title, routeSegments(record.url).at(-1)]),
    structuredIdentifierCount: 0,
    excerpt: '',
    links: [],
  }
  let html
  let selectedSourcePath
  for (const sourcePath of sourcePathsForRoute(intermediateHtmlDirectory, record.url)) {
    try {
      html = await readFile(sourcePath, 'utf8')
      selectedSourcePath = sourcePath
      break
    }
    catch (error) {
      if (error?.code !== 'ENOENT') throw error
    }
  }
  if (html === undefined) return fallback

  const $ = load(html)
  const article = $('.markdown-body').first()
  if (!article.length) return fallback
  const catalogOnly = isCatalogOnlyArticle($, article)
  const excerpt = extractExcerpt($, article)
  const sections = existingSections($, article)
  const headings = article.find('h1,h2,h3,h4,h5,h6').map((_, element) => $(element).text()).get()
  article.find('h1,h2,h3,h4,h5,h6').each((_, element) => {
    addIdentifierSignals(signals, $(element).text(), 'heading', sections.get(element))
  })
  article.find('pre').each((_, element) => {
    addIdentifierSignals(signals, $(element).text(), 'code', sections.get(element))
  })
  article.find('td,th').each((_, element) => {
    const content = $(element).text()
    const section = sections.get(element)
    addIdentifierSignals(signals, content, 'table', section)
    const peers = $(element).siblings('td,th').map((_, peer) => $(peer).text().trim()).get()
    const compactRow = peers.length > 0
      && peers.every(value => value.length <= 32 && !/[。！？；.!?;]/.test(value))
    addDefinitionSignal(signals, content, compactRow ? 'catalogDefinition' : 'definition', section)
  })
  article.find('tr').each((_, element) => {
    const row = $(element).find('th,td').map((_, cell) => $(cell).text().replace(/\s+/g, ' ').trim()).get()
      .filter(Boolean)
    if (row.length > 1) {
      addIdentifierContext(signals, row.join('：'), 'catalogDefinition', sections.get(element))
    }
  })
  article.find('li').each((_, element) => {
    const item = $(element).clone()
    item.children('ul,ol,table,pre,.code-block').remove()
    const content = item.text()
    const definition = markedListDefinition($, element, sections.get(element))
    const section = definition?.section || sections.get(element)
    addIdentifierSignals(signals, content, 'list', section)
    addDefinitionSignal(signals, content, 'definition', section)
    if (definition) {
      for (const identifier of definition.identifiers) {
        if (!signals.has(identifier)) signals.set(identifier, createIdentifierSignal())
        signals.get(identifier).catalogDefinition++
      }
      addIdentifierContext(signals, content, 'catalogDefinition', section)
    }
  })
  article.find('p').each((_, element) => {
    if ($(element).parents('li,td,th,pre').length > 0) return
    addIdentifierSignals(signals, $(element).text(), 'prose', sections.get(element))
  })

  const articleText = article.text().normalize('NFKC').toLocaleLowerCase()
  for (const [identifier, signal] of signals) {
    const position = identifier === KERNEL_LAUNCH_TOKEN
      ? findKernelLaunch(articleText)?.index ?? -1
      : articleText.indexOf(identifier)
    signal.first = position < 0 ? 1 : position / Math.max(1, articleText.length)
  }
  for (const identifier of record.identifiers) {
    if (!signals.has(identifier)) {
      const signal = createIdentifierSignal()
      signal.reference = 1
      signals.set(identifier, signal)
    }
  }

  const normalizedRecordRoute = normalizeRoute(record.url)
  const recordPath = selectedSourcePath.endsWith(`${sep}index.html`)
    ? normalizedRecordRoute === '/' ? '/index.html' : `${normalizedRecordRoute}/index.html`
    : `${normalizedRecordRoute}.html`
  const baseUrl = new URL(encodeURI(recordPath), 'http://asc-search.local')
  const links = new Set()
  article.find('a[href]').each((_, element) => {
    const href = $(element).attr('href')
    if (!href || href.startsWith('#')) return
    try {
      const target = new URL(href, baseUrl)
      if (target.origin !== baseUrl.origin) return
      const route = normalizeRoute(target.pathname)
      if (route !== normalizedRecordRoute) links.add(route)
    }
    catch {}
  })
  const structuredIdentifierCount = [...signals.values()]
    .filter(signal => signal.list > 0 || signal.table > 0).length
  return {
    catalogOnly,
    signals,
    identityTokens: identityTokens([record.meta.title, routeSegments(record.url).at(-1), ...headings]),
    structuredIdentifierCount,
    excerpt,
    links: [...links],
  }
}

async function analyzeRecords(intermediateHtmlDirectory, records) {
  const analyses = new Array(records.length)
  const batchSize = 32
  for (let offset = 0; offset < records.length; offset += batchSize) {
    const batch = await Promise.all(records.slice(offset, offset + batchSize)
      .map(record => analyzeRecord(intermediateHtmlDirectory, record)))
    batch.forEach((analysis, index) => { analyses[offset + index] = analysis })
  }
  return analyses
}

function calculatePageRanks(records, analyses) {
  const routeToPage = new Map(records.map((record, pageId) => [normalizeRoute(record.url), pageId]))
  const outgoing = analyses.map((analysis) => {
    return [...new Set(analysis.links.map(route => routeToPage.get(route))
      .filter(pageId => pageId !== undefined))]
  })
  const pageCount = records.length
  let ranks = new Float64Array(pageCount).fill(1 / pageCount)
  for (let iteration = 0; iteration < PAGE_RANK_ITERATIONS; iteration++) {
    let danglingRank = 0
    for (let pageId = 0; pageId < pageCount; pageId++) {
      if (outgoing[pageId].length === 0) danglingRank += ranks[pageId]
    }
    const baseRank = (1 - PAGE_RANK_DAMPING) / pageCount
      + PAGE_RANK_DAMPING * danglingRank / pageCount
    const next = new Float64Array(pageCount).fill(baseRank)
    for (let pageId = 0; pageId < pageCount; pageId++) {
      if (outgoing[pageId].length === 0) continue
      const share = PAGE_RANK_DAMPING * ranks[pageId] / outgoing[pageId].length
      for (const target of outgoing[pageId]) next[target] += share
    }
    let difference = 0
    for (let pageId = 0; pageId < pageCount; pageId++) difference += Math.abs(next[pageId] - ranks[pageId])
    ranks = next
    if (difference < PAGE_RANK_TOLERANCE) break
  }
  return {
    ranks,
    edgeCount: outgoing.reduce((sum, links) => sum + links.length, 0),
  }
}

function createRecord(route, rawTitle, rawContent = '') {
  const title = String(rawTitle || '').replace(/\s+/g, ' ').trim()
    || routeSegments(route).at(-1) || 'Ascend C'
  const segments = routeSegments(route)
  return {
    url: route,
    content: [...new Set([title, ...segments, ...cjkAliases([title, ...segments])])].join(' '),
    language: 'zh',
    meta: { title, ascSearchSource: 'title-route' },
    identifiers: extractIdentifiers(rawContent),
  }
}

function createIdentifierIndex(records, analyses, pageRanks) {
  const pages = records.map((record, pageId) => [
    record.url,
    record.meta.title,
    analyses[pageId].excerpt,
    analyses[pageId].catalogOnly ? 1 : 0,
  ])
  const snippets = []
  const snippetIds = new Map()
  const sections = []
  const sectionIds = new Map()
  function internSnippet(value) {
    if (!value) return -1
    if (!snippetIds.has(value)) {
      snippetIds.set(value, snippets.length)
      snippets.push(value)
    }
    return snippetIds.get(value)
  }
  function internSection(id, title) {
    if (!id) return -1
    const key = `${id}\u0000${title}`
    if (!sectionIds.has(key)) {
      sectionIds.set(key, sections.length)
      sections.push([id, title])
    }
    return sectionIds.get(key)
  }
  const identifiers = new Map()
  for (let pageId = 0; pageId < records.length; pageId++) {
    const record = records[pageId]
    for (const identifier of new Set([...record.identifiers, ...analyses[pageId].signals.keys()])) {
      if (!identifiers.has(identifier)) identifiers.set(identifier, new Set())
      identifiers.get(identifier).add(pageId)
    }
  }

  const entries = [...identifiers].map(([key, identifierPages]) => {
    const candidates = [...identifierPages].map(pageId => ({
      pageId,
      relevance: identifierRelevance(key, records[pageId], analyses[pageId]),
      authority: pageRanks[pageId],
    }))
    const selected = candidates
      .map(candidate => ({
        ...candidate,
        fusion: candidate.relevance
          + PAGE_RANK_WEIGHT * Math.log1p(candidate.authority * records.length),
      }))
      .sort((left, right) => right.fusion - left.fusion
        || right.relevance - left.relevance
        || right.authority - left.authority
        || records[left.pageId].url.localeCompare(records[right.pageId].url))
      .slice(0, key === KERNEL_LAUNCH_TOKEN ? MAX_SPECIAL_SYNTAX_ROUTES : MAX_IDENTIFIER_ROUTES)
    const pageIds = selected.map(candidate => candidate.pageId)
    const contextIds = selected.map(candidate => internSnippet(
      analyses[candidate.pageId].signals.get(key)?.excerpt || ''
    ))
    const selectedSectionIds = selected.map((candidate) => {
      const signal = analyses[candidate.pageId].signals.get(key)
      return internSection(signal?.sectionId || '', signal?.sectionTitle || '')
    })
    return [key, pageIds, contextIds, selectedSectionIds]
  }).sort((left, right) => left[0] < right[0] ? -1 : left[0] > right[0] ? 1 : 0)
  return { v: 8, p: pages, i: entries, s: snippets, h: sections }
}

function decodeFragment(buffer) {
  let decoded = buffer
  if (!buffer.subarray(0, 12).toString().startsWith('pagefind_dcd')) decoded = gunzipSync(buffer)
  const content = decoded.subarray(12)
  if (content[0] === 0x7B) return JSON.parse(content.toString('utf8'))
  return content
}

async function readPrimaryRecords(distDirectory) {
  const pagefindDirectory = resolve(distDirectory, 'pagefind')
  const fragmentDirectory = resolve(pagefindDirectory, 'fragment')
  const entry = JSON.parse(await readFile(resolve(pagefindDirectory, 'pagefind-entry.json'), 'utf8'))
  const language = Object.keys(entry.languages || {})[0]
  const metadataHash = entry.languages?.[language]?.hash
  if (!metadataHash) throw new Error('Primary Pagefind entry does not contain a language index')
  const metadata = decodeFragment(await readFile(resolve(pagefindDirectory, `pagefind.${metadataHash}.pf_meta`)))
  const fragmentFiles = [...new Set(
    metadata.toString('latin1').match(/[a-z]{2}_[0-9a-f]{7}/g) || []
  )].map(hash => `${hash}.pf_fragment`)
  const records = (await Promise.all(fragmentFiles.map(async (file) => {
    try {
      const fragment = decodeFragment(await readFile(join(fragmentDirectory, file)))
      return createRecord(fragment.url, fragment.meta?.title, fragment.content)
    }
    catch (error) {
      if (error?.code === 'ENOENT') return null
      throw error
    }
  }))).filter(Boolean)
  if (!records.some(record => record.url === '/')) records.push(createRecord('/', 'Ascend C'))
  return records.sort((left, right) => left.url.localeCompare(right.url))
}

async function buildTitleIndex(distDirectory) {
  const response = await createIndex({ forceLanguage: 'zh', includeCharacters: '_:' })
  if (!response.index || response.errors.length > 0) {
    throw new Error(`Unable to create Pagefind title index: ${response.errors.join('; ')}`)
  }

  const records = await readPrimaryRecords(distDirectory)
  let indexed = 0
  const batchSize = 64
  for (let offset = 0; offset < records.length; offset += batchSize) {
    const results = await Promise.all(
      records.slice(offset, offset + batchSize).map(({ identifiers, ...record }) => response.index.addCustomRecord(record))
    )
    const errors = results.flatMap(result => result.errors)
    if (errors.length > 0) throw new Error(errors.join('; '))
    indexed += results.length
  }

  const outputPath = resolve(distDirectory, 'pagefind-titles')
  const written = await response.index.writeFiles({ outputPath })
  await response.index.deleteIndex()
  if (written.errors.length > 0) throw new Error(written.errors.join('; '))
  console.log(`[pagefind-titles] Indexed ${indexed} page titles and routes into ${outputPath}`)

  // mdparser writes lean HTML containing .markdown-body beside .vitepress;
  // analyze it instead of the much larger VitePress output in distDirectory.
  const intermediateHtmlDirectory = resolve(distDirectory, '..', '..')
  const analyses = await analyzeRecords(intermediateHtmlDirectory, records)
  const pageRank = calculatePageRanks(records, analyses)
  console.log(`[search-identifiers] Ranked ${records.length} pages over ${pageRank.edgeCount} body links`)
  const identifierIndex = createIdentifierIndex(records, analyses, pageRank.ranks)
  const identifierPath = resolve(distDirectory, 'search-identifiers.json')
  const identifierJson = JSON.stringify(identifierIndex)
  await writeFile(identifierPath, identifierJson)
  console.log(`[search-identifiers] Indexed ${identifierIndex.i.length} identifiers into ${identifierPath} (${Math.round(Buffer.byteLength(identifierJson) / 1024)} KB)`)
}

if (resolve(process.argv[1] || '') === fileURLToPath(import.meta.url)) {
  const distDirectory = process.argv[2]
  if (!distDirectory) throw new Error('Usage: node build-pagefind-title-index.mjs <dist-directory>')
  await buildTitleIndex(resolve(distDirectory))
}

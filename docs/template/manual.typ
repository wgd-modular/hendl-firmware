// ============================================================
//  hendl user manual — pandoc → typst template
//  Monochrome, humanist sans. Prefers system "Gill Sans"
//  (the house typeface); falls back to the bundled OFL "Cabin"
//  so CI and other machines render an identical-in-spirit PDF.
// ============================================================

#let manual-title    = "$title$"
#let manual-subtitle = "$subtitle$"
#let manual-version  = "$version$"
#let manual-revision = "$revision$"
#let manual-date     = "$date$"

#let display = ("Gill Sans", "Cabin")
#let mono    = ("DejaVu Sans Mono",)

#let ink   = luma(20)
#let grey  = luma(105)
#let faint = luma(150)
#let hair  = luma(205)

#let label(body) = text(font: display, style: "italic", weight: "bold")[#upper(body)]

// ── document + base text ────────────────────────────────────
#set document(title: manual-title, author: "hendl")
#set text(font: display, fill: ink, size: 10.5pt, lang: "en", hyphenate: false)
#set par(justify: false, leading: 0.68em, spacing: 1.15em, first-line-indent: 0pt)
#set heading(numbering: none)
#show raw: set text(font: mono, size: 9pt)
#show link: it => text(weight: "bold")[#it]
#show emph: it => text(style: "italic")[#it]
#show regex("(?i)\\bhendl([-.]\\w)?"): it => {
  if lower(it.text) == "hendl" { text(style: "italic")[HENDL] } else { it }
}

#let horizontalrule = align(center, block(above: 1.2em, below: 1.2em,
  line(length: 38%, stroke: 0.5pt + hair)))

// ── headings ────────────────────────────────────────────────
#show heading: set text(font: display, style: "italic", weight: "bold", hyphenate: false)

#show heading.where(level: 1): it => {
  pagebreak(weak: true)
  block(above: 0pt, below: 1.4em, width: 100%)[
    #set text(size: 26pt, tracking: 0.01em)
    #upper(it.body)
    #v(0.35em, weak: true)
    #line(length: 100%, stroke: 1pt + ink)
  ]
}

#show heading.where(level: 2): it => block(above: 1.9em, below: 0.7em)[
  #set text(size: 13pt, tracking: 0.02em)
  #upper(it.body)
]

#show heading.where(level: 3): it => block(above: 1.3em, below: 0.45em)[
  #set text(size: 10pt, tracking: 0.05em, fill: grey)
  #upper(it.body)
]

// ── lists ───────────────────────────────────────────────────
#set list(indent: 0.6em, spacing: 0.9em, marker: text(fill: faint)[–])
#set enum(indent: 0.6em, spacing: 0.9em)

// ── definition lists → uppercase-italic term, inline body ───
#show terms.item: it => block(breakable: false, above: 0.85em)[
  #label(it.term) #h(0.6em) #it.description
]

// ── tables ──────────────────────────────────────────────────
#set table(
  stroke: (_, y) => (bottom: if y == 0 { none } else { 0.4pt + hair }),
  inset: (x: 4pt, y: 8pt),
  align: left + horizon,
)
#show table.cell.where(y: 0): it => {
  set text(font: display, style: "italic", weight: "bold", size: 8.5pt, tracking: 0.03em)
  upper(it)
}
#show figure.where(kind: table): set block(breakable: false)
#show figure.where(kind: table): set figure(supplement: none, numbering: none)
#show figure.where(kind: table): set align(left)

// ── notes (blockquotes) ─────────────────────────────────────
#show quote.where(block: true): it => block(
  width: 100%, inset: (left: 1em, y: 0.4em), above: 1.3em, below: 1.3em,
  stroke: (left: 1.5pt + ink),
)[
  #text(font: display, style: "italic", weight: "bold", size: 8pt, fill: grey)[NOTE]
  #linebreak()
  #set text(fill: grey)
  #it.body
]

// ============================================================
//  COVER
// ============================================================
#page(paper: "a4", margin: (x: 2cm, top: 1.8cm, bottom: 1.6cm), numbering: none)[
  #set par(spacing: 0pt)
  #place(top + right, dy: 0.2em)[
    #text(font: display, style: "italic", size: 9pt, fill: faint)[VERSION #manual-version]
  ]

  #text(font: display, style: "italic", weight: "bold", size: 13pt, fill: grey, tracking: 0.18em)[
    WGD MODULAR
  ]
  #v(0.7em)
  #text(font: display, style: "italic", weight: "bold", size: 82pt, tracking: 0.005em)[
    #upper(manual-title)
  ]
  #v(0.25em)
  #text(font: display, style: "italic", size: 15pt, fill: grey, tracking: 0.02em)[
    #upper(manual-subtitle)
  ]

  #v(0.8cm)
  #align(center)[#image("/docs/assets/front-panel.svg", height: 16.5cm)]

  #place(bottom + left)[
    #line(length: 3.2cm, stroke: 1.2pt + ink)
    #v(0.6em)
    #text(font: display, style: "italic", size: 12pt)[USER MANUAL]
    #v(0.35em)
    #text(font: display, style: "italic", size: 9pt, fill: faint)[
      V#manual-version · #manual-revision · #manual-date
    ]
    #v(0.2em)
    #text(font: display, style: "italic", size: 9pt, fill: grey)[
      #link("https://wgdmodular.de")[wgdmodular.de]
    ]
  ]
]

// ============================================================
//  RUNNING PAGES
// ============================================================
#set page(
  paper: "a4",
  margin: (x: 2.4cm, top: 2.4cm, bottom: 2cm),
  numbering: "1",
  header: context {
    let pg = here().page()
    let cur = none
    for h in query(heading.where(level: 1)) {
      if h.location().page() <= pg { cur = h.body }
    }
    set text(font: display, style: "italic", size: 8pt, fill: faint)
    grid(columns: (1fr, 1fr),
      align(left)[WGD MODULAR],
      align(right)[#if cur != none { upper(cur) }],
    )
    v(-0.3em)
    line(length: 100%, stroke: 0.4pt + hair)
  },
  footer: context {
    set text(font: display, style: "italic", size: 8pt, fill: faint)
    line(length: 100%, stroke: 0.4pt + hair)
    v(0.15em)
    grid(columns: (1fr, 1fr),
      align(left)[#counter(page).display("1")],
      align(right)[HENDL · V#manual-version · #manual-revision],
    )
  },
)
#counter(page).update(1)

// ── table of contents ───────────────────────────────────────
#{
  set text(font: display)
  block(below: 1.2em)[
    #text(style: "italic", weight: "bold", size: 20pt)[#upper("Contents")]
    #v(0.3em, weak: true)
    #line(length: 100%, stroke: 1pt + ink)
  ]
}
#show outline.entry: it => {
  set text(font: display, style: "italic", size: 10.5pt)
  it
}
#outline(title: none, depth: 2, indent: 1.2em)

$body$

// ============================================================
//  PATCH SHEET — final page
// ============================================================
#let fieldline(w) = box(width: w, baseline: 0.1em, line(length: 100%, stroke: 0.5pt + grey))
#let notelines(n) = stack(spacing: 0.92cm, ..range(n).map(_ => line(length: 100%, stroke: 0.4pt + hair)))

= Patch Sheet

#grid(columns: (1fr, 1fr), gutter: 1.2cm,
  [#label("Patch")  #fieldline(1fr)],
  [#label("Date")  #fieldline(1fr)],
)
#v(1.0em)

#grid(columns: (auto, 1fr), gutter: 1.1cm, align: top,
  box(stroke: 0.6pt + luma(165), inset: 0pt,
    image("/docs/assets/front-panel-blank.svg", height: 18cm)),
  [
    #label("Patched")
    #v(0.5em)
    #notelines(8)
    #v(1.3em)
    #label("Notes")
    #v(0.5em)
    #notelines(8)
  ],
)

# hendl manual

The user manual is written as Markdown in `content/` and rendered to a single
PDF with [Typst](https://typst.app), driven by pandoc. Everything is wired
through pixi, so there is nothing to install by hand.

## Build it

```sh
pixi run docs
```

The PDF lands at `build/hendl-manual.pdf`. The firmware version in the footer is
read from `.release-please-manifest.json`, with the short commit hash appended.

## Editing

- Content lives in `content/*.md` and `content/modes/*.md` — plain Markdown that
  also reads well straight on GitHub.
- Reading order and which files are included is the `PAGES` list in
  [`../scripts/build_docs.py`](../scripts/build_docs.py). Numeric filename
  prefixes keep the on-disk order matching.
- Each top-level `#` heading starts a new section on its own page. Use `##` and
  `###` for sub-sections; they appear in the table of contents down to `##`.
- Use blockquotes (`>`) for callouts — they render as a **NOTE** block.
- Tables render as flush-left reference blocks; the header row is set in the
  display face automatically.

## Design

The whole look is one file: [`template/manual.typ`](template/manual.typ).

Type is set in **Gill Sans** — the house face — when it is installed on the
machine doing the build. Where it is not (CI, most non-macOS machines) it falls
back to the bundled OFL **Cabin**, a humanist sans in the same spirit, so the
manual always renders. Only the typeface differs; the layout is identical.

The front panel on the cover is `assets/front-panel.svg`, the same artwork as
the physical panel.

The manual ends with a **patch sheet** — a drawable, single-tone version of the
panel (`assets/front-panel-blank.svg`) with near-white pot/jack holes and open
mode LEDs, alongside space to record settings and connections. That blank panel
is generated from the real one with `scripts/make_blank_panel.py`; re-run it only
if `front-panel.svg` changes.

## In CI

`.github/workflows/docs.yml` builds the manual on every push and pull request
and uploads `hendl-manual.pdf` as a downloadable artifact.

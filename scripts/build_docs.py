#!/usr/bin/env python3
"""Build the hendl user manual PDF from the Markdown sources in docs/.

Markdown is converted to a Typst document with pandoc, then rendered to PDF
by typst. The two-step keeps the intermediate .typ inside the repo so the
front-panel SVG and bundled fonts resolve against the project root.
"""

import datetime
import json
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DOCS = ROOT / "docs"
OUT = ROOT / "build"
TEMPLATE = DOCS / "template" / "manual.typ"
FONT_DIR = DOCS / "assets" / "fonts"

# Reading order of the manual. Numeric filenames keep the on-disk order sane,
# but the sequence that actually matters lives here so it is easy to reshuffle.
PAGES = [
    "content/01-overview.md",
    "content/02-quick-start.md",
    "content/03-controls.md",
    "content/04-normalization.md",
    "content/05-saving.md",
    "content/modes/00-overview.md",
    "content/modes/10-lfo.md",
    "content/modes/20-envelope.md",
    "content/modes/30-sample-hold.md",
    "content/modes/40-turing.md",
    "content/modes/50-euclidean.md",
    "content/modes/60-smooth-random.md",
    "content/90-specifications.md",
]


def run(cmd):
    subprocess.run(cmd, cwd=ROOT, check=True)


def firmware_version():
    manifest = ROOT / ".release-please-manifest.json"
    try:
        return json.loads(manifest.read_text())["."]
    except (OSError, KeyError, json.JSONDecodeError):
        pass
    for line in (ROOT / "pixi.toml").read_text().splitlines():
        if line.strip().startswith("version"):
            return line.split("=", 1)[1].strip().strip('"')
    return "0.0.0"


def git(*args, default=""):
    try:
        out = subprocess.run(
            ["git", *args], cwd=ROOT, capture_output=True, text=True, check=True
        )
        return out.stdout.strip()
    except (OSError, subprocess.CalledProcessError):
        return default


def revision():
    sha = git("rev-parse", "--short", "HEAD", default="local")
    dirty = git("status", "--porcelain")
    return f"{sha}+" if dirty else sha


def build_date():
    iso = git("log", "-1", "--format=%cs")
    return iso or datetime.date.today().isoformat()


def main():
    for tool in ("pandoc", "typst"):
        if shutil.which(tool) is None:
            sys.exit(f"error: '{tool}' not found — run inside 'pixi run docs'")

    missing = [p for p in PAGES if not (DOCS / p).exists()]
    if missing:
        sys.exit("error: missing content:\n  " + "\n  ".join(missing))

    OUT.mkdir(exist_ok=True)
    typ = OUT / "manual.typ"
    pdf = OUT / "hendl-manual.pdf"

    version = firmware_version()
    print(f"hendl manual · v{version} · {revision()} · {build_date()}")

    run([
        "pandoc",
        *[str(DOCS / p) for p in PAGES],
        "--from=gfm",
        "--to=typst",
        "--standalone",
        "--wrap=none",
        "--syntax-highlighting=none",
        f"--template={TEMPLATE}",
        "--metadata=title:hendl",
        "--metadata=subtitle:Multi-Modulation Source",
        f"--metadata=version:{version}",
        f"--metadata=revision:{revision()}",
        f"--metadata=date:{build_date()}",
        f"--output={typ}",
    ])

    # pandoc wraps every table in `align(center)`, which the cells then inherit.
    # Flush-left reads more deliberate for a control reference.
    typ.write_text(typ.read_text().replace("align(center)[#table", "align(left)[#table"))

    run([
        "typst", "compile", str(typ), str(pdf),
        "--root", str(ROOT),
        "--font-path", str(FONT_DIR),
    ])

    print(f"\n  {pdf.relative_to(ROOT)}  ({pdf.stat().st_size // 1024} KB)")


if __name__ == "__main__":
    main()

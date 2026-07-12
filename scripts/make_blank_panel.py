#!/usr/bin/env python3
"""Generate the greyed, drawable "blank" panel used by the patch sheet.

Derives docs/assets/front-panel-blank.svg from the real front-panel.svg:
  - drops the black backdrop,
  - collapses every silk / copper / stroke colour to one light grey,
  - leaves the 15 pot/jack hole centres near-white so knob positions can be marked,
  - turns the 6 mode LEDs into open rings.

Run this only when front-panel.svg changes; the output is committed.
"""

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SRC = ROOT / "docs" / "assets" / "front-panel.svg"
OUT = ROOT / "docs" / "assets" / "front-panel-blank.svg"

TONE = "rgb(203,203,203)"   # the single panel tone
HOLES = "rgb(250,250,250)"  # near-white pot/jack centres
HOLE_R = "9.213"            # radius of the 15 pot/jack holes
LED_R = "4.345"             # radius of the 6 mode LEDs


def _circles(radius):
    return re.compile(r'(<circle\b[^>]*?r="' + re.escape(radius) + r'"[^>]*?)(/?>)')


def _set_fill(svg, radius, fill):
    sub = lambda m: re.sub(r"fill:rgb\([0-9, ]+\)", f"fill:{fill}", m.group(1)) + m.group(2)
    return _circles(radius).sub(sub, svg)


def _open_ring(svg, radius, stroke, width):
    repl = f"fill:none;stroke:{stroke};stroke-width:{width}"
    sub = lambda m: re.sub(r"fill:rgb\([0-9, ]+\)", repl, m.group(1)) + m.group(2)
    return _circles(radius).sub(sub, svg)


def main():
    svg = SRC.read_text()
    svg = re.sub(r'<g id="background">.*?</g>', "", svg, flags=re.S)
    svg = re.sub(r"(fill|stroke):white", lambda m: f"{m.group(1)}:{TONE}", svg)
    svg = re.sub(r"(fill|stroke):rgb\([0-9, ]+\)", lambda m: f"{m.group(1)}:{TONE}", svg)
    svg = _set_fill(svg, HOLE_R, HOLES)
    svg = _open_ring(svg, LED_R, TONE, "1.4")
    OUT.write_text(svg)
    print(f"wrote {OUT.relative_to(ROOT)}")


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""Generate the greyed, drawable "blank" panel used by the patch sheet.

Derives docs/assets/front-panel-blank.svg from the real front-panel.svg:
  - drops the black backdrop,
  - collapses every silk / copper / stroke colour to one light grey,
  - gives the channel/jack number glyphs (which have no fill, so default to
    black) an explicit white fill, so they read as clean digits on the discs,
  - leaves the 15 pot/jack hole centres near-white so knob positions can be marked,
  - turns the 6 mode LEDs into clean open rings (dropping the knurled collar
    each one is drawn over); the OUT trimmers keep their fill.

Run this only when front-panel.svg changes; the output is committed.
"""

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SRC = ROOT / "docs" / "assets" / "front-panel.svg"
OUT = ROOT / "docs" / "assets" / "front-panel-blank.svg"

TONE = "rgb(185,185,185)"    # the single panel tone
HOLES = "rgb(250,250,250)"   # near-white pot/jack centres
DIGITS = "white"             # channel/jack number glyphs
HOLE_R = "9.213"             # radius of the 15 pot/jack holes
LED_R = "4.345"              # radius of the mode LEDs (and OUT trimmer centres)
LED_COLLAR = "M44.819,172.952"  # knurled collar path drawn under each mode LED


def _set_hole_fill(svg, radius, fill):
    circ = re.compile(r'(<circle\b[^>]*?r="' + re.escape(radius) + r'"[^>]*?)(/?>)')
    sub = lambda m: re.sub(r"fill:rgb\([0-9, ]+\)", f"fill:{fill}", m.group(1)) + m.group(2)
    return circ.sub(sub, svg)


def _fill_untyped(svg, fill):
    """Elements with no style attribute default to black; give them a fill."""
    def add(m):
        tag = m.group(0)
        if "style=" in tag:
            return tag
        return (tag[:-2] + f' style="fill:{fill}"/>') if tag.endswith("/>") \
            else (tag[:-1] + f' style="fill:{fill}">')
    return re.sub(r"<(?:path|circle)\b[^>]*>", add, svg)


def _open_leds(svg):
    """Hide the knurled collar and open the LED circle, but only for the 6 mode
    LEDs (the ones drawn over a collar) — the collar-less OUT trimmers are left
    filled so they still read as small knobs."""
    pair = re.compile(
        r'(<path\b[^>]*d="' + re.escape(LED_COLLAR) + r'[^"]*"[^>]*>)(\s*)'
        r'(<circle\b[^>]*r="' + re.escape(LED_R) + r'"[^>]*>)'
    )

    def repl(m):
        collar = re.sub(r"fill:(?:white|rgb\([0-9, ]+\))", "fill:none", m.group(1))
        led = re.sub(r"fill:(?:white|rgb\([0-9, ]+\))",
                     f"fill:none;stroke:{TONE};stroke-width:1.4", m.group(3))
        return collar + m.group(2) + led

    return pair.sub(repl, svg)


def main():
    svg = SRC.read_text()
    svg = re.sub(r'<g id="background">.*?</g>', "", svg, flags=re.S)
    svg = re.sub(r"(fill|stroke):white", lambda m: f"{m.group(1)}:{TONE}", svg)
    svg = re.sub(r"(fill|stroke):rgb\([0-9, ]+\)", lambda m: f"{m.group(1)}:{TONE}", svg)
    svg = _fill_untyped(svg, DIGITS)
    svg = _set_hole_fill(svg, HOLE_R, HOLES)
    svg = _open_leds(svg)
    OUT.write_text(svg)
    print(f"wrote {OUT.relative_to(ROOT)}")


if __name__ == "__main__":
    main()

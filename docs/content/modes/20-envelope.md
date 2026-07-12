# Envelope

A two-stage attack-decay envelope with an exponential shape. Fire it from a
trigger for percussive shapes, or let it loop as a rounded, envelope-shaped LFO.

## Controls

| Control | Function |
|---------|----------|
| **Pot 1** | Attack time (1 ms – 4 s) |
| **Pot 2** | Decay time (1 ms – 8 s) |
| **CV** | Decay modulation |
| **Trigger** | Start / retrigger the envelope |

## How it works

A rising edge at the **trigger** starts the attack: the output rises to full
level over the **Pot 1** time, then falls back to zero over the **Pot 2** time.
Both stages are exponential, giving a natural, snappy contour rather than a
straight-line ramp. Both time ranges are logarithmic, so there is fine control
over short times and plenty of reach into long, slow shapes.

With a cable in the **CV** input, CV scales the decay time around the knob — a
quick way to make repeated hits open and close under external control.

## Looping vs. triggered

- **Triggered** — one attack-decay cycle per trigger. This is the classic
  envelope: patch a gate or trigger and play it.
- **Looping** — the envelope repeats on its own, running attack-decay-attack
  continuously without any trigger. Pot 1 and Pot 2 now set the up and down
  times of a shape you can use as an LFO. A trigger still restarts the cycle.

## Patch ideas

- **Triggered**, short attack, medium decay, into a VCA for plucks.
- **Looping**, long attack, long decay, into a filter for slow tidal swells.
- Patch a slow LFO into **CV** to breathe the decay time in and out.

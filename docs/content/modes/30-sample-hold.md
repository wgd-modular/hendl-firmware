# Sample & Hold

Samples a signal on each clock and holds it steady until the next one. Patch a
source to sample it; patch nothing and hendl samples its own internal noise for
classic random stepped voltages.

## Controls

| Control | Function |
|---------|----------|
| **Pot 1** | Internal clock rate (0.05 – 30 Hz) |
| **Pot 2** | Level — attenuates the held value |
| **CV** | Signal to be sampled |
| **Trigger** | External clock (in triggered mode) |

## How it works

On every clock, hendl reads the input and freezes it at the output until the
next clock. There is no slew — steps are instant.

- With a cable in the **CV** input, that signal is what gets sampled. Feed it an
  LFO, another modulator, or a slow CV to quantise into steps.
- With **nothing patched**, hendl samples an internal noise source, giving the
  familiar random stepped voltages of a classic sample & hold.

**Pot 2** scales the held value, so you can set exactly how far the steps range —
from tiny wobble to the full 0–5 V.

## Looping vs. triggered

- **Looping** — the internal clock runs the sample rate, set by **Pot 1**. Turn
  it up for a busy stream of steps, down for slow, deliberate changes.
- **Triggered** — each rising edge at the **trigger** takes one sample, so the
  steps land exactly with your clock or rhythm.

## Patch ideas

- Unpatched, **looping**, into pitch through a quantiser for random melodies.
- **Triggered** from a clock, sampling a slow LFO on **CV**, for a stepped
  version of that LFO locked to tempo.

# The Interface

Every channel is controlled the same way. Once you know one channel, you know
all three.

## Pots

The two potentiometers are the primary parameters of the active mode. Their
function changes from mode to mode — and sometimes between looping and triggered
— so each mode chapter lists exactly what Pot 1 and Pot 2 do. As a rough guide:

- **Pot 1** tends to be the *time* parameter — rate, attack, clock speed,
  sequence length.
- **Pot 2** tends to be the *shape* parameter — waveform, decay, fill, level.

## CV input

Each channel has one CV input that modulates a parameter of the active mode.
hendl continuously senses whether a cable is patched (see *Normalization*), and
**CV only takes effect when a cable is present**. With nothing patched, the
knobs are fully in charge.

CV is read as a control signal centred around the middle of its range, so a
CV at rest sits at "no change" and swings the parameter up or down from there.

## Trigger input

The trigger input reacts to a **rising edge**. Depending on the mode and the
operating mode it will:

- reset an LFO's phase,
- fire or retrigger an envelope,
- take a new sample in Sample & Hold,
- or advance a sequencer by one step.

Triggers are read continuously, so anything from a short trigger to a long gate
works as a clock.

## The mode button

One button per channel does two jobs:

| Action | Result |
|--------|--------|
| **Short press** | Advance to the next mode (LFO → Envelope → S&H → Unique → LFO) |
| **Long press** (~0.8 s) | Toggle the channel between **looping** and **triggered** |

The button is debounced, and a long press does not also change the mode.

## The LEDs

The two LEDs per channel are a two-bit display of the active mode, and they
double as feedback:

- **Mode** — shown as a binary count (see *Quick Start*).
- **Operating mode change** — a long press plays a short confirmation
  animation: both LEDs **flash together** for triggered, or **alternate** for
  looping.

## Looping vs. triggered

This single choice reshapes every mode:

| | Looping | Triggered |
|---|---------|-----------|
| **Clock source** | internal, free-running | external trigger input |
| **Feels like** | an always-moving modulator | a synced, playable voice |
| **Patch** | nothing required | clock / trigger into the channel |

Each mode chapter describes exactly what changes between the two.

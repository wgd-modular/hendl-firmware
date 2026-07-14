# Euclidean Sequencer

*Channel 2's unique mode.*

Generates rhythmic trigger patterns by spreading a number of hits as evenly as
possible across the pattern length — the Euclidean rhythms behind countless
drum and percussion patterns.

## Controls

| Control | Function |
|---------|----------|
| **Pot 1** | Length — total steps (2 – 16) |
| **Pot 2** | Steps — how many of them play (the hits) |
| **CV** | Rotation — shifts the pattern's start point |
| **Trigger** | Advance one step (in triggered mode) |

## How it works

Set the pattern **length** with Pot 1 and how many **steps** play with Pot 2, and
hendl spreads those active steps as evenly as the length allows. A few active
steps in a long pattern give sparse, syncopated rhythms; filling most of them
gives dense, driving ones. On every active step the output fires a short
**trigger pulse**; the rest stay low.

With a cable in the **CV** input, CV **rotates** the pattern, sliding which step
counts as the first. Rotation keeps the same rhythm but moves its accent, so a
slow modulation into CV gradually shifts the groove.

> The number of active steps is read as a proportion of the length, so changing
> the length keeps a similar density rather than a fixed count.

## Looping vs. triggered

- **Looping** — an internal clock steps the pattern on its own.
- **Triggered** — each rising edge at the **trigger** advances one step, so the
  pattern runs from your master clock and stays locked to everything else.

Because the output is a trigger pattern rather than a smooth voltage, this mode
is at its best driving drums, envelopes and other trigger inputs.

## Patch ideas

- **Triggered** from a clock into a drum voice — dial length and steps for
  instant Euclidean grooves.
- Patch a slow LFO into **CV** (rotation) to keep the pattern turning over so it
  never feels static.
- Run all three of hendl's channels together: this one as rhythm, the others as
  the modulation it triggers.

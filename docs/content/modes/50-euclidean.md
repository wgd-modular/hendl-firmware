# Euclidean Sequencer

*Channel 2's unique mode.*

Generates rhythmic trigger patterns by spreading a number of hits as evenly as
possible across a number of steps — the Euclidean rhythms behind countless
drum and percussion patterns.

## Controls

| Control | Function |
|---------|----------|
| **Pot 1** | Length — total steps (2 – 16) |
| **Pot 2** | Fill — number of hits |
| **CV** | Rotation — shifts the pattern's start point |
| **Trigger** | Advance one step (in triggered mode) |

## How it works

Set the pattern **length** with Pot 1 and the number of **hits** with Pot 2, and
hendl distributes those hits as evenly as the length allows. A few hits in many
steps gives sparse, syncopated patterns; filling most of the steps gives dense,
driving ones. On every step that lands on a hit, the output fires a short
**trigger pulse**; steps without a hit stay low.

With a cable in the **CV** input, CV **rotates** the pattern, sliding which step
counts as the first. Rotation keeps the same rhythm but moves its accent, so a
slow modulation into CV gradually shifts the groove.

> Fill is read as a proportion of the length, so changing the length keeps a
> similar density rather than a fixed number of hits.

## Looping vs. triggered

- **Looping** — an internal clock steps the pattern on its own.
- **Triggered** — each rising edge at the **trigger** advances one step, so the
  pattern runs from your master clock and stays locked to everything else.

Because the output is a trigger pattern rather than a smooth voltage, this mode
is at its best driving drums, envelopes and other trigger inputs.

## Patch ideas

- **Triggered** from a clock into a drum voice — dial length and fill for
  instant Euclidean grooves.
- Patch a slow LFO into **CV** (rotation) to keep the pattern turning over so it
  never feels static.
- Run all three of hendl's channels together: this one as rhythm, the others as
  the modulation it triggers.

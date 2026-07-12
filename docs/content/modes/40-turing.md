# Turing Machine

*Channel 1's unique mode.*

A looping random sequencer built on a 16-bit shift register. It generates a
looping pattern of stepped voltages that you can freeze, mutate or let run wild
— the classic "random but repeating" sequencer.

## Controls

| Control | Function |
|---------|----------|
| **Pot 1** | Change — from locked, through random, to locked-inverted |
| **Pot 2** | Loop length (2 – 16 steps) |
| **CV** | Bias applied to the Change amount |
| **Trigger** | Advance one step (in triggered mode) |

## The Change knob

Pot 1 is the heart of the mode. It sets the chance that each step mutates as the
pattern cycles:

- **Fully counter-clockwise** — locked. The pattern repeats exactly, a stable
  loop of the chosen length.
- **Centre** — maximum randomness. Every step is a coin flip and the pattern
  never settles.
- **Fully clockwise** — locked again, but *inverted*. The loop is stable and
  repeats over **twice** the chosen length, with the second pass the mirror of
  the first.

Anywhere between the ends, the pattern slowly evolves — a little mutation near
the extremes, total chaos toward the middle. This is how you "grow" a sequence:
find something you like near the centre, then turn toward an end to lock it in.

> The clockwise end locks a loop of double the selected length because it
> inverts the recycled step each pass. It is a locked loop like the
> counter-clockwise end, just longer and mirrored.

## Length and output

**Pot 2** sets the loop length from 2 to 16 steps. Shorter loops give tight,
recognisable motifs; longer loops give sprawling sequences that take longer to
come around. The output is the shift register read as a voltage, so each locked
pattern is a fixed sequence of levels — patch it through a quantiser for
melodies, or straight into any parameter for repeating modulation.

With a cable in the **CV** input, CV biases the Change amount, so you can open a
locked loop up to mutation and close it again from another modulator or a manual
control voltage.

## Looping vs. triggered

- **Looping** — an internal clock advances the sequence at a steady pace.
- **Triggered** — each rising edge at the **trigger** advances exactly one step,
  locking the sequence to your clock.

## Patch ideas

- **Triggered** from your clock, into a quantiser, Change near the centre —
  turn slowly toward an end until a melody locks.
- Short length, fully locked, into a filter for a repeating rhythmic sweep.

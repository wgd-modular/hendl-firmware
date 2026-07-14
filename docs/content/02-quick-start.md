# Quick Start

## Power

hendl is a standard Eurorack module. Switch off your case, connect the ribbon
cable with the **red stripe to −12 V** on both the module and the bus board,
then power up.

<!-- TODO: confirm and add power draw (mA on +12 / −12) -->

## Your first modulation

1. Take any channel's **CV output** to something you can hear move — a filter
   cutoff, a VCA, an oscillator's pitch.
2. Leave the mode on its default (**LFO**, both channel LEDs off).
3. Turn **Pot 1** to set the rate and **Pot 2** to sweep through the waveforms.

That is a free-running LFO. No patch cables into hendl are required — in
**looping** mode every channel generates on its own.

## Clocking it to your patch

1. Patch a clock or trigger into the channel's **trigger input**.
2. **Long-press** the channel button once. The LEDs double-flash to confirm the
   channel is now in **triggered** mode.
3. The channel now locks to your clock — the LFO becomes a clock-synced ramp,
   the sequencers advance one step per trigger, and so on.

Long-press again to return to **looping**.

## Reading the LEDs

The two LEDs show the active mode as a simple count:

| LEDs | Mode |
|------|------|
| ○ ○ | LFO |
| ○ ● | Envelope |
| ● ○ | Sample & Hold |
| ● ● | Unique (Turing / Euclidean / Smooth Random) |

Short-press the button to step through them. When you toggle looping /
triggered, the LEDs play a short animation: they **flash together** for
triggered, and **alternate** for looping.

Your mode and looping/triggered choices are saved automatically and restored
the next time you power up.

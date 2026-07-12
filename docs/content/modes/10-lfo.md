# LFO

A low-frequency oscillator with five continuously blended waveforms. In looping
mode it free-runs as a classic LFO; in triggered mode it becomes a
clock-synced ramp that locks to your patch.

## Controls

| Control | Looping | Triggered |
|---------|---------|-----------|
| **Pot 1** | Rate (0.05 – 20 Hz) | Phase offset (0 – 360°) |
| **Pot 2** | Waveform | Waveform |
| **CV** | Rate modulation | Waveform modulation |
| **Trigger** | Reset phase | Set period · reset phase |

## Waveforms

Pot 2 sweeps smoothly through five shapes, crossfading from one to the next:

**sine → triangle → ramp up → ramp down → square**

Because the shapes are blended rather than switched, the positions between them
are useful in their own right — a rounded ramp, a soft square, and so on.

## Looping

The LFO runs from its own clock. **Pot 1** sets the rate over a wide,
musical range from very slow sweeps up to audible-edge territory. A **trigger**
resets the phase to the start of the cycle, so you can re-align the LFO to a
downbeat at any time. With a cable in the **CV** input, CV pushes the rate up
and down around the knob setting.

## Triggered

The LFO locks to your clock. It measures the time between two rising edges and
stretches exactly one cycle across that interval, resetting phase on every edge
— so one LFO cycle equals one clock period. **Pot 1** now slides the output
phase, letting you dial the LFO early or late against the clock. **Pot 2** still
chooses the waveform, and patched **CV** nudges the waveform selection.

> In triggered mode the LFO needs two triggers to learn the tempo. The first
> edge starts timing; from the second onward it tracks your clock.

## Patch ideas

- **Looping**, slow rate, ramp-up into a filter for evolving sweeps.
- **Triggered** from your main clock, phase on Pot 1, into pitch for synced
  vibrato that sits exactly where you want it in the bar.

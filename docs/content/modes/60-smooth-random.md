# Smooth Random

*Channel 3's unique mode.*

A continuously wandering random voltage. Instead of stepping between values like
Sample & Hold, it glides smoothly toward each new random target, giving flowing,
organic modulation that never quite repeats.

## Controls

| Control | Function |
|---------|----------|
| **Pot 1** | Smoothness — how quickly it chases each new target |
| **Pot 2** | Change rate — how often a new target is chosen (0.1 – 10 Hz) |
| **CV** | Bias — shifts the range the targets are drawn from |
| **Trigger** | Choose a new target (in triggered mode) |

## How it works

At a rate set by **Pot 2**, hendl picks a new random target value. The output
then slides toward it at a speed set by **Pot 1** — fully clockwise for a quick,
responsive chase, counter-clockwise for long, lazy glides that smooth the motion
into gentle drift. The result is a random voltage with no hard steps.

With a cable in the **CV** input, CV **biases** the targets, nudging the whole
range up or down. Patch a slow envelope or LFO there to make the random motion
drift toward high or low values over time.

## Looping vs. triggered

- **Looping** — new targets are chosen automatically at the **Pot 2** rate.
- **Triggered** — a new target is chosen on each rising edge at the **trigger**,
  so the wandering re-aims in time with your patch while still gliding smoothly
  between points.

## Patch ideas

- Slow change rate, heavy smoothing, into a filter or oscillator for living,
  breathing drift.
- **Triggered** from a slow clock, into pitch through a quantiser, for melodies
  that glide rather than step.
- Bias with a manual CV to lean the randomness bright or dark.

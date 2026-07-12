# Overview

**hendl** is a three-channel modulation source for Eurorack. Each channel is
an independent generator with its own controls, outputs and character, and each
one can run any of four modes. Three of those modes are shared across all
channels; the fourth is unique to each channel, so a single hendl gives you a
small, deep palette of movement — LFOs, envelopes, sample & hold, a Turing
machine, a Euclidean sequencer and a smooth random generator.

There is no menu and no screen. Every parameter is a knob, a jack or the mode
button, and the two LEDs per channel tell you where you are.

## The three channels

The three channels are laid out top to bottom and are identical in their
controls. They differ only in the *unique* mode (mode 4):

| Channel | Shared modes | Unique mode |
|---------|--------------|-------------|
| **Channel 1** | LFO · Envelope · Sample & Hold | **Turing Machine** |
| **Channel 2** | LFO · Envelope · Sample & Hold | **Euclidean Sequencer** |
| **Channel 3** | LFO · Envelope · Sample & Hold | **Smooth Random** |

## What each channel gives you

Every channel has the same physical interface:

| Element | Role |
|---------|------|
| **Pot 1 / Pot 2** | The two main parameters — their meaning depends on the mode |
| **CV input** | Modulates a parameter; hendl senses whether a cable is patched |
| **Trigger input** | Resets, clocks or fires the generator |
| **Mode button** | Short press cycles the mode; long press toggles looping / triggered |
| **Two LEDs** | Show the active mode, and flash to confirm mode changes |
| **CV output** | 0–5 V, the channel's modulation signal |

## Two ways to run

Every mode runs in one of two operating modes, toggled by a long press of the
channel button:

- **Looping** — the generator free-runs from its own internal clock. Patch
  nothing and a channel still moves.
- **Triggered** — the generator is clocked or fired by the trigger input, so it
  locks to the rest of your patch.

The rest of this manual walks through the interface, the way hendl senses your
patch cables, and then each mode in detail.

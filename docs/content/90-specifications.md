# Specifications

| | |
|---|---|
| **Format** | Eurorack |
| **Width** | 10 HP |
| **Depth** | 30 mm incl. power header · 27 mm without |
| **Channels** | 3, independent |
| **Modes per channel** | 4 (LFO · Envelope · Sample & Hold · Unique) |
| **Controls per channel** | 2 pots · 1 CV input · 1 trigger input · 1 button |
| **Indicators per channel** | 2 LEDs |
| **CV outputs** | 3 × 0 – 5 V |
| **Output resolution** | 11-bit, ~73 kHz PWM, RC-smoothed |
| **Processing** | 20 kHz control-rate DSP |
| **Processor** | Raspberry Pi Pico 2 · RP2350 (Cortex-M33 @ 150 MHz) |
| **Patch sensing** | Automatic per-input cable detection |
| **Memory** | Mode and looping/triggered state saved to flash |

<!-- TODO: add power draw (mA @ +12 V / −12 V) -->

## Trigger and CV

- **Triggers** react to a rising edge and accept anything from a short trigger
  to a sustained gate.
- **CV inputs** are active only while a cable is patched, and are read relative
  to their mid-point so a resting CV means "no change".

## Firmware

hendl's behaviour is defined entirely by its firmware, and this manual is built
from the same source tree. The firmware version it documents is printed in the
footer of every page.

## Credits

hendl is designed and built by **WGD Modular** — [wgdmodular.de](https://wgdmodular.de).
It is an open hardware and firmware project; the firmware and this manual live at
[github.com/wgd-modular/hendl-firmware](https://github.com/wgd-modular/hendl-firmware).

The manual is typeset with Typst; body text falls back to the OFL-licensed
*Cabin* typeface where the house *Gill Sans* is unavailable.

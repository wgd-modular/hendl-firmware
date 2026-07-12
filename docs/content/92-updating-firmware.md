# Updating the Firmware

hendl runs on a Raspberry Pi Pico 2, and its firmware updates over USB — no
programmer or special software required.

1. Grab the latest firmware — the `hendl.uf2` file — from the releases page:
   [github.com/wgd-modular/hendl-firmware/releases](https://github.com/wgd-modular/hendl-firmware/releases)
2. Hold the **BOOT** button on the Pico 2 and connect it to your computer with a
   USB cable, keeping BOOT held as you plug in.
3. The Pico appears as a USB flash drive named **RP2350**.
4. Drag `hendl.uf2` onto that drive. It copies across, the Pico reboots on its
   own, and hendl comes back running the new firmware.

That's it — release the BOOT button and you're done. Your saved modes and
looping/triggered settings carry over across updates.

> The firmware, and this manual, are open source at
> [github.com/wgd-modular/hendl-firmware](https://github.com/wgd-modular/hendl-firmware).

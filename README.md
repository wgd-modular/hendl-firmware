# hendl

hendl is a three-channel modulation source for Eurorack, running on a Raspberry
Pi Pico 2 (RP2350). Each channel offers an LFO, an envelope, sample & hold and a
per-channel extra — a Turing machine, a Euclidean sequencer or a smooth random
generator. This repo holds the firmware (C, Pico SDK) and the source for the
user manual.

## Build

Install [pixi](https://pixi.sh), then everything runs through it — the ARM
toolchain and the Pico SDK are fetched automatically on first run:

    pixi run build    # firmware -> build/hendl.uf2
    pixi run docs     # manual   -> build/hendl-manual.pdf
    pixi run flash    # flash a Pico 2 in BOOTSEL mode

`pixi task list` shows the rest.

## Manual

Download the latest [hendl-manual.pdf][manual] from the releases, or build it
yourself with `pixi run docs`. The sources are in [docs/](docs/); see
[docs/README.md](docs/README.md) for how it fits together.

[manual]: https://github.com/wgd-modular/hendl-firmware/releases/latest/download/hendl-manual.pdf

## License

MIT — see [LICENSE](LICENSE).

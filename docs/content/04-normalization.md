# Sensing Your Patch

hendl knows whether a cable is plugged into each CV input, without any special
"normalled" switching jacks. This is what lets the knobs stay in full control
until the moment you patch something in.

## How it works

A faint probe signal is quietly driven into all three CV inputs through a very
high resistance. With **nothing patched**, the input follows that probe, and
hendl sees its own signal come back. As soon as you patch a **real cable**, your
source easily overrides the tiny probe, the returning signal no longer matches,
and hendl marks that input as *patched*.

The detection runs continuously and settles within a few milliseconds of
plugging or unplugging, so it always reflects the current state of your patch.

> The probe is far below the threshold of anything you will hear or see at the
> output. It exists only so hendl can tell patched from unpatched.

## What it changes

For every mode, the rule is the same:

- **Unpatched** — CV is ignored. Pot 1 and Pot 2 fully define the sound.
- **Patched** — the CV input modulates the mode's CV destination, listed in
  each mode chapter.

This means a patch cable is effectively a switch: plug one in to *bring CV into
play*, pull it out to hand control back to the panel. It also means you can
leave a channel completely unpatched and it will still generate — useful for
self-contained movement with zero cables.

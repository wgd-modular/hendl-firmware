#!/usr/bin/env python3
"""
Download ARM GNU Toolchain 13.3 into .toolchain/
Idempotent — skips if already present.
"""

import shutil
import sys
import tarfile
import tempfile
import urllib.request
from pathlib import Path

VERSION = "13.3.rel1"
ROOT = Path(__file__).parent.parent
DEST = ROOT / ".toolchain"
MARKER = DEST / ".ok"

URLS = {
    "linux": (
        f"https://developer.arm.com/-/media/Files/downloads/gnu/{VERSION}/binrel/"
        f"arm-gnu-toolchain-{VERSION}-x86_64-arm-none-eabi.tar.xz"
    ),
    "macos-arm64": (
        f"https://developer.arm.com/-/media/Files/downloads/gnu/{VERSION}/binrel/"
        f"arm-gnu-toolchain-{VERSION}-darwin-arm64-arm-none-eabi.tar.xz"
    ),
    "macos-x64": (
        f"https://developer.arm.com/-/media/Files/downloads/gnu/{VERSION}/binrel/"
        f"arm-gnu-toolchain-{VERSION}-darwin-x86_64-arm-none-eabi.tar.xz"
    ),
}


def main():
    platform = sys.argv[1] if len(sys.argv) > 1 else "linux"
    if MARKER.exists():
        print("[toolchain] Already installed, skipping.")
        return

    url = URLS[platform]
    archive = ROOT / Path(url).name

    if not archive.exists():
        print(f"[toolchain] Downloading {url} ...")
        urllib.request.urlretrieve(
            url,
            archive,
            reporthook=lambda b, bs, t: print(
                f"\r  {min(b * bs, t) / 1e6:.1f}/{t / 1e6:.1f} MB", end="", flush=True
            ),
        )
        print()

    print("[toolchain] Extracting (this takes ~30s) ...")
    with tempfile.TemporaryDirectory(dir=ROOT) as tmp:
        tmp_path = Path(tmp)
        with tarfile.open(archive) as tf:
            tf.extractall(
                tmp_path, filter="tar"
            )  # filter="tar" erlaubt Symlinks sicher
        extracted = next(tmp_path.iterdir())  # einziger Unterordner im Archiv
        if DEST.exists():
            shutil.rmtree(DEST)
        shutil.move(str(extracted), str(DEST))

    archive.unlink()
    MARKER.touch()
    print(f"[toolchain] Done → {DEST}/bin/arm-none-eabi-gcc")


if __name__ == "__main__":
    main()

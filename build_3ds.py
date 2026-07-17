#!/usr/bin/env python3
"""
build_3ds.py
------------
Wrapper que chama Platforms/Nintendo3DS/export_3ds.py para compilar o jogo
usando devkitARM e gerar build/game.elf e build/game.3dsx.
"""
import os
import sys
import subprocess

ROOT_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
SCRIPT = os.path.join(ROOT_DIR, "Platforms", "Nintendo3DS", "export_3ds.py")


def main():
    return subprocess.call([sys.executable, SCRIPT])


if __name__ == "__main__":
    sys.exit(main())

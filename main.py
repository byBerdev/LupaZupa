#!/usr/bin/env python3
"""
LupaZupaEngine Editor
=====================
Editor visual multiplataforma (Windows/Linux/macOS) construído com PySide6.

Permite:
- Criar/abrir cenas (.json)
- Adicionar/remover entidades
- Editar Transform (posição, rotação, escala)
- Atribuir sprites e scripts Lua às entidades
- Exportar o jogo para PC, Nintendo 3DS e Nintendo GameCube (via Exporters/)

Rodar com:
    pip install -r requirements.txt
    python main.py
"""
import sys
from PySide6.QtWidgets import QApplication

from editor_window import EditorWindow


def main():
    app = QApplication(sys.argv)
    app.setApplicationName("LupaZupaEngine Editor")

    window = EditorWindow()
    window.show()

    sys.exit(app.exec())


if __name__ == "__main__":
    main()

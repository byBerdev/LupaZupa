"""
editor_window.py
-----------------
Janela principal do Editor da LupaZupaEngine. Mostra:
- Uma árvore de entidades da cena atual (esquerda)
- Um inspetor de propriedades (Transform, Sprite, Script) da entidade
  selecionada (direita)
- Um menu para Novo/Abrir/Salvar cena e para Exportar o jogo para
  cada plataforma suportada
"""
import json
import os
import subprocess
import sys

from PySide6.QtWidgets import (
    QMainWindow, QWidget, QHBoxLayout, QVBoxLayout, QFormLayout,
    QTreeWidget, QTreeWidgetItem, QLineEdit, QDoubleSpinBox, QPushButton,
    QFileDialog, QMenuBar, QMenu, QMessageBox, QLabel
)
from PySide6.QtGui import QAction

ROOT_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
EXPORTERS_DIR = os.path.join(ROOT_DIR, "Exporters")
EXAMPLES_DIR = os.path.join(ROOT_DIR, "Examples")


class EditorWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("LupaZupaEngine Editor")
        self.resize(1000, 650)

        self.scene_data = {"name": "NovaCena", "entities": []}
        self.current_scene_path = None
        self.selected_entity_index = None

        self._build_ui()
        self._build_menu()

    # ------------------------------------------------------------------
    # UI
    # ------------------------------------------------------------------
    def _build_ui(self):
        central = QWidget()
        layout = QHBoxLayout(central)

        # --- Árvore de entidades ---
        self.tree = QTreeWidget()
        self.tree.setHeaderLabels(["Entidades"])
        self.tree.itemSelectionChanged.connect(self._on_selection_changed)
        layout.addWidget(self.tree, 1)

        # --- Inspetor ---
        inspector_widget = QWidget()
        inspector_layout = QVBoxLayout(inspector_widget)

        form = QFormLayout()
        self.name_edit = QLineEdit()
        self.x_spin = self._make_spin()
        self.y_spin = self._make_spin()
        self.rotation_spin = self._make_spin()
        self.scale_x_spin = self._make_spin(default=1.0)
        self.scale_y_spin = self._make_spin(default=1.0)
        self.sprite_edit = QLineEdit()
        self.script_edit = QLineEdit()

        form.addRow("Nome:", self.name_edit)
        form.addRow("X:", self.x_spin)
        form.addRow("Y:", self.y_spin)
        form.addRow("Rotação:", self.rotation_spin)
        form.addRow("Escala X:", self.scale_x_spin)
        form.addRow("Escala Y:", self.scale_y_spin)
        form.addRow("Sprite (caminho):", self.sprite_edit)
        form.addRow("Script Lua (caminho):", self.script_edit)

        for widget in (self.name_edit, self.x_spin, self.y_spin, self.rotation_spin,
                       self.scale_x_spin, self.scale_y_spin, self.sprite_edit, self.script_edit):
            if isinstance(widget, QDoubleSpinBox):
                widget.valueChanged.connect(self._on_field_changed)
            else:
                widget.textChanged.connect(self._on_field_changed)

        inspector_layout.addLayout(form)

        add_button = QPushButton("+ Adicionar Entidade")
        add_button.clicked.connect(self._add_entity)
        remove_button = QPushButton("- Remover Entidade")
        remove_button.clicked.connect(self._remove_entity)

        inspector_layout.addWidget(add_button)
        inspector_layout.addWidget(remove_button)
        inspector_layout.addStretch()

        layout.addWidget(inspector_widget, 2)

        self.setCentralWidget(central)
        self.statusBar().showMessage("Pronto.")

    def _make_spin(self, default=0.0):
        spin = QDoubleSpinBox()
        spin.setRange(-100000, 100000)
        spin.setValue(default)
        return spin

    def _build_menu(self):
        menu_bar = self.menuBar()

        file_menu = menu_bar.addMenu("Arquivo")

        new_action = QAction("Nova Cena", self)
        new_action.triggered.connect(self._new_scene)
        file_menu.addAction(new_action)

        open_action = QAction("Abrir Cena...", self)
        open_action.triggered.connect(self._open_scene)
        file_menu.addAction(open_action)

        save_action = QAction("Salvar Cena...", self)
        save_action.triggered.connect(self._save_scene)
        file_menu.addAction(save_action)

        export_menu = menu_bar.addMenu("Exportar")

        export_targets = [
            ("Windows (.exe)", "build_windows.py"),
            ("macOS (.app)", "build_mac.py"),
            ("Linux", "build_linux.py"),
            ("Nintendo 3DS (.elf / .3dsx)", "build_3ds.py"),
            ("Nintendo GameCube (.dol)", "build_gamecube.py"),
        ]
        for label, script in export_targets:
            action = QAction(label, self)
            action.triggered.connect(lambda checked=False, s=script: self._run_exporter(s))
            export_menu.addAction(action)

    # ------------------------------------------------------------------
    # Cena
    # ------------------------------------------------------------------
    def _refresh_tree(self):
        self.tree.clear()
        for entity in self.scene_data["entities"]:
            item = QTreeWidgetItem([entity.get("name", "Entity")])
            self.tree.addTopLevelItem(item)

    def _new_scene(self):
        self.scene_data = {"name": "NovaCena", "entities": []}
        self.current_scene_path = None
        self._refresh_tree()
        self.statusBar().showMessage("Nova cena criada.")

    def _open_scene(self):
        path, _ = QFileDialog.getOpenFileName(self, "Abrir Cena", EXAMPLES_DIR, "Cenas (*.json)")
        if not path:
            return
        with open(path, "r", encoding="utf-8") as f:
            self.scene_data = json.load(f)
        self.current_scene_path = path
        self._refresh_tree()
        self.statusBar().showMessage(f"Cena carregada: {path}")

    def _save_scene(self):
        path, _ = QFileDialog.getSaveFileName(self, "Salvar Cena", EXAMPLES_DIR, "Cenas (*.json)")
        if not path:
            return
        with open(path, "w", encoding="utf-8") as f:
            json.dump(self.scene_data, f, indent=2, ensure_ascii=False)
        self.current_scene_path = path
        self.statusBar().showMessage(f"Cena salva: {path}")

    def _add_entity(self):
        self.scene_data["entities"].append({
            "name": f"Entidade{len(self.scene_data['entities']) + 1}",
            "x": 0, "y": 0, "rotation": 0, "scaleX": 1, "scaleY": 1,
            "sprite": "", "script": ""
        })
        self._refresh_tree()

    def _remove_entity(self):
        if self.selected_entity_index is None:
            return
        del self.scene_data["entities"][self.selected_entity_index]
        self.selected_entity_index = None
        self._refresh_tree()

    # ------------------------------------------------------------------
    # Inspetor
    # ------------------------------------------------------------------
    def _on_selection_changed(self):
        index = self.tree.currentIndex().row()
        if index < 0 or index >= len(self.scene_data["entities"]):
            self.selected_entity_index = None
            return

        self.selected_entity_index = index
        entity = self.scene_data["entities"][index]

        self._loading = True
        self.name_edit.setText(entity.get("name", ""))
        self.x_spin.setValue(entity.get("x", 0))
        self.y_spin.setValue(entity.get("y", 0))
        self.rotation_spin.setValue(entity.get("rotation", 0))
        self.scale_x_spin.setValue(entity.get("scaleX", 1))
        self.scale_y_spin.setValue(entity.get("scaleY", 1))
        self.sprite_edit.setText(entity.get("sprite", ""))
        self.script_edit.setText(entity.get("script", ""))
        self._loading = False

    def _on_field_changed(self, *_args):
        if getattr(self, "_loading", False):
            return
        if self.selected_entity_index is None:
            return

        entity = self.scene_data["entities"][self.selected_entity_index]
        entity["name"] = self.name_edit.text()
        entity["x"] = self.x_spin.value()
        entity["y"] = self.y_spin.value()
        entity["rotation"] = self.rotation_spin.value()
        entity["scaleX"] = self.scale_x_spin.value()
        entity["scaleY"] = self.scale_y_spin.value()
        entity["sprite"] = self.sprite_edit.text()
        entity["script"] = self.script_edit.text()

        self._refresh_tree()

    # ------------------------------------------------------------------
    # Exportação
    # ------------------------------------------------------------------
    def _run_exporter(self, script_name):
        script_path = os.path.join(EXPORTERS_DIR, script_name)
        if not os.path.exists(script_path):
            QMessageBox.warning(self, "Erro", f"Exportador não encontrado: {script_path}")
            return

        self.statusBar().showMessage(f"Executando {script_name}...")
        try:
            result = subprocess.run([sys.executable, script_path], capture_output=True, text=True)
            if result.returncode == 0:
                QMessageBox.information(self, "Exportação concluída",
                                         f"{script_name} executado com sucesso.\n\n{result.stdout}")
            else:
                QMessageBox.warning(self, "Falha na exportação",
                                     f"{script_name} falhou:\n\n{result.stdout}\n{result.stderr}")
        except Exception as exc:
            QMessageBox.critical(self, "Erro", str(exc))
        finally:
            self.statusBar().showMessage("Pronto.")

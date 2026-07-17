#pragma once
#include <vector>
#include <memory>
#include <string>
#include "Entity.hpp"

namespace LupaZupa {

// Uma Scene é uma coleção de Entities. Cenas são serializadas em JSON
// (arquivos .scene) e editadas visualmente pelo Editor (Python/PySide6).
class Scene {
public:
    explicit Scene(std::string name) : m_name(std::move(name)) {}

    Entity& CreateEntity(const std::string& name);
    Entity* FindEntityByName(const std::string& name);

    std::vector<std::unique_ptr<Entity>>& GetEntities() { return m_entities; }
    const std::string& GetName() const { return m_name; }

    // Carrega uma cena a partir de um arquivo JSON (ver Examples/HelloWorld/scene.json).
    static std::unique_ptr<Scene> LoadFromFile(const std::string& path);

    // Salva a cena atual em JSON (usado pelo Editor).
    bool SaveToFile(const std::string& path) const;

private:
    std::string m_name;
    std::vector<std::unique_ptr<Entity>> m_entities;
    int m_nextId = 0;
};

} // namespace LupaZupa

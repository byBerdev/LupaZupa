#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <any>

namespace LupaZupa {

struct Transform {
    float x = 0.0f;
    float y = 0.0f;
    float rotation = 0.0f;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
};

// Entidade simples baseada em componentes (estilo "bag of components").
// Cada entidade tem sempre um Transform, e opcionalmente Sprite/Script,
// que são anexados dinamicamente pelo Editor ou pelo código do jogo.
class Entity {
public:
    explicit Entity(std::string name, int id)
        : m_name(std::move(name)), m_id(id) {}

    int GetId() const { return m_id; }
    const std::string& GetName() const { return m_name; }

    Transform& GetTransform() { return m_transform; }
    const Transform& GetTransform() const { return m_transform; }

    // Componentes genéricos extras (Sprite, Script, Audio, etc.)
    template <typename T>
    void SetComponent(const std::string& key, T value) {
        m_components[key] = std::move(value);
    }

    template <typename T>
    T* GetComponent(const std::string& key) {
        auto it = m_components.find(key);
        if (it == m_components.end()) return nullptr;
        return std::any_cast<T>(&it->second);
    }

    bool HasComponent(const std::string& key) const {
        return m_components.find(key) != m_components.end();
    }

    bool active = true;

private:
    std::string m_name;
    int m_id;
    Transform m_transform;
    std::unordered_map<std::string, std::any> m_components;
};

} // namespace LupaZupa

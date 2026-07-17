#pragma once
#include <string>
#include <sol/sol.hpp>

namespace LupaZupa {

class Entity;
class Scene;

// Encapsula a máquina virtual Lua e expõe a API da engine para os scripts
// dos jogos. Cada Entity com um componente "script" possui um LuaScript
// próprio, carregado a partir de um arquivo .lua do projeto.
class LuaScript {
public:
    LuaScript();

    // Registra as funções/objetos da engine acessíveis a partir do Lua
    // (entity:GetX(), input.IsDown("A"), etc). Chamado uma vez na
    // inicialização da engine.
    void RegisterEngineAPI(Scene& scene);

    bool LoadFile(const std::string& path);

    void CallOnStart(Entity& entity);
    void CallOnUpdate(Entity& entity, float deltaTime);

private:
    sol::state m_lua;
    sol::protected_function m_onStart;
    sol::protected_function m_onUpdate;
};

} // namespace LupaZupa

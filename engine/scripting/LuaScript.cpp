#include "LuaScript.hpp"
#include "../Core/Entity.hpp"
#include "../Core/Scene.hpp"
#include <cstdio>

namespace LupaZupa {

LuaScript::LuaScript() {
    m_lua.open_libraries(
        sol::lib::base,
        sol::lib::math,
        sol::lib::table,
        sol::lib::string
    );
}

void LuaScript::RegisterEngineAPI(Scene& scene) {
    // --- Transform ---
    m_lua.new_usertype<Transform>("Transform",
        "x", &Transform::x,
        "y", &Transform::y,
        "rotation", &Transform::rotation,
        "scaleX", &Transform::scaleX,
        "scaleY", &Transform::scaleY
    );

    // --- Entity ---
    m_lua.new_usertype<Entity>("Entity",
        "GetName", &Entity::GetName,
        "GetTransform", [](Entity& e) -> Transform& { return e.GetTransform(); }
    );

    // --- Input (funções globais simplificadas: input.IsDown("A")) ---
    sol::table inputTable = m_lua.create_table();
    inputTable.set_function("IsDown", [](const std::string& buttonName) -> bool {
        // A ligação real com o IInput ativo é feita pela Application,
        // que injeta uma função nativa aqui em tempo de execução.
        (void)buttonName;
        return false;
    });
    m_lua["input"] = inputTable;

    // --- Scene (busca de outras entidades pelo nome) ---
    sol::table sceneTable = m_lua.create_table();
    sceneTable.set_function("FindEntity", [&scene](const std::string& name) -> Entity* {
        return scene.FindEntityByName(name);
    });
    m_lua["scene"] = sceneTable;

    // --- Log utilitário ---
    m_lua.set_function("print_log", [](const std::string& msg) {
        std::printf("[Lua] %s\n", msg.c_str());
    });
}

bool LuaScript::LoadFile(const std::string& path) {
    auto result = m_lua.safe_script_file(path, sol::script_pass_on_error);
    if (!result.valid()) {
        sol::error err = result;
        std::fprintf(stderr, "[LupaZupaEngine] Erro ao carregar script Lua '%s': %s\n",
                      path.c_str(), err.what());
        return false;
    }

    m_onStart = m_lua["OnStart"];
    m_onUpdate = m_lua["OnUpdate"];
    return true;
}

void LuaScript::CallOnStart(Entity& entity) {
    if (m_onStart.valid()) {
        auto result = m_onStart(&entity);
        if (!result.valid()) {
            sol::error err = result;
            std::fprintf(stderr, "[LupaZupaEngine] Erro em OnStart: %s\n", err.what());
        }
    }
}

void LuaScript::CallOnUpdate(Entity& entity, float deltaTime) {
    if (m_onUpdate.valid()) {
        auto result = m_onUpdate(&entity, deltaTime);
        if (!result.valid()) {
            sol::error err = result;
            std::fprintf(stderr, "[LupaZupaEngine] Erro em OnUpdate: %s\n", err.what());
        }
    }
}

} // namespace LupaZupa

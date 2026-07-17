#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include "Scene.hpp"
#include "Input.hpp"
#include "../Graphics/Renderer.hpp"
#include "../Audio/AudioManager.hpp"
#include "../Scripting/LuaScript.hpp"

namespace LupaZupa {

// Classe principal da engine: junta Renderer, Input, Audio e Scripting
// e roda o loop principal do jogo. Usada tanto pelo executável de PC
// (Platforms/Windows/main.cpp) quanto, conceitualmente, pelas versões
// de 3DS/GameCube (que reimplementam Renderer/Input mas reusam esta classe).
class Application {
public:
    Application(std::unique_ptr<IRenderer> renderer, std::unique_ptr<IInput> input);

    bool Init(const std::string& windowTitle, int width, int height);
    bool LoadScene(const std::string& sceneJsonPath);
    void Run();
    void Shutdown();

private:
    void UpdateFrame(float deltaTime);
    void RenderFrame();

    std::unique_ptr<IRenderer> m_renderer;
    std::unique_ptr<IInput> m_input;
    AudioManager m_audio;

    std::unique_ptr<Scene> m_scene;
    std::unordered_map<int, std::unique_ptr<LuaScript>> m_entityScripts; // por Entity::GetId()

    bool m_running = false;
};

} // namespace LupaZupa

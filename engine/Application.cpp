#include "Application.hpp"
#include "../Graphics/Sprite.hpp"
#include <SDL.h>
#include <cstdio>

namespace LupaZupa {

Application::Application(std::unique_ptr<IRenderer> renderer, std::unique_ptr<IInput> input)
    : m_renderer(std::move(renderer)), m_input(std::move(input)) {}

bool Application::Init(const std::string& windowTitle, int width, int height) {
    if (!m_renderer->Init(width, height, windowTitle)) return false;
    if (!m_audio.Init()) return false;
    return true;
}

bool Application::LoadScene(const std::string& sceneJsonPath) {
    m_scene = Scene::LoadFromFile(sceneJsonPath);
    if (!m_scene) return false;

    for (auto& entity : m_scene->GetEntities()) {
        // Carrega textura do sprite, se houver.
        if (auto* sprite = entity->GetComponent<Sprite>("sprite")) {
            sprite->nativeTextureHandle = m_renderer->LoadTexture(sprite->texturePath);
        }

        // Carrega e inicializa o script Lua, se houver.
        if (auto* scriptPath = entity->GetComponent<std::string>("script")) {
            auto script = std::make_unique<LuaScript>();
            script->RegisterEngineAPI(*m_scene);
            if (script->LoadFile(*scriptPath)) {
                script->CallOnStart(*entity);
                m_entityScripts[entity->GetId()] = std::move(script);
            }
        }
    }

    return true;
}

void Application::UpdateFrame(float deltaTime) {
    m_input->Update();
    if (m_input->ShouldQuit()) {
        m_running = false;
        return;
    }

    for (auto& entity : m_scene->GetEntities()) {
        auto it = m_entityScripts.find(entity->GetId());
        if (it != m_entityScripts.end()) {
            it->second->CallOnUpdate(*entity, deltaTime);
        }
    }
}

void Application::RenderFrame() {
    m_renderer->BeginFrame();

    for (auto& entity : m_scene->GetEntities()) {
        if (auto* sprite = entity->GetComponent<Sprite>("sprite")) {
            const auto& t = entity->GetTransform();
            m_renderer->DrawSprite(*sprite, t.x, t.y, t.rotation, t.scaleX, t.scaleY);
        }
    }

    m_renderer->EndFrame();
}

void Application::Run() {
    m_running = true;
    Uint32 lastTicks = SDL_GetTicks();

    while (m_running) {
        Uint32 currentTicks = SDL_GetTicks();
        float deltaTime = (currentTicks - lastTicks) / 1000.0f;
        lastTicks = currentTicks;

        UpdateFrame(deltaTime);
        if (!m_running) break;
        RenderFrame();
    }
}

void Application::Shutdown() {
    m_audio.Shutdown();
    m_renderer->Shutdown();
}

} // namespace LupaZupa

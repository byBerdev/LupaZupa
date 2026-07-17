#pragma once
#include "Renderer.hpp"
#include <SDL.h>
#include <unordered_map>

namespace LupaZupa {

// Implementação do IRenderer usando SDL2 + SDL2_image, usada em
// Windows, Linux e macOS. Usa o renderer 2D acelerado do SDL2 (que por
// baixo dos panos usa OpenGL/Direct3D/Metal conforme a plataforma).
class SDLRenderer : public IRenderer {
public:
    bool Init(int screenWidth, int screenHeight, const std::string& windowTitle) override;
    void Shutdown() override;

    void BeginFrame() override;
    void EndFrame() override;

    void* LoadTexture(const std::string& path) override;
    void DestroyTexture(void* handle) override;

    void DrawSprite(const Sprite& sprite, float x, float y,
                     float rotation, float scaleX, float scaleY) override;

private:
    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;
};

} // namespace LupaZupa

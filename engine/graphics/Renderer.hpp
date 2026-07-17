#pragma once
#include <string>
#include "Sprite.hpp"

namespace LupaZupa {

// Interface abstrata de renderização. Cada plataforma implementa a sua
// própria versão (SDL2/OpenGL no PC, Citro2D no 3DS, GX no GameCube).
class IRenderer {
public:
    virtual ~IRenderer() = default;

    virtual bool Init(int screenWidth, int screenHeight, const std::string& windowTitle) = 0;
    virtual void Shutdown() = 0;

    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;

    // Carrega uma textura a partir de um caminho lógico (relativo aos assets
    // do projeto) e devolve um handle nativo opaco.
    virtual void* LoadTexture(const std::string& path) = 0;
    virtual void DestroyTexture(void* handle) = 0;

    virtual void DrawSprite(const Sprite& sprite, float x, float y,
                             float rotation, float scaleX, float scaleY) = 0;
};

} // namespace LupaZupa

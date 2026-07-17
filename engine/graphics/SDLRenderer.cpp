#include "SDLRenderer.hpp"
#include <SDL_image.h>
#include <cstdio>

namespace LupaZupa {

bool SDLRenderer::Init(int screenWidth, int screenHeight, const std::string& windowTitle) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::fprintf(stderr, "[LupaZupaEngine] Falha ao iniciar SDL2: %s\n", SDL_GetError());
        return false;
    }

    m_window = SDL_CreateWindow(
        windowTitle.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        screenWidth, screenHeight,
        SDL_WINDOW_SHOWN
    );
    if (!m_window) {
        std::fprintf(stderr, "[LupaZupaEngine] Falha ao criar janela: %s\n", SDL_GetError());
        return false;
    }

    m_renderer = SDL_CreateRenderer(m_window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!m_renderer) {
        std::fprintf(stderr, "[LupaZupaEngine] Falha ao criar renderer: %s\n", SDL_GetError());
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::fprintf(stderr, "[LupaZupaEngine] Falha ao iniciar SDL2_image: %s\n", IMG_GetError());
        return false;
    }

    return true;
}

void SDLRenderer::Shutdown() {
    if (m_renderer) SDL_DestroyRenderer(m_renderer);
    if (m_window) SDL_DestroyWindow(m_window);
    IMG_Quit();
    SDL_Quit();
}

void SDLRenderer::BeginFrame() {
    SDL_SetRenderDrawColor(m_renderer, 25, 25, 35, 255);
    SDL_RenderClear(m_renderer);
}

void SDLRenderer::EndFrame() {
    SDL_RenderPresent(m_renderer);
}

void* SDLRenderer::LoadTexture(const std::string& path) {
    SDL_Texture* tex = IMG_LoadTexture(m_renderer, path.c_str());
    if (!tex) {
        std::fprintf(stderr, "[LupaZupaEngine] Falha ao carregar sprite '%s': %s\n",
                      path.c_str(), IMG_GetError());
    }
    return tex;
}

void SDLRenderer::DestroyTexture(void* handle) {
    if (handle) SDL_DestroyTexture(static_cast<SDL_Texture*>(handle));
}

void SDLRenderer::DrawSprite(const Sprite& sprite, float x, float y,
                              float rotation, float scaleX, float scaleY) {
    if (!sprite.visible || !sprite.nativeTextureHandle) return;

    auto* tex = static_cast<SDL_Texture*>(sprite.nativeTextureHandle);
    int texW = sprite.srcW, texH = sprite.srcH;
    if (texW == 0 || texH == 0) {
        SDL_QueryTexture(tex, nullptr, nullptr, &texW, &texH);
    }

    SDL_Rect srcRect{ sprite.srcX, sprite.srcY, texW, texH };
    SDL_Rect dstRect{
        static_cast<int>(x),
        static_cast<int>(y),
        static_cast<int>(texW * scaleX),
        static_cast<int>(texH * scaleY)
    };

    SDL_RenderCopyEx(m_renderer, tex, &srcRect, &dstRect, rotation, nullptr, SDL_FLIP_NONE);
}

} // namespace LupaZupa

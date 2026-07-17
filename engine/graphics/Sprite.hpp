#pragma once
#include <string>

namespace LupaZupa {

// Componente de sprite: guarda o caminho da textura e o retângulo de recorte.
// A textura em si é carregada/gerenciada pelo Renderer (SDL2_image no PC,
// Citro2D no 3DS, GX no GameCube) e referenciada aqui pelo caminho lógico,
// para manter o componente independente de plataforma.
struct Sprite {
    std::string texturePath;
    int srcX = 0;
    int srcY = 0;
    int srcW = 0; // 0 = usar largura total da textura
    int srcH = 0; // 0 = usar altura total da textura
    bool visible = true;

    // Handle interno preenchido pelo Renderer (não serializado).
    void* nativeTextureHandle = nullptr;
};

} // namespace LupaZupa

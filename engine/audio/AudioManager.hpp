#pragma once
#include <string>
#include <unordered_map>

namespace LupaZupa {

// Gerenciador de áudio simples. No PC usa SDL2_mixer; no 3DS pode usar a
// API de áudio do libctru (ndsp); no GameCube, o AXFX/libogc audio.
// A interface pública é a mesma em todas as plataformas para manter o
// código de jogo/scripts Lua portável.
class AudioManager {
public:
    bool Init();
    void Shutdown();

    // Carrega um efeito sonoro/música a partir de um caminho lógico de asset.
    bool LoadSound(const std::string& name, const std::string& path);
    void PlaySound(const std::string& name, bool loop = false);
    void StopSound(const std::string& name);

    void SetMasterVolume(float volume01);

private:
    std::unordered_map<std::string, void*> m_sounds;
    float m_masterVolume = 1.0f;
};

} // namespace LupaZupa

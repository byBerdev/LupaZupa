#include "AudioManager.hpp"

#if defined(LUPAZUPA_PLATFORM_PC)
#include <SDL_mixer.h>
#endif

#include <cstdio>

namespace LupaZupa {

bool AudioManager::Init() {
#if defined(LUPAZUPA_PLATFORM_PC)
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::fprintf(stderr, "[LupaZupaEngine] Falha ao iniciar audio: %s\n", Mix_GetError());
        return false;
    }
#endif
    return true;
}

void AudioManager::Shutdown() {
    for (auto& [name, handle] : m_sounds) {
#if defined(LUPAZUPA_PLATFORM_PC)
        Mix_FreeChunk(static_cast<Mix_Chunk*>(handle));
#else
        (void)handle;
#endif
    }
    m_sounds.clear();
#if defined(LUPAZUPA_PLATFORM_PC)
    Mix_CloseAudio();
#endif
}

bool AudioManager::LoadSound(const std::string& name, const std::string& path) {
#if defined(LUPAZUPA_PLATFORM_PC)
    Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
    if (!chunk) {
        std::fprintf(stderr, "[LupaZupaEngine] Falha ao carregar som '%s': %s\n",
                      path.c_str(), Mix_GetError());
        return false;
    }
    m_sounds[name] = chunk;
    return true;
#else
    // Nas builds de 3DS/GameCube, este método é substituído pela
    // implementação específica de ndsp/AXFX (ver Platforms/<plataforma>).
    m_sounds[name] = nullptr;
    return true;
#endif
}

void AudioManager::PlaySound(const std::string& name, bool loop) {
    auto it = m_sounds.find(name);
    if (it == m_sounds.end()) return;

#if defined(LUPAZUPA_PLATFORM_PC)
    Mix_PlayChannel(-1, static_cast<Mix_Chunk*>(it->second), loop ? -1 : 0);
#endif
}

void AudioManager::StopSound(const std::string& name) {
    (void)name;
#if defined(LUPAZUPA_PLATFORM_PC)
    Mix_HaltChannel(-1);
#endif
}

void AudioManager::SetMasterVolume(float volume01) {
    m_masterVolume = volume01;
#if defined(LUPAZUPA_PLATFORM_PC)
    Mix_Volume(-1, static_cast<int>(volume01 * MIX_MAX_VOLUME));
#endif
}

} // namespace LupaZupa

#pragma once
#include "Input.hpp"
#include <SDL.h>
#include <array>

namespace LupaZupa {

// Implementação de IInput usando teclado SDL2 (usada em Windows/Linux/macOS).
// Mapeamento padrão: setas = direção, Z = A, X = B, Enter = Start, Backspace = Select.
class InputSDL : public IInput {
public:
    void Update() override;

    bool IsDown(Button button) const override;
    bool WasPressed(Button button) const override;
    bool WasReleased(Button button) const override;

    float GetAxisX() const override;
    float GetAxisY() const override;

    bool ShouldQuit() const override { return m_shouldQuit; }

private:
    static SDL_Scancode MapButton(Button button);

    std::array<bool, static_cast<size_t>(Button::Count)> m_current{};
    std::array<bool, static_cast<size_t>(Button::Count)> m_previous{};
    bool m_shouldQuit = false;
};

} // namespace LupaZupa

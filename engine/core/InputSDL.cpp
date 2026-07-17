#include "InputSDL.hpp"

namespace LupaZupa {

SDL_Scancode InputSDL::MapButton(Button button) {
    switch (button) {
        case Button::Up:     return SDL_SCANCODE_UP;
        case Button::Down:   return SDL_SCANCODE_DOWN;
        case Button::Left:   return SDL_SCANCODE_LEFT;
        case Button::Right:  return SDL_SCANCODE_RIGHT;
        case Button::A:      return SDL_SCANCODE_Z;
        case Button::B:      return SDL_SCANCODE_X;
        case Button::X:      return SDL_SCANCODE_A;
        case Button::Y:      return SDL_SCANCODE_S;
        case Button::Start:  return SDL_SCANCODE_RETURN;
        case Button::Select: return SDL_SCANCODE_BACKSPACE;
        default:             return SDL_SCANCODE_UNKNOWN;
    }
}

void InputSDL::Update() {
    m_previous = m_current;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            m_shouldQuit = true;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);
    for (size_t i = 0; i < static_cast<size_t>(Button::Count); ++i) {
        SDL_Scancode code = MapButton(static_cast<Button>(i));
        m_current[i] = (code != SDL_SCANCODE_UNKNOWN) && state[code];
    }
}

bool InputSDL::IsDown(Button button) const {
    return m_current[static_cast<size_t>(button)];
}

bool InputSDL::WasPressed(Button button) const {
    size_t i = static_cast<size_t>(button);
    return m_current[i] && !m_previous[i];
}

bool InputSDL::WasReleased(Button button) const {
    size_t i = static_cast<size_t>(button);
    return !m_current[i] && m_previous[i];
}

float InputSDL::GetAxisX() const {
    if (IsDown(Button::Left)) return -1.0f;
    if (IsDown(Button::Right)) return 1.0f;
    return 0.0f;
}

float InputSDL::GetAxisY() const {
    if (IsDown(Button::Up)) return -1.0f;
    if (IsDown(Button::Down)) return 1.0f;
    return 0.0f;
}

} // namespace LupaZupa

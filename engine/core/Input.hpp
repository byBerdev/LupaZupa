#pragma once

namespace LupaZupa {

// Botões lógicos unificados. Cada plataforma mapeia seu hardware de input
// (teclado/gamepad no PC, botões físicos no 3DS/GameCube) para este enum,
// para que o código do jogo (C++ ou Lua) seja portável.
enum class Button {
    Up,
    Down,
    Left,
    Right,
    A,
    B,
    X,
    Y,
    Start,
    Select,
    Count
};

// Interface abstrata de input. Implementações concretas ficam em cada
// pasta de Platforms/ (ex: InputSDL para PC, InputCTR para 3DS, InputGX
// para GameCube).
class IInput {
public:
    virtual ~IInput() = default;

    virtual void Update() = 0;

    virtual bool IsDown(Button button) const = 0;
    virtual bool WasPressed(Button button) const = 0;
    virtual bool WasReleased(Button button) const = 0;

    // Estado analógico (0.0 a 1.0 / -1.0 a 1.0), usado quando disponível
    // (analógico do 3DS, GameCube Controller). Plataformas sem analógico
    // (teclado puro) podem simplesmente retornar 0.
    virtual float GetAxisX() const = 0;
    virtual float GetAxisY() const = 0;

    virtual bool ShouldQuit() const = 0;
};

} // namespace LupaZupa

//
// Created by Leo Gogichaishvili on 29.01.22.
//

#ifndef GOSCHESS_GAMEPLAYFUNCTIONAL_H
#define GOSCHESS_GAMEPLAYFUNCTIONAL_H


#include "../../chess/global/MainObjects/GameObjects.h"
#include "../../chess/global/GameGlobals.h"
#include "../../render/GamePlayRender.h"
#include "../../chess/computation/InGameCalculations.h"
#include "../../util/GameInput.h"
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>

namespace GosChess {

    class GameModeListener {
    protected:
        sf::RenderWindow &game_window;
    public:
        explicit GameModeListener(sf::RenderWindow &game_window) : game_window(game_window) {}

        virtual void Action(GosChess::Board &board) = 0;

    };

    class MultiPlayerListener : public GameModeListener {
    public:
        explicit MultiPlayerListener(sf::RenderWindow &game_window) : GameModeListener(game_window) {}

        void Action(GosChess::Board &board) override;
    };

    class GamePlayAIListener : public GameModeListener {
    public:
        explicit GamePlayAIListener(sf::RenderWindow &game_window) : GameModeListener(game_window) {}

        void Action(GosChess::Board &board) override;
    };

    class MainMenuListener : public GameModeListener {
    public:
        explicit MainMenuListener(sf::RenderWindow &game_window) : GameModeListener(game_window) {}

        void Action(GosChess::Board &board) override;
    };


    namespace Time {

        enum Time {
            ZERO
        };

        class Timer {
        private :
            void Convert(const float &);

            short minutes;

            float seconds;
        public:
            explicit Timer(const float &);

            Timer() = default;

            bool Subtract(const float &);

            void Set(const float &);

            std::string ToString() const;

            float GetAmount() const;

        };

        struct TimerTransferObject {
            TimerTransferObject(float player, float enemy) : player_timer_amount(player), enemy_timer_amount(enemy) {}

            TimerTransferObject() = default;

            float player_timer_amount;
            float enemy_timer_amount;
        };

    }


    std::string GetInitialFenBoard();

    bool Play(GosChess::Board &, const GosChess::Move &);

    std::optional<GosChess::Square> ChooseTrgFigure(GosChess::Board &, sf::Window &);

    std::optional<GosChess::Square> ChooseSrcFigure(GosChess::Board &, sf::Window &);

    void CheckReceivedMove(std::optional<GosChess::Move>, GosChess::Board &);

    void SetGameFlagFinished();

    bool CheckGameModeFinished();

    bool CheckMenuModeFinished();

    extern bool highlited;

    extern char opponent_ip[];

}


#endif //PROJECT_NAME_GAMEPLAYFUNCTIONAL_H

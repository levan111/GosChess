
#ifndef GOSCHESS_GAMEPLAYRENDER_H
#define GOSCHESS_GAMEPLAYRENDER_H

#include "../chess/global/main_objects/GameObjects.h"
#include "SFML/Graphics.hpp"
#include "../chess/global/GameGlobals.h"
#include "../chess/computation/InGameCalculations.h"


namespace GosChess {

    void ChessDrawingConfig();

    void BoardRenderConfig();

    void ColorizeAvailableMoves(const int &, Board &);

    void DrawBoard(sf::RenderWindow &);

    void DrawTimer(sf::RenderWindow &, const std::string &, const std::string &);

    void LoadChessFigureSprites();

    void DrawFigure(unsigned char, sf::Vector2f, sf::RenderWindow &);

    void DrawCurrentBoardState(const unsigned char *, sf::RenderWindow &, const std::string &, const std::string &);

    void ResetBoardColours();

}

#endif

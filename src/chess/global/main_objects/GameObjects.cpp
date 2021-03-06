//
// Created by Leo Gogichaishvili on 23.11.21.
//

#include "GameObjects.h"


static bool IsCharacter(const char &character_figure) {
    return (character_figure >= 'a' && character_figure <= 'z') ||
           (character_figure >= 'A' && character_figure <= 'Z');
}


static GosChess::Figure FigureFromChar(const char &figure_rep) {
    if (IsCharacter(figure_rep)) {
        GosChess::Figure figure;
        if (std::isupper(figure_rep))figure.color = GosChess::Color::WHITE;
        else figure.color = GosChess::Color::BLACK;
        figure.type = GosChess::Board::FEN_TO_FIG.at(std::tolower(figure_rep));
        return figure;
    }
    return GosChess::Figure(0);
}

typedef enum GosChess::FigureTypes FIGTYPE;

const std::map<char, unsigned char> GosChess::Board::FEN_TO_FIG = {{'p', FIGTYPE::PAWN},
                                                                   {'q', FIGTYPE::QUEEN},
                                                                   {'k', FIGTYPE::KING},
                                                                   {'b', FIGTYPE::BISHOP},
                                                                   {'r', FIGTYPE::ROOK},
                                                                   {'n', FIGTYPE::KNIGHT}};


const std::map<unsigned char, char> GosChess::Board::FIG_TO_FEN = {{Figure(Color::WHITE, FIGTYPE::ROOK).full_type, 'R'},
                                                                   {Figure(Color::WHITE,
                                                                           FIGTYPE::BISHOP).full_type,             'B'},
                                                                   {Figure(Color::WHITE,
                                                                           FIGTYPE::PAWN).full_type,               'P'},
                                                                   {Figure(Color::WHITE,
                                                                           FIGTYPE::KNIGHT).full_type,             'N'},
                                                                   {Figure(Color::WHITE,
                                                                           FIGTYPE::KING).full_type,               'K'},
                                                                   {Figure(Color::WHITE,
                                                                           FIGTYPE::QUEEN).full_type,              'Q'},
                                                                   {Figure(Color::BLACK,
                                                                           FIGTYPE::ROOK).full_type,               'r'},
                                                                   {Figure(Color::BLACK,
                                                                           FIGTYPE::BISHOP).full_type,             'b'},
                                                                   {Figure(Color::BLACK,
                                                                           FIGTYPE::PAWN).full_type,               'p'},
                                                                   {Figure(Color::BLACK,
                                                                           FIGTYPE::KNIGHT).full_type,             'n'},
                                                                   {Figure(Color::BLACK,
                                                                           FIGTYPE::KING).full_type,               'k'},
                                                                   {Figure(Color::BLACK,
                                                                           FIGTYPE::QUEEN).full_type,              'q'}};

unsigned char *GosChess::Board::decodeFen(std::string fen_str) {
    unsigned char *board_demo = new unsigned char[Board::BOARD_SIZE]{0};
    short curr_pos = 0;
    for (int i = 0; i < fen_str.length(); i++) {
        if (fen_str[i] == '/')continue;
        if (IsCharacter(fen_str[i])) {
            Figure fig = FigureFromChar(fen_str[i]);
            board_demo[curr_pos] = fig.full_type;
            curr_pos++;
        } else {
            int num_jump = fen_str[i] - '0';
            curr_pos += num_jump;
        }
    }
    return board_demo;
}

GosChess::Board::Board(const std::string &initial_state) {
    this->board = Board::decodeFen(initial_state);
}

void GosChess::Board::setPosition(const int &grid_num, unsigned char figure) {
    this->board[grid_num] = figure;
}

// ignore this block of code, it was little challenge for my self :))) i know it looks hoooriibbleee :))
std::string GosChess::Board::boardStateToFen() const {
    unsigned char *target_board = this->board;
    std::string fen_str = "";
    int empty = 0;
    for (int i = 0; i < Board::BOARD_SIZE; i++) {
        if (i % 8 == 0) {
            fen_str += ((empty != 0) ? std::string() + (char) ('0' + empty) : "") + "/";
            empty = 0;
        }
        std::string to_add = (target_board[i] == 0) ? "" : Board::FIG_TO_FEN.at(target_board[i]) + std::string();
        (to_add == "") ? (std::string() + (char) empty++) : (empty == 0 ? fen_str += to_add : fen_str += (char) ('0' +
                                                                                                                 empty) +
                                                                                                         std::string() +
                                                                                                         to_add);
        if (to_add != "")empty = 0;
    }
    return fen_str;
}

const unsigned char *GosChess::Board::getRawBoard() const {
    return this->board;
}

GosChess::Figure GosChess::Board::at(const int &index) const {
    unsigned char fig_char = this->board[index];
    return GosChess::Figure(fig_char);
}

GosChess::Figure::Figure(const unsigned char &color, const unsigned char &type) {
    this->full_type = 0;
    this->color = color;
    this->type = type;
}

void GosChess::Board::saveState() {
    std::array<unsigned char, Board::BOARD_SIZE> arr{};
    std::copy(Board::board, Board::board + BOARD_SIZE, arr.begin());
    this->game_rev.push(arr);
}

GosChess::Board::~Board() {
    delete[] this->board;
}


GosChess::Board::Board(const GosChess::Board &board) {
    this->board = new unsigned char[BOARD_SIZE];
    memcpy(this->board, board.board, BOARD_SIZE * sizeof(unsigned char));
    this->game_rev = board.game_rev;
}

void GosChess::Board::setState(const std::string &state) {
    this->board = Board::decodeFen(state);
}


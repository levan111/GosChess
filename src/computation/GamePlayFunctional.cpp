

#include "GamePlayFunctional.h"

GosChess::Offset *GosChess::precalculated_offsets = new GosChess::Offset[GosChess::Board::BOARD_SIZE];

const std::int8_t *const GosChess::direction_offsets = new std::int8_t[]{8, -8, -1, 1, 9, -7, 7, -9};


std::unordered_map<std::int8_t, std::unordered_set<GosChess::Move, GosChess::MoveHash>> GosChess::available_moves;


const unsigned char *GosChess::sliding_pieces = new unsigned char[]{GosChess::FigureTypes::BISHOP,
                                                                    GosChess::FigureTypes::ROOK,
                                                                    GosChess::FigureTypes::QUEEN};


static const std::int8_t *const knight_moves_y = new std::int8_t[]{2, 2, 1, -1, 1, -1, -2, -2};

static const std::int8_t *const knight_moves_x = new std::int8_t[]{1, -1, 2, 2, -2, -2, 1, -1};


static bool IsPawn(const GosChess::Figure &_fig) {
    return _fig.type == GosChess::FigureTypes::PAWN;
}


static bool IsKing(const GosChess::Figure &_fig) {
    return _fig.type == GosChess::FigureTypes::KING;
}

static bool IsKnight(const GosChess::Figure &_fig) {
    return _fig.type == GosChess::FigureTypes::KNIGHT;
}


static bool IsSlidingPiece(const GosChess::Figure &_fig) {
    for (int i = 0; i < 3; i++) {
        if (_fig.type == GosChess::sliding_pieces[i])return true;
    }
    return false;
}


static int GetKingPosition(const unsigned char *board, GosChess::Color clr) {
    GosChess::Figure fig_check;
    for (int king_ind = 0; king_ind < GosChess::Board::BOARD_SIZE; king_ind++) {
        fig_check.full_type = board[king_ind];
        if (fig_check.type == GosChess::FigureTypes::KING && fig_check.color == clr)return king_ind;
    }
    return -1;
}


static bool CheckForKingChecksSlidingPieces(const unsigned char *brd, GosChess::Figure kng, int king_index) {
    GosChess::Figure test_fig;
    for (int direction_index = 0; direction_index < 8; direction_index++) {
        for (int offset = 0; offset < GosChess::precalculated_offsets[king_index][direction_index]; offset++) {
            int target_index = king_index + (offset + 1) * GosChess::direction_offsets[direction_index];
            test_fig.full_type = brd[target_index];;
            if (test_fig.full_type != 0) {
                if (test_fig.color != kng.color) {
                    if (direction_index < 4) {
                        if (test_fig.type == GosChess::FigureTypes::QUEEN ||
                            test_fig.type == GosChess::FigureTypes::ROOK)
                            return true;
                    } else {
                        if (test_fig.type == GosChess::FigureTypes::QUEEN ||
                            test_fig.type == GosChess::FigureTypes::BISHOP)
                            return true;
                    }
                }
                break;
            }
        }
    }
    return false;
}

static bool CheckForKingChecksPawns(const unsigned char *brd, GosChess::Figure kng, int king_index) {
    GosChess::Figure test_fig;
    int direction_start_index = kng.color == GosChess::Color::WHITE ? 4 : 5;
    for (int direction_index = direction_start_index; direction_index < 8; direction_index += 2) {
        if (GosChess::precalculated_offsets[king_index][direction_index] >= 1) {
            unsigned char target_fig = brd[king_index + GosChess::direction_offsets[direction_index]];
            if (target_fig != 0) {
                test_fig.full_type = target_fig;
                if (test_fig.color != kng.color && test_fig.type == GosChess::FigureTypes::PAWN)
                    return true;
            }

        }
    }
    return false;
}

static bool CheckForKingChecksKing(const unsigned char *brd, GosChess::Figure kng, int king_index) {
    GosChess::Figure test_fig;
    for (int direction_index = 0; direction_index < 8; direction_index++) {
        if (GosChess::precalculated_offsets[king_index][direction_index] >= 1) {
            unsigned char targ = brd[king_index + GosChess::direction_offsets[direction_index]];
            if (targ != 0) {
                test_fig.full_type = targ;
                if (test_fig.color != kng.color && test_fig.type == GosChess::FigureTypes::KING)return true;
            }
        }
    }
    return false;
}


static bool CheckForKingChecksKnight(const unsigned char *brd, GosChess::Figure kng, int king_index) {
    int additional_moves[] = {3, 3, 2, 2};
    GosChess::Figure test_fig;
    for (int direction_index = 4; direction_index < 8; direction_index++) {
        if (GosChess::precalculated_offsets[king_index][direction_index] >= 1) {
            int targ_index = king_index + GosChess::direction_offsets[direction_index];
            if (direction_index % 2 == 0) {
                if (GosChess::precalculated_offsets[targ_index][0] >= 1) {
                    int curr_index = targ_index + GosChess::direction_offsets[0];
                    if (brd[curr_index] != 0) {
                        test_fig.full_type = brd[curr_index];
                        if (test_fig.color != kng.color && test_fig.type == GosChess::FigureTypes::KNIGHT)return true;
                    }
                }
                if (GosChess::precalculated_offsets[targ_index][additional_moves[direction_index - 4]] >= 1) {
                    int curr_index = targ_index + GosChess::direction_offsets[additional_moves[direction_index - 4]];
                    if (brd[curr_index] != 0) {
                        test_fig.full_type = brd[curr_index];
                        if (test_fig.color != kng.color && test_fig.type == GosChess::FigureTypes::KNIGHT)return true;
                    }
                }
            } else {
                if (GosChess::precalculated_offsets[targ_index][1] >= 1) {
                    int curr_index = targ_index + GosChess::direction_offsets[1];
                    if (brd[curr_index] != 0) {
                        test_fig.full_type = brd[curr_index];
                        if (test_fig.color != kng.color && test_fig.type == GosChess::FigureTypes::KNIGHT)return true;
                    }
                }
                if (GosChess::precalculated_offsets[targ_index][additional_moves[direction_index - 4]] >= 1) {
                    int curr_index = targ_index + GosChess::direction_offsets[additional_moves[direction_index - 4]];
                    if (brd[curr_index] != 0) {
                        test_fig.full_type = brd[curr_index];
                        if (test_fig.color != kng.color && test_fig.type == GosChess::FigureTypes::KNIGHT)return true;
                    }
                }
            }
        }
    }
    return false;
}


static bool CheckForKingCheck(const unsigned char *brd, GosChess::Color clr) {
    int king_index = GetKingPosition(brd, clr);
    GosChess::Figure arg_kng(brd[king_index]);
    return CheckForKingChecksPawns(brd, arg_kng, king_index) ||
           CheckForKingChecksSlidingPieces(brd, arg_kng, king_index) ||
           CheckForKingChecksKnight(brd, arg_kng, king_index) || CheckForKingChecksKing(brd, arg_kng, king_index);
}


static void GenerateSlidingMoves(const unsigned char *board, GosChess::Figure piece, int index) {
    int direction_start_index = (piece.type == GosChess::FigureTypes::BISHOP) ? 4 : 0;
    int direction_end_index = (piece.type == GosChess::FigureTypes::ROOK) ? 4 : 8;

    for (int i = direction_start_index; i < direction_end_index; i++) {
        for (int offset = 0; offset < GosChess::precalculated_offsets[index][i]; offset++) {
            int target_index = index + (offset + 1) * GosChess::direction_offsets[i];
            if (board[target_index] != 0 && GosChess::Figure(board[target_index]).color == GosChess::color_to_play)
                break;
            else if (board[target_index] != 0) {
                GosChess::available_moves[index].insert(GosChess::Move(index, target_index));
                break;
            }
            GosChess::available_moves[index].insert(GosChess::Move(index, target_index));
        }
    }
}


static void GeneratePawnKillMoves(const unsigned char *board, GosChess::Figure piece, int index) {
    int start_index = piece.color == GosChess::Color::WHITE ? 4 : 5;
    if (GosChess::precalculated_offsets[index][start_index] >= 1) {
        unsigned char fig_rep = board[index + GosChess::direction_offsets[start_index]];
        if (fig_rep != 0) {
            GosChess::Figure targ_fig(board[index + GosChess::direction_offsets[start_index]]);
            if (targ_fig.color != GosChess::color_to_play) {
                GosChess::available_moves[index].insert(
                        GosChess::Move(index, index + GosChess::direction_offsets[start_index]));
            }
        }
    }
    if (GosChess::precalculated_offsets[index][start_index + 2] >= 1) {
        unsigned char fig_rep = board[index + GosChess::direction_offsets[start_index + 2]];
        if (fig_rep != 0) {
            GosChess::Figure targ_fig(board[index + GosChess::direction_offsets[start_index + 2]]);
            if (targ_fig.color != GosChess::color_to_play) {
                GosChess::available_moves[index].insert(
                        GosChess::Move(index, index + GosChess::direction_offsets[start_index + 2]));
            }
        }
    }
}


static void GeneratePawnMoves(const unsigned char *board, GosChess::Figure piece, int index) {
    if (piece.type != GosChess::FigureTypes::PAWN)return;
    int multiplier = piece.color == GosChess::Color::BLACK ? -1 : 1;
    if (index + 8 * multiplier < GosChess::Board::BOARD_SIZE && index + 8 * multiplier >= 0) {
        if (board[index + 8 * multiplier] == 0)
            GosChess::available_moves[index].insert(GosChess::Move(index, index + 8 * multiplier));
        if ((piece.color == GosChess::Color::WHITE && index / GosChess::Board::ROW_NUM == 1) ||
            (piece.color == GosChess::Color::BLACK && index / GosChess::Board::ROW_NUM == 6)) {
            if (board[index + 8 * 2 * multiplier] == 0)
                GosChess::available_moves[index].insert(GosChess::Move(index, index + 8 * 2 * multiplier));
        }
    }
    GeneratePawnKillMoves(board, piece, index);
}


static void GenerateKingMoves(const unsigned char *board, GosChess::Figure piece, int index) {
    if (piece.type != GosChess::FigureTypes::KING)return;
    for (int i = 0; i < 8; i++) {
        if (GosChess::precalculated_offsets[index][i] >= 1) {
            if (board[index + GosChess::direction_offsets[i]] == 0 ||
                GosChess::Figure(board[index + GosChess::direction_offsets[i]]).color != piece.color) {
                GosChess::available_moves[index].insert(
                        GosChess::Move(index, index + GosChess::direction_offsets[i]));
            }
        }
    }
}

static void GenerateKnightMoves(const unsigned char *board, GosChess::Figure piece, int index) {
    if (piece.type != GosChess::FigureTypes::KNIGHT)return;
    for (int i = 0; i < 8; i++) {
        GosChess::Cell curr = GosChess::GetNode(index);
        GosChess::Cell res = GosChess::Cell(knight_moves_y[i], knight_moves_x[i]) + curr;
        if ((res.x >= 0 && res.x < GosChess::Board::ROW_LENGTH && res.y >= 0 && res.y < GosChess::Board::ROW_NUM) &&
            (board[GosChess::GetNumFromNode(res)] == 0 ||
             GosChess::Figure(board[GosChess::GetNumFromNode(res)]).color != GosChess::color_to_play)) {
            GosChess::available_moves[index].insert(GosChess::Move(index, GosChess::GetNumFromNode(res)));
        }
    }
}


GosChess::Cell GosChess::GetNode(const int &num) {
    int x = num % GosChess::Board::ROW_LENGTH;
    int y = num / GosChess::Board::ROW_NUM;
    return {y, x};
}


void GosChess::GenerateOffsets() {
    int curr_y, curr_x;
    for (int i = 0; i < GosChess::Board::BOARD_SIZE; i++) {
        curr_y = i / GosChess::Board::ROW_NUM, curr_x = i % GosChess::Board::ROW_LENGTH;
        std::int8_t north = GosChess::Board::ROW_NUM - curr_y - 1, south = curr_y, west = curr_x,
                east = GosChess::Board::ROW_LENGTH - curr_x - 1;
        GosChess::precalculated_offsets[i] = GosChess::Offset(north, south, west, east);
    }
}

void GosChess::CalculateAvailableMoves(const unsigned char *game_board) {
    GosChess::available_moves.clear();
    for (int i = 0; i < Board::BOARD_SIZE; i++) {
        if (game_board[i] == 0)continue;
        Figure curr_fig(game_board[i]);
        if (curr_fig.color == GosChess::color_to_play) {
            if (IsSlidingPiece(curr_fig)) {
                GenerateSlidingMoves(game_board, curr_fig, i);
            } else if (IsPawn(curr_fig)) {
                GeneratePawnMoves(game_board, curr_fig, i);
            } else if (IsKing(curr_fig)) {
                GenerateKingMoves(game_board, curr_fig, i);
            } else if (IsKnight(curr_fig)) {
                GenerateKnightMoves(game_board, curr_fig, i);
            }
        }
    }
}


GosChess::Cell GosChess::GetNodeFromScreen(const float &_y, const float &_x) {
    int real_y = (int) (GosChess::window_height - _y) / (int) GosChess::square_size;
    int real_x = (int) _x / (int) GosChess::square_size;
    return GosChess::Cell(real_y, real_x);
}


int GosChess::GetNumFromNode(const GosChess::Cell &_cell) {
    return _cell.y * GosChess::Board::ROW_NUM + _cell.x;
}


bool GosChess::CanMakeMove(GosChess::Move mv) {
    return GosChess::available_moves[mv.move_from].find(mv) != GosChess::available_moves[mv.move_from].end();
}


bool GosChess::MakeMove(GosChess::Move mv, GosChess::Board &brd) {
    if (!CanMakeMove(mv)) return false;
    brd.SaveState();
    unsigned char current = brd.GetPosition(mv.move_from).full_type;
    brd.SetPosition(mv.move_from, 0);
    brd.SetPosition(mv.move_to, current);
    if (CheckForKingCheck(brd.GetRawBoard(), GosChess::color_to_play)) {
        brd.Undo();
        return false;
    }
    return true;
}


void GosChess::ChangeActiveColour() {
    GosChess::color_to_play = static_cast<GosChess::Color>(!(int) GosChess::color_to_play);
}


bool GosChess::CheckMate(GosChess::Board &brd, GosChess::Color clr) {
    GosChess::Figure test_fig;
    for (auto it = GosChess::available_moves.begin(); it != GosChess::available_moves.end(); it++) {
        test_fig.full_type = brd.GetRawBoard()[it->first];
        if (test_fig.color == clr) {
            for (auto &move: GosChess::available_moves[it->first]) {
                if (GosChess::MakeMove(move, brd)) {
                    brd.Undo();
                    return false;
                }
            }
        }
    }
    return true;
}



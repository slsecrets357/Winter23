#include <array>

class Chessboard {
    private:
        // A 2D array to store the positions of the pieces on the board
        std::array<std::array<Piece, 8>, 8> board;

    public:
        // Constructor to initialize the board to the starting position
        Chessboard() {
            // Set up the white pieces
            board[0][0] = Piece(PieceType::ROOK, Color::WHITE, {0, 0});
            board[0][1] = Piece(PieceType::KNIGHT, Color::WHITE, {0, 1});
            board[0][2] = Piece(PieceType::BISHOP, Color::WHITE, {0, 2});
            board[0][3] = Piece(PieceType::QUEEN, Color::WHITE, {0, 3});
            board[0][4] = Piece(PieceType::KING, Color::WHITE, {0, 4});
            board[0][5] = Piece(PieceType::BISHOP, Color::WHITE, {0, 5});
            board[0][6] = Piece(PieceType::KNIGHT, Color::WHITE, {0, 6});
            board[0][7] = Piece(PieceType::ROOK, Color::WHITE, {0, 7});
            for (int i = 0; i < 8; i++) {
                board[1][i] = Piece(PieceType::PAWN, Color::WHITE, {1, i});
            }

            // Set up the black pieces
            board[7][0] = Piece(PieceType::ROOK, Color::BLACK, {7, 0});
            board[7][1] = Piece(PieceType::KNIGHT, Color::BLACK, {7, 1});
            board[7][2] = Piece(PieceType::BISHOP, Color::BLACK, {7, 2});
            board[7][3] = Piece(PieceType::QUEEN, Color::BLACK, {7, 3});
            board[7][4] = Piece(PieceType::KING, Color::BLACK, {7, 4});
            board[7][5] = Piece(PieceType::BISHOP, Color::BLACK, {7, 5});
            board[7][6] = Piece(PieceType::KNIGHT, Color::BLACK, {7, 6});
            board[7][7] = Piece(PieceType::ROOK, Color::BLACK, {7, 7});
            for (int i = 0; i < 8; i++) {
                board[6][i] = Piece(PieceType::PAWN, Color::BLACK, {6, i});
            }

            // Set all other squares to empty
            for (int i = 2; i < 6; i++) {
                for (int j = 0; j < 8; j++) {
                    board[i][j] = Piece();
                }
            }
        }

        // Function to move a piece from one square to another
        void movePiece(Square from, Square to) {
            // Check if the move is legal
            if (!board[from.row][from.col].canMoveTo(to, board)) {
                throw std::invalid_argument("Illegal move");
            }

            // Update the board state
            board[to.row][to.col] = board[from.row][from.col];
            board[from.row][from.col] = Piece();

            // Update the piece's position
            board[to.row][to.col].position = to;
        }
}
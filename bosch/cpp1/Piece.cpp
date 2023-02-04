class Piece {
private:
    // Private variables and functions
    PieceType type;
    Color color;
    Square position;

public:
    // Constructor to create an empty piece (i.e. no piece at this square)
    Piece() : type(PieceType::NONE), color(Color::NONE), position(-1, -1) {}

    // Constructor to create a piece with a given type, color, and position
    Piece(PieceType type, Color color, Square position) : type(type), color(color), position(position) {}

    // Function to check if the piece can move to a particular square
    bool canMoveTo(Square to, const Chessboard& board) const {
        // Implement the specific movement rules for each piece type
        switch (type) {
            case PieceType::PAWN:
                // Pawns can move forward one square, or capture diagonally
                // ...
                break;
            case PieceType::KNIGHT:
                // Knights can move to any of the squares two squares horizontally and one square vertically, or vice versa
                // ...
                break;
            case PieceType::BISHOP:
                // Bishops can move diagonally any number of squares
                // ...
                break;
            case PieceType::ROOK:
                // Rooks can move horizontally or vertically any number of squares
                // ...
                break;
            case PieceType::QUEEN:
                // Queens can move like bishops or rooks
                // ...
                break;
            case PieceType::KING:
                // Kings can move one square in any direction
                // ...
                break;
            default:
                // Empty squares cannot move
                return false;
        }

        // Check if the destination square is occupied by a piece of the same color
        if (board[to.row][to.col].color == color) {
            return false;
        }

        return true;
    }

    // Function to check if the piece can attack a particular square
    bool canAttack(Square to, const Chessboard& board) const {
        // Implement the specific attack rules for each piece type
        // ...

        return false;
    }
};
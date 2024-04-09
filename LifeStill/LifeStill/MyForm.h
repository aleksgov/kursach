#pragma once
#using <System.dll>
#using <System.Windows.Forms.dll>
#include <algorithm>
#include <utility>
#include <cstdlib>
#include <ctime>
#include <vcclr.h>

namespace LifeStill {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;
    using namespace System::Collections::Generic;

    public ref class Checker {
    public:
        property Color CheckerColor;
        property Point CheckerPosition;
        property bool IsKing;

        Checker(Color color, Point position) {
            CheckerColor = color;
            CheckerPosition = position;
            IsKing = false; 
        }
    };

    public ref class MyForm : public System::Windows::Forms::Form {
    public:
        MyForm() {
            InitializeComponent();
            isWhiteTurn = true;
            this->WindowState = FormWindowState::Maximized;
            this->BackgroundImage = Image::FromFile("background.png");
            this->BackgroundImageLayout = ImageLayout::Stretch;

            button1->Image = Image::FromFile("button.png");
            button1->FlatStyle = FlatStyle::Flat;
            button1->BackColor = Color::Transparent;
            button1->FlatAppearance->BorderSize = 0;
            button1->FlatAppearance->MouseOverBackColor = Color::Transparent;
            button1->FlatAppearance->MouseDownBackColor = Color::Transparent;

            button2->Image = Image::FromFile("button.png");
            button2->FlatStyle = FlatStyle::Flat;
            button2->BackColor = Color::Transparent;
            button2->FlatAppearance->BorderSize = 0;
            button2->FlatAppearance->MouseOverBackColor = Color::Transparent;
            button2->FlatAppearance->MouseDownBackColor = Color::Transparent;

            button3->Image = Image::FromFile("button.png");
            button3->FlatStyle = FlatStyle::Flat;
            button3->BackColor = Color::Transparent;
            button3->FlatAppearance->BorderSize = 0;
            button3->FlatAppearance->MouseOverBackColor = Color::Transparent;
            button3->FlatAppearance->MouseDownBackColor = Color::Transparent;

            button4->Image = Image::FromFile("button.png");
            button4->FlatStyle = FlatStyle::Flat;
            button4->BackColor = Color::Transparent;
            button4->FlatAppearance->BorderSize = 0;
            button4->FlatAppearance->MouseOverBackColor = Color::Transparent;
            button4->FlatAppearance->MouseDownBackColor = Color::Transparent;

            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;

            this->button1->Click += gcnew System::EventHandler(this, &MyForm::button1_Click);

        }

    protected:
        ~MyForm() {
            if (components) {
                delete components;
            }
        }

    private:


        bool isGameOver; 
        bool isWhiteTurn;
        bool isCheckerSelected;
        Point selectedCheckerPosition;
        array<Checker^, 2>^ checkersArray;
        array<Button^, 2>^ boardButtons;
        const int boardSize = 8, checkerSize = 8;;

        System::Windows::Forms::Button^ button1;
        System::Windows::Forms::Button^ button2;
        System::Windows::Forms::Button^ button3;
        System::Windows::Forms::Button^ button4;
        System::Windows::Forms::Panel^ optionPanel;
        System::Windows::Forms::Panel^ fullScreenPanel;
        System::Windows::Forms::Panel^ boardPanel;

        System::Void button1_Click(System::Object^ sender, System::EventArgs^ e) {
            gamePanel();
        }

        System::Void button4_Click(System::Object^ sender, System::EventArgs^ e) {
            Application::Exit();
        }

        void MyForm::boardButton_Click(System::Object^ sender, System::EventArgs^ e) {
            Button^ clickedButton = dynamic_cast<Button^>(sender);

            if (clickedButton == nullptr)
                return;

            int row = (clickedButton->Location.Y - (fullScreenPanel->Height - clickedButton->Height * boardSize) / 2) / clickedButton->Height;
            int col = (clickedButton->Location.X - (fullScreenPanel->Width - clickedButton->Width * boardSize) / 2) / clickedButton->Width;

            Point clickedPosition(row, col);
            GetPossibleMoves(clickedPosition);

            if (isCheckerSelected) {
                Checker^ selectedChecker = checkersArray[selectedCheckerPosition.X, selectedCheckerPosition.Y];

                if (((isWhiteTurn && selectedChecker->CheckerColor == Color::White) ||
                    (!isWhiteTurn && selectedChecker->CheckerColor == Color::Black)) && IsValidMove(selectedCheckerPosition, clickedPosition)) {
                    MoveChecker(checkersArray, selectedCheckerPosition, clickedPosition);
                    isCheckerSelected = false;
                    MakeBotMove();
                    
                    if (IsGameOver()) {
                        MessageBox::Show("Игра окончена!", "Конец игры");
                    }
                }
            }
            else {
                Checker^ clickedChecker = checkersArray[row, col];

                if (clickedChecker != nullptr &&
                    ((isWhiteTurn && clickedChecker->CheckerColor == Color::White) ||
                        (!isWhiteTurn && clickedChecker->CheckerColor == Color::Black))) {
                    isCheckerSelected = true;
                    selectedCheckerPosition = clickedPosition;
                }
            }
        }

        
        bool MyForm::IsValidMove(Point source, Point target) {
            
            int rowDiff = target.X - source.X;
            int colDiff = target.Y - source.Y;

            if (target.X < 0 || target.X >= boardSize || target.Y < 0 || target.Y >= boardSize) {
               return false;
            }

            if (checkersArray[source.X, source.Y] == nullptr) {
               return false;
            }


            if (checkersArray[target.X, target.Y] != nullptr) {
                return false; 
            }

            
            Checker^ sourceChecker = checkersArray[source.X, source.Y];

            if (sourceChecker->IsKing) {
                return false;
            }

            if ((sourceChecker->CheckerColor == Color::White && rowDiff == -1 && (colDiff == -1 || colDiff == 1)) ||
                (sourceChecker->CheckerColor == Color::Black && rowDiff == 1 && (colDiff == -1 || colDiff == 1))) {
                return true;
            }

            if ((sourceChecker->CheckerColor == Color::White && rowDiff == -2 && (colDiff == -2 || colDiff == 2)) ||
                (sourceChecker->CheckerColor == Color::Black && rowDiff == 2 && (colDiff == -2 || colDiff == 2))) {
                int row = (source.X + target.X) / 2;
                int col = (source.Y + target.Y) / 2;
                Point jumpedPosition(row, col);
                Checker^ jumpedChecker = checkersArray[jumpedPosition.X, jumpedPosition.Y];
                if (jumpedChecker != nullptr && jumpedChecker->CheckerColor != sourceChecker->CheckerColor) {
                    return true;
                }
            }

            return false;
        }


        void MyForm::MoveChecker(array<Checker^, 2>^ board, Point source, Point target) {
            if (IsValidMove(source, target)) {
                Checker^ sourceChecker = checkersArray[source.X, source.Y];
                checkersArray[source.X, source.Y] = nullptr;

                if (Math::Abs(target.X - source.X) == 2 && Math::Abs(target.Y - source.Y) == 2) {
                    int row = (source.X + target.X) / 2;
                    int col = (source.Y + target.Y) / 2;
                    Point capturedCheckerPosition(row, col);
                    checkersArray[capturedCheckerPosition.X, capturedCheckerPosition.Y] = nullptr;
                    boardButtons[capturedCheckerPosition.X, capturedCheckerPosition.Y]->BackgroundImage = nullptr;
                }

                checkersArray[target.X, target.Y] = sourceChecker;

                Button^ sourceButton = boardButtons[source.X, source.Y];
                Button^ targetButton = boardButtons[target.X, target.Y];

                targetButton->BackgroundImage = sourceButton->BackgroundImage;
                targetButton->BackgroundImageLayout = ImageLayout::Zoom;
                targetButton->Tag = sourceButton->Tag;

                sourceButton->BackgroundImage = nullptr;
                sourceButton->Tag = nullptr;
                CheckForEnemyNearKings();
            }
        }

        int MyForm::CheckForEnemyNearKings() {
            int enemyCount = 0;

            for (int i = 0; i < boardSize; ++i) {
                for (int j = 0; j < boardSize; ++j) {
                    Checker^ currentChecker = checkersArray[i, j];

                    if (currentChecker != nullptr && currentChecker->IsKing) {
                        if (i - 1 >= 0 && j - 1 >= 0 && checkersArray[i - 1, j - 1] != nullptr &&
                            checkersArray[i - 1, j - 1]->CheckerColor != currentChecker->CheckerColor) {
                            enemyCount++;
                        }
                        if (i - 1 >= 0 && j + 1 < boardSize && checkersArray[i - 1, j + 1] != nullptr &&
                            checkersArray[i - 1, j + 1]->CheckerColor != currentChecker->CheckerColor) {
                            enemyCount++;
                        }
                        if (i + 1 < boardSize && j - 1 >= 0 && checkersArray[i + 1, j - 1] != nullptr &&
                            checkersArray[i + 1, j - 1]->CheckerColor != currentChecker->CheckerColor) {
                            enemyCount++;
                        }
                        if (i + 1 < boardSize && j + 1 < boardSize && checkersArray[i + 1, j + 1] != nullptr &&
                            checkersArray[i + 1, j + 1]->CheckerColor != currentChecker->CheckerColor) {
                            enemyCount++;
                        }
                    }
                }
            }
            return enemyCount;
        }

        bool MyForm::IsGameOver() {
            int whiteCount = 0;
            int blackCount = 0;

            for (int i = 0; i < boardSize; ++i) {
                for (int j = 0; j < boardSize; ++j) {
                    if (checkersArray[i, j] != nullptr) {
                        if (checkersArray[i, j]->CheckerColor == Color::White) {
                            whiteCount++;
                        }
                        else if (checkersArray[i, j]->CheckerColor == Color::Black) {
                            blackCount++;
                        }
                    }
                }
            }
            if (whiteCount < 3) {
                MessageBox::Show("Игра окончена! Черные победили!", "Конец игры");
                return true;
            }
            else if (blackCount < 3) {
                MessageBox::Show("Игра окончена! Белые победили!", "Конец игры");
                return true;
            }
            else if (CheckForEnemyNearKings() >= 3) {
                MessageBox::Show("Игра окончена. Захвачена крепость!");
                return true;
            }
            return false;
        }

        void MyForm::GetPossibleMoves(Point currentPosition) {
            ClearHighlightedMoves();

            int row = currentPosition.X;
            int col = currentPosition.Y;

            if (checkersArray[row, col] != nullptr) {
                int direction = (checkersArray[row, col]->CheckerColor == Color::White) ? -1 : 1;

                if (!checkersArray[row, col]->IsKing) {
                    for (int j = -1; j <= 1; j += 2) {
                        int newRow = row + direction;
                        int newCol = col + j;
                        if (newRow >= 0 && newRow < boardSize && newCol >= 0 && newCol < boardSize) {
                            if (checkersArray[newRow, newCol] == nullptr) {
                                boardButtons[newRow, newCol]->FlatAppearance->BorderColor = Color::Yellow;
                                boardButtons[newRow, newCol]->FlatAppearance->BorderSize = 3;
                                boardButtons[newRow, newCol]->Click += gcnew System::EventHandler(this, &MyForm::highlightedButton_Click);
                            }
                            else if (checkersArray[newRow, newCol]->CheckerColor != checkersArray[row, col]->CheckerColor) {
                                int opponentRow = newRow + direction;
                                int opponentCol = newCol + j;
                                if (opponentRow >= 0 && opponentRow < boardSize && opponentCol >= 0 && opponentCol < boardSize) {
                                    if (checkersArray[opponentRow, opponentCol] == nullptr) {
                                        boardButtons[opponentRow, opponentCol]->FlatAppearance->BorderColor = Color::Yellow;
                                        boardButtons[opponentRow, opponentCol]->FlatAppearance->BorderSize = 3;
                                        boardButtons[opponentRow, opponentCol]->Click += gcnew System::EventHandler(this, &MyForm::highlightedButton_Click);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        List<Tuple<Point, Point>^>^ MyForm::GetAllPossibleMoves(array<Checker^, 2>^ board, Color playerColor) {
            List<Tuple<Point, Point>^>^ possibleMoves = gcnew List<Tuple<Point, Point>^>();

            for (int i = 0; i < boardSize; ++i) {
                for (int j = 0; j < boardSize; ++j) {
                    if (board[i, j] != nullptr && board[i, j]->CheckerColor == playerColor) {
                        Point sourcePosition(i, j);
                        for (int di = -2; di <= 2; ++di) {
                            for (int dj = -2; dj <= 2; ++dj) {
                                int newRow = i + di;
                                int newCol = j + dj;
                                if (newRow >= 0 && newRow < boardSize && newCol >= 0 && newCol < boardSize) {
                                    Point targetPosition(newRow, newCol);
                                    if (IsValidMove(sourcePosition, targetPosition)) {
                                        Tuple<Point, Point>^ move = gcnew Tuple<Point, Point>(sourcePosition, targetPosition);
                                        possibleMoves->Add(move);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            return possibleMoves;
        }

        void MyForm::ClearHighlightedMoves() {
            for (int i = 0; i < boardSize; ++i) {
                for (int j = 0; j < boardSize; ++j) {
                    boardButtons[i, j]->FlatAppearance->BorderColor = Color::Gray;
                    boardButtons[i, j]->FlatAppearance->BorderSize = 1;
                    boardButtons[i, j]->Click -= gcnew System::EventHandler(this, &MyForm::highlightedButton_Click);
                }
            }
        }

        array<int>^ MyForm::AddToArray(array<int>^ sourceArray, int newValue) {
            array<int>^ newArray = gcnew array<int>(sourceArray->Length + 1);
            Array::Copy(sourceArray, newArray, sourceArray->Length);
            newArray[sourceArray->Length] = newValue;
            return newArray;
        }

        int MyForm::EvaluateBoard(array<Checker^, 2>^ checkersArray, int boardSize) {
            int whiteCount = 0;
            int blackCount = 0;

            for (int i = 0; i < boardSize; ++i) {
                for (int j = 0; j < boardSize; ++j) {
                    if (checkersArray[i, j] != nullptr) {
                        if (checkersArray[i, j]->CheckerColor == Color::White) {
                            whiteCount++;

                            if (checkersArray[i, j]->IsKing) {
                                whiteCount += boardSize - i; 
                            }
                            else {
                                whiteCount += DistanceToNearestKing(checkersArray, Point(i, j), Color::Black);
                            }
                        }
                        else if (checkersArray[i, j]->CheckerColor == Color::Black) {
                            blackCount++;

                            if (checkersArray[i, j]->IsKing) {
                                blackCount += i;
                            }
                            else {
                                blackCount += DistanceToNearestKing(checkersArray, Point(i, j), Color::White);
                            }
                        }
                    }
                }
            }

            return whiteCount - blackCount;
        }

        int MyForm::DistanceToNearestKing(array<Checker^, 2>^ checkersArray, Point source, Color kingColor) {
            int minDistance = boardSize * boardSize;

            for (int i = 0; i < boardSize; ++i) {
                for (int j = 0; j < boardSize; ++j) {
                    if (checkersArray[i, j] != nullptr && checkersArray[i, j]->CheckerColor == kingColor && checkersArray[i, j]->IsKing) {
                        int distance = Math::Abs(i - source.X) + Math::Abs(j - source.Y);
                        if (distance < minDistance) {
                            minDistance = distance;
                        }
                    }
                }
            }
            return minDistance;
        }

        void MyForm::highlightedButton_Click(System::Object^ sender, System::EventArgs^ e) {
            Button^ clickedButton = dynamic_cast<Button^>(sender);
            if (clickedButton != nullptr) {
                int row = (clickedButton->Location.Y - (fullScreenPanel->Height - clickedButton->Height * boardSize) / 2) / clickedButton->Height;
                int col = (clickedButton->Location.X - (fullScreenPanel->Width - clickedButton->Width * boardSize) / 2) / clickedButton->Width;
                int sourceRow = selectedCheckerPosition.X;
                int sourceCol = selectedCheckerPosition.Y;
                if (IsValidMove(Point(sourceRow, sourceCol), Point(row, col))) {
                    MoveChecker(checkersArray, selectedCheckerPosition, Point(row, col));
                    ClearHighlightedMoves();
                }
            }
        }

        void MyForm::CopyBoard(array<Checker^, 2>^ sourceBoard, array<Checker^, 2>^ targetBoard) {
            for (int i = 0; i < boardSize; ++i) {
                for (int j = 0; j < boardSize; ++j) {
                    if (sourceBoard[i, j] != nullptr) {
                        targetBoard[i, j] = gcnew Checker(sourceBoard[i, j]->CheckerColor, sourceBoard[i, j]->CheckerPosition);
                        targetBoard[i, j]->IsKing = sourceBoard[i, j]->IsKing;
                    }
                    else {
                        targetBoard[i, j] = nullptr;
                    }
                }
            }
        }

        int MyForm::MiniMaxAlphaBetta(array<Checker^, 2>^ board, int depth, int alpha, int beta, bool maximizingPlayer) {
            if (depth == 0 || IsGameOver()) {
                return EvaluateBoard(board, boardSize);
            }
            List<Tuple<Point, Point>^>^ possibleMoves;
            Color currentPlayerColor;
            if (maximizingPlayer) {
                possibleMoves = GetAllPossibleMoves(board, Color::Black);
                currentPlayerColor = Color::Black;
            }
            else {
                possibleMoves = GetAllPossibleMoves(board, Color::White);
                currentPlayerColor = Color::White;
            }
            for each (Tuple<Point, Point> ^ move in possibleMoves) {
                Point source = move->Item1;
                Point target = move->Item2;
                Checker^ tempChecker = board[source.X, source.Y];

                board[source.X, source.Y] = nullptr;
                board[target.X, target.Y] = tempChecker;

                int score = MiniMaxAlphaBetta(board, depth - 1, alpha, beta, !maximizingPlayer);

                board[target.X, target.Y] = nullptr;
                board[source.X, source.Y] = tempChecker;

                if (maximizingPlayer) {
                    alpha = Math::Max(alpha, score);
                }
                else {
                    beta = Math::Min(beta, score);
                }

                if (beta <= alpha) {
                    break;
                }
            }
            return maximizingPlayer ? alpha : beta;
        }


        void MyForm::MakeBotMove() {
            List<Tuple<Point, Point>^>^ possibleMoves = GetAllPossibleMoves(checkersArray, Color::Black);
            int bestMoveIndex = -1;
            int bestEval = INT_MIN;

            for (int i = 0; i < possibleMoves->Count; ++i) {
                Tuple<Point, Point>^ move = possibleMoves[i];
                Point source = move->Item1;
                Point target = move->Item2;
                array<Checker^, 2>^ newBoard = gcnew array<Checker^, 2>(boardSize, boardSize);
                CopyBoard(checkersArray, newBoard);
                MoveChecker(newBoard, source, target);

                int eval = MiniMaxAlphaBetta(newBoard, 4, INT_MIN, INT_MAX, false);
                if (eval > bestEval) {
                    bestEval = eval;
                    bestMoveIndex = i;
                }
                isWhiteTurn != isWhiteTurn;
                break;
                
            }

            if (bestMoveIndex != -1) {
                Tuple<Point, Point>^ bestMove = possibleMoves[bestMoveIndex];
                Point source = bestMove->Item1;
                Point target = bestMove->Item2;
                MoveChecker(checkersArray, source, target);            
            }
        }


        void gamePanel() {

            this->Controls->Clear();

            if (fullScreenPanel == nullptr) {
                fullScreenPanel = gcnew System::Windows::Forms::Panel();
                fullScreenPanel->Size = this->ClientSize;
                fullScreenPanel->BackgroundImage = Image::FromFile("background.png");
                fullScreenPanel->BackgroundImageLayout = ImageLayout::Stretch;
                fullScreenPanel->BackColor = Color::Transparent;
                int boardPanelSize = Math::Min(fullScreenPanel->Width, fullScreenPanel->Height);
                int boardPanelLocationX = (fullScreenPanel->Width - boardPanelSize) / 2;
                int boardPanelLocationY = (fullScreenPanel->Height - boardPanelSize) / 2;
                System::Drawing::Color darkColor = System::Drawing::Color::FromArgb(83, 69, 58);
                System::Drawing::Color lightColor = System::Drawing::Color::FromArgb(192, 138, 86);
                System::Drawing::Color borderColor = System::Drawing::Color::FromArgb(121, 94, 61);
                boardButtons = gcnew array<Button^, 2>(boardSize, boardSize);
                checkersArray = gcnew array<Checker^, 2>(boardSize, boardSize);


                int checkerboardPattern[8][8] = {
                    {0, 0, 0, 4, 4, 0, 0, 0},
                    {0, 2, 2, 2, 2, 2, 2, 0},
                    {0, 0, 2, 2, 2, 2, 0, 0},
                    {0, 0, 0, 0, 0, 0, 0, 0},
                    {0, 0, 0, 0, 0, 0, 0, 0},
                    {0, 0, 1, 1, 1, 1, 0, 0},
                    {0, 1, 1, 1, 1, 1, 1, 0},
                    {0, 0, 0, 3, 3, 0, 0, 0}
                 };

                
                for (int i = 0; i < boardSize; ++i) {
                    for (int j = 0; j < boardSize; ++j) {
                        boardButtons[i, j] = gcnew Button();
                        boardButtons[i, j]->Size = System::Drawing::Size(100, 100);
                        boardButtons[i, j]->Location = System::Drawing::Point(
                            boardPanelLocationX + (boardPanelSize - boardButtons[i, j]->Width * boardSize) / 2 + j * boardButtons[i, j]->Width,
                            boardPanelLocationY + (boardPanelSize - boardButtons[i, j]->Height * boardSize) / 2 + i * boardButtons[i, j]->Height
                        );
                        boardButtons[i, j]->FlatStyle = FlatStyle::Flat;
                        boardButtons[i, j]->FlatAppearance->BorderColor = Color::Gray;
                        boardButtons[i, j]->FlatAppearance->BorderSize = 1;
                        boardButtons[i, j]->BackColor = (i + j) % 2 == 0 ? lightColor : darkColor;
                        boardButtons[i, j]->Click += gcnew System::EventHandler(this, &MyForm::boardButton_Click);

                        switch (checkerboardPattern[i][j]) {
                        case 1:
                            checkersArray[i, j] = gcnew Checker(Color::White, Point(i, j));
                            boardButtons[i, j]->BackgroundImage = Image::FromFile("white.png");
                            break;
                        case 2:
                            checkersArray[i, j] = gcnew Checker(Color::Black, Point(i, j));
                            boardButtons[i, j]->BackgroundImage = Image::FromFile("black.png");
                            break;
                        case 3:
                            checkersArray[i, j] = gcnew Checker(Color::White, Point(i, j));
                            checkersArray[i, j]->IsKing = true;
                            boardButtons[i, j]->BackgroundImage = Image::FromFile("white_king.png");
                            break;
                        case 4:
                            checkersArray[i, j] = gcnew Checker(Color::Black, Point(i, j));
                            checkersArray[i, j]->IsKing = true;
                            boardButtons[i, j]->BackgroundImage = Image::FromFile("black_king.png");
                            break;
                        default:
                            break;
                        }
                        if (checkersArray[i, j] != nullptr) {
                            boardButtons[i, j]->BackgroundImageLayout = ImageLayout::Stretch;
                            boardButtons[i, j]->Tag = checkersArray[i, j];
                        }
                        fullScreenPanel->Controls->Add(boardButtons[i, j]);
                    }
                }

                this->Controls->Add(fullScreenPanel);
                fullScreenPanel->BringToFront();
            }
        }

        System::ComponentModel::Container^ components;

        void InitializeComponent(void) {
            this->button1 = (gcnew System::Windows::Forms::Button());
            this->button2 = (gcnew System::Windows::Forms::Button());
            this->button3 = (gcnew System::Windows::Forms::Button());
            this->button4 = (gcnew System::Windows::Forms::Button());
            // 
            // button1
            // 
            this->button1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Left | System::Windows::Forms::AnchorStyles::Right));
            this->button1->Font = (gcnew System::Drawing::Font(L"Brahms Gotisch Cyr", 40, System::Drawing::FontStyle::Bold));
            this->button1->ForeColor = System::Drawing::Color::Black;
            this->button1->Location = System::Drawing::Point(690, 120);
            this->button1->Name = L"button1";
            this->button1->Size = System::Drawing::Size(543, 198);
            this->button1->TabIndex = 0;
            this->button1->Text = L"Новая игра";
            this->button1->UseVisualStyleBackColor = true;
            // 
            // button2
            // 
            this->button2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Left | System::Windows::Forms::AnchorStyles::Right));
            this->button2->Font = (gcnew System::Drawing::Font(L"Brahms Gotisch Cyr", 40, System::Drawing::FontStyle::Bold));
            this->button2->Location = System::Drawing::Point(690, 330);
            this->button2->Name = L"button2";
            this->button2->Size = System::Drawing::Size(543, 198);
            this->button2->TabIndex = 1;
            this->button2->Text = L"Опции";
            this->button2->UseVisualStyleBackColor = true;
            // 
            // button3
            // 
            this->button3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Left | System::Windows::Forms::AnchorStyles::Right));
            this->button3->Font = (gcnew System::Drawing::Font(L"Brahms Gotisch Cyr", 40, System::Drawing::FontStyle::Bold));
            this->button3->Location = System::Drawing::Point(690, 540);
            this->button3->Name = L"button3";
            this->button3->Size = System::Drawing::Size(543, 198);
            this->button3->TabIndex = 2;
            this->button3->Text = L"Мануал";
            this->button3->UseVisualStyleBackColor = true;
            // 
            // button4
            // 
            this->button4->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Left | System::Windows::Forms::AnchorStyles::Right));
            this->button4->Font = (gcnew System::Drawing::Font(L"Brahms Gotisch Cyr", 40, System::Drawing::FontStyle::Bold));
            this->button4->Location = System::Drawing::Point(690, 750);
            this->button4->Name = L"button4";
            this->button4->Size = System::Drawing::Size(543, 198);
            this->button4->TabIndex = 3;
            this->button4->Text = L"Выход";
            this->button4->UseVisualStyleBackColor = true;
            this->button4->Click += gcnew System::EventHandler(this, &MyForm::button4_Click);
            // 
            // MyForm
            // 
            this->ClientSize = System::Drawing::Size(1920, 1061);
            this->Controls->Add(this->button4);
            this->Controls->Add(this->button3);
            this->Controls->Add(this->button2);
            this->Controls->Add(this->button1);
            this->Name = L"MyForm";
            this->Text = L"MyForm";
            this->ResumeLayout(false);


        }
    };
}
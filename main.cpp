#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <cmath>

using namespace std;

int** currentBoard;
int** goalBoard;
int** moveHistory;
int globalSize = 0;
string currentUser = "";
struct Node {
    int row;
    int col;
    Node* next;
};
Node* topNode = NULL;
int undoCount = 0;
void push(int row, int col) {
    if (undoCount >= 5)
        return;
    Node* newNode = new Node;
    newNode->row = row;
    newNode->col = col;
    newNode->next = topNode;
    topNode = newNode;
    undoCount++;
}
int pop(int &row, int &col) {
    if (topNode == NULL)
        return 0;
    Node* temp = topNode;
    row = temp->row;
    col = temp->col;
    topNode = topNode->next;
    delete temp;
    undoCount--;
    return 1;
}
void clearStack() {
    while (topNode != NULL) {
        Node* temp = topNode;
        topNode = topNode->next;
        delete temp;
    }
    undoCount = 0;
}
void allocateBoards(int size) {
    globalSize = size;
    currentBoard = new int*[size];
    goalBoard = new int*[size];
    moveHistory = new int*[size];
    for (int i = 0; i < size; i++) {
        currentBoard[i] = new int[size];
        goalBoard[i] = new int[size];
        moveHistory[i] = new int[size];
    }
}
void createGoalBoard(int size) {
    int value = 1;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i == size - 1 && j == size - 1)
                goalBoard[i][j] = 0;
            else
                goalBoard[i][j] = value++;
        }
    }
}
void copyGoalToCurrent(int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            currentBoard[i][j] = goalBoard[i][j];
        }
    }
}
void printBoard(int** board, int size) {
    cout << endl;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            cout << "----";
        }
        cout << "-" << endl;
        for (int j = 0; j < size; j++) {
            if (board[i][j] == 0) {
                cout << "| _ ";
            }
            else if (board[i][j] < 10) {
                cout << "| " << board[i][j] << " ";
            }
            else {
                cout << "|" << board[i][j] << " ";
            }
        }
        cout << "|" << endl;
    }
    for (int j = 0; j < size; j++) {
        cout << "----";
    }
    cout << "-" << endl;
}
void findNull(int** board, int size, int &x, int &y) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (board[i][j] == 0) {
                x = i;
                y = j;
                return;
            }
        }
    }
}
int moveTile(int** board, int size, char dir) {
    int x, y;
    findNull(board, size, x, y);
    int oldX = x;
    int oldY = y;
    int nx = x;
    int ny = y;
    if (dir == 'w')
        nx--;
    else if (dir == 's')
        nx++;
    else if (dir == 'a')
        ny--;
    else if (dir == 'd')
        ny++;
    else
        return 0;
    if (nx < 0 || nx >= size || ny < 0 || ny >= size)
        return 0;
    int temp = board[x][y];
    board[x][y] = board[nx][ny];
    board[nx][ny] = temp;
    push(oldX, oldY);
    return 1;
}
void undoMove(int** board, int size) {
    int row, col;
    if (!pop(row, col)) {
        cout << "No undo available!\n";
        return;
    }
    int x, y;
    findNull(board, size, x, y);
    int temp = board[x][y];
    board[x][y] = board[row][col];
    board[row][col] = temp;
    cout << "Undo successful.\n";
}
void shuffleBoard(int** board, int size) {
    char dirs[4] = {'w','a','s','d'};
    for (int i = 0; i < 1000; i++) {
        moveTile(board, size, dirs[rand() % 4]);
    }
    clearStack();
}
int manhattanDistance(int** board, int size) {
    int total = 0;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int value = board[i][j];
            if (value != 0) {
                int goalRow = (value - 1) / size;
                int goalCol = (value - 1) % size;
                total += abs(i - goalRow)
                       + abs(j - goalCol);
            }
        }
    }
    return total;
}
int checkWin(int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (*(*(currentBoard + i) + j) !=
                *(*(goalBoard + i) + j)) {
                return 0;
            }
        }
    }
    return 1;
}
void saveGame(int size) {
    string filename = currentUser + "_save.txt";
    ofstream file(filename.c_str());
    if (!file) {
        cout << "Save error!\n";
        return;
    }
    file << size << endl;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            file << currentBoard[i][j] << " ";
        }
        file << endl;
    }
    file.close();
    cout << "Game saved.\n";
}
void loadGame() {
    string filename = currentUser + "_save.txt";
    ifstream file(filename.c_str());
    if (!file) {
        cout << "No save file found!\n";
        return;
    }
    int size;
    file >> size;
    allocateBoards(size);
    createGoalBoard(size);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            file >> currentBoard[i][j];
        }
    }
    file.close();
    globalSize = size;
    cout << "Game loaded.\n";
}
void freeBoards(int size) {
    for (int i = 0; i < size; i++) {
        delete[] currentBoard[i];
        delete[] goalBoard[i];
        delete[] moveHistory[i];
    }
    delete[] currentBoard;
    delete[] goalBoard;
    delete[] moveHistory;
    clearStack();
}
int usernameExists(string username) {
    ifstream file("users.txt");
    string u, p;
    while (file >> u >> p) {
        if (u == username) {
            file.close();
            return 1;
        }
    }
    file.close();
    return 0;
}
void registerUser() {
    string username;
    string password;
    cout << "Enter username: ";
    cin >> username;
    if (usernameExists(username)) {
        cout << "Username already exists!\n";
        return;
    }
    cout << "Enter password: ";
    cin >> password;
    ofstream file("users.txt", ios::app);
    file << username << " " << password << endl;
    file.close();
    cout << "Registration successful.\n";
}
int loginUser() {
    string username;
    string password;
    int attempts = 0;
    while (attempts < 3) {
        cout << "Username: ";
        cin >> username;
        cout << "Password: ";
        cin >> password;
        ifstream file("users.txt");
        string u, p;
        while (file >> u >> p) {
            if (u == username && p == password) {
                currentUser = username;
                file.close();
                cout << "Login successful.\n";
                return 1;
            }
        }
        file.close();
        attempts++;
        cout << "Wrong username or password!\n";
    }
    cout << "Too many failed attempts!\n";
    return 0;
}
void printMenu() {
    cout << "\n===== MENU =====\n";
    cout << "1 - 3x3 Game\n";
    cout << "2 - 4x4 Game\n";
    cout << "3 - 5x5 Game\n";
    cout << "4 - Load Game\n";
    cout << "5 - Quit\n";
    cout << "Choice: ";
}
int main() {
    srand(time(0));
    int firstChoice;
    while (true) {
        cout << "\n===== USER SYSTEM =====\n";
        cout << "1 - Register\n";
        cout << "2 - Login\n";
        cout << "3 - Quit\n";
        cout << "Choice: ";
        cin >> firstChoice;
        if (firstChoice == 1) {
            registerUser();
        }
        else if (firstChoice == 2) {
            if (loginUser())
                break;
            else
                return 0;
        }
        else if (firstChoice == 3) {
            return 0;
        }
        else {
            cout << "Invalid selection!\n";
        }
    }
    int choice;
    int size;
    while (true) {
        printMenu();
        cin >> choice;
        if (choice == 5) {
            cout << "Exit...\n";
            return 0;
        }
        if (choice == 4) {
            loadGame();
            size = globalSize;
        }
        else {
            if (choice == 1)
                size = 3;
            else if (choice == 2)
                size = 4;
            else if (choice == 3)
                size = 5;
            else {
                cout << "Invalid selection!\n";
                continue;
            }
            allocateBoards(size);
            createGoalBoard(size);
            copyGoalToCurrent(size);
            shuffleBoard(currentBoard, size);
        }
        while (true) {
            printBoard(currentBoard, size);
            cout << "Distance to solution: "
                 << manhattanDistance(currentBoard, size)
                 << endl;
            if (checkWin(size)) {
                cout << "YOU WIN!\n";
                break;
            }
            char input;
            cout << "(w/a/s/d) move, u undo, p save, q quit: ";
            cin >> input;
            if (input == 'q')
                break;
            if (input == 'u') {
                undoMove(currentBoard, size);
                continue;
            }
            if (input == 'p') {
                saveGame(size);
                continue;
            }
            if (!moveTile(currentBoard, size, input)) {
                cout << "Invalid move!\n";
            }
        }
        freeBoards(size);
    }
    return 0;
}

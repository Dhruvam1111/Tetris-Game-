
# Tetris Game

## Overview
This is a console-based Tetris game built in C++ for Windows. The game features smooth gameplay, color-coded tetrominoes, and essential Tetris mechanics like line clearing, scoring, and increasing difficulty. It runs in the Windows terminal and uses Windows-specific APIs for input handling.

## Student IDS
- **Dhruvam Panchal - 202401415** 
- **Dwij Patel - 202401418** 
- **Shubh Patel - 202401478** 
- **Yashraj Solanki - 202401481**

## Features
- **Classic Tetris Gameplay**: Blocks fall from the top, and players must complete full lines to clear them.
- **Colorful UI**: Tetrominoes and the game board are displayed in different colors.
- **Score and Level System**: The score increases with each line cleared, and the speed of falling blocks increases as levels progress.
- **Smooth Input Handling**: Arrow keys allow movement, and holding keys enables continuous movement.
- **Pause and Restart Options**: Players can pause the game or restart without closing the application.
- **Game Over Screen**: Displays the final score before allowing a restart.
- **Windows Console Compatibility**: The game is built using Windows-specific console functions.

## Controls
- **Left Arrow (←)**: Move tetromino left
- **Right Arrow (→)**: Move tetromino right
- **Down Arrow (↓)**: Soft drop (move piece down faster)
- **Up Arrow (↑)**: Rotate tetromino clockwise
- **Spacebar**: Instantly drop the tetromino
- **S/s** : Smash 
- **P/p** : Pause
- **Q/q**: Quit the game

## Game Mechanics
- The game board is **10x20** (width x height).
- **Tetrominoes**: Standard Tetris shapes (I, O, T, L, J, S, Z) are included.
- **Clearing Lines**: Completing a full horizontal row removes that line and moves upper lines down.
- **Increasing Difficulty**: The game speeds up after every few cleared lines.
- **Game Over Condition**: The game ends when new blocks can no longer spawn at the top.

## Code Structure
- **`tetris.cpp`**: Main game logic, including rendering, input handling, and game loop.
- **Data Structures Used**:
  - **2D Array**: Represents the game board.
  - **Structs**: Used for tetromino shapes and their properties.
  - **Enumerations**: Define different block types and game states.

## OS Dependencies
- **Windows Only**: The game uses Windows-specific functions like `SetConsoleCursorPosition` and `GetAsyncKeyState` for handling input and rendering in the console.
- **No External Libraries Required**: The game runs with built-in Windows API functions.

## How to Run
1. Open a **Windows Command Prompt**.
2. Compile the code using a C++ compiler like MinGW or MSVC:
   ```sh
   g++ tetris.cpp -o tetris.exe
   ```
3. Run the game:
   ```sh
   tetris.exe
   ```

## Future Improvements
- Adding **cross-platform compatibility**.
- Implementing **sound effects** for a richer experience.
- Enhancing **graphics** using a better rendering approach.

## License
This project is open-source and available under the MIT License.
Feel free to modify and use it for educational purposes!

<img src ="Tetris Screenshot.png">

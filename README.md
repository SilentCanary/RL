# RL

# Tic Tac Toe with Reinforcement Learning (Value Iteration)

This is a simple Tic Tac Toe game built using **C++** and **SFML** for the GUI.  
It includes a basic AI agent trained using **Reinforcement Learning (Value Iteration)**.

- The AI learns optimal moves by simulating all possible game states.
- It assumes the opponent plays optimally (minimax-style reasoning).
- For its **first move**, the AI follows an **epsilon-greedy strategy** to add some randomness.

### How to Play
- Run the executable
- Click to place your move (you play as 'O', AI plays as 'X')
- Press `R` to restart the game after it's over

### Requirements
- SFML library (used for graphics and input)
- C++ compiler (e.g., g++, Visual Studio)

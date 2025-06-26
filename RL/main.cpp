#include <iostream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <unordered_set>
#include <unordered_map>
using namespace std;
using namespace sf;

class RL
{
    unordered_set<string>all_states;
    unordered_map<string,double>V;

    double gamma=0.9;
    double threshold=1e-5;

    bool check_winner(const string& state,string& result)
    {
        vector<vector<int>>win_lines={{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};
        for(const auto&line:win_lines)
        {
            char a=state[line[0]]; char b=state[line[1]] ; char c=state[line[2]];
            if(a!=' ' && a==b && b==c)
            {
                result=string(1,a);
                return true;
            }
        }
        return false;
    }

    bool is_terminal(const string&state,string& result)
    {
        if(check_winner(state,result)) return true;
        if(count(state.begin(), state.end(), ' ') == 0)
        {
            result = "DRAW";
            return true;
        }
        return false;
    }

    void generate_states(string state,char turn)
    {
        int count_X=count(state.begin(),state.end(),'X');
        int count_O=count(state.begin(),state.end(),'O');
        if (count_X < count_O || count_X > count_O + 1) return;
        if(all_states.count(state)) return;
        all_states.insert(state);string result;
        if(is_terminal(state,result)) return;
        for(int i=0;i<9;i++)
        {
            if(state[i]==' ')
            {
                string next=state;
                next[i]=turn;
                char next_turn=(turn=='X')?'O':'X';
                generate_states(next,next_turn);
            }
        }
    }

    int get_reward(const string& state,char player)
    {
        string result;
        if(!is_terminal(state,result)) return 0;
        if(result=="DRAW") return 0;
        if(result[0]==player) return 1;
        else return -1;
    }
    
    public:
    unordered_map<string,int>policy_pi;
    RL()
    {
        generate_states("         ",'X');
        int s=all_states.size();
        for(const auto& s:all_states)
        {
            V[s]=0;
        }
    }

    void  train(char agent_symbol='X')
    {
        while(true)
        {
            bool converged=true;
            for(const auto&s:all_states)
            {
                string result;
                if(is_terminal(s,result)) continue;

                vector<int>actions;
                for(int i=0;i<9;i++)
                {
                    if(s[i]==' ') actions.push_back(i);
                }
                double best_val=INT_MIN;
                for(int a:actions)
                {
                    string next_state=s;
                    next_state[a]=agent_symbol;
                    double reward=get_reward(next_state,agent_symbol);
                    if(is_terminal(next_state,result))
                    {
                        best_val=max(best_val,reward);
                    }
                    else
                    {
                        char opponent=(agent_symbol=='X')?'O':'X';
                        double worst_case=INT_MAX;
                        for(int b=0;b<9;b++)
                        {
                            if(next_state[b]!=' ') continue;
                            string opp_next=next_state;
                            opp_next[b]=opponent;
                            double value;
                            string opp_result;
                            if (is_terminal(opp_next, opp_result))
                            {
                                value = get_reward(opp_next, agent_symbol);  // reward from agent's POV
                            } 
                            else 
                            {
                                value = V[opp_next];
                            }
                            worst_case=min(worst_case,value);
                        }
                        double total=reward+gamma*worst_case;
                        best_val=max(best_val,total);
                    }
                }
                
                if(abs(V[s]-best_val)>threshold)
                {
                    converged=false;
                }
                V[s]=best_val;
            }
            if (converged)
            {
                cout << "Training complete" << endl << flush;
                break;
            }
        }

    }

    void extract_policy(char agent_symbol='X')
    {
        for(const auto& s:all_states)
        {
            string result;
            if(is_terminal(s,result)) continue;
            double best_val=INT_MIN;
            int best_action=-1;
            for(int a=0;a<9;a++)
            {
                if(s[a]!=' ') continue;
                string next_state=s;
                next_state[a]=agent_symbol;
                double reward=get_reward(next_state,agent_symbol);
                double future_value;
                if (is_terminal(next_state, result))
                {
                    future_value = 0.0;
                }
                else
                {
                    char opponent = (agent_symbol == 'X') ? 'O' : 'X';
                    double worst_case = INT_MAX;
    
                    for (int b = 0; b < 9; b++)
                    {
                        if (next_state[b] != ' ') continue;
    
                        string opp_next = next_state;
                        opp_next[b] = opponent;
    
                        string opp_result;
                        double value;
                        if (is_terminal(opp_next, opp_result))
                            value = get_reward(opp_next, agent_symbol);
                        else
                            value = V[opp_next];
    
                        worst_case = min(worst_case, value);
                    }
                    future_value = worst_case;
                }

                double total = reward + gamma * future_value;
                if(total>best_val)
                {
                    best_val=total;
                    best_action=a;
                }
            }
            policy_pi[s]=best_action;
        }

    }
};

class Board
{
    vector<vector<char>>board;
    char current_player;
    bool game_over;

    public:
    void reset()
    {
        
        board = vector<vector<char>>(3, vector<char>(3, ' '));
        current_player='X';
        game_over=false;
    }
    Board()
    {
        reset();
    }
    char get_current_player()
    {
        return current_player;
    }

    char get_cell(int row,int col)
    {
        return board[row][col];
    }
    bool is_game_over()
    {
        return game_over;
    }
    bool make_move(int row,int col)
    {
        if(board[row][col]!=' ' || game_over) return false;
        board[row][col]=current_player;

        string result;
        bool game_completed=check_winner(result);
        if(game_completed)
        {
            cout<<"GAME OVER!! "<<endl;
            if(result=="DRAW")
            {
                cout<<result;
            }
            else
            {
                cout<<"Winner is : "<<result<<endl;
            }
            game_over=true;
        }
        else
        {
            current_player=(current_player=='X')?'O':'X';
        }
        return true;
    }

    bool check_winner(string& result)
    {
        for(int i=0;i<3;i++)
        {
            if(board[i][0]!=' ' && board[i][0]==board[i][1] && board[i][1]==board[i][2])
            {
                result=board[i][0];
                return true;
            }
            if(board[0][i]!=' ' && board[0][i]==board[1][i] && board[1][i]==board[2][i])
            {
                result=board[i][0];
                return true;
            }
        }
        if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2])
        {
           result=board[0][0];
           return true;
        }
        if (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0])
        {
            result=board[0][2];
            return true;
        }
        for (int i = 0; i < 3; i++)
        {
            for(int j=0;j<3;j++)
            {
                if(board[i][j]==' ')  return false;
            }
        }
        result="DRAW";
        return true;
    }
};

class Game
{
    RenderWindow window;
    Board board;
    Font font;
    const int boardSize = 600;
    const int windowSize = 800;
    const int offset = (windowSize - boardSize) / 2;
    Texture wood_texture;
    Sprite background;

    RL rl;
    bool ai_turn=true;

    public:
    Game():window(VideoMode(800,800),"Tic Tac Toe")
    {
        if(!font.loadFromFile("ARCADE.ttf"))
        {
            cout<<"Couldn't Load font"<<endl;
        }
        if(!wood_texture.loadFromFile("Seamless-Wood-Texture-3.jpg"));
        background.setTexture(wood_texture);
        background.setScale(float(window.getSize().x) / wood_texture.getSize().x,float(window.getSize().y) / wood_texture.getSize().y);
        rl.train('X');
        rl.extract_policy('X');
    }
    string get_board_state_string() 
    {
       string state = "";
       for (int i = 0; i < 3; ++i)
           for (int j = 0; j < 3; ++j)
               state += board.get_cell(i, j);
       return state;
   }

    void render()
    {
        window.clear();
        window.draw(background);
        draw_board();
        draw_marks();
        window.display();
    }
    void draw_board()
    {
        for (int i = 1; i <= 2; i++) 
        {
            RectangleShape hLine(Vector2f(600, 5));
            hLine.setFillColor(Color::Black);
            hLine.setPosition(offset, offset + i * 200);
            window.draw(hLine);

            RectangleShape vLine(Vector2f(5, 600));
            vLine.setFillColor(Color::Black);
            vLine.setPosition(offset + i * 200, offset);
            window.draw(vLine);
        }
    }
    void draw_marks() 
    {
        for (int i = 0; i < 3; i++) 
        {
            for (int j = 0; j < 3; j++) 
            {
                char symbol = board.get_cell(i, j);
                if (symbol != ' ') 
                {
                    Text text;
                    text.setFont(font);
                    text.setString(string(1, symbol));
                    if (symbol=='X')
                    {
                        text.setFillColor(Color(165, 42, 42));
                    }
                    else
                    {
                        text.setFillColor(Color(47,79,79));
                    }
                    text.setCharacterSize(150);
                  //  text.setFillColor(Color(50, 50, 50));
                    text.setPosition(offset + j * 200 + 50, offset + i * 200 - 20);
                    window.draw(text);
                }
            }
        }
        if (board.is_game_over())
        {
            Text gameOverText;
            gameOverText.setFont(font);
            gameOverText.setCharacterSize(40);
            gameOverText.setFillColor(Color(40, 26, 13));
            gameOverText.setPosition(150, 20);
            gameOverText.setString("Game Over! Press R to Reset");
            window.draw(gameOverText);
        }
    }
    void handle_input()
    {
        Event ev;
        while (window.pollEvent(ev))
        {
            if(ev.type==Event::Closed)
            {
                window.close();
            }
            if(ev.type==Event::MouseButtonPressed && !board.is_game_over())
            {
               int x = ev.mouseButton.x - offset;
               int y = ev.mouseButton.y - offset;
               if (x >= 0 && x < boardSize && y >= 0 && y < boardSize)
               {
                   int row = y / 200;
                   int col = x / 200;
                   board.make_move(row, col);
               }
            }
            if (ev.type == Event::KeyPressed && ev.key.code == Keyboard::R) 
            {
                board.reset();
                render();
            }
        }
        
    }

    void AI_move()
    {
        if(!board.is_game_over() && board.get_current_player()=='X')
        {
            string state=get_board_state_string();
            if(rl.policy_pi.count(state))
            {
                int move;
                int empty_count=count(state.begin(),state.end(),' ');
                if(empty_count==9)
                {
                    double epsilon=0.4;
                    double prob=(double)rand()/RAND_MAX;
                    if(prob<epsilon)
                    {
                        move=rand()%9;
                    }
                    else move=rl.policy_pi[state];
                }
                else move=rl.policy_pi[state];
                int row=move/3;
                int col=move%3;
                board.make_move(row,col);
            }
        }
    }
    void run()
    {
        while (window.isOpen())
        {
            handle_input();
            AI_move();
            render();
        }
        
    }
};

int main()
{
    Game game;
    game.run();
    return 0;
}
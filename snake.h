#pragma once
#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <fstream>
#include <map>
#include <chrono>
#include <thread>

#ifdef _WIN32
    #include <conio.h>   // Windows input
#else
    #include <termios.h> // Linux/macOS input
    #include <unistd.h>
#endif

using namespace std;

const int BOARD_SIZE = 10;
const int MAX_SCORES = 10;
const int INITIAL_SPEED = 500;

enum Direction { UP, DOWN, LEFT, RIGHT };

class ScoreManager {
    vector<int> top_scores;
public:
    void load() {
        ifstream infile("top_scores.txt");
        int s;
        top_scores.clear();
        while (infile >> s) top_scores.push_back(s);
        infile.close();
        sort(top_scores.rbegin(), top_scores.rend());
        if (top_scores.size() > MAX_SCORES) top_scores.resize(MAX_SCORES);
    }

    void save() {
        ofstream outfile("top_scores.txt");
        for (int s : top_scores) outfile << s << endl;
        outfile.close();
    }

    void add_score(int score) {
        top_scores.push_back(score);
        sort(top_scores.rbegin(), top_scores.rend());
        if (top_scores.size() > MAX_SCORES) top_scores.resize(MAX_SCORES);
    }

    vector<int> get_scores() const { return top_scores; }
};

class SnakeGame {
    deque<pair<int,int>> snake;
    pair<int,int> food, poison, head;
    Direction dir = RIGHT;
    bool paused = false;
    int score = 0, food_eaten = 0, speed = INITIAL_SPEED, poison_timer = 0;

public:
    SnakeGame() {
        snake.push_back({0,0});
        head = {0,1};
        food = generate_food();
        poison = generate_poison();
    }

    void set_direction(Direction d) { dir = d; }
    void toggle_pause() { paused = !paused; }
    bool is_paused() const { return paused; }
    int get_score() const { return score; }
    deque<pair<int,int>> get_snake() const { return snake; }
    pair<int,int> get_food() const { return food; }
    pair<int,int> get_poison() const { return poison; }

    pair<int,int> next_head() {
        switch(dir) {
            case RIGHT: return {head.first, (head.second+1)%BOARD_SIZE};
            case LEFT:  return {head.first, head.second==0? BOARD_SIZE-1 : head.second-1};
            case DOWN:  return {(head.first+1)%BOARD_SIZE, head.second};
            case UP:    return {head.first==0? BOARD_SIZE-1 : head.first-1, head.second};
        }
        return head;
    }

    bool step() {
        if (paused) return true;
        head = next_head();

        // collision with self
        if (find(snake.begin(), snake.end(), head) != snake.end()) return false;
        // collision with poison
        if (head == poison) return false;

        if (head == food) {
            snake.push_back(head);
            score += 10;
            food_eaten++;
            food = generate_food();
            if (food_eaten % 10 == 0 && speed > 100) speed -= 50;
        } else {
            snake.push_back(head);
            snake.pop_front();
        }

        poison_timer += speed;
        if (poison_timer >= 5000) {
            poison = generate_poison();
            poison_timer = 0;
        }
        return true;
    }

private:
    pair<int,int> generate_food() {
        pair<int,int> f;
        do { f = {rand()%BOARD_SIZE, rand()%BOARD_SIZE}; }
        while (find(snake.begin(), snake.end(), f) != snake.end());
        return f;
    }

    pair<int,int> generate_poison() {
        pair<int,int> p;
        do { p = {rand()%BOARD_SIZE, rand()%BOARD_SIZE}; }
        while (find(snake.begin(), snake.end(), p) != snake.end() || p==food);
        return p;
    }
};

class Renderer {
public:
    static void render(const SnakeGame& game) {
        auto snake = game.get_snake();
        auto food = game.get_food();
        auto poison = game.get_poison();

        for (int i=0;i<BOARD_SIZE;i++) {
            for (int j=0;j<BOARD_SIZE;j++) {
                if (make_pair(i,j)==food) cout << "F";
                else if (make_pair(i,j)==poison) cout << "X";
                else if (find(snake.begin(),snake.end(),make_pair(i,j))!=snake.end()) cout << "O";
                else cout << ".";
            }
            cout << endl;
        }
    }
};

class InputHandler {
public:
    static void run(SnakeGame &game) {
#ifdef _WIN32
        while (true) {
            if (_kbhit()) {
                char input = _getch();
                if (input=='d') game.set_direction(RIGHT);
                else if (input=='a') game.set_direction(LEFT);
                else if (input=='w') game.set_direction(UP);
                else if (input=='s') game.set_direction(DOWN);
                else if (input=='p') game.toggle_pause();
                else if (input=='q') exit(0);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
#else
        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        map<char,Direction> keymap = {{'d',RIGHT},{'a',LEFT},{'w',UP},{'s',DOWN}};
        while(true){
            char input = getchar();
            if (keymap.count(input)) game.set_direction(keymap[input]);
            else if (input=='p') game.toggle_pause();
            else if (input=='q') exit(0);
        }
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
    }
};

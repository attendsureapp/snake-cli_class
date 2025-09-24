#include "snake.h"
#include <thread>
#include <chrono>

int main() {
    SnakeGame game;
    ScoreManager scores;

    // Thread for input
    std::thread input_thread([&](){ 
        InputHandler::run(game); 
    });

    // Thread for game loop
    std::thread game_thread([&](){
        while (game.step()) {
#ifdef _WIN32
            system("cls");   
#else
            system("clear"); 
#endif
            Renderer::render(game);
            std::cout << "Score: " << game.get_score() << std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds(INITIAL_SPEED));
        }
        std::cout << "Game Over! Final score: " << game.get_score() << std::endl;
    });

    input_thread.join();
    game_thread.join();
    return 0;
}

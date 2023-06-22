#include <ctime>
#include <filesystem>
#include "Game.h"
#include "GlobalSettings.h"

int main(int argc, char *argv[])
{
    srand(static_cast<unsigned int>(time(nullptr)));

    auto path = std::filesystem::current_path();
    std::filesystem::current_path(path / "resources");
    
    Game pGame{Window{ GlobalSettings::WINDOW_TITLE, GlobalSettings::WINDOW_WIDTH , GlobalSettings::WINDOW_HEIGHT,GlobalSettings::USE_VSYNC}};
    pGame.Run();

    return 0;
}

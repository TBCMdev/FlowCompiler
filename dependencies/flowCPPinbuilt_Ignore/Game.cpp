#include <string>
#include <thread>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>


#include "C:\Flow\bin\ERROR_SIMPLE.cpp"
#include "C:\Flow\bin\console.cpp"
#include "C:\Flow\bin\file.cpp"
#include "C:\Flow\bin\inbuilt.cpp"
#include "C:\Flow\bin\Game.h"

using namespace std;

using namespace sf;

using namespace Game;
//g++ -c Game.cpp -I"C:\Flow\SFML-2.5.1\include" -DSFML_STATIC
//g++ Game.o -o test_build/sfml-app -L"C:\SFML-2.5.1\lib" -lsfml-graphics-s -lsfml-window-s -lsfml-system-s 

int main(){
    game g("my new game!",800,800);
    g.Start();
    Player p("./assets/mario.png",{},DefaultMoveKeys,true);

}
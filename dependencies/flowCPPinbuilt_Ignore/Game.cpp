#include <string>
#include <thread>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

using namespace std;

using namespace sf;


class Game
{
    Window window;
    int width, height;
    string title;


public:
    void main()
    {
        //window.create(VideoMode(width,height),title);
        while (window.isOpen())
        {
            //window.clear(Color::Red);

            if (Keyboard::isKeyPressed(Keyboard::Key::Escape))
                break;

            window.display();
        }
    }

public:
    void Start()
    {
        main();// do multi threading
    }
    Game(string title = "", int w = 800, int h = 800)
    {
        this->title = title;
        width = w;
        height = h;
        RenderWindow wi(VideoMode(width,height),title);
        //w.create(VideoMode(width,height),title);
    }
    void refreshRate(int f)
    {
        window.setFramerateLimit(f);
    }


};

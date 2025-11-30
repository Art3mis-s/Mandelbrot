#include <SFML/Graphics.hpp>
#include <iostream>
#include "ComplexPlane.h"

using namespace sf;
using namespace std;


int main()
{
    int pixelWidth = VideoMode::getDesktopMode().width / 2;
    int pixelHeight = VideoMode::getDesktopMode().height / 2;
    VideoMode vm(pixelWidth, pixelHeight);
    RenderWindow window(vm, "Rainbow Screen", Style::Default);
    ComplexPlane plane(pixelWidth, pixelHeight);

    sf::Font font;
    if (!font.loadFromFile("Times.ttc")) {
        std::cerr << "Warning: failed to load Times.ttc. On-screen text will not appear.\n";
    }
    sf::Text infoText;
    infoText.setFont(font);
    infoText.setCharacterSize(18);
    infoText.setFillColor(sf::Color::White);
    infoText.setPosition(10.f, 10.f);

    
    while(window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            // Input
            if (event.type == Event::Closed)
            {
                window.close();
            }
            else if (event.type == Event::MouseButtonPressed)
            {
                Vector2i pixel(event.mouseButton.x, event.mouseButton.y);

                if (event.mouseButton.button == Mouse::Left)
                {
                    plane.setCenter(pixel);
                    plane.zoomIn();
                }
                else if (event.mouseButton.button == Mouse::Right)
                {
                    plane.setCenter(pixel);
                    plane.zoomIn();
                }
            }

            else if (event.type == Event::MouseMoved)
            {
                Vector2i pixel(event.mouseMove.x, event.mouseMove.y);
                plane.setMouseLocation(pixel);
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Escape))
        {
            window.close();
        }

        plane.updateRender();
        plane.loadText(infoText);

        window.clear();
        window.draw(plane);
        window.draw(infoText);
        window.display();
    }
    return 0;
}

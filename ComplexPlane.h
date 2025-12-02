#include <iostream>
#include <complex>
#include <thread>
#include <SFML/Graphics.hpp>


using namespace sf;
using namespace std;

const unsigned int MAX_ITER = 64;
const float BASE_WIDTH = 4.0;
const float BASE_HEIGHT = 4.0;
const float BASE_ZOOM = 0.5;

enum class State
{
    CALCULATING, 
    DISPLAYING
};

// Drawable class is from SFML library 
class ComplexPlane : public Drawable 
{
public:
    ComplexPlane(int pixelWidth, int pixelHeight);
    void draw(RenderTarget& target, RenderStates states) const override;
    void updateRender();
    void zoomIn();
    void zoomOut();
    void setCenter(Vector2i mousePixel);
    void setMouseLocation(Vector2i mousePixel);
    void loadText(Text& text);
private: 
    State m_state;             // Current state: CALCULATING or DISPLAYING
    VertexArray m_vArray;      // The array of points (pixels) to be drawn 
    Vector2f m_mouseLocation;  // Complex coordinate corresponding to the mouse
    Vector2i m_pixel_size;     
    Vector2f m_plane_center;   // Center of the complex plane view
    Vector2f m_plane_size;     // Size (width/height) of the complex plane view
    int m_zoomCount;           // Tracks how many times the user has zoomed
    float m_aspectRatio;       // Aspect ratio (height / width)
    size_t countIterations(Vector2f coord);
    void iterationsToRGB(size_t count, Uint8& r, Uint8& g, Uint8& b);
    Vector2f mapPixelToCoords(Vector2i mousePixel);
    void computeRows(int startRow, int endRow);
};




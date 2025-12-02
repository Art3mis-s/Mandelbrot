#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <sstream>
#include <complex>
#include <thread>
#include "ComplexPlane.h"
using namespace std;
using namespace sf;

ComplexPlane::ComplexPlane(int pixelWidth, int pixelHeight)
{
    // m_pixel_size : { pixelWidth, pixelHeight }
    m_pixel_size.x = pixelWidth;
    m_pixel_size.y = pixelHeight;
    m_aspectRatio = static_cast<float>(m_pixel_size.y) / m_pixel_size.x;
    m_plane_center = {0.0f, 0.0f};
    m_plane_size = { BASE_WIDTH, BASE_HEIGHT * m_aspectRatio };
    m_zoomCount = 0;
    m_state = State::CALCULATING;
    m_vArray.setPrimitiveType(sf::Points);
    m_vArray.resize(static_cast<size_t>(pixelWidth) * static_cast<size_t>(pixelHeight));
}

void ComplexPlane::draw(RenderTarget& target, RenderStates states) const
{
    target.draw(m_vArray);
}
// Updated it for multithreading 
void ComplexPlane::computeRows(int startRow, int endRow)
{
    for (int j = 0; j < m_pixel_size.x; j++)
        {
            for (int i = 0; i < m_pixel_size.y; i++)
            {
                // for example: j = 2, x = 1 then 
                // vArray[6].position = (2.0, 1.0)
                int index = j + i * m_pixel_size.x;
                m_vArray[index].position = { (float)j,(float)i };
                // Whatever we call it with, the output is something like 
                // (real_value, imaginary_value)
                Vector2f coord =  mapPixelToCoords(Vector2i(j,i));
                size_t iteration = countIterations(coord);

                Uint8 r,g,b;
                iterationsToRGB(iteration, r, g, b);
                m_vArray[index].color = Color(r,g,b);

            }
        }
}
// Updated it for multithreading 
void ComplexPlane::updateRender()
{
    if (m_state == State::CALCULATING)
    {
        int totalRows = m_pixel_size.y;

        // 4 threads splitting the image vertically
        std::thread t1(&ComplexPlane::computeRows, this, 0, totalRows / 4);
        std::thread t2(&ComplexPlane::computeRows, this, totalRows / 4, totalRows / 2);
        std::thread t3(&ComplexPlane::computeRows, this, totalRows / 2, 3 * totalRows / 4);
        std::thread t4(&ComplexPlane::computeRows, this, 3 * totalRows / 4, totalRows);

        // Wait for all 4 threads to finish
        t1.join();
        t2.join();
        t3.join();
        t4.join();

        m_state = State::DISPLAYING;
    }
}

void ComplexPlane::zoomIn()
{
    m_zoomCount++;
    m_plane_size.x = BASE_WIDTH * (pow(BASE_ZOOM, m_zoomCount));
    m_plane_size.y = BASE_HEIGHT * m_aspectRatio * (pow(BASE_ZOOM, m_zoomCount));
    m_state = State::CALCULATING;
}
void ComplexPlane::zoomOut()
{
    m_zoomCount--;
    m_plane_size.x = BASE_WIDTH * (pow(BASE_ZOOM, m_zoomCount));
    m_plane_size.y = BASE_HEIGHT * m_aspectRatio * (pow(BASE_ZOOM, m_zoomCount));
    m_state = State::CALCULATING;
}
void ComplexPlane::setCenter(Vector2i mousePixel)
{
    m_plane_center = mapPixelToCoords(mousePixel);
    m_state = State::CALCULATING;
}
void ComplexPlane::setMouseLocation(Vector2i mousePixel)
{
    m_mouseLocation = mapPixelToCoords(mousePixel);
}
void ComplexPlane::loadText(Text& text)
{
    stringstream ss;

    ss << "Mandelbrot Set\n";
    ss << "Center: (" << m_plane_center.x << ", " << m_plane_center.y << ")\n";
    ss << "Cursor: (" << m_mouseLocation.x << ", " << m_mouseLocation.y << ")\n";
    ss << "Left-click to Zoom in\n";
    ss << "Right-click to Zoom out";

    // everything that’s inside the stringstream as one string
    text.setString(ss.str());
}
// Tracks how many times the user has zoomed
size_t ComplexPlane::countIterations(Vector2f coord)
{
    size_t counter = 0;
    // Turn coord into a complex number
    complex<double>c( coord.x, coord.y );
    // Start z = c
    complex<double>z = c;

    while (counter < MAX_ITER)
    {
        // |z|(Magnitude) means how far is z from the center 
        if (std::abs(z) > 2)
        {
            return counter;
        }
        z = (z*z)+ c; 
        counter++;
    }
    return counter;
} 
void ComplexPlane::iterationsToRGB(size_t count, Uint8& r, Uint8& g, Uint8& b)
{
    if (count == MAX_ITER) { r = g = b = 0; return; }

    float t = static_cast<float>(count) / MAX_ITER;

    r = static_cast<Uint8>(30  + 200 * pow(t, 3));      // soft purple → pink
    g = static_cast<Uint8>(20  + 255 * sqrt(t));        // green glow
    b = static_cast<Uint8>(100 + 155 * t);              // deep blue
}
Vector2f ComplexPlane::mapPixelToCoords(Vector2i mousePixel)
{

    // figure out the left, right, bottom, top edges of the complex plane
    // 1. Compute complex-plane boundaries
    float realMin = m_plane_center.x - (m_plane_size.x / 2.0f);
    float realMax = m_plane_center.x + (m_plane_size.x / 2.0f);

    float imagMin = m_plane_center.y - (m_plane_size.y / 2.0f);
    float imagMax = m_plane_center.y + (m_plane_size.y / 2.0f);

    // 2. Map pixel X into real axis range: [0..pixelWidth] → [realMin..realMax]
    float realVal =
        ((mousePixel.x - 0.0f) / (m_pixel_size.x - 0.0f)) * (realMax - realMin) + realMin;

    // 3. Map pixel Y into imaginary axis range (flip vertically!)
    float imagVal =
        ((mousePixel.y - m_pixel_size.y) / (0.0f - m_pixel_size.y)) * (imagMax - imagMin) + imagMin;

    // 4. Return final complex coordinate
    return Vector2f(realVal, imagVal);
}

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <chrono>



///add rotation
//make function to tell if a point is in veiwing area

//define structures
struct Point2D { double x, y; };
struct Point3D { double x, y, z; };
struct Size {double width, height, length;};
struct Line { int start, end; };
struct rotation_angle { float x, y, z; };



double playerX = 0.0;
double playerY = 0.0;
double playerZ = 0.0;
double playerUPS = 1.0;
double camerRPS = 10;
float fov = 10;
float size = 50;

int window_height = 1000;
int window_width = 1200;

sf::RenderWindow window(sf::VideoMode(window_width, window_height), "SFML Canvas");


//turn 3c point into 3d point (change later)
Point2D project( Point3D point ) {
    Point2D p;
    p.x = ((point.x * fov) / (point.z + fov)) * size + window_width/2 ;
    p.y = (((point.y * fov) / (point.z + fov)) * size - window_height/2) * -1;
    return p;

}

//shift point 
Point3D shift( Point3D point, Point3D shift) {
    Point3D p;
    p.x = point.x + shift.x;
    p.y = point.y + shift.y;
    p.z = point.z + shift.z;
    return p;
}

//rotate 3D point around aribtrary axis
Point3D rotate( Point3D point, Point3D axis, Point3D axisDirection, double theta) {
    Point3D p;
    double x = point.x;
    double y = point.y;
    double z = point.z;
    double a = axis.x;
    double b = axis.y;
    double c = axis.z;
    double u = axisDirection.x;
    double v = axisDirection.y;
    double w = axisDirection.z;
    p.x = ((a * (v*v + w*w) - u * ( b * v + c * w - u * x - u * y - w * z)) * (1 - cos(theta)) + x * cos(theta) + (-1 * c*v + b*w - w*y + v *z) * sin(theta));
    p.y = ((b * (u*u + w*w) - v * (a * u + c * w - u * x - u * y - w * z)) * (1 - cos(theta)) + y * cos(theta) + ( c*u - a*w + w*x - u*z) * sin(theta));
    p.z = ((c * (u*u + v*v) - w * (a * u + b * v - u * x - u * y - w * z)) * (1 - cos(theta)) + z * cos(theta) + ( -1*b*u + a*v - v*x + u*y) * sin(theta));
    return p;
}


//make object class
class object3D {
    private: std::vector<Point3D> BluePrint;
    private: std::vector<Line> lines;
    private: std::vector<Point3D> Shifted_BluePrint;
    private: std::vector<Point3D> vertices;
    private: Size object_size = {2.0, 2.0, 2.0};
    private: Point3D position;


    public: void set_size(Size x) {
        object_size = x;
    }

    public: void set_position(Point3D point) {
        position = point;
    }

    //makes this 3D object a square
    public: void makesquare() {
        double height = object_size.height;
        double width = object_size.width;
        double length = object_size.length;
        BluePrint = { Point3D{ -1 * width/2, -1 * height/2, -1 * length/2 }, Point3D{ -1 * width/2, 1  * height/2, -1 * length/2 }, Point3D{ 1 * width/2, -1  * height/2, -1 * length/2 }, Point3D{ 1 * width/2, 1  * height/2, -1 * length/2 }, Point3D{ -1 * width/2, -1 * height/2, 1 * length/2 }, Point3D{ -1 * width/2, 1 * height/2, 1 * length/2 }, Point3D{ 1 * width/2, -1 * height/2, 1 * length/2 }, Point3D {1 * width/2, 1 * height/2, 1 * length/2 } };
        lines = { Line{ 1, 3 }, Line{ 1, 5 }, Line{ 1, 2 }, Line{ 2, 6 }, Line{ 2, 4 }, Line{ 4, 8 }, Line{ 4, 3 }, Line{ 3, 7 }, Line{ 7, 5 }, Line{ 7, 8 }, Line{ 6, 8 }, Line{ 6, 5 } };
        
        const int vecSize = BluePrint.size();
        Shifted_BluePrint = BluePrint;
        for (int i = 0; i < vecSize; i++) {
            Point3D s = {position.x - playerX, position.y - playerY, position.z - playerZ};
            Shifted_BluePrint[i] = shift(BluePrint[i], s);
        }

        vertices = Shifted_BluePrint;


    }




    public: void translate(Point3D axis, Point3D axisDirection, double theta) {
        const int vecSize = vertices.size();
        for (int i = 0; i < vecSize; i++) {
            vertices[i] = rotate(vertices[i], axis, axisDirection, theta);
        }

    }



    //Draws the 3D object
    public: void draw() {
        const int vecSize = lines.size();
        for( int i = 0; i < vecSize; i++) {
            float start = lines[i].start - 1;
            float end = lines[i].end - 1;

            


            //project points
            Point2D projected_point1 = project(vertices[start]);
            Point2D projected_point2 = project(vertices[end]);

            sf::Vertex line[] = {sf::Vertex(sf::Vector2f(projected_point1.x, projected_point1.y)), sf::Vertex(sf::Vector2f(projected_point2.x, projected_point2.y))};
            window.draw(line, 2, sf::Lines);
        }

    }
};





int main() {

    //start clock
    auto time1 = std::chrono::high_resolution_clock::now();    

   object3D cube;
   Point3D p = {0.0, 0.0, 0.0};
   cube.set_position(p);
   cube.makesquare();


   //temporary
   object3D player;
   Size player_size = {1, 1, 1};
   player.set_size(player_size);
   Point3D dodo = {playerX, playerY, playerZ};
   player.set_position(dodo);
   player.makesquare();
   


    // Main loop that continues until the window is closed
    while (window.isOpen()) {
        // Process events
        sf::Event event;
        while (window.pollEvent(event)) {
            // Close window: exit
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Clear the window with a black color
        window.clear(sf::Color::Black);

        //temporary
        Point3D player_location= {playerX, playerY, playerZ};
        player.set_position(player_location);

        // Draw the shape
        cube.draw();
        player.draw();
        


        // Display the contents of the window
        window.display();


        //set delta_time to time since last loop
        auto time2 = std::chrono::high_resolution_clock::now();
        auto delta_time = time2 - time1;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            {
                playerX = playerX - playerUPS * delta_time.count() / 1000000000;
            }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                playerX = playerX + playerUPS * delta_time.count() / 1000000000;
            }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            {
                playerY = playerY + playerUPS * delta_time.count() / 1000000000;
            }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            {
                playerY = playerY - playerUPS * delta_time.count() / 1000000000;
            }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            {
                playerZ = playerZ + playerUPS * delta_time.count() / 1000000000;
            }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            {
                playerZ = playerZ - playerUPS * delta_time.count() / 1000000000;
            }

        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            {
                Point3D p = {playerX, playerY, playerZ};
                Point3D d = {-1, 0, 0};
                cube.translate(p, d, camerRPS * delta_time.count() / 1000000000);
                player.translate(p, d, camerRPS * delta_time.count() / 1000000000);
            }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            {
                Point3D p = {playerX, playerY, playerZ};
                Point3D d = {1, 0, 0};
                cube.translate(p, d, camerRPS * delta_time.count() / 1000000000);
                player.translate(p, d, camerRPS * delta_time.count() / 1000000000);
            }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            {
                Point3D p = {playerX, playerY, playerZ};
                Point3D d = {0, -1, 0};
                cube.translate(p, d, camerRPS * delta_time.count() / 1000000000);
                player.translate(p, d, camerRPS * delta_time.count() / 1000000000);
            }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            {
                Point3D p = {playerX, playerY, playerZ};
                Point3D d = {0, -1, 0};
                cube.translate(p, d, camerRPS * delta_time.count() / 1000000000);
                player.translate(p, d, camerRPS * delta_time.count() / 1000000000);
            }
        







        time1 = time2;


    }

    return 0;
}
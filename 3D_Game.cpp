#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <chrono>

//Define Structures
struct Point2D { double x, y; }; 
struct Point3D { double x, y, z; };
struct Line { int start, end; }; 

double playerX = 0.0; 
double playerY = 0.0;
double playerZ = 0.0;
double playerUPS = 4.0; //Player move speed in units per second
double camerDPS = 2.5; //Camera rotation speed in degrees per second
float fov = 10; //artificial distance of the user from the screen
float size = 50; //Pixel size of a unit

const int window_height = 1000; //in pixels
const int window_width = 1200; //in pixels

sf::RenderWindow window(sf::VideoMode(window_width, window_height), "SFML Canvas"); //creates the window

//Turn 2D point into 3D point
Point2D project( Point3D point ) {
    Point2D p;
    p.x = ((point.x * fov) / (point.z + fov)) * size + window_width/2 ;
    p.y = (((point.y * fov) / (point.z + fov)) * size - window_height/2) * -1;
    return p;

}

//Shift a 3D point by another 3D point
Point3D shift( Point3D point, Point3D shift) {
    Point3D p;
    p.x = point.x + shift.x;
    p.y = point.y + shift.y;
    p.z = point.z + shift.z;
    return p;
}

//Rotates a 3D point (point) around an axis that goes through a point (axis) and has a direction(axis Direction) by theta degrees
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

//Class containing all 3D objects
class Object3D {
    private: std::vector<Point3D> BluePrint; //blueprint for the object
    private: std::vector<Point3D> vertices; //vertices that have been rotated and shifted
    private: std::vector<Line> lines;
    private: Point3D position = {0.0, 0.0, 0.0}; //3D coridinates of the objects position
    private: Point3D object_size = {2.0, 2.0, 2.0}; //Objects horizontal, vertical, and depth

    //sets the objects size
    public: void set_size(Point3D x) {
        object_size = x;
    }

    //sets the objects position
    public: void set_position(Point3D point) {
        position = point;
    }

    //makes the objects a square
     public: void makesquare() {
        double height = object_size.y;
        double width = object_size.x;
        double length = object_size.z;

        //Set Blueprint and lines
        BluePrint = { Point3D{ -1 * width/2, -1 * height/2, -1 * length/2 }, Point3D{ -1 * width/2, 1  * height/2, -1 * length/2 }, Point3D{ 1 * width/2, -1  * height/2, -1 * length/2 }, Point3D{ 1 * width/2, 1  * height/2, -1 * length/2 }, Point3D{ -1 * width/2, -1 * height/2, 1 * length/2 }, Point3D{ -1 * width/2, 1 * height/2, 1 * length/2 }, Point3D{ 1 * width/2, -1 * height/2, 1 * length/2 }, Point3D {1 * width/2, 1 * height/2, 1 * length/2 } };
        lines = { Line{ 1, 3 }, Line{ 1, 5 }, Line{ 1, 2 }, Line{ 2, 6 }, Line{ 2, 4 }, Line{ 4, 8 }, Line{ 4, 3 }, Line{ 3, 7 }, Line{ 7, 5 }, Line{ 7, 8 }, Line{ 6, 8 }, Line{ 6, 5 } };

        //Set initial shift
        const int vecSize = BluePrint.size();
        vertices = BluePrint;
        for (int i = 0; i < vecSize; i++) {
            Point3D s = {position.x, position.y, position.z};
            vertices[i] = shift(BluePrint[i], s);
        }
     }

     //makes the objects a grid
     public: void makegrid() {
        double width = object_size.x;
        double length = object_size.z;

        //Horizontal lines
        for (int i = 0; i < length; i+=2) {
            Point3D p = {(width -1 )/2 * -1, 0, (length -1)/2*-1 + i};
            Point3D d = {(width -1)/2, 0, (length -1)/2*-1 + i};
            BluePrint.push_back(p);
            int l = BluePrint.size();
            BluePrint.push_back(d);
            int y = BluePrint.size();
            lines.push_back({l, y});
         }

         //Horizontal lines
        for (int i = 0; i < width; i+=2) {
            Point3D p = {(width -1 )/2 * -1 + i, 0, (length -1)/2*-1};
            Point3D d = {(width -1)/2 * -1 + i, 0, (length -1)/2};
            BluePrint.push_back(p);
            int l = BluePrint.size();
            BluePrint.push_back(d);
            int y = BluePrint.size();
            lines.push_back(Line{l, y});
         }


        //Set initial shift
        const int vecSize = BluePrint.size();
        vertices = BluePrint;
        for (int i = 0; i < vecSize; i++) {
            Point3D s = {position.x, position.y, position.z};
            vertices[i] = shift(BluePrint[i], s);
        }
     }

    //Shifts Object by p
    public: void shiftObject (Point3D p) {

        const int vecSize = vertices.size();
        for (int i = 0; i < vecSize; i++) {
            vertices[i] = shift(vertices[i], p);
        }
    }

    //Rotates Object
    public: void rotateObject(Point3D axis, Point3D axisDirection, double theta) {
        const int vecSize = vertices.size();
        for (int i = 0; i < vecSize; i++) {
            vertices[i] = rotate(vertices[i], axis, axisDirection, theta);
        }

        //draw axis thats being rotated around
        Point2D p = project({axis.x, axis.y, axis.z});
        Point2D d = project({axis.x + axisDirection.x, axis.y + axisDirection.y, axis.z + axisDirection.z});
        sf::Vertex line[] = {sf::Vertex(sf::Vector2f(p.x, p.y)), sf::Vertex(sf::Vector2f(d.x,  d.y))};
        window.draw(line, 2, sf::Lines);

    }


    //Draws the 3D object
    public: void draw() {
        const int vecSize = lines.size();
        for( int i = 0; i < vecSize; i++) {
            float start = lines[i].start - 1;
            float end = lines[i].end - 1;

            //check if both vertices are in view
            if ((vertices[start].z >= fov*-1) && (vertices[end].z >= fov*-1)) {
                 //project points
                Point2D projected_point1 = project(vertices[start]);
                Point2D projected_point2 = project(vertices[end]);

                sf::Vertex line[] = {sf::Vertex(sf::Vector2f(projected_point1.x, projected_point1.y)), sf::Vertex(sf::Vector2f(projected_point2.x, projected_point2.y))};
                window.draw(line, 2, sf::Lines);

            }
            //check if first vertices is out of view
            else if ((vertices[start].z < fov*-1) && !(vertices[end].z < fov*-1)) {
                double z = - 1 * fov + 0.0001;
                double x =  ((z - vertices[start].z) / (vertices[end].z - vertices[start].z)) * (vertices[end].x - vertices[start].x) + vertices[start].x;
                double y =  ((z - vertices[start].z) / (vertices[end].z - vertices[start].z)) * (vertices[end].y - vertices[start].y) + vertices[start].y;
                Point2D projected_point1 = project({x, y, z});
                Point2D projected_point2 = project(vertices[end]);

                sf::Vertex line[] = {sf::Vertex(sf::Vector2f(projected_point1.x, projected_point1.y)), sf::Vertex(sf::Vector2f(projected_point2.x, projected_point2.y))};
                window.draw(line, 2, sf::Lines);


            }
            //check if second vertice is out of view
            else if ((vertices[end].z < fov*-1) && !(vertices[start].z < fov*-1)) {
                double z = fov * -1 + 0.00001;
                double x =  ((z - vertices[start].z) / (vertices[end].z - vertices[start].z)) * (vertices[end].x - vertices[start].x) + vertices[start].x;
                double y =  ((z - vertices[start].z) / (vertices[end].z - vertices[start].z)) * (vertices[end].y - vertices[start].y) + vertices[start].y;
                Point2D projected_point1 = project(vertices[start]);
                Point2D projected_point2 = project({x, y, z});

                sf::Vertex line[] = {sf::Vertex(sf::Vector2f(projected_point1.x, projected_point1.y)), sf::Vertex(sf::Vector2f(projected_point2.x, projected_point2.y))};
                window.draw(line, 2, sf::Lines);


            }

           
        }
    }

};

std::vector<Object3D *> objects;

//Main Function
int main() {

    //start clock
    auto time1 = std::chrono::high_resolution_clock::now();

    //makes cube object
    Object3D cube;
    Point3D p = {3.0, 0.0, 0.0};
    cube.set_position(p);
    cube.set_size({1, 2.0, 3.0});
    cube.makesquare();
    objects.push_back(&cube);

    //temporary player
    Object3D player;
    player.set_position({0.0, 0.0, 0.0});
    cube.set_size({1, 1, 1});
    player.makesquare();
    objects.push_back(&player);

    //Grid object
    Object3D grid;
    grid.set_size({20, 0, 20});
    grid.set_position({0, -1.0, 0});
    grid.makegrid();
    objects.push_back(&grid);
    

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

        //Draw cube
        const int vecSize = objects.size();
                for (int i=0; i < vecSize; i++) {
                    objects[i]->draw();

                }


        

        //set delta_time to time since last loop
        auto time2 = std::chrono::high_resolution_clock::now();
        auto delta_time = time2 - time1;


        //Move player
        Point3D objectShift = {0.0, 0.0, 0.0};
        //Move Left and Right
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            {
                playerX = playerX - playerUPS * delta_time.count() / 1000000000;
                objectShift.x += playerUPS * delta_time.count() / 1000000000;

            }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                playerX = playerX + playerUPS * delta_time.count() / 1000000000;
                objectShift.x -= playerUPS * delta_time.count() / 1000000000;
            }

        //Move Up and Down
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            {
                playerY = playerY + playerUPS * delta_time.count() / 1000000000;
                objectShift.y -= playerUPS * delta_time.count() / 1000000000;
            }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            {
                playerY = playerY - playerUPS * delta_time.count() / 1000000000;
                objectShift.y += playerUPS * delta_time.count() / 1000000000;
            }

        //Move Forward and Backward
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            {
                playerZ = playerZ + playerUPS * delta_time.count() / 1000000000;
                objectShift.z -= playerUPS * delta_time.count() / 1000000000;
            }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            {
                playerZ = playerZ - playerUPS * delta_time.count() / 1000000000;
                objectShift.z += playerUPS * delta_time.count() / 1000000000;
            }

        

        //Rotate Camera around player
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            {
                Point3D p = {0, 0, 0};
                Point3D d = {-1, 0, 0};
                const int vecSize = objects.size();
                for (int i=0; i < vecSize; i++) {
                    objects[i]->rotateObject(p, d, camerDPS * delta_time.count() / 1000000000);

                }
            }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            {
                Point3D p = {0, 0, 0};
                Point3D d = {1, 0, 0};
                const int vecSize = objects.size();
                for (int i=0; i < vecSize; i++) {
                    objects[i]->rotateObject(p, d, camerDPS * delta_time.count() / 1000000000);

                }
            }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            {
                Point3D p = {0, 0, 0};
                Point3D d = {0, -1, 0};
                const int vecSize = objects.size();
                for (int i=0; i < vecSize; i++) {
                    objects[i]->rotateObject(p, d, camerDPS * delta_time.count() / 1000000000);

                }
            }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            {
                Point3D p = {0, 0, 0};
                Point3D d = {0, 1, 0};
                const int vecSize = objects.size();
                for (int i=0; i < vecSize; i++) {
                    objects[i]->rotateObject(p, d, camerDPS * delta_time.count() / 1000000000);

                };
            }

        //Reset Clock
        time1 = time2;

        //Shift objects
        cube.shiftObject(objectShift);
        grid.shiftObject((objectShift));
        objectShift = {0.0, 0.0, 0.0};


        // Display the contents of the window
        window.display();
    }

    return 0;
}


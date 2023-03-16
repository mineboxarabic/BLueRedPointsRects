#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>

using namespace std;

struct Point {
    int x;
    int y;
    bool isBlue;

    Point() {
        this->x = 0;
        this->y = 0;
        this->isBlue = false;
    }

    Point(int x, int y) {
        this->x = x;
        this->y = y;
        this->isBlue = false;
    }
};

struct Rectangle {
    Point topLeft;
    Point bottomRight;
    bool isValidRectangle;
    Rectangle() {
        this->topLeft = Point(0,0);
        this->bottomRight = Point(0,0);
        this->isValidRectangle = false;
    }
    Rectangle(Point topLeft, Point bottomRight) {
        if(topLeft.x > bottomRight.x || topLeft.y > bottomRight.y)
        {
            this->isValidRectangle = false;
        }
        else {
            this->topLeft = topLeft;
            this->bottomRight = bottomRight;
            this->isValidRectangle = true;
        }
    }

    bool contains(Point p) {
        return p.x >= this->topLeft.x && p.x <= this->bottomRight.x && p.y >= this->topLeft.y && p.y <= this->bottomRight.y;
    }

    bool intersects(Rectangle r) {
        return !(r.topLeft.x > this->bottomRight.x || r.bottomRight.x < this->topLeft.x || r.topLeft.y > this->bottomRight.y || r.bottomRight.y < this->topLeft.y);
    }

    bool disjoint(Rectangle r) {
        return r.topLeft.x > this->bottomRight.x || r.bottomRight.x < this->topLeft.x || r.topLeft.y > this->bottomRight.y || r.bottomRight.y < this->topLeft.y;
    }

    bool contains(Rectangle r) {
        return this->contains(r.topLeft) && this->contains(r.bottomRight);
    }
};


struct Node {
    Rectangle box;
    int nPoints;
    Point point;
    Node *left;
    Node *right;

    Node(){
        this->box = Rectangle(Point(0,0), Point(0,0));
        this->nPoints = 0;
        this->point = Point();
        this->left = nullptr;
        this->right = nullptr;
    }
    Node(Rectangle box, int nPoints, Point point, Node *left, Node *right) {
        this->box = box;
        this->nPoints = nPoints;
        this->point = point;
        this->left = left;
        this->right = right;
    }
    bool isLeaf() {
        return this->left == nullptr && this->right == nullptr;
    }

    int query(Rectangle r) {
        if(this->isLeaf()) {
            return r.contains(this->point) ? 1: 0;
        }
        if(r.contains(this->box)) {
            return this->nPoints;   
        }
        if(r.disjoint(this->box)) {
            return 0;
        }
        else {
            return this->left->query(r) + this->right->query(r);
        }
    }
};

int maxBlueMinusRed(vector<Point>& red, vector<Point>& blue) {
    int maxDiff = 0;
    std::vector<Point> points = red;

    for(Point p : blue)
    {
        points.push_back(p);
    }

    //sort points by x
    std::sort(points.begin(), points.end(), [](Point a, Point b) { return a.x < b.x; });

    for (int i = 0; i < points.size(); i++){
        for (int j = i; j < points.size(); j++){
            Rectangle r = Rectangle(points[i], points[j]);
            if (!r.isValidRectangle){
                continue;
            }
            int blueCount = 0;
            int redCount = 0;

            for (int k = 0; k < blue.size(); k++){
                if (blue[k].x >= r.topLeft.x && blue[k].x <= r.bottomRight.x && blue[k].y >= r.topLeft.y && blue[k].y <= r.bottomRight.y){
                    blueCount++;
                }
            }

            for (int l = 0; l < red.size(); l++){
                if (red[l].x >= r.topLeft.x && red[l].x <= r.bottomRight.x && red[l].y >= r.topLeft.y && red[l].y <= r.bottomRight.y){
                    redCount++;
                }
            }

            int diff = blueCount - redCount;
            if (diff > maxDiff){
                maxDiff = diff;
            }



        }
    }

    return maxDiff;
}


std::vector<Point> getBluePointsFromSVG(std::string filename) {
    std::vector<Point> points;
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("blue") != std::string::npos) {
            Point p;
            p.isBlue = true;
            p.x = std::stoi(line.substr(line.find("cx=\"") + 4, line.find("\" cy") - line.find("cx=\"") - 4));
            p.y = std::stoi(line.substr(line.find("cy=\"") + 4, line.find("\" r") - line.find("cy=\"") - 4));
            points.push_back(p);
        }
    }
    return points;
}

std::vector<Point> getRedPointsFromSVG(std::string filename) {
    std::vector<Point> points;
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("red") != std::string::npos) {
            Point p;
            p.isBlue = false;
            p.x = std::stoi(line.substr(line.find("cx=\"") + 4, line.find("\" cy") - line.find("cx=\"") - 4));
            p.y = std::stoi(line.substr(line.find("cy=\"") + 4, line.find("\" r") - line.find("cy=\"") - 4));
            points.push_back(p);
        }
    }
    return points;
}


Rectangle boundingBox(std::vector<Point> points){
    int minX = points[0].x;
    int minY = points[0].y;
    int maxX = points[0].x;
    int maxY = points[0].y;

    for (int i = 1; i < points.size(); i++){
        if (points[i].x < minX){
            minX = points[i].x;
        }
        if (points[i].y < minY){
            minY = points[i].y;
        }
        if (points[i].x > maxX){
            maxX = points[i].x;
        }
        if (points[i].y > maxY){
            maxY = points[i].y;
        }
    }

    return Rectangle(Point(minX, minY), Point(maxX, maxY));
}

Node * buildTree(vector<Point>& points, bool h = false) {
    if (points.size() == 0) {
        return nullptr;
    }
    if (points.size() == 1) {
        return new Node(Rectangle(points[0], points[0]), 1, points[0], nullptr, nullptr);
    }

    if (h) {
        std::sort(points.begin(), points.end(), [](Point a, Point b) { return a.x < b.x; });
    }
    else {
        std::sort(points.begin(), points.end(), [](Point a, Point b) { return a.y < b.y; });
    }

    int mid = points.size() / 2;
    vector<Point> leftPoints;
    vector<Point> rightPoints;

    for (int i = 0; i < mid; i++) {
        leftPoints.push_back(points[i]);
    }
    for (int i = mid; i < points.size(); i++) {
        rightPoints.push_back(points[i]);
    }

    Node * left = buildTree(leftPoints, !h);
    Node * right = buildTree(rightPoints, !h);

    Rectangle box = boundingBox(points);
    int nPoints = left->nPoints + right->nPoints;
    Point point = Point();
    return new Node(box, nPoints, point, left, right);
}

int main() {
    vector<Point> red = getRedPointsFromSVG("5000.svg");
    vector<Point> blue = getBluePointsFromSVG("5000.svg");

   

    Node * bluePoints = buildTree(blue,false);
    Node * redPoints = buildTree(red,false);

    std::vector<Point> points = red;
    for(Point p : blue)
    {
        points.push_back(p);
    }
    int maxDiff = 0;
    for(Point p : points)
    {
        for(Point q : points){
            Rectangle r = Rectangle(p,q);
            if(p.x <= q.x && p.y <= q.y){
                int blueCount = bluePoints->query(r);
                int redCount = redPoints->query(r);

                int value = blueCount - redCount;
                if(value > maxDiff){
                    maxDiff = value;
                }
            }
           
        }
    }

    cout << "Number of blue points: " << blue.size() << endl;
    cout << "Number of red points: " << red.size() << endl;
    cout << "Max difference between blue and red points in a rectangle: " << maxDiff << endl;

    //cout << "Max difference between blue and red points in a rectangle: " << maxDiff << endl;

    return 0;
}
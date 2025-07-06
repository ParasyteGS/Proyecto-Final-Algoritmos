#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

const int MAX_POINTS = 1;

struct Point
{
    int x, y;
    Point(int _x, int _y) : x(_x), y(_y) {}
};

struct Boundary
{
    int x, y;
    int halfWidth, halfHeight;

    Boundary(int _x, int _y, int hw, int hh) : x(_x), y(_y), halfWidth(hw), halfHeight(hh) {}

    bool contains(const Point &p) const
    {
        return (p.x >= x - halfWidth && p.x <= x + halfWidth &&
                p.y >= y - halfHeight && p.y <= y + halfHeight);
    }
};

class QuadTree
{
public:
    Boundary boundary;
    vector<Point> points;
    bool divided;
    QuadTree *NE, *NW, *SE, *SW;
    int id;

    static int counter;

    QuadTree(Boundary b) : boundary(b), divided(false), NE(nullptr), NW(nullptr), SE(nullptr), SW(nullptr)
    {
        id = counter++;
    }

    bool insert(Point p)
    {
        if (!boundary.contains(p))
            return false;

        if (points.size() < MAX_POINTS && !divided)
        {
            points.push_back(p);
            return true;
        }

        if (!divided)
            subdivide();

        return (NE->insert(p) || NW->insert(p) || SE->insert(p) || SW->insert(p));
    }

    void subdivide()
    {
        int hw = boundary.halfWidth / 2;
        int hh = boundary.halfHeight / 2;

        NE = new QuadTree(Boundary(boundary.x + hw, boundary.y - hh, hw, hh));
        NW = new QuadTree(Boundary(boundary.x - hw, boundary.y - hh, hw, hh));
        SE = new QuadTree(Boundary(boundary.x + hw, boundary.y + hh, hw, hh));
        SW = new QuadTree(Boundary(boundary.x - hw, boundary.y + hh, hw, hh));

        for (Point &p : points)
        {
            NE->insert(p);
            NW->insert(p);
            SE->insert(p);
            SW->insert(p);
        }
        points.clear();
        divided = true;
    }

    void toDot(ofstream &out)
    {
        out << "node" << id << " [label=\"(" << boundary.x << "," << boundary.y << ")\"];\n";

        if (!divided)
        {
            for (const auto &p : points)
            {
                int pid = counter++;
                out << "node" << pid << " [label=\"P(" << p.x << "," << p.y << ")\",shape=box];\n";
                out << "node" << id << " -> node" << pid << ";\n";
            }
        }
        else
        {
            NE->toDot(out);
            NW->toDot(out);
            SE->toDot(out);
            SW->toDot(out);
            out << "node" << id << " -> node" << NE->id << ";\n";
            out << "node" << id << " -> node" << NW->id << ";\n";
            out << "node" << id << " -> node" << SE->id << ";\n";
            out << "node" << id << " -> node" << SW->id << ";\n";
        }
    }

    ~QuadTree()
    {
        delete NE;
        delete NW;
        delete SE;
        delete SW;
    }
};

int main()
{

    return 0;
}
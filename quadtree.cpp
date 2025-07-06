#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

const int MAX_POINTS = 4;

struct Point
{
    int id, x, y;
    Point(int _id, int _x, int _y) : id(_id), x(_x), y(_y) {}
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

bool intersects(const Boundary &a, const Boundary &b)
{
    return !(a.x - a.halfWidth > b.x + b.halfWidth ||
             a.x + a.halfWidth < b.x - b.halfWidth ||
             a.y - a.halfHeight > b.y + b.halfHeight ||
             a.y + a.halfHeight < b.y - b.halfHeight);
}

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
            if (NE->insert(p))
                continue;
            if (NW->insert(p))
                continue;
            if (SE->insert(p))
                continue;
            if (SW->insert(p))
                continue;

            cerr << "No se pudo insertar el punto P" << p.id << " (" << p.x << "," << p.y << ") tras subdivisión.\n";
        }

        points.clear();
        divided = true;
    }

    bool search(int x, int y) const
    {
        if (!boundary.contains(Point(0, x, y)))
            return false;

        for (const auto &p : points)
            if (p.x == x && p.y == y)
                return true;

        if (divided)
            return (NE->search(x, y) || NW->search(x, y) || SE->search(x, y) || SW->search(x, y));

        return false;
    }

    void rangeQuery(const Boundary &area, std::vector<Point> &found) const
    {
        if (!intersects(area, boundary))
            return;

        for (const auto &p : points)
            if (area.contains(p))
                found.push_back(p);

        if (divided)
        {
            NE->rangeQuery(area, found);
            NW->rangeQuery(area, found);
            SE->rangeQuery(area, found);
            SW->rangeQuery(area, found);
        }
    }

    bool remove(int id, int x, int y)
    {
        if (!boundary.contains(Point(id, x, y)))
            return false;

        for (auto it = points.begin(); it != points.end(); ++it)
        {
            if (it->id == id && it->x == x && it->y == y)
            {
                points.erase(it);
                return true;
            }
        }

        if (divided)
        {
            return (NE->remove(id, x, y) ||
                    NW->remove(id, x, y) ||
                    SE->remove(id, x, y) ||
                    SW->remove(id, x, y));
        }

        return false;
    }

    void toDot(ofstream &out)
    {
        out << "node" << id << " [label=\"Area (" << boundary.x << "," << boundary.y << ")\", shape=circle, style=filled, fillcolor=lightblue];\n";

        if (!divided)
        {
            for (const auto &p : points)
            {
                int pid = counter++;
                out << "node" << pid << " [label=\"P" << p.id << " (" << p.x << "," << p.y << ")\", shape=box, style=filled, fillcolor=lightgreen];\n";
                out << "node" << id << " -> node" << pid << ";\n";
            }
        }
        else
        {
            NE->toDot(out);
            NW->toDot(out);
            SE->toDot(out);
            SW->toDot(out);

            out << "node" << id << " -> node" << NE->id << " [label=\"NE\"];\n";
            out << "node" << id << " -> node" << NW->id << " [label=\"NW\"];\n";
            out << "node" << id << " -> node" << SE->id << " [label=\"SE\"];\n";
            out << "node" << id << " -> node" << SW->id << " [label=\"SW\"];\n";
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

int QuadTree::counter = 0;

int main()
{

    Boundary world(50, 50, 50, 50);
    QuadTree qt(world);

    vector<Point> puntos = {
        {1, 12, 2}, {2, 70, 20}, {3, 25, 50}, {4, 10, 80}, {5, 45, 25}, {6, 67, 98}, {7, 10, 11}};

    for (const auto &p : puntos)
        qt.insert(p);

    ofstream file("quadtree_proyect.dot");
    file << "digraph QuadTree {\n";
    qt.toDot(file);
    file << "}\n";
    file.close();

    cout << (qt.search(45, 25) ? "Existe" : "No existe") << "\n";

    Boundary region(50, 50, 30, 30);
    vector<Point> found;
    qt.rangeQuery(region, found);

    cout << "Puntos en la region (50,50) +/- 30:\n";
    for (const auto &p : found)
        cout << " - P" << p.id << " (" << p.x << "," << p.y << ")\n";

    if (qt.remove(5, 45, 25))
        cout << "Punto P5 eliminado\n";
    else
        cout << "Punto P5 no encontrado\n";

    ofstream file2("quadtree_proyect_2.dot");
    file2 << "digraph QuadTree {\n";
    qt.toDot(file2);
    file2 << "}\n";
    file2.close();

    return 0;
}
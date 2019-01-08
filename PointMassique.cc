#include "PointMassique.h"
#include <iostream>

using namespace std;

class Point;

PointMassique::PointMassique()
{
    x = 0;
    y = 0;
    w = 0;
}

PointMassique::PointMassique(const Point& p, double W)
{
    x = p.getX();
    y = p.getY();
    w = W;
}

PointMassique::PointMassique(double X, double Y, double W)
{
    x = X;
    y = Y;
    w = W;
}

PointMassique &PointMassique::operator+=(PointMassique const &p)
{
    if (w == 0 && p.w == 0)
    {
        x += p.x;
        y += p.y;
    }
    else if (w != 0 && p.w == 0)
    {
        x += (p.x / w);
        y += (p.y / w);
    }
    else if (w + p.w == 0)
    {
        x = (x - p.x) * w;
        y = (y - p.y) * w;
        w = 0;
    }
    else if (w + p.w != 0)
    {
        Point bar = PointMassique::barycentre(PointMassique(x, y, w), p);
        x = bar.getX();
        y = bar.getY();
        w += p.w;
    }

    return *this;
}

PointMassique &PointMassique::operator*=(double scalaire)
{

    if (scalaire != 0 && w == 0)
    {
        x *= scalaire;
        y *= scalaire;
    }
    else if(scalaire == 0 && w != 0)
    {
        x = 0;
        y = 0;
        w = 0;
    }
    else if (scalaire != 0)
        w *= scalaire;

    return *this;
}

Point PointMassique::barycentre(const PointMassique &a, const PointMassique &b)
{
    double x = 0;
    double y = 0;

    if(a.w + b.w == 0)
    {
        x = b.x - a.x;
        y = b.y - a.y;
    }
    else
    {
        x = (a.x * a.w + b.x * b.w) / (a.w + b.w);
        y = (a.y * a.w + b.y * b.w) / (a.w + b.w);
    }

    return Point(x, y);
}

void PointMassique::display() const
{
    cout << "x : " << x << " y : " << y << " w : " << w << endl;
}

PointMassique operator+(PointMassique const &p1, PointMassique const &p2)
{
    PointMassique copie(p1);
    copie += p2;

    return copie;
}

PointMassique operator*(PointMassique const &p, double scalaire)
{
    PointMassique copie(p);
    copie *= scalaire;

    return copie;
}

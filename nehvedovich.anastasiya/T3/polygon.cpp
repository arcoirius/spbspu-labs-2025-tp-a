#include "polygon.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>

namespace nehvedovich
{
  bool operator==(const Point &p1, const Point &p2)
  {
    return p1.x == p2.x && p1.y == p2.y;
  }

  std::istream &operator>>(std::istream &in, Point &dest)
  {
    std::istream::sentry sentry(in);
    if (!sentry)
    {
      return in;
    }

    char ch;
    if (!(in >> ch) || ch != '(')
    {
      in.setstate(std::ios::failbit);
      return in;
    }
    if (!(in >> dest.x))
    {
      in.setstate(std::ios::failbit);
      return in;
    }
    if (!(in >> ch) || ch != ';')
    {
      in.setstate(std::ios::failbit);
      return in;
    }
    if (!(in >> dest.y))
    {
      in.setstate(std::ios::failbit);
      return in;
    }
    if (!(in >> ch) || ch != ')')
    {
      in.setstate(std::ios::failbit);
      return in;
    }

    return in;
  }

  std::ostream &operator<<(std::ostream &out, const Point &src)
  {
    std::ostream::sentry sentry(out);
    if (!sentry)
    {
      return out;
    }
    return out << '(' << src.x << ';' << src.y << ')';
  }

  bool operator==(const Polygon &p1, const Polygon &p2)
  {
    return p1.points == p2.points;
  }

  bool operator!=(const Polygon &p1, const Polygon &p2)
  {
    return !(p1 == p2);
  }

  std::istream &operator>>(std::istream &in, Polygon &dest)
  {
    std::istream::sentry sentry(in);
    if (!sentry)
    {
      return in;
    }

    size_t vertexCount;
    if (!(in >> vertexCount) || vertexCount < 3)
    {
      in.setstate(std::ios::failbit);
      return in;
    }

    dest.points.resize(vertexCount);
    for (Point &p : dest.points)
    {
      if (!(in >> p))
      {
        in.setstate(std::ios::failbit);
        return in;
      }
    }

    return in;
  }

  std::ostream &operator<<(std::ostream &out, const Polygon &src)
  {
    std::ostream::sentry sentry(out);
    if (!sentry)
    {
      return out;
    }

    out << src.points.size() << ' ';
    for (const Point &p : src.points)
    {
      out << p << ' ';
    }
    return out;
  }

  struct PolygonAreaAccumulator
  {
    PolygonAreaAccumulator(const Polygon &p):
      poly(p),
      n(p.points.size())
    {}

    double operator()(double acc, const Point &p1) const
    {
      size_t i = &p1 - &poly.points[0];
      const nehvedovich::Point &p2 = poly.points[(i + 1) % n];
      return acc + (p1.x * p2.y - p2.x * p1.y);
    }
    const Polygon &poly;
    size_t n;
  };

  double calcPolygonArea(const Polygon &poly)
  {
    if (poly.points.size() < 3)
    {
      return 0.0;
    }
    double area = std::accumulate(poly.points.begin(), poly.points.end(), 0.0, PolygonAreaAccumulator(poly)) / 2.0;

    return std::abs(area);
  }

}

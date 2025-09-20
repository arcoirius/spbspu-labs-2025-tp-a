#include "polygon.hpp"
#include <algorithm>
#include <iterator>
#include <numeric>
#include <cmath>

namespace nehvedovich
{
  bool operator==(const Point &p1, const Point &p2)
  {
    return p1.x == p2.x && p1.y == p2.y;
  }

  std::istream &operator>>(std::istream &in, Polygon &dest)
  {
    std::istream::sentry s(in);
    if (!s)
    {
      return in;
    }

    std::size_t n = 0;
    if (!(in >> n))
    {
      return in;
    }

    if (n < 3)
    {
      in.setstate(std::ios::failbit);
      return in;
    }

    std::istream &operator>>(std::istream &in, Point &pt)
    {
      std::istream::sentry s(in);
      if (!s)
      {
        return in;
      }

      char lparen = 0, semi = 0, rparen = 0;
      int x = 0, y = 0;

      if (!(in >> lparen) || lparen != '(')
      {
        in.setstate(std::ios::failbit);
        return in;
      }
      if (!(in >> x))
      {
        return in;
      }
      if (!(in >> semi) || semi != ';')
      {
        in.setstate(std::ios::failbit);
        return in;
      }
      if (!(in >> y))
      {
        return in;
      }
      if (!(in >> rparen) || rparen != ')')
      {
        in.setstate(std::ios::failbit);
        return in;
      }

      pt.x = x;
      pt.y = y;
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

    std::vector< Point > pts;
    pts.reserve(n);
    std::istream_iterator< Point > it(in);
    std::copy_n(it, n, std::back_inserter(pts));

    if (in.fail())
    {
      return in;
    }

    std::vector< Point > tmp(pts);
    std::sort(tmp.begin(), tmp.end(), PointLess());
    struct PointEq
    {
      bool operator()(const Point &a, const Point &b) const
      {
        return a.x == b.x && a.y == b.y;
      }
    };
    if (std::adjacent_find(tmp.begin(), tmp.end(), PointEq()) != tmp.end())
    {
      in.setstate(std::ios::failbit);
      return in;
    }

    dest.points.swap(pts);
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
    explicit PolygonAreaAccumulator(const Polygon &p):
      poly(p),
      n(p.points.size())
    {}

    double operator()(double acc, const Point &p1) const
    {
      std::size_t i = static_cast< std::size_t >(&p1 - &poly.points[0]);
      const Point &p2 = poly.points[(i + 1) % n];
      return acc + (p1.x * p2.y - p2.x * p1.y);
    }

    const Polygon &poly;
    std::size_t n;
  };

  double calcPolygonArea(const Polygon &poly)
  {
    if (poly.points.size() < 3)
    {
      return 0.0;
    }
    const double area =
        std::accumulate(poly.points.begin(), poly.points.end(), 0.0, PolygonAreaAccumulator(poly)) / 2.0;
    return std::abs(area);
  }
}

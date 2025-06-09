#include "polygon.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iterator>
#include <wrappers_io.hpp>

bool nehvedovich::operator==(const Point &p1, const Point &p2)
{
  return p1.x == p2.x && p1.y == p2.y;
}

std::istream &nehvedovich::operator>>(std::istream &in, Point &dest)
{
  std::istream::sentry s(in);
  if (!s)
  {
    return in;
  }
  Point temp;
  in >> DelimiterIO {'('} >> temp.x >> DelimiterIO {';'} >> temp.y >> DelimiterIO {')'};
  if (in)
  {
    dest = temp;
  }
  return in;
}

std::ostream &nehvedovich::operator<<(std::ostream &out, const Point &src)
{
  std::ostream::sentry sentry(out);
  if (!sentry)
  {
    return out;
  }
  return out << '(' << src.x << ';' << src.y << ')';
}

bool nehvedovich::operator==(const Polygon &p1, const Polygon &p2)
{
  return p1.points == p2.points;
}

bool nehvedovich::operator!=(const Polygon &p1, const Polygon &p2)
{
  return !(p1 == p2);
}

std::istream &nehvedovich::operator>>(std::istream &in, Polygon &dest)
{
  std::istream::sentry sentry(in);
  if (!sentry)
  {
    return in;
  }

  size_t vertexCount;
  in >> vertexCount;
  if (!in || vertexCount < 3)
  {
    throw std::runtime_error("Not enough vertices.");
  }

  std::vector< Point > points(vertexCount, Point {});
  using inputItPoint = std::istream_iterator< Point >;
  std::copy_n(inputItPoint {in}, vertexCount, points.begin());
  if (in && points.size() == vertexCount)
  {
    dest.points = std::move(points);
  }
  else
  {
    in.setstate(std::ios::failbit);
  }
  return in;
}

std::ostream &nehvedovich::operator<<(std::ostream &out, const Polygon &src)
{
  std::ostream::sentry sentry(out);
  if (!sentry)
  {
    return out;
  }

  using outputItPoint = std::ostream_iterator< Point >;
  out << src.points.size() << ' ';
  std::copy_n(src.points.begin(), src.points.size(), outputItPoint {out, " "});
  return out;
}

struct PolygonAreaAccumulator
{
  double operator()(double acc, const nehvedovich::Point &p1, const nehvedovich::Point &p2) const
  {
    return acc + (p1.x * p2.y - p2.x * p1.y);
  }
};

double nehvedovich::calcPolygonArea(const Polygon &poly)
{
  if (poly.points.size() < 3)
  {
    return 0.0;
  }

  const auto &points = poly.points;
  double sum =
      std::inner_product(points.begin(), std::prev(points.end()), std::next(points.begin()), 0.0, std::plus<>(),
                         [](const Point &a, const Point &b)
                         {
                           return a.x * b.y - b.x * a.y;
                         });

  sum += points.back().x * points.front().y - points.front().x * points.back().y;

  return std::abs(sum) / 2.0;
}


#include "commands.hpp"
#include <algorithm>
#include <iomanip>
#include <limits>
#include <functional>
#include <numeric>
#include <stdexcept>
#include "format_guard.hpp"
#include "functional.hpp"
#include <string>

namespace nehvedovich
{
  double getArea(const Polygon &poly)
  {
    return calcPolygonArea(poly);
  }  

  struct AreaFilter
  {
    bool operator()(const Polygon &poly, const std::string &param) const
    {
      if (param == "EVEN")
      {
        return poly.points.size() % 2 == 0;
      }
      if (param == "ODD")
      {
        return poly.points.size() % 2 != 0;
      }
      size_t num = std::stoul(param);
      return poly.points.size() == num;
    }
  };

  struct SumAllAreas
{
  double operator()(double acc, const Polygon &poly) const
  {
    return acc + getArea(poly);
  }
};

struct SumAreasIfParam
{
  explicit SumAreasIfParam(const std::string &p):
    param(p)
  {}

  double operator()(double acc, const Polygon &poly) const
  {
    AreaFilter filter;
    return filter(poly, param) ? (acc + getArea(poly)) : acc;
  }

  std::string param;
};

  struct VertexCountFilter
  {
    bool operator()(const Polygon &poly, const std::string &param) const
    {
      if (param == "EVEN")
      {
        return poly.points.size() % 2 == 0;
      }
      if (param == "ODD")
      {
        return poly.points.size() % 2 != 0;
      }
      size_t num = std::stoul(param);
      return poly.points.size() == num;
    }
  };

  struct CountByParam
{
  explicit CountByParam(const std::string &p):
    param(p)
  {}

  bool operator()(const Polygon &poly) const
  {
    VertexCountFilter filter;
    return filter(poly, param);
  }

  std::string param;
};

struct LessThanArea
{
  explicit LessThanArea(double a):
    area(a)
  {}

  bool operator()(const Polygon &p) const
  {
    return getArea(p) < area;
  }

  double area;
};


}

void nehvedovich::areaCommand(std::istream &in, std::ostream &out, const std::vector< nehvedovich::Polygon > &polygons)
{
  std::string param;
  in >> param;

  if (param == "MEAN")
  {
    if (polygons.empty())
    {
      throw std::runtime_error("No polygons for MEAN calculation");
    }
    const double total = std::accumulate(polygons.begin(), polygons.end(), 0.0, SumAllAreas());
out << std::fixed << std::setprecision(1) << (total / polygons.size()) << '\n';
    return;
  }
  const double sum = std::accumulate(
    polygons.begin(),
    polygons.end(),
    0.0,
    SumAreasIfParam(param)
  );

  out << std::fixed << std::setprecision(1) << sum << '\n';
}

void nehvedovich::maxCommand(std::istream &in, std::ostream &out, const std::vector< nehvedovich::Polygon > &polygons)
{
  if (polygons.empty())
  {
    throw std::runtime_error("No polygons for MAX calculation");
  }

  std::string param;
  in >> param;

  if (param == "AREA")
  {
    auto it = std::max_element(polygons.begin(), polygons.end(), AreaComparator());
    out << std::fixed << std::setprecision(1) << getArea(*it) << '\n';
  }
  else if (param == "VERTEXES")
  {
    auto it = std::max_element(polygons.begin(), polygons.end(), VerticesComparator());

    out << it->points.size() << '\n';
  }
  else
  {
    throw std::runtime_error("Invalid MAX parameter");
  }
}

void nehvedovich::minCommand(std::istream &in, std::ostream &out, const std::vector< nehvedovich::Polygon > &polygons)
{
  std::istream::sentry sentry(in);
  if (!sentry)
  {
    return;
  }
  FormatGuard guard(in);
  out << std::fixed << std::setprecision(1);

  if (polygons.empty())
  {
    throw std::runtime_error("No polygons for MIN calculation");
  }

  std::string param;
  in >> param;

  if (param == "AREA")
  {
    auto it = std::min_element(polygons.begin(), polygons.end(), AreaComparator());
    out << std::fixed << std::setprecision(1) << getArea(*it) << '\n';
  }
  else if (param == "VERTEXES")
  {
    auto it = std::min_element(polygons.begin(), polygons.end(), VerticesComparator());
    out << it->points.size() << '\n';
  }
  else
  {
    throw std::runtime_error("Invalid MIN parameter");
  }
}

void nehvedovich::countCommand(std::istream &in, std::ostream &out, const std::vector< nehvedovich::Polygon > &polygons)
{
  std::string param;
in >> param;
std::size_t count = std::count_if(polygons.begin(), polygons.end(), CountByParam(param));
out << count << '\n';

}


void nehvedovich::lessAreaCommand(std::istream &in,
                                  std::ostream &out,
                                  const std::vector< nehvedovich::Polygon > &polygons)
                                  {
                                    nehvedovich::Polygon poly;
                                    in >> poly;
                                    if (in.fail())
                                    {
                                      throw std::runtime_error("Invalid polygon for LESSAREA");
                                    }
                                  
                                    const double area = getArea(poly);
                                    const std::size_t count = std::count_if(
                                        polygons.begin(),
                                        polygons.end(),
                                        LessThanArea(area)
                                    );
                                  
                                    out << count << '\n';
                                  }
                                  

void nehvedovich::inFrameCommand(std::istream &in,
                                 std::ostream &out,
                                 const std::vector< nehvedovich::Polygon > &polygons)
{
  if (polygons.empty())
  {
    throw std::runtime_error("No polygons for frame calculation");
  }

  nehvedovich::Polygon poly;
  in >> poly;
  if (in.fail())
  {
    throw std::runtime_error("Invalid polygon for INFRAME");
  }

  BoundingBox box(polygons);
  out << (box.contains(poly) ? "<TRUE>\n" : "<FALSE>\n");
}

void nehvedovich::sameCommand(std::istream &in, std::ostream &out, std::vector< Polygon > &polygons)
{
  std::istream::sentry sentry(in);
  if (!sentry)
  {
    return;
  }
  Polygon reference;
  if (!(in >> reference))
  {
    out << "<INVALID COMMAND>\n";
    return;
  }
  try
  {
    BoundingBox bbox(polygons);
    if (bbox.contains(reference))
    {
      out << "<TRUE>\n";
    }
    else
    {
      out << "<FALSE>\n";
    }
  }
  catch (const std::exception &)
  {
    out << "<INVALID COMMAND>\n";
  }
}

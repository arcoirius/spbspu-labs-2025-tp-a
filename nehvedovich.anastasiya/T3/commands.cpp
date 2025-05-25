#include "commands.hpp"
#include <algorithm>
#include <iomanip>
#include <limits>
#include <numeric>
#include <stdexcept>
#include "format_guard.hpp"
#include <string>

namespace nehvedovich {

double getArea(const Polygon& poly) {
    if (poly.points.size() < 3) return 0.0;
    double sum = 0.0;
    for (size_t i = 0; i < poly.points.size(); ++i) {
        const Point& p1 = poly.points[i];
        const Point& p2 = poly.points[(i + 1) % poly.points.size()];
        sum += (p1.x * p2.y - p2.x * p1.y);
    }
    return std::abs(sum) / 2.0;
}

struct AreaFilter {
    bool operator()(const Polygon& poly, const std::string& param) const {
        if (param == "EVEN") return poly.points.size() % 2 == 0;
        if (param == "ODD") return poly.points.size() % 2 != 0;
        size_t num = std::stoul(param);
        return poly.points.size() == num;
    }
};

struct VertexCountFilter {
    bool operator()(const Polygon& poly, const std::string& param) const {
        if (param == "EVEN") return poly.points.size() % 2 == 0;
        if (param == "ODD") return poly.points.size() % 2 != 0;
        size_t num = std::stoul(param);
        return poly.points.size() == num;
    }
};

struct BoundingBox {
    int minX, maxX, minY, maxY;
    
    BoundingBox(const std::vector<Polygon>& polygons) {
        if (polygons.empty()) {
            throw std::runtime_error("No polygons to calculate bounding box");
        }
        minX = maxX = polygons[0].points[0].x;
        minY = maxY = polygons[0].points[0].y;
        
        for (const auto& poly : polygons) {
            for (const auto& point : poly.points) {
                minX = std::min(minX, point.x);
                maxX = std::max(maxX, point.x);
                minY = std::min(minY, point.y);
                maxY = std::max(maxY, point.y);
            }
        }
    }
    
    bool contains(const Polygon& poly) const {
        for (const auto& point : poly.points) {
            if (point.x < minX || point.x > maxX || point.y < minY || point.y > maxY) {
                return false;
            }
        }
        return true;
    }
};

}

void nehvedovich::areaCommand(std::istream& in, std::ostream& out, const std::vector<nehvedovich::Polygon>& polygons) {
    std::string param;
    in >> param;
    
    if (param == "MEAN") {
        if (polygons.empty()) {
            throw std::runtime_error("No polygons for MEAN calculation");
        }
        double total = std::accumulate(polygons.begin(), polygons.end(), 0.0,
            [](double sum, const nehvedovich::Polygon& poly) { return sum + getArea(poly); });
        out << std::fixed << std::setprecision(1) << (total / polygons.size()) << '\n';
        return;
    }
    
    AreaFilter filter;
    double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0,
        [&filter, &param](double sum, const nehvedovich::Polygon& poly) {
            return filter(poly, param) ? sum + getArea(poly) : sum;
        });
    out << std::fixed << std::setprecision(1) << sum << '\n';
}

void nehvedovich::maxCommand(std::istream& in, std::ostream& out, const std::vector<nehvedovich::Polygon>& polygons) {
    if (polygons.empty()) {
        throw std::runtime_error("No polygons for MAX calculation");
    }
    
    std::string param;
    in >> param;
    
    if (param == "AREA") {
        auto it = std::max_element(polygons.begin(), polygons.end(),
            [](const nehvedovich::Polygon& a, const nehvedovich::Polygon& b) { return getArea(a) < getArea(b); });
        out << std::fixed << std::setprecision(1) << getArea(*it) << '\n';
    } else if (param == "VERTEXES") {
        auto it = std::max_element(polygons.begin(), polygons.end(),
            [](const nehvedovich::Polygon& a, const nehvedovich::Polygon& b) { return a.points.size() < b.points.size(); });
        out << it->points.size() << '\n';
    } else {
        throw std::runtime_error("Invalid MAX parameter");
    }
}

void nehvedovich::minCommand(std::istream& in, std::ostream& out, const std::vector<nehvedovich::Polygon>& polygons) 
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
    
    if (param == "AREA") {
        auto it = std::min_element(polygons.begin(), polygons.end(),
            [](const nehvedovich::Polygon& a, const nehvedovich::Polygon& b) { return getArea(a) < getArea(b); });
        out << std::fixed << std::setprecision(1) << getArea(*it) << '\n';
    } else if (param == "VERTEXES") {
        auto it = std::min_element(polygons.begin(), polygons.end(),
            [](const nehvedovich::Polygon& a, const nehvedovich::Polygon& b) { return a.points.size() < b.points.size(); });
        out << it->points.size() << '\n';
    } else {
        throw std::runtime_error("Invalid MIN parameter");
    }
}

void nehvedovich::countCommand(std::istream& in, std::ostream& out, const std::vector<nehvedovich::Polygon>& polygons) {
    std::string param;
    in >> param;
    
    VertexCountFilter filter;
    size_t count = std::count_if(polygons.begin(), polygons.end(),
        [&filter, &param](const Polygon& poly) { return filter(poly, param); });
    out << count << '\n';
}

void nehvedovich::lessAreaCommand(std::istream& in, std::ostream& out, const std::vector<nehvedovich::Polygon>& polygons) {
    nehvedovich::Polygon poly;
    in >> poly;
    if (in.fail()) {
        throw std::runtime_error("Invalid polygon for LESSAREA");
    }
    
    double area = getArea(poly);
    size_t count = std::count_if(polygons.begin(), polygons.end(),
        [area](const nehvedovich::Polygon& p) { return getArea(p) < area; });
    out << count << '\n';
}

void nehvedovich::inFrameCommand(std::istream& in, std::ostream& out, const std::vector<nehvedovich::Polygon>& polygons) {
    if (polygons.empty()) {
        throw std::runtime_error("No polygons for frame calculation");
    }
    
    nehvedovich::Polygon poly;
    in >> poly;
    if (in.fail()) {
        throw std::runtime_error("Invalid polygon for INFRAME");
    }
    
    BoundingBox box(polygons);
    out << (box.contains(poly) ? "<TRUE>\n" : "<FALSE>\n");
}
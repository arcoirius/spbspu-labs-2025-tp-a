#include "functional.hpp"
#include <algorithm>
#include <iterator>
#include <limits>
#include <numeric>

namespace nehvedovich
{
  AreaIt maxAreaElement(AreaIt first, AreaIt last, const AreaComparator &comp)
  {
    return std::max_element(first, last, comp);
  }

  AreaIt minAreaElement(AreaIt first, AreaIt last, const AreaComparator &comp)
  {
    return std::min_element(first, last, comp);
  }

  VertIt maxVertElement(VertIt first, VertIt last, const VerticesComparator &comp)
  {
    return std::max_element(first, last, comp);
  }

  VertIt minVertElement(VertIt first, VertIt last, const VerticesComparator &comp)
  {
    return std::min_element(first, last, comp);
  }

  void readPolygons(std::istream &in, std::vector< Polygon > &polygons)
  {
    using input_it_t = std::istream_iterator< Polygon >;
    while (!in.eof())
    {
      std::copy(input_it_t(in), input_it_t(), std::back_inserter(polygons));
      if (in.fail())
      {
        in.clear();
        in.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
      }
    }
  }

  bool is_congruent_by_shift(const Polygon &candidate, const Polygon &reference, std::size_t shift)
  {
    if (candidate.points.size() != reference.points.size())
    {
      return false;
    }

    std::vector< Point > rotated(candidate.points.size());
    std::rotate_copy(candidate.points.begin(), candidate.points.begin() + shift, candidate.points.end(),
                     rotated.begin());

    int dx = reference.points[0].x - rotated[0].x;
    int dy = reference.points[0].y - rotated[0].y;

    std::vector< Point > translated(rotated.size());
    std::transform(rotated.begin(), rotated.end(), translated.begin(), TranslatePoint(dx, dy));

    return std::equal(translated.begin(), translated.end(), reference.points.begin());
  }

  unsigned count_translation_congruent(const PolyVector &polygons, const Polygon &reference)
  {
    return std::count_if(polygons.begin(), polygons.end(), IsTranslationCongruent(reference));
  }
}

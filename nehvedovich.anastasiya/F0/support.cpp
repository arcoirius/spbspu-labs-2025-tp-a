#include "support.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iterator>

bool nehvedovich::isLetter(char c)
{
  return std::isalpha(static_cast< unsigned char >(c));
}

char nehvedovich::toLower(char c)
{
  return static_cast< char >(std::tolower(static_cast< unsigned char >(c)));
}

bool nehvedovich::isValidChar(unsigned char c)
{
  return std::isalnum(c) || c == '_';
}

bool nehvedovich::isValidName(const std::string &name)
{
  return !name.empty() && std::all_of(name.begin(), name.end(), isValidChar);
}

void nehvedovich::addSortedTranslation(std::vector< std::string > &translations, const std::string &translation)
{
  translations.push_back(translation);
  std::sort(translations.begin(), translations.end());
  translations.erase(std::unique(translations.begin(), translations.end()), translations.end());
}

bool nehvedovich::WordComparator::operator()(const std::pair< std::string, std::vector< std::string > > &a,
                                             const std::pair< std::string, std::vector< std::string > > &b) const
{
  return a.first < b.first;
}

std::string nehvedovich::Join::operator()(const std::vector< std::string > &v, const std::string &sep) const
{
  if (v.empty())
  {
    return "";
  }

  std::ostringstream oss;
  std::copy(v.begin(), v.end() - 1, std::ostream_iterator< std::string >(oss, sep.c_str()));
  oss << v.back();
  return oss.str();
}

nehvedovich::WordFormatter::WordFormatter(Mode m) : mode(m)
{
}

std::string nehvedovich::WordFormatter::operator()(
  const std::pair< std::string, std::vector< std::string > > &entry) const
{
  switch (mode)
  {
    case IdentityMode:
      return entry.first;
    case ExtractMode:
      return entry.first;
    case PrintMode:
      return entry.first + " - " + formatTranslations(entry.second);
  }
  return "";
}

std::string nehvedovich::formatTranslations(const std::vector< std::string > &translations)
{
  std::vector< std::string > sortedTranslations = translations;
  std::sort(sortedTranslations.begin(), sortedTranslations.end());
  return Join()(sortedTranslations);
}

nehvedovich::TranslationFilter::TranslationFilter(Mode m, size_t c, std::string t) : mode(m),
                                                                                     count(c),
                                                                                     target(std::move(t))
{
}

bool nehvedovich::TranslationFilter::operator()(
  const std::pair< std::string, std::vector< std::string > > &entry) const
{
  const auto &translations = entry.second;

  switch (mode)
  {
    case MoreThanMode:
      return translations.size() > count;
    case ContainsMode:
      return std::find(translations.begin(), translations.end(), target) != translations.end();
  }

  return false;
}

const std::string &nehvedovich::AddTranslation::operator()(const std::string &tr) const
{
  addSortedTranslation(translations, tr);
  return tr;
}

std::pair< std::string, std::vector< std::string > > nehvedovich::MergeDictionaries::operator()(
  const std::pair< std::string, std::vector< std::string > > &entry) const
{
  auto it = result.find(entry.first);

  if (it != result.end())
  {
    std::vector< std::string > tmp = entry.second;
    std::transform(tmp.begin(), tmp.end(), tmp.begin(),
                   AddTranslation{it->second});
  }
  else
  {
    result[entry.first] = entry.second;
  }

  return entry;
}

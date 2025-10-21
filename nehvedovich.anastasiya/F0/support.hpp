#ifndef SUPPORT_HPP
#define SUPPORT_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>

namespace nehvedovich
{
  using Dictionary = std::unordered_map< std::string, std::vector< std::string > >;
  using DictionarySet = std::unordered_map< std::string, Dictionary >;

  bool isLetter(char c);
  char toLower(char c);
  bool isValidChar(unsigned char c);
  bool isValidName(const std::string &name);

  void addSortedTranslation(std::vector< std::string > &translations, const std::string &translation);

  struct WordComparator
  {
    bool operator()(const std::pair< std::string, std::vector< std::string > > &a,
                    const std::pair< std::string, std::vector< std::string > > &b) const;
  };

  std::string formatTranslations(const std::vector< std::string > &translations);

  struct WordFormatter
  {
    enum Mode
    {
      IdentityMode,
      ExtractMode,
      PrintMode
    };
    Mode mode;

    explicit WordFormatter(Mode m);
    std::string operator()(const std::pair< std::string, std::vector< std::string > > &entry) const;
  };

  struct Join
  {
    std::string operator()(const std::vector< std::string > &v, const std::string &sep = ", ") const;
  };

  struct TranslationFilter
  {
    enum Mode
    {
      MoreThanMode,
      ContainsMode
    };

    Mode mode;
    size_t count = 0;
    std::string target;

    TranslationFilter(Mode m, size_t c, std::string t = "");
    bool operator()(const std::pair< std::string, std::vector< std::string > > &entry) const;
  };

  struct AddTranslation
  {
    std::vector< std::string > &translations;
    const std::string &operator()(const std::string &tr) const;
  };

  struct MergeDictionaries
  {
    Dictionary &result;
    std::pair< std::string, std::vector< std::string > > operator()(
      const std::pair< std::string, std::vector< std::string > > &entry) const;
  };
} // namespace nehvedovich

#endif

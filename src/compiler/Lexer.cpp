#include <regex>
#include <string>
#include <vector>

std::vector<std::string> tokenizeString(const std::string && str)
{
    std::regex regex(R"(([A-Za-z0-9-]+)|(\s+)|([\",\(,\),\[,\],\<,\>,\,,\.])|([_,+,=,*,/]+))",
                     std::regex::multiline | std::regex_constants::icase);
    auto words_begin = std::sregex_token_iterator(str.begin(), str.end(), regex);
    auto words_end = std::sregex_token_iterator();
    std::vector<std::string> tokenized_strings;
    tokenized_strings.reserve(std::distance(words_begin, words_end));
    std::move(words_begin, words_end, std::back_inserter(tokenized_strings));
    return tokenized_strings;
}
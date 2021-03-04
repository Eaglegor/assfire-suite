#include "MarkdownTableParser.hpp"
#include <cassert>
#include <regex>
#include <iostream>

namespace {
    static const std::regex MD_REGEX("\\|\\s*(.*?)\\s*(?=\\|)");
}

void assfire::MarkdownTableParser::parseHeader(const std::string &h)
{
    auto header_begin =
            std::sregex_iterator(h.begin(), h.end(), MD_REGEX);
    auto header_end = std::sregex_iterator();

    for(auto iter = header_begin; iter != header_end; ++iter) {
        std::string w = iter->str(1);
        header.push_back(w);
    }
}

const assfire::MarkdownTableParser::Entry& assfire::MarkdownTableParser::processEntry(const std::string &row)
{
    assert(!header.empty());

    auto entry_begin =
            std::sregex_iterator(row.begin(), row.end(), MD_REGEX);
    auto entry_end = std::sregex_iterator();

    assfire::MarkdownTableParser::Entry result;

    int index = 0;
    for(auto iter = entry_begin; iter != entry_end; ++iter) {
        std::string w = iter->str(1);
        result.insert(header[index++], w);
    }

    entries.push_back(result);
    return entries.back();
}

const assfire::MarkdownTableParser::Entries &assfire::MarkdownTableParser::getEntrires() const
{
    return entries;
}

void assfire::MarkdownTableParser::Entry::insert(const std::string &key, const std::string &value)
{
    data.insert_or_assign(key, value);
}

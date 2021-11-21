#pragma once

#include <vector>
#include <string>
#include <unordered_map>

namespace assfire
{
    class MarkdownTableParser
    {
    public:
        class Entry {
        public:
            bool contains(const std::string& key) const;
            const std::string& at(const std::string& key) const;
            void insert(const std::string& key, const std::string& value);

        private:
            std::unordered_map<std::string, std::string> data;
        };

        using Entries = std::vector<Entry>;

        void parseTable(const std::string& table);
        bool parseHeader(const std::string& header);
        const Entry& processEntry(const std::string& row);

        const Entries& getEntries() const;

    private:
        std::vector<std::string> header;
        std::vector<Entry> entries;
    };
}


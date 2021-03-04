#pragma once

#include <unordered_map>

namespace assfire
{
    class MarkdownTableParser
    {
    public:
        class Entry {
        public:
            void insert(const std::string& key, const std::string& value);

        private:
            std::unordered_map<std::string, std::string> data;
        };

        using Entries = std::vector<Entry>;

        void parseHeader(const std::string& header);
        const Entry& processEntry(const std::string& row);

        const Entries& getEntrires() const;

    private:
        std::vector<std::string> header;
        std::vector<Entry> entries;
    };
}


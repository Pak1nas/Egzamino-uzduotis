#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <cctype>
#include <regex>

std::string cleanWord(const std::string& token) {
    std::string result;
    for (char c : token) {
        if (std::isalpha(static_cast<unsigned char>(c)) || c == '-') {
            result += std::tolower(static_cast<unsigned char>(c));
        }
    }
    return result;
}

void task1and2(const std::string& inputFile) {
    std::ifstream inFile(inputFile);
    if (!inFile) {
        std::cerr << "[1/2] Klaida: nepavyko atidaryti " << inputFile << "\n";
        return;
    }

    std::map<std::string, int>           freq;
    std::map<std::string, std::set<int>> crossRef;

    std::string line;
    int lineNum = 1;
    while (std::getline(inFile, line)) {
        std::istringstream iss(line);
        std::string token;
        while (iss >> token) {
            std::string word = cleanWord(token);
            if (!word.empty()) {
                freq[word]++;
                crossRef[word].insert(lineNum);
            }
        }
        lineNum++;
    }
    inFile.close();

    // Surinkti žodžius pasikartojančius >1 kartą, surūšiuoti pagal dažnį
    std::vector<std::pair<int, std::string>> sorted;
    for (const auto& [word, count] : freq)
        if (count > 1)
            sorted.push_back({count, word});
    std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) {
        return a.first > b.first;
    });

    std::ofstream out1("uzd1.txt");
    out1 << std::left;
    out1 << "Zodis                          | Kiekis\n";
    out1 << std::string(42, '-') << "\n";
    for (const auto& [count, word] : sorted) {
        out1.width(31);
        out1 << word << "| " << count << "\n";
    }
    out1.close();
    std::cout << "[1] Zodziu daznis (>1 kartas): " << sorted.size()
              << " zodziu -> uzd1.txt\n";

    std::ofstream out2("uzd2.txt");
    out2 << std::left;
    out2 << "Zodis                          | Sk. | Eilutes\n";
    out2 << std::string(60, '-') << "\n";
    for (const auto& [count, word] : sorted) {
        out2.width(31);
        out2 << word << "| ";
        out2.width(3);
        out2 << count << " | ";
        bool first = true;
        for (int ln : crossRef[word]) {
            if (!first) out2 << ", ";
            out2 << ln;
            first = false;
        }
        out2 << "\n";
    }
    out2.close();
    std::cout << "[2] Cross-reference lentele -> uzd2.txt\n";
}

//URL adresų suradimas
void task3(const std::string& inputFile) {
    std::ifstream inFile(inputFile);
    if (!inFile) {
        std::cerr << "[3] Klaida: nepavyko atidaryti " << inputFile << "\n";
        return;
    }
    std::string content((std::istreambuf_iterator<char>(inFile)),
                         std::istreambuf_iterator<char>());
    inFile.close();

    std::set<std::string> urls;

    auto extract = [&](const std::regex& re) {
        auto it  = std::sregex_iterator(content.begin(), content.end(), re);
        auto end = std::sregex_iterator();
        for (; it != end; ++it) {
            std::string url = (*it)[1].str();
            // Pašalinti galinę skyrybą
            while (!url.empty() && (url.back() == '.' || url.back() == ',' ||
                                    url.back() == ')' || url.back() == ';'))
                url.pop_back();
            if (!url.empty())
                urls.insert(url);
        }
    };

    // Pilni URL: https://... arba http://...
    extract(std::regex(R"((https?://[^\s,;\"'<>\)]+))"));
    // Sutrumpinti su www.: www.something.tld
    extract(std::regex(R"(\b(www\.[a-zA-Z0-9\-]+(?:\.[a-zA-Z]{2,4})+(?:/[^\s,;\"'<>\)]*)?))"));
    // Pliki domenai: vu.lt ir pan. (be http:// arba www. priekyje)
    // Surenkame visus potencialius domenus ir filtruojame rankiškai
    {
        std::regex bareRe(R"(\b([a-zA-Z0-9\-]{2,}\.[a-zA-Z]{2,3})\b)");
        auto it  = std::sregex_iterator(content.begin(), content.end(), bareRe);
        auto end = std::sregex_iterator();
        for (; it != end; ++it) {
            std::string url = (*it)[1].str();
            // Praleisti jei prieš jį yra '/', ':' arba raidė (jau dalis pilno URL)
            std::string::size_type pos = it->position(1);
            if (pos > 0) {
                char before = content[pos - 1];
                if (before == '/' || before == ':' || std::isalnum(static_cast<unsigned char>(before)) || before == '.')
                    continue;
            }
            if (!url.empty())
                urls.insert(url);
        }
    }

    std::ofstream out3("uzd3.txt");
    out3 << "Rasti unikalus URL adresai:\n";
    out3 << std::string(40, '-') << "\n";
    for (const auto& url : urls)
        out3 << url << "\n";
    out3.close();
    std::cout << "[3] Rasta " << urls.size()
              << " unikaliu URL adresu -> uzd3.txt\n";
}

int main() {

    task1and2("vilnius.txt");

    std::cout << "\n";

    task3("vilnius.txt");

    return 0;
}

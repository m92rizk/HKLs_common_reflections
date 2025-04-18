#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <array>
#include <vector>
#include <chrono>
#include <sstream>
#include <unordered_set>
#include <utility>
#include <cmath>
#include <iomanip>
#include <functional>  // For std::hash


bool is_data_line(const std::string& line) {
    return !line.empty() && line[0] != '!';
}

void read_reflections(std::ifstream& file, std::vector<std::vector<std::string>>& table) {
    // std::ifstream infile(file);
    std::string line;
    bool in_table = false;

    while (std::getline(file, line)) {
        if (!in_table && is_data_line(line)) {
            in_table = true;
            continue;
        }

        if (in_table && !is_data_line(line)) {
            continue;
        }

        if (in_table) {
            std::istringstream rowStream(line);
            std::string cell;
            std::vector<std::string> row;

            while (rowStream >> cell) {
                row.push_back(cell);
            }

            if (!row.empty()) {
                table.push_back(row);
            }
        }
    }
    file.close();
}

std::vector<std::vector<std::string>> truncate_table(std::vector<std::vector<std::string>>& table) {
    size_t table_width = 18;
    std::vector<std::vector<std::string>> truncated_table;
    for (const auto& row : table) {
        auto end_iter = row.begin() + std::min(row.size(), table_width);
        truncated_table.emplace_back(row.begin(), end_iter);
        for (const auto& cell : row) {
            std::cout << cell << " ";
        }
        std::cout << std::endl;
    }
    return truncated_table;
}


struct VectorHash {
    size_t operator()(const std::vector<std::string>& vec) const {
        std::hash<std::string> hasher;
        size_t seed = 0;
        for (const auto& str : vec) {
            seed ^= hasher(str) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

size_t countCommonRows(std::vector<std::vector<std::string>>& table1, std::vector<std::vector<std::string>>& table2) {
    // Requires std::hash<std::vector<std::string>> (or a custom hash)
    std::unordered_set<std::vector<std::string>, VectorHash> rows2(table2.begin(), table2.end());

    size_t common = 0;
    for (const auto& row : table1) {
        if (rows2.count(row)) {
            common++;
        }
    }
    return common;
}

std::string get_percentage_of_COMMON_reflections(int commons, int total_reflections) {
    float percentage = (commons/total_reflections) *100;
    std::ostringstream percentage_stream;
    percentage_stream << std::fixed << std::setprecision(2) << percentage ;
    std::string percentage_str = percentage_stream.str();
    return percentage_str;
}


bool calculate_common_reflections(const std::string& filename1, const std::string& filename2) {
    std::ifstream infile1(filename1);
    std::ifstream infile2(filename2);
    if (!infile1) {
        std::cerr << "Error opening input file: " << filename1 << "\n";
        return false;
    }
    if (!infile2) {
        std::cerr << "Error opening input file: " << filename2 << "\n";
        return false;
    }

    std::vector<std::vector<std::string>> table1;
    std::vector<std::vector<std::string>> table2;
    std::vector<std::vector<std::string>> truncated_table1 = truncate_table(table1);
    std::vector<std::vector<std::string>> truncated_table2 = truncate_table(table2);
    // for (const auto& cell : table1) {
    //     std::cout << cell << "  " ;
    // }
    // std::cout << std::endl;

    read_reflections(infile1, table1);
    read_reflections(infile2, table2);
    int common_reflections = countCommonRows(table1, table2);
    std::string percentage_common = "0";
    if (table1.size() < table2.size()) {
        percentage_common = get_percentage_of_COMMON_reflections(common_reflections, table1.size());
    } else {
        percentage_common = get_percentage_of_COMMON_reflections(common_reflections, table2.size());
    }
    std::cout << filename1 << " number of reflections : " << table1.size() << std::endl;
    std::cout << filename2 << " number of reflections : " << table2.size() << std::endl;
    std::cout << "number of COMMON reflections : " << common_reflections << " (" << percentage_common <<"%)" << std::endl;


    return true;
}

int main(int argc, char* argv[]) {
    // Check if filename was provided
    if (argc != 3)  {
        std::cerr << "\n### Usage: " << argv[0] << " <filename1.HKL> <filename2.HKL>\n### This should calculate the percentage of the number of common reflections between the two files\n" << std::endl;
        return 1;
    }
    calculate_common_reflections(argv[1], argv[2]);
}

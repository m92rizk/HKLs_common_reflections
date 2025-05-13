#include <iostream>
#include <fstream>
//#include <string>
//#include <algorithm>
//#include <array>
#include <vector>
//#include <chrono>
#include <sstream>
#include <unordered_set>
//#include <utility>
//#include <cmath>
#include <iomanip>
#include <functional>  // For std::hash


bool is_data_line(const std::string& line) {
    return !line.empty() && line[0] != '!';
}

std::string vector_to_string(const std::vector<std::string>& row) {
    std::ostringstream oss;
    for (size_t i = 0; i < row.size(); ++i) {

        oss << row[i];
        // if (i != row.size() - 1) {
        //     oss << '\t';  // Add tab between items
        // }
    }
    return oss.str();
}
void push_back_padded(std::vector<std::string>& strings, const std::string& item, int width, int space_after) {
    std::ostringstream val_stream;
    val_stream << std::setw (width) << item ;
    std::string val_str = val_stream.str();
    strings.push_back( val_str );
    for (int i=0; i<space_after; i++) {
        strings.push_back(" ");
    }
}

void read_reflections(std::ifstream& file, std::vector<std::vector<std::string>>& table) {
    // std::ifstream infile(file);
    std::cout << "\r" << "Reading reflections .. " << std::flush;
    std::string line;
    bool in_table = false;
    bool printed_once = true; //false;  // Flag to track if we've printed already

    while (std::getline(file, line)) {
        if (!in_table && is_data_line(line)) {
            in_table = true;
            continue;
        }

        if (in_table && !is_data_line(line)) {
            continue;
        }

        if (in_table) {
            std::vector<std::string> row;
            std::istringstream rowStream(line);
            // std::string cell;
            // while (rowStream >> cell) {
            //     row.push_back(cell);
            // }
            // std::cout<<line<<std::endl;
            // std::string line_string = "";
            // int n = line.length();
            // for(int i=0 ; i<n ; i++){
            //     line_string.append(line[i]);
            // }
            // for (char* c in line) {
            //     line_string.append(c);
            // }
            std::string h=line.substr(1,5);
            //std::cout<<"H"<<h<<std::endl;
            std::string k=line.substr(7,5);
            //std::cout<<"K"<<k<<std::endl;
            std::string l=line.substr(13,5);
            //std::cout<<"L"<<l<<std::endl;
            std::string I=line.substr(19,10);
            std::string sigma=line.substr(30,10);
            


            // std::cout<<line<<std::endl;
            //
            // hhhhh kkkkk lllll iiiiiiiiii ssssssssss
            //     0     0     1 -5.735E-01  1.823E+01
            std::vector<std::string> new_row;
            new_row.push_back(" ");
            push_back_padded(new_row, h, 4, 1);
            push_back_padded(new_row, k, 4, 1);
            push_back_padded(new_row, l, 4, 1);
            push_back_padded(new_row, I, 8, 1);
            push_back_padded(new_row, sigma, 8, 2);
            
            if (!printed_once) {
                for (const auto& val : new_row) std::cout << val;
                std::cout << std::endl;
                printed_once = true;  // Mark that we've printed
            }
            //             table.push_back(new_row);
            // std::string new_row_str = vector_to_string(new_row);
            // std::cout<<new_row_str<<std::endl;
//             file << new_row_str;
//             file << "\n";
//             

            if (!line.empty()) {
                table.push_back(new_row);
            }
        }
    }
    file.close();
}

std::vector<std::vector<std::string>> truncate_table(std::vector<std::vector<std::string>>& table) {
    size_t table_width = 6;
    std::vector<std::vector<std::string>> truncated_table;
    bool printed_once = true ; //false;  // Flag to track if we've printed already
    std::cout << "\r" << "Preparing reflection files ... " << std::flush;
    if (table.empty()) {
        std::cout << "Warning: table is empty\n";
    }
    for (const auto& row : table) {
        auto end_iter = row.begin() + std::min(row.size(), table_width);
        std::vector<std::string> truncated_row(row.begin(), end_iter);
        truncated_table.emplace_back(truncated_row);
        if (!printed_once) {
            for (const auto& val : truncated_row) std::cout << val;
            std::cout << std::endl;
            printed_once = true;  // Mark that we've printed
        }
//         for (const auto& cell : truncated_row) {
//             std::cout << cell << " ";
//         }
//         std::cout << std::endl;
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
    std::cout << "\r" << "Counting common reflections ... " << std::flush;
    std::unordered_set<std::vector<std::string>, VectorHash> rows2(table2.begin(), table2.end());

    size_t common = 0;
    for (const auto& row : table1) {
        if (rows2.count(row)) {
            common++;
        }
    }
    return common;
}

std::string get_percentage_of_COMMON_reflections(int common_reflections, int total_reflections) {
    if (total_reflections == 0) return "0.00%";  // To avoid division by zero
    std::cout << "\r" << "Wrapping up results ... " << std::flush;
    double percentage = (static_cast<double>(common_reflections) / total_reflections) * 100;
    std::ostringstream oss;
    oss.precision(2);
    oss << std::fixed << percentage << "%";
    return oss.str();
}


bool calculate_common_reflections(const std::string& filename1, const std::string& filename2) {
    std::ifstream infile1(filename1);
    std::ifstream infile2(filename2);
        // Spinner setup
    std::cout << "\r" << "Loading ... " << std::flush;
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

    read_reflections(infile1, table1);
    read_reflections(infile2, table2);
    std::vector<std::vector<std::string>> truncated_table1 = truncate_table(table1);
    std::vector<std::vector<std::string>> truncated_table2 = truncate_table(table2);
    // for (const auto& cell : table1) {
    //     std::cout << cell << "  " ;
    // }
    // std::cout << std::endl;
    int common_reflections = countCommonRows(truncated_table1, truncated_table2);
    std::string percentage_common = "0";
    if (table1.size() < table2.size()) {
        percentage_common = get_percentage_of_COMMON_reflections(common_reflections, table1.size());
    } else {
        percentage_common = get_percentage_of_COMMON_reflections(common_reflections, table2.size());
    }
    std::cout << "\r" << filename1 << " number of reflections : " << table1.size() << std::endl;
    std::cout << filename2 << " number of reflections : " << table2.size() << std::endl;
    std::cout << "COMMON reflections : " << common_reflections << " (" << percentage_common <<")" << std::endl;


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

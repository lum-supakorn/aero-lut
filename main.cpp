#include <string>
#include <fstream>
#include <map>
#include <utility> // for std::pair
#include <stdexcept> // for std::runtime_error
#include <sstream> // std::stringstream
#include <iostream>
#include <vector>
#include <cmath>

/**
 * Read a CSV file which contains XFLR5-exported *-α relations (polar)
 * and construct a lookup table
 *
 * @param[out] *-α lookup table (map<X, Y>) where X represents α in radians
 * and Y represents the corresponding aerodynamic value
 * @param[in] *-α CSV file name
 */
std::map<std::string, std::vector<std::pair<double, double>>> read_csv(std::string filename) {
    // The resulting LUT
    std::map<std::string, std::vector<std::pair<double, double>>> result;
    // Column names
    std::vector<std::string> columns;

    // Create an input filestream
    std::ifstream input_file(filename);

    // Check if the file is accessible
    if (!input_file.is_open()) throw std::runtime_error("Could not open the file");

    // Line string
    std::string line;

    // Skip the first 9 lines (XFLR5 exported polar file header)
    // This is hard-coded under the assumption that the exported
    // polar file are of this format.
    for (int i = 0; i < 9; i++) {
        std::getline(input_file, line);
    }

    // Read and extract the column names
    if (input_file.good()) {
        // Extract the first line which defines the column names
        std::getline(input_file, line);

        // Create a stringstream from the line
        std::stringstream ss(line);

        std::string column;

        // Skip the alpha column as it is used as the
        // key to every column in the result map
        ss >> column;

        // Extract the rest of the column names and
        // store them in the column vector
        while (ss >> column) {
            columns.push_back(column);
        }
    }

    // Skip another line (dash underline)
    std::getline(input_file, line);

    // Read the data row by row
    while (std::getline(input_file, line)) {
        // Reading is done when an empty line is found
        if (line.empty()) break;

        // Create a stringstream for each line
        std::stringstream ss(line);

        // Create a row-unique alpha (angle of attack)
        // and convert from degree to radian
        double alpha, val;
        ss >> alpha;
        alpha *= M_PI / 180;

        // Write data into columns
        for (auto& column : columns) {
            // Read each parameter
            ss >> val;

            // Create an α and parameter pair
            std::pair<double, double> data_pair;
            data_pair = std::make_pair(alpha, val);

            // Push the pair into a column vector
            result[column].push_back(data_pair);
        }
    }

    return result;
}

int main() {
    std::map<std::string, std::vector<std::pair<double, double>>> result = read_csv("../T1_Re0.100_M0.00_N9.0");

    for (auto& pair : result["Cpmin"]) {
        std::cout << pair.first << " " << pair.second << std::endl;
    }
    return 0;
}

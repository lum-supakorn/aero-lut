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
 * @param[out] *-α lookup table -- map<string, vector<pair<X, Y>>> where the string key is
 * the aerodynamic parameter of interest (e.g. Cl, Cd, or Cm), X represents α in radians,
 * and Y represents the corresponding aerodynamic parameter value
 * @param[in] *-α CSV file name
 */
std::map<std::string, std::vector<std::pair<double, double>>> read_csv(std::string filename) {
    // The resulting LUT
    std::map<std::string, std::vector<std::pair<double, double>>> result;
    // Column names
    std::vector<std::string> columns = {
            "CL",
            "CD",
            "CDp",
            "Cm",
            "Top Xtr",
            "Bot Xtr",
            "Cpmin",
            "Chinge",
            "XCp"
    };

    // Create an input filestream
    std::ifstream input_file(filename);

    // Check if the file is accessible
    if (!input_file.is_open()) throw std::runtime_error("Could not open the file");

    // A string for storing lines
    std::string line;

    // Skip the first 11 lines (XFLR5 exported polar file header)
    // This is hard-coded under the assumption that the exported
    // polar file are of this format.
    for (int i = 0; i < 11; i++) {
        std::getline(input_file, line);
    }

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

    input_file.close();

    return result;
}

double get_value(
        std::map<std::string, std::vector<std::pair<double, double>>> const &lut,
        std::string const &target_param,
        double const x_value
        ) {
    double result;

    // Check if out of range
    if (x_value <= lut.at(target_param).front().first) {
        return lut.at(target_param).front().second;
    } else if (x_value >= lut.at(target_param).back().first) {
        return lut.at(target_param).back().second;
    }

    // Check for data
    // Keep track of the last index
    int last_idx = 0;
    for (int i = 0; i < lut.at(target_param).size(); i++) {
        if (x_value == lut.at(target_param)[i].first) {
            // Found the exact x-value in the LUT
            return lut.at(target_param)[i].second;
        } else if (x_value < lut.at(target_param)[i].first) {
            // x-value is less than the current x-value in LUT
            last_idx = i;
            break;
        }
    }

    // Return the linearly interpolated data
    return (lut.at(target_param)[last_idx - 1].second + lut.at(target_param)[last_idx].second) / 2;
}

int main() {
    std::map<std::string, std::vector<std::pair<double, double>>> result = read_csv("../T1_Re0.100_M0.00_N9.0");

    std::cout << get_value(result, "CL", -0.22) << std::endl;
    return 0;
}
#include <iostream>
#include <fstream>
#include <string>
#include <map>

std::map<std::string, double> price_avg_map;

void load_price_avg_map() {
    std::ifstream file("price_avg.csv");
    std::string line, crop, avg;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        size_t comma = line.find(',');
        crop = line.substr(0, comma);
        avg = line.substr(comma + 1);
        price_avg_map[crop] = std::stod(avg);
    }
    file.close();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <crop_name>" << std::endl;
        return 1;
    }

    load_price_avg_map();

    std::string crop_name = argv[1];
    if (price_avg_map.find(crop_name) != price_avg_map.end()) {
        std::cout << crop_name << ": " << price_avg_map[crop_name] << std::endl;
    } else {
        std::cerr << "Crop not found: " << crop_name << std::endl;
        return 1;
    }

    return 0;
}
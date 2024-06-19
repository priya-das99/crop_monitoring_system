#include <iostream>
#include <fstream>
#include <chrono>
#include <windows.h>

class CropHealthMonitoringSystem {

public:
    CropHealthMonitoringSystem() : soilMoistureLevel(100.0), phLevel(12.0), irrigationSystem(false) {
        std::ofstream file("sensor_data.csv", std::ios::app);
        file << "soil_moisture_level,ph_level\n";
        file.close();
    }

    void updateCropParameters() {
        writeDataToFile(soilMoistureLevel, phLevel);
        auto startTime = std::chrono::steady_clock::now();
        while (std::chrono::steady_clock::now() - startTime < std::chrono::seconds(1)) {
            // Wait for 1 second
        }

        // Drop soil moisture level by 5% every 1 second
        soilMoistureLevel -= 5.0;
        if (soilMoistureLevel < 0.0) {
            soilMoistureLevel = 0.0;
        }

        // Drop pH level by 1 every 3 seconds
        if (std::chrono::steady_clock::now() - lastPhUpdate >= std::chrono::seconds(3)) {
            lastPhUpdate = std::chrono::steady_clock::now();
            phLevel -= 1.0;
            if (phLevel < 0.0) {
                phLevel = 0.0;
            }
        }

        // Update JSON data
        updateJsonData();
    }

    void checkCropHealth() {
        std::cout << "Soil Moisture Level: " << soilMoistureLevel << std::endl;
        std::cout << "pH Level: " << phLevel << std::endl;
        writeDataToFile(soilMoistureLevel, phLevel);
        auto startTime = std::chrono::steady_clock::now();
        while (std::chrono::steady_clock::now() - startTime < std::chrono::seconds(1)) {
            // Wait for 1 second
        }

        if (soilMoistureLevel < 30.0) {
            std::cout << "Alert: Soil moisture level is below threshold!" << std::endl;
            Beep(2500, 1000);
            // PlaySound(TEXT("soilMoisture.mp3"), NULL, SND_SYNC);
            startIrrigationSystem();
        }
        if (phLevel < 4.0) {
            std::cout << "Alert: pH level is below threshold!" << std::endl;
            Beep(2500, 1000);
            // PlaySound(TEXT("phLevel.mp3"), NULL, SND_SYNC);
            giveFertilizers();
        }

        // Update JSON data
        updateJsonData();
    }

    void startIrrigationSystem() {
        std::cout << "Irrigation system started!" << std::endl;

        auto startTime = std::chrono::steady_clock::now();
        while (std::chrono::steady_clock::now() - startTime < std::chrono::seconds(3)) {
            // Wait for 3 seconds
        }

        soilMoistureLevel = 100.0;
        std::cout << "Irrigation system stopped!" << std::endl;

        // Update JSON data
        updateJsonData();
    }

    void giveFertilizers() {
        std::cout << "Sprinking Fertilizers!" << std::endl;
        auto startTime = std::chrono::steady_clock::now();
        while (std::chrono::steady_clock::now() - startTime < std::chrono::seconds(5)) {
            // Wait for 5 seconds
        }
        phLevel = 12.0;
        std::cout << "Fertilizers stopped!" << std::endl;

        // Update JSON data
        updateJsonData();
    }

private:
    double soilMoistureLevel;
    double phLevel;
    bool irrigationSystem;
    std::chrono::steady_clock::time_point lastPhUpdate;

    // Function to update JSON data
    void updateJsonData() {
        std::ofstream outFile("data.json");
        outFile << "{\n";
        outFile << "  \"soil_moisture_level\": " << soilMoistureLevel << ",\n";
        outFile << "  \"ph_level\": " << phLevel << "\n";
        outFile << "}\n";
    }

    void writeDataToFile(float soilMoistureLevel, float phLevel){
        std::ofstream file("sensor_data.csv", std::ios::app);
        file << soilMoistureLevel << "," << phLevel << "\n";
        file.close();
    }
};

int main() {
    CropHealthMonitoringSystem system;

    while (true) {
        system.updateCropParameters();
        system.checkCropHealth();
    }

    return 0;
}

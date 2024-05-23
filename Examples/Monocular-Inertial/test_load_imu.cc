#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <opencv2/core.hpp>

using namespace std;

// Function to load IMU data from a CSV file
bool LoadIMUCsvData(const string &path_to_imu_file,
                    vector<double> &vTimeStamps,
                    vector<cv::Point3f> &vAcc,
                    vector<cv::Point3f> &vGyro) {

    std::ifstream file(path_to_imu_file.c_str());
    if (!file.is_open()) {
        cerr << "Failed to open IMU data file at: " << path_to_imu_file << endl;
        return false;
    }

    string line;
    getline(file, line); // Skip the header

    std::map<double, cv::Point3f> sorted_acc;
    std::map<double, cv::Point3f> sorted_gyr;

    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        vector<string> tokens;

        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        double timestamp = stod(tokens[0]);
        cv::Point3f accel(stof(tokens[1]), stof(tokens[2]), stof(tokens[3]));
        cv::Point3f gyro(stof(tokens[4]), stof(tokens[5]), stof(tokens[6]));

        sorted_acc.insert(std::make_pair(timestamp, accel));
        sorted_gyr.insert(std::make_pair(timestamp, gyro));
    }

    file.close();

    if (sorted_acc.empty() || sorted_gyr.empty()) {
        cerr << "No IMU data found in the file." << endl;
        return false;
    }

    double imu_start_t = sorted_acc.begin()->first;
    for (const auto &acc : sorted_acc) {
        vTimeStamps.push_back(acc.first - imu_start_t);
        vAcc.push_back(acc.second);
    }
    for (const auto &gyr : sorted_gyr) {
        vGyro.push_back(gyr.second);
    }

    return true;
}

// Main function for testing
int main(int argc, char** argv) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <path_to_imu_csv>" << endl;
        return -1;
    }

    string path_to_imu_file = argv[1];
    vector<double> vTimeStamps;
    vector<cv::Point3f> vAcc;
    vector<cv::Point3f> vGyro;

    if (!LoadIMUCsvData(path_to_imu_file, vTimeStamps, vAcc, vGyro)) {
        cerr << "Failed to load IMU data." << endl;
        return -1;
    }

    // Output the loaded data
    cout << "Loaded IMU data:" << endl;
    cout << "TimeStamps (relative to first): " << endl;
    for (const auto &timestamp : vTimeStamps) {
        cout << timestamp << " ";
    }
    cout << endl;

    cout << "Accelerometer data: " << endl;
    for (const auto &acc : vAcc) {
        cout << "[" << acc.x << ", " << acc.y << ", " << acc.z << "] ";
    }
    cout << endl;

    cout << "Gyroscope data: " << endl;
    for (const auto &gyro : vGyro) {
        cout << "[" << gyro.x << ", " << gyro.y << ", " << gyro.z << "] ";
    }
    cout << endl;

    return 0;
}

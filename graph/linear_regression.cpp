#include <fstream>
#include <vector>
#include <cmath>
#include <sstream>
#include <iostream>
#include <locale>

struct DataPoint {
    double complexity;
    double time;
};

// Custom numpunct facet to use comma as decimal separator
struct CommaDecimalSeparator : std::numpunct<char> {
protected:
    virtual char do_decimal_point() const override {
        return ',';
    }
};

// Function to parse the input file
std::vector<DataPoint> parseInputFile(const std::string& filename) {
    std::vector<DataPoint> data;
    std::ifstream infile(filename);
    std::cout << filename << std::endl;

    if (!infile.is_open()) {
        throw std::runtime_error("Could not open file");
    }

    std::string line;
    while (std::getline(infile, line)) {
        if (line.find("Complexity Value") != std::string::npos) {
            // Extract complexity value
            size_t start = line.find('(') + 1;
            size_t end = line.find(')');
            double complexity = std::stod(line.substr(start, end - start));

            // Read next line for time
            if (!std::getline(infile, line)) {
                break;
            }

            start = line.find("Time taken: ") + 12;
            end = line.find(" milliseconds");
            double time = std::stod(line.substr(start, end - start));

            data.push_back(DataPoint{complexity, time});
        }
    }

    infile.close();
    return data;
}

// Function to perform linear regression
std::pair<double, double> linearRegression(const std::vector<DataPoint>& data) {
    double sumX = 0, sumY = 0, sumXY = 0, sumXX = 0;
    size_t n = data.size();

    for (size_t i = 0; i < n; i++) {
        sumX += data[i].complexity;
        sumY += data[i].time;
        sumXY += data[i].complexity * data[i].time;
        sumXX += data[i].complexity * data[i].complexity;
    }

    double slope = (n * sumXY - sumX * sumY) / (n * sumXX - sumX * sumX);
    double intercept = (sumY - slope * sumX) / n;

    return std::make_pair(slope, intercept);
}

// Function to write the output to a file
void writeOutputFile(const std::string& filename, const std::vector<DataPoint>& data, double slope, double intercept) {
    std::ofstream outfile(filename);

    if (!outfile.is_open()) {
        throw std::runtime_error("Could not open output file");
    }

    // Set locale with custom decimal separator
    outfile.imbue(std::locale(std::locale(), new CommaDecimalSeparator));

    outfile << "Complexity\tTime\tPredicted\n";
    for (size_t i = 0; i < data.size(); i++) {
        double predicted = slope * data[i].complexity + intercept;
        outfile << data[i].complexity << "\t" << data[i].time << "\t" << predicted << "\n";
    }

    outfile << "\nLinear Regression: y = " << slope << "x + " << intercept << "\n";
    outfile.close();
}

int main() {
    try {
        std::string inputFilename = "output.txt";
        std::string outputFilename = "output2.txt";

        // Parse input file
        std::vector<DataPoint> data = parseInputFile(inputFilename);

        // Perform linear regression
        std::pair<double, double> regressionResult = linearRegression(data);
        double slope = regressionResult.first;
        double intercept = regressionResult.second;

        // Write output file
        writeOutputFile(outputFilename, data, slope, intercept);

        std::cout << "Output written to " << outputFilename << "\n";
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
    }

    return 0;
}

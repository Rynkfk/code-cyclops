#include <iostream>
#include <cmath>
#include <random>
#include <iomanip>
#include <fstream>

void inputData(double &initialPrice, double &expectedReturn, double &volatility, int &n, double &dt);
void simulateGBM(double initialPrice, double expectedReturn, double volatility, int n, double dt, double arrp[], double &price);
void workSummary(double arrp[], int n, double &highestPrice, double &lowestPrice, double &avg);
void exportCSV(double arrp[], int n);
void monteCarloSimulation(double initialPrice, double expectedReturn, double volatility, int n, double dt);

using namespace std;
int main () {

    cout << fixed << setprecision(2);

    char answer = 'y';
    while (answer == 'y') {

        int choice;
        cout << "1. Single GBM Simulation\n";
        cout << "2. Monte Carlo Simulation\n";
        cout << "Choice: ";
        cin >> choice;

        double initialPrice; double expectedReturn, volatility;   // initial stock price = St, expectedReturn(drift) = μ, volatility = σ
        int n;   // trading days
        double dt;  // Δt

        inputData(initialPrice, expectedReturn, volatility, n,dt);

        if (choice == 1) {

            cout << "=========================================================\n";
            cout << "      STOCK PRICE SIMULATION USING\n";
            cout << "      GEOMETRIC BROWNIAN MOTION (GBM)\n";
            cout << "=========================================================\n\n";

            double arrp[n+1]; double price;
            simulateGBM(initialPrice, expectedReturn, volatility, n, dt, arrp, price);

            double highestPrice, lowestPrice, avg;
            workSummary(arrp, n, highestPrice,lowestPrice,avg);

            exportCSV(arrp, n);   // CSV for future python project continuation


            cout << "SUMMARY" << endl;
            cout << "-----------------------------------------\n";
            cout << "Initial price: " << initialPrice << endl;
            cout << "Final price: " << price << endl;
            cout << "Highest price: " << highestPrice << endl;
            cout << "Lowest price: " << lowestPrice << endl;
            cout << "Average price: " << avg << endl << endl;
            cout << "-----------------------------------------\n";
        }

        else if (choice == 2)
        {
            cout << "=========================================================\n";
            cout << "            MONTE CARLO SIMULATION\n";
            cout << "=========================================================\n\n";

            monteCarloSimulation(initialPrice, expectedReturn, volatility, n, dt);
        }

        cout << "Do you want to make another simulation? y/n:";
        cin >> answer;
    }

    return 0;
}

void inputData(double &initialPrice, double &expectedReturn, double &volatility, int &n, double &dt) {

    cout << "Simulation Parameters\n";
    cout << "-----------------------------------------\n";
    cout << "Enter initial stock price: " << endl; cin >> initialPrice;
    cout << "Enter expected annual return (%): " << endl; cin >> expectedReturn; expectedReturn /= 100.0;
    cout << "Enter volatility (%): " << endl; cin >> volatility; volatility /= 100.0;
    cout << "Enter number of trading days: " << endl; cin >> n; dt = 1.0/n;  // Δt
    cout << "-----------------------------------------\n\n";

}

void simulateGBM(double initialPrice, double expectedReturn, double volatility, int n, double dt, double arrp[], double &price) {

    cout << "Day" << setw(15);
    cout << "Price" << endl;
    cout << "---------------------" << endl;

    random_device rd;
    mt19937 gen(rd());
    normal_distribution<double> dis(0.0, 1.0);

    price = initialPrice;
    arrp[0] = initialPrice;
    cout << 0 << setw(15);
    cout << price << endl;

    for (int i=1; i<=n; i++) {

        double z = dis(gen);
        price = price * exp((expectedReturn - 0.5*pow(volatility,2))*dt + volatility * sqrt(dt) * z);
        arrp[i] = price;
        cout << i << setw(15);
        cout << price <<  endl;
    }
    cout << endl;
}

void workSummary(double arrp[], int n, double &highestPrice, double &lowestPrice, double &avg) {

    highestPrice = arrp[0];
    lowestPrice = arrp[0];
    double sum = 0;

    for (int i=0; i<=n; i++) {

        if (arrp[i] > highestPrice) {
            highestPrice = arrp[i];
        }
        if (arrp[i] < lowestPrice) {
            lowestPrice = arrp[i];
        }
        sum += arrp[i];
    }
    avg = sum / (n+1);
}

void exportCSV(double arrp[], int n) {

    ofstream file("simulation.csv");

    file << "Day,Price\n";

    for (int i = 0; i <= n; i++)
    {
        file << i << "," << arrp[i] << endl;
    }

    file.close();

    cout << "Simulation exported to simulation.csv\n";
}

void monteCarloSimulation(double initialPrice, double expectedReturn, double volatility, int n, double dt) {

    int simulations;

    cout << "Number of simulations: ";
    cin >> simulations;

    double lowerTarget, upperTarget;

    cout << "Enter lower target price: ";
    cin >> lowerTarget;

    cout << "Enter upper target price: ";
    cin >> upperTarget;

    random_device rd;
    mt19937 gen(rd());
    normal_distribution<double> dis(0.0, 1.0);

    double highestFinal = initialPrice;
    double lowestFinal = initialPrice;
    double totalFinal = 0;

    double finalPrices[simulations];

    int lowerCount = 0;
    int upperCount = 0;
    int successCount = 0;

    for (int sim = 1; sim <= simulations; sim++)
    {
        double price = initialPrice;

        for (int day = 1; day <= n; day++)
        {
            double z = dis(gen);

            price = price * exp(
                (expectedReturn - 0.5 * volatility * volatility) * dt
                + volatility * sqrt(dt) * z
            );
        }

        finalPrices[sim - 1] = price;

        if (sim == 1)
        {
            highestFinal = price;
            lowestFinal = price;
        }

        if (price > highestFinal)
            highestFinal = price;

        if (price < lowestFinal)
            lowestFinal = price;

        totalFinal += price;

        // Probability counters
        if (price < lowerTarget)
            lowerCount++;

        if (price > upperTarget)
            upperCount++;

        if (price >= lowerTarget && price <= upperTarget)
            successCount++;
    }

    double averageFinal = totalFinal / simulations;

    // Standard deviation
    double variance = 0;

    for (int i = 0; i < simulations; i++)
    {
        variance += pow(finalPrices[i] - averageFinal, 2);
    }

    variance /= (simulations - 1);

    double standardDeviation = sqrt(variance);

    // 95% Confidence Interval
    double margin = 1.96 * standardDeviation / sqrt(simulations);

    double lowerCI = averageFinal - margin;
    double upperCI = averageFinal + margin;

    // Probabilities
    double lowerProbability = (double)lowerCount / simulations * 100.0;
    double upperProbability = (double)upperCount / simulations * 100.0;
    double successRate = (double)successCount / simulations * 100.0;

    cout << "\nMONTE CARLO RESULTS\n";
    cout << "--------------------------------------\n";
    cout << "Simulations              : " << simulations << endl;
    cout << "Average Final Price      : " << averageFinal << endl;
    cout << "Highest Final Price      : " << highestFinal << endl;
    cout << "Lowest Final Price       : " << lowestFinal << endl;
    cout << "Standard Deviation       : " << standardDeviation << endl;
    cout << endl;
    cout << "Probability < " << lowerTarget << " : "
         << lowerProbability << "%" << endl;
    cout << "Probability > " << upperTarget << " : "
         << upperProbability << "%" << endl;
    cout << "Success Rate             : "
         << successRate << "%" << endl;
    cout << endl;
    cout << "95% Confidence Interval" << endl;
    cout << "[" << lowerCI << ", " << upperCI << "]" << endl;
}

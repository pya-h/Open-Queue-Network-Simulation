#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <queue>

using namespace std;

class QueueSimulator {
public:
    QueueSimulator(double landa1, double mu1, double p21, double mu2, double mu3)
        : landa1(landa1), mu1(mu1), p21(p21), mu2(mu2), mu3(mu3),
        clock(0.0), nextArrivalTime1(generateExponentialTime(landa1)),
        nextDepartureTime1(numeric_limits<double>::infinity()),
        nextDepartureTime2(numeric_limits<double>::infinity()),
        nextDepartureTime3(numeric_limits<double>::infinity()) {}

    void runSimulation(int numEvents);
    void calculatePerformanceMeasures();

private:
    double generateExponentialTime(double mu);
    void handleArrival1();
    void handleDeparture1();
    void handleDeparture2();
    void handleDeparture3();

    double landa1;  // Arrival rate of the first queue
    double mu1;      // Service rate of the first queue
    double p21;      // Probability of connection from the first to the second queue
    double mu2;      // Service rate of the second queue
    double mu3;      // Service rate of the third queue

    double clock;   // Current simulation time
    double nextArrivalTime1;
    double nextDepartureTime1;
    double nextDepartureTime2;
    double nextDepartureTime3;

    queue<pair<double, double>> queue1;  // Pair of entry time and departure time for the first queue
    queue<pair<double, double>> queue2;  // Pair of entry time and departure time for the second queue
    queue<pair<double, double>> queue3;  // Pair of entry time and departure time for the third queue

    // Variables for performance measures
    double totalQueueLength1;
    double totalQueueLength2;
    double totalQueueLength3;
    double totalSystemTime;
    double lastEventTime;
    int numEvents;
};

double QueueSimulator::generateExponentialTime(double rate) {
    return -log(1.0 - ((double)rand() / RAND_MAX)) / rate;
}

void QueueSimulator::handleArrival1() {
    clock = nextArrivalTime1;
    queue1.push({ clock, -1.0 }); // Entry time is current time, departure time is set to -1 for tracking

    nextArrivalTime1 = clock + generateExponentialTime(landa1);

    if (queue1.size() == 1) {
        nextDepartureTime1 = clock + generateExponentialTime(mu1);
    }
}

void QueueSimulator::handleDeparture1() {
    clock = nextDepartureTime1;
    queue1.front().second = clock; // Set departure time for the customer

    double randNum = (double)rand() / RAND_MAX;

    if (randNum < p21) {
        // Connect to the second queue
        queue2.push({ clock, -1.0 });

        if (queue2.size() == 1) {
            nextDepartureTime2 = clock + generateExponentialTime(mu2);
        }
    }
    else {
        // Connect to the third queue
        queue3.push({ clock, -1.0 });

        if (queue3.size() == 1) {
            nextDepartureTime3 = clock + generateExponentialTime(mu3);
        }
    }

    queue1.pop();

    if (!queue1.empty()) {
        nextDepartureTime1 = clock + generateExponentialTime(mu1);
    }
    else {
        nextDepartureTime1 = numeric_limits<double>::infinity();
    }
}

void QueueSimulator::handleDeparture2() {
    clock = nextDepartureTime2;
    queue2.front().second = clock; // Set departure time for the customer
    queue2.pop();

    if (!queue2.empty()) {
        nextDepartureTime2 = clock + generateExponentialTime(mu2);
    }
    else {
        nextDepartureTime2 = numeric_limits<double>::infinity();
    }
}

void QueueSimulator::handleDeparture3() {
    clock = nextDepartureTime3;
    queue3.front().second = clock; // Set departure time for the customer
    queue3.pop();

    if (!queue3.empty()) {
        nextDepartureTime3 = clock + generateExponentialTime(mu3);
    }
    else {
        nextDepartureTime3 = numeric_limits<double>::infinity();
    }
}

void QueueSimulator::runSimulation(int numEvents) {
    this->numEvents = numEvents;
    totalQueueLength1 = 0.0;
    totalQueueLength2 = 0.0;
    totalQueueLength3 = 0.0;
    totalSystemTime = 0.0;
    lastEventTime = 0.0;

    for (int i = 0; i < numEvents; ++i) {

        if (nextArrivalTime1 < nextDepartureTime1) {
            handleArrival1();
        }

        if (nextDepartureTime1 < nextDepartureTime2 && nextDepartureTime1 < nextDepartureTime3) {
            handleDeparture1();
        }
        else if (nextDepartureTime2 < nextDepartureTime3) {
            handleDeparture2();
        }
        else {
            handleDeparture3();
        }

        // Track performance measures during the simulation
        totalQueueLength1 += queue1.size();
        totalQueueLength2 += queue2.size();
        totalQueueLength3 += queue3.size();
        totalSystemTime += ((double)queue1.size() + (double)queue2.size() + (double)queue3.size()) * (clock - lastEventTime);
        lastEventTime = clock;
    }
}

void QueueSimulator::calculatePerformanceMeasures() {
    double averageW = totalSystemTime / numEvents;
    // Queue 1 measures
    double averageL1 = totalQueueLength1 / clock;
    double averageLQ1 = averageL1 - landa1 / mu1; // Little's Law
    double averageW1 = totalQueueLength1 / numEvents;
    double averageWQ1 = averageW1 - 1.0 / mu1; // Little's Law
    double rho1 = averageL1 / mu1;

    cout << "Queue 1:" << endl;
    cout << "Number of Customers: " << totalQueueLength1 << endl;
    cout << "Average Number of Customers (L1): " << averageL1 << endl;
    cout << "Average Number of Customers in Queue (LQ1): " << averageLQ1 << endl;
    cout << "Average Time in System (W1): " << averageW1 << endl;
    cout << "Average Time in Queue (WQ1): " << averageWQ1 << endl;
    cout << "Traffic Intensity (rho1): " << rho1 << endl;

    // Queue 2 measures
    double averageL2 = totalQueueLength2 / clock;
    double averageLQ2 = averageL2 - landa1 * (1 - p21) / mu2; // Little's Law
    double averageW2 = totalQueueLength2 / numEvents;
    double averageWQ2 = averageW2 - 1.0 / mu2; // Little's Law
    double rho2 = averageL2 / mu2;

    cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -" << endl;
    cout << "Queue 2:" << endl;
    cout << "Number of Customers: " << totalQueueLength2 << endl;
    cout << "Average Number of Customers (L2): " << averageL2 << endl;
    cout << "Average Number of Customers in Queue (LQ2): " << averageLQ2 << endl;
    cout << "Average Time in System (W2): " << averageW2 << endl;
    cout << "Average Time in Queue (WQ2): " << averageWQ2 << endl;
    cout << "Traffic Intensity (rho2): " << rho2 << endl;

    // Queue 3 measures
    double averageL3 = totalQueueLength3 / clock;
    double averageLQ3 = averageL3 - landa1 * p21 / mu3; // Little's Law
    double averageW3 = totalQueueLength3 / numEvents;
    double averageWQ3 = averageW3 - 1.0 / mu3; // Little's Law
    double rho3 = averageL3 / mu3;

    cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -" << endl;
    cout << "Queue 3:" << endl;
    cout << "Number of Customers: " << totalQueueLength3 << endl;
    cout << "Average Number of Customers (L3): " << averageL3 << endl;
    cout << "Average Number of Customers in Queue (LQ3): " << averageLQ3 << endl;
    cout << "Average Time in System (W3): " << averageW3 << endl;
    cout << "Average Time in Queue (WQ3): " << averageWQ3 << endl;
    cout << "Traffic Intensity (rho3): " << rho3 << endl << endl;

    double totalResponseTime = 0.0;
    double totalCustomers = totalQueueLength1 + totalQueueLength2 + totalQueueLength3;

    // Sum up the response times from each queue
    for (; !queue1.empty(); queue1.pop()) {
        const auto& customer = queue1.front();
        totalResponseTime += customer.first - customer.second; // Departure time - Entry time
    }

    for (; !queue2.empty(); queue2.pop()) {
        const auto& customer = queue2.front();
        totalResponseTime += customer.first - customer.second; // Departure time - Entry time

    }
    for (; !queue3.empty(); queue3.pop()) {
        const auto& customer = queue3.front();
        totalResponseTime += customer.first - customer.second; // Departure time - Entry time

    }
    double averageResponseTime = totalResponseTime / totalCustomers;
    cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -" << endl;
    cout << "Network-wide Measures:" << endl;
    cout << "Average Response Time (R): " << averageResponseTime << endl;
    cout << "Average Time in System (W): " << averageW << endl;
    cout << "Total Time in System (T): " << totalSystemTime << endl;
    cout << "Average Number of Customers in the Network (N): \n\t\tL1 + L2 + L3 = " << averageL1 + averageL2 + averageL3 << endl;
    cout << "\t\t[Number of all customers] / time = " << totalCustomers / clock << endl;
    cout << "Average Number of Customers in Queuees: LQ1 + LQ2 + LQ3 = " << averageLQ1 + averageLQ2 + averageLQ3 << endl;

}

int main() {
    srand(time(0)); // Seed for random number generation

    // Simulation parameters
    double landa1 = 1.0; // Arrival rate of the first queue
    double mu1 = 2.0;    // Service rate of the first queue
    double p21 = 0.4;    // Probability of connection from the first to the second queue
    double mu2 = 4.0;    // Service rate of the second queue
    double mu3 = 3.0;    // Service rate of the third queue

    // Create and run simulation
    QueueSimulator simulator(landa1, mu1, p21, mu2, mu3);
    simulator.runSimulation(10000); // Run for a large number of events for stability
    simulator.calculatePerformanceMeasures();

    return 0;
}

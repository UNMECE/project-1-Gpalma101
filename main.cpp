#include <iostream>
#include <iomanip>
#include "capacitor.h"

static const double DT = 1e-10;      
static const double FINAL_TIME = 5e-6; //s
static const int TIMESTEPS = static_cast<int>(FINAL_TIME / DT); // 50000
static const double R = 1e3;         // ohms
static const double C_VALUE = 100e-12; // F

bool allocate_capacitor(Capacitor &cap, int size)
{
    cap.time = new (std::nothrow) double[size];
    cap.voltage = new (std::nothrow) double[size];
    cap.current = new (std::nothrow) double[size];

    if (!cap.time || !cap.voltage || !cap.current)
    {
        delete[] cap.time;
        delete[] cap.voltage;
        delete[] cap.current;
        cap.time = nullptr;
        cap.voltage = nullptr;
        cap.current = nullptr;
        return false;
    }

    return true;
}

void free_capacitor(Capacitor &cap)
{
    delete[] cap.time;
    delete[] cap.voltage;
    delete[] cap.current;
    cap.time = nullptr;
    cap.voltage = nullptr;
    cap.current = nullptr;
}

void simulate_constant_current(Capacitor &cap, int size, double source_current)
{
    cap.C = C_VALUE;

    cap.time[0] = 0.0;
    cap.voltage[0] = 0.0;          // initial condition
    cap.current[0] = source_current;

    for (int n = 1; n < size; n++)
    {
        cap.time[n] = n * DT;
        cap.current[n] = source_current;
        cap.voltage[n] = cap.voltage[n - 1] + cap.current[n - 1] * DT * (1.0 / cap.C);
    }
}

void simulate_constant_voltage(Capacitor &cap, int size, double V0)
{
    cap.C = C_VALUE;

    cap.time[0] = 0.0;
    cap.current[0] = V0 / R;       // initial condition
    cap.voltage[0] = 0.0;          // capacitor initially uncharged

    for (int n = 1; n < size; n++)
    {
        cap.time[n] = n * DT;
        cap.current[n] = cap.current[n - 1] - (cap.current[n - 1] / (R * cap.C)) * DT;
        cap.voltage[n] = V0 - cap.current[n] * R;
    }
}

void print_every_200(const Capacitor &cap, int size, const char *title)
{
    std::cout << "\n" << title << "\n";
    std::cout << "step\ttime(s)\t\tcurrent(A)\tvoltage(V)\n";

    for (int n = 0; n < size; n += 200)
    {
        std::cout << n << "\t"
                  << std::scientific << std::setprecision(6)
                  << cap.time[n] << "\t"
                  << cap.current[n] << "\t"
                  << cap.voltage[n] << "\n";
    }

    if ((size - 1) % 200 != 0)
    {
        int n = size - 1;
        std::cout << n << "\t"
                  << std::scientific << std::setprecision(6)
                  << cap.time[n] << "\t"
                  << cap.current[n] << "\t"
                  << cap.voltage[n] << "\n";
    }
}

int main()
{
    const int size = TIMESTEPS + 1; // include t = 0

    Capacitor constant_current_cap{};
    Capacitor constant_voltage_cap{};

    if (!allocate_capacitor(constant_current_cap, size) || !allocate_capacitor(constant_voltage_cap, size))
    {
        std::cerr << "Error: dynamic memory allocation failed.\n";
        free_capacitor(constant_current_cap);
        free_capacitor(constant_voltage_cap);
        return 1;
    }

    simulate_constant_current(constant_current_cap, size, 1e-2);
    simulate_constant_voltage(constant_voltage_cap, size, 10.0);

    print_every_200(constant_current_cap, size, "Constant current source (I = 1e-2 A)");
    print_every_200(constant_voltage_cap, size, "Constant voltage source (V0 = 10 V, R = 1k Ohm)");

    free_capacitor(constant_current_cap);
    free_capacitor(constant_voltage_cap);

    return 0;
}

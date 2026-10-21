#include "battery.h"
#include <cmath>

Battery::Battery() : cell_count(4), voltage(0.0), capacity_mAh(5000.0) {}

Battery::Battery(int cells)
    : cell_count(cells > 0 ? cells : 1), voltage(0.0), capacity_mAh(5000.0) {}

Battery::Battery(int cells, double initial_voltage, double capacity)
    : cell_count(cells > 0 ? cells : 1),
    voltage(std::isfinite(initial_voltage) && initial_voltage >= 0.0
                ? initial_voltage : 0.0),
    capacity_mAh(std::isfinite(capacity) && capacity >= 0.0 ? capacity : 0.0) {}

double Battery::get_percentage() const {
    const double min_v = get_min_voltage();
    const double max_v = get_max_voltage();
    const double result = (voltage - min_v) / (max_v - min_v) * 100.0;
    if (result < 0.0) return 0.0;
    if (result > 100.0) return 100.0;
    return result;
}

bool Battery::is_low() const {
    return get_percentage() < 20.0;
}

QString Battery::get_status() const {
    const double p = get_percentage();
    if (p < 10.0) return QStringLiteral("CRITICAL");
    if (p < 20.0) return QStringLiteral("LOW");
    if (p <= 80.0) return QStringLiteral("NORMAL");
    return QStringLiteral("FULL");
}

void Battery::set_voltage(double new_voltage) {
    if (!std::isfinite(new_voltage) || new_voltage < 0.0)
        return;                       // молча игнорируем (как раньше)
    voltage = new_voltage;
}

bool Battery::set_voltage_checked(double new_voltage) {
    if (!std::isfinite(new_voltage) || new_voltage < 0.0)
        return false;
    voltage = new_voltage;
    return true;
}

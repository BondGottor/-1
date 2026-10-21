#ifndef BATTERY_H
#define BATTERY_H

#include <QString>

class Battery {
private:
    int cell_count;
    double voltage;
    double capacity_mAh;

public:
    Battery();
    explicit Battery(int cells);
    Battery(int cells, double initial_voltage, double capacity);

    double get_percentage() const;
    bool is_low() const;
    QString get_status() const;

    void set_voltage(double new_voltage);
    bool set_voltage_checked(double new_voltage);   // вариант с возвратом успеха

    int get_cell_count() const { return cell_count; }
    double get_voltage() const { return voltage; }
    double get_capacity() const { return capacity_mAh; }

    double get_min_voltage() const { return 3.0 * cell_count; }
    double get_max_voltage() const { return 4.2 * cell_count; }
};

#endif // BATTERY_H
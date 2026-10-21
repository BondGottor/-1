#ifndef BATTERYMONITOR_H
#define BATTERYMONITOR_H

#include <QWidget>
#include "battery.h"

namespace Ui { class BatteryMonitor; }

class BatteryMonitor : public QWidget {
    Q_OBJECT

public:
    explicit BatteryMonitor(QWidget *parent = nullptr);
    ~BatteryMonitor() override;

private slots:
    void onVoltageChangedBySlider(int value);
    void onVoltageChangedBySpin(double value);
    void onResetClicked();

private:
    void applyVoltage(double v);
    void refreshUi();

    Ui::BatteryMonitor *ui;
    Battery m_battery;
};

#endif // BATTERYMONITOR_H
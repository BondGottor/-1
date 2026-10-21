#include "batterymonitor.h"
#include "ui_batterymonitor.h"

#include <QString>
#include <QColor>

BatteryMonitor::BatteryMonitor(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::BatteryMonitor),
    m_battery(4, 16.8, 5200.0)          // стартуем с полного заряда 4S
{
    ui->setupUi(this);

    // Начальная настройка диапазонов под конкретную батарею
    const double vmin = m_battery.get_min_voltage();
    const double vmax = m_battery.get_max_voltage();

    ui->sliderVoltage->setMinimum(int(vmin * 100));
    ui->sliderVoltage->setMaximum(int(vmax * 100));
    ui->sliderVoltage->setValue(int(m_battery.get_voltage() * 100));

    ui->spinVoltage->setMinimum(vmin);
    ui->spinVoltage->setMaximum(vmax);
    ui->spinVoltage->setValue(m_battery.get_voltage());

    ui->lblCellInfo->setText(
        QString("%1S, %2 mAh")
            .arg(m_battery.get_cell_count())
            .arg(m_battery.get_capacity(), 0, 'f', 0));

    // Связи сигналов и слотов
    connect(ui->sliderVoltage, &QSlider::valueChanged,
            this, &BatteryMonitor::onVoltageChangedBySlider);
    connect(ui->spinVoltage, qOverload<double>(&QDoubleSpinBox::valueChanged),
            this, &BatteryMonitor::onVoltageChangedBySpin);
    connect(ui->btnReset, &QPushButton::clicked,
            this, &BatteryMonitor::onResetClicked);

    refreshUi();
}

BatteryMonitor::~BatteryMonitor() {
    delete ui;
}

void BatteryMonitor::onVoltageChangedBySlider(int value) {
    const double v = value / 100.0;
    // Синхронизируем спинбокс, не вызывая повторной рекурсии —
    // блокируем сигналы на время программного изменения
    QSignalBlocker blocker(ui->spinVoltage);
    ui->spinVoltage->setValue(v);
    applyVoltage(v);
}

void BatteryMonitor::onVoltageChangedBySpin(double value) {
    QSignalBlocker blocker(ui->sliderVoltage);
    ui->sliderVoltage->setValue(int(value * 100));
    applyVoltage(value);
}

void BatteryMonitor::onResetClicked() {
    const double vmax = m_battery.get_max_voltage();
    ui->sliderVoltage->setValue(int(vmax * 100));   // вызовет onVoltageChangedBySlider
    ui->spinVoltage->setValue(vmax);
    applyVoltage(vmax);
}

void BatteryMonitor::applyVoltage(double v) {
    if (!m_battery.set_voltage_checked(v)) {
        return;   // некорректное значение — молча игнорируем
    }
    refreshUi();
}

void BatteryMonitor::refreshUi() {
    const double pct   = m_battery.get_percentage();
    const QString st   = m_battery.get_status();
    const double volt  = m_battery.get_voltage();

    // Прогресс-бар и текст
    ui->progressBar->setValue(int(pct));
    ui->lblPercentage->setText(QString("%1 %").arg(pct, 0, 'f', 1));
    ui->lblVoltage->setText(QString("%1 V").arg(volt, 0, 'f', 2));

    // Статус + цвет
    ui->lblStatus->setText(st);

    QString color;
    if (pct < 10.0)      color = "#c62828";   // красный
    else if (pct < 20.0) color = "#ef6c00";   // оранжевый
    else if (pct <= 80.0) color = "#2e7d32";  // зелёный
    else                 color = "#1565c0";   // синий

    ui->lblStatus->setStyleSheet(
        QString("font-size: 28pt; font-weight: bold; color: %1;").arg(color));

    // Цвет прогресс-бара
    ui->progressBar->setStyleSheet(
        QString("QProgressBar::chunk { background-color: %1; }").arg(color));
}
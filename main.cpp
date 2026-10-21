#include "batterymonitor.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    BatteryMonitor w;
    w.show();
    return app.exec();
}

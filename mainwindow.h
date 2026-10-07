#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QSlider>
#include <QMouseEvent>
#include <QList>
#include <QPointF>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void placeDrones();
    void resetScene();

private:
    Ui::MainWindow *ui;

    QGraphicsScene *scene;
    QGraphicsEllipseItem *zone;
    QGraphicsRectItem *base;
    QList<QGraphicsEllipseItem*> drones;

    QPointF basePos;
    double radius;

    void redrawZone();
    void checkDrones();

protected:
    // фильтр событий от viewport graphicsView
    bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // MAINWINDOW_H
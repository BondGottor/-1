#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPen>
#include <QBrush>
#include <QLineF>
#include <QRandomGenerator>
#include <QMouseEvent>
#include <QEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ---------- Графическая сцена ----------
    scene = new QGraphicsScene(0, 0, 600, 400, this);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);

    // ---------- Начальное положение базы ----------
    basePos = QPointF(300, 200);

    // ---------- Начальный радиус ----------
    radius = 150;

    // ---------- Базовая станция ----------
    // Прямоугольник 20x20 с центром в начале координат,
    // позиционируем его через setPos(basePos).
    base = scene->addRect(
        -10, -10, 20, 20,
        QPen(Qt::blue),
        QBrush(Qt::blue)
        );
    base->setPos(basePos);
    base->setZValue(2);

    // ---------- Зона покрытия ----------
    zone = nullptr;
    redrawZone();

    // ---------- Ползунок радиуса ----------
    ui->sliderRadius->setMinimum(50);
    ui->sliderRadius->setMaximum(300);
    ui->sliderRadius->setValue(150);

    // ---------- Подпись ----------
    ui->labelInZone->setText("В зоне: 0 из 5");

    // ---------- Кнопки ----------
    connect(ui->btnPlace, &QPushButton::clicked,
            this, &MainWindow::placeDrones);

    connect(ui->btnReset, &QPushButton::clicked,
            this, &MainWindow::resetScene);

    // ---------- Изменение радиуса ----------
    connect(ui->sliderRadius, &QSlider::valueChanged,
            this, [this](int value)
            {
                radius = value;
                redrawZone();
                checkDrones();
            });

    // ---------- Клик по сцене (перемещение базы) ----------
    // ВАЖНО: события мыши поглощает QGraphicsView,
    // поэтому ставим фильтр на его viewport.
    ui->graphicsView->viewport()->installEventFilter(this);
    ui->graphicsView->setMouseTracking(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ============================================================
//  Фильтр событий: ловим клик по graphicsView
// ============================================================
bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->graphicsView->viewport()
        && event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *me = static_cast<QMouseEvent*>(event);
        if (me->button() == Qt::LeftButton)
        {
            // Координаты клика в системе сцены
            QPointF scenePos = ui->graphicsView->mapToScene(me->pos());

            // Перемещаем базу
            basePos = scenePos;
            base->setPos(basePos);

            // Перерисовываем зону и пересчитываем БПЛА
            redrawZone();
            checkDrones();

            return true; // событие обработано
        }
    }

    return QMainWindow::eventFilter(watched, event);
}

// ============================================================
//  Рисуем зону покрытия
// ============================================================
void MainWindow::redrawZone()
{
    if (zone != nullptr) {
        scene->removeItem(zone);
        delete zone;
        zone = nullptr;
    }

    zone = scene->addEllipse(
        basePos.x() - radius,
        basePos.y() - radius,
        radius * 2,
        radius * 2,
        QPen(Qt::blue, 2),
        QBrush(QColor(100, 150, 255, 40))
        );

    // База должна быть поверх зоны
    zone->setZValue(0);
    base->setZValue(2);
}

// ============================================================
//  Размещаем 5 БПЛА
// ============================================================
void MainWindow::placeDrones()
{
    // Удаляем старые БПЛА
    for (auto drone : drones) {
        scene->removeItem(drone);
        delete drone;
    }
    drones.clear();

    // Создаём 5 случайных БПЛА
    for (int i = 0; i < 5; ++i) {

        double x = QRandomGenerator::global()->bounded(20, 580);
        double y = QRandomGenerator::global()->bounded(20, 380);

        // Эллипс 16x16, центр в начале координат
        QGraphicsEllipseItem *drone = scene->addEllipse(
            -8, -8, 16, 16,
            QPen(Qt::black),
            QBrush(Qt::red)
            );

        drone->setPos(x, y);
        drone->setZValue(1);

        drones.append(drone);
    }

    checkDrones();
}

// ============================================================
//  Проверяем, какие БПЛА находятся в зоне
// ============================================================
void MainWindow::checkDrones()
{
    int inZone = 0;

    for (auto drone : drones) {

        // Так как прямоугольник/эллипс задан с центром в (0,0),
        // позиция drone->pos() — это и есть центр.
        QPointF dronePos = drone->pos();

        double distance = QLineF(dronePos, basePos).length();

        if (distance <= radius) {
            drone->setBrush(QBrush(Qt::green));
            ++inZone;
        } else {
            drone->setBrush(QBrush(Qt::red));
        }
    }

    ui->labelInZone->setText(
        QString("В зоне: %1 из 5").arg(inZone)
        );
}

// ============================================================
//  Сброс
// ============================================================
void MainWindow::resetScene()
{
    // Удаляем БПЛА
    for (auto drone : drones) {
        scene->removeItem(drone);
        delete drone;
    }
    drones.clear();

    // Возвращаем базу в центр
    basePos = QPointF(300, 200);
    base->setPos(basePos);

    // Возвращаем радиус
    radius = 150;
    ui->sliderRadius->setValue(150);

    redrawZone();

    ui->labelInZone->setText("В зоне: 0 из 5");
}
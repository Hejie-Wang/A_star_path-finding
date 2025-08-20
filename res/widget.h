#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QTimer>
#include "map_manager.h"
#include "a_star.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void startPathfinding();
    void loadMapFile();
    //void showMap();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateAnimation();

private:
    Ui::Widget *ui;
    QTimer *animationTimer;
    std::vector<Node*> path;
    int currentPathIndex;
    bool isAnimating;
};
#endif // WIDGET_H

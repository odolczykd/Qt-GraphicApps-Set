#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QImage>
#include <QMouseEvent>

#include <cmath>
#include <QVector>

#define EPSILON 0.0001

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QImage *img;

    // wymiary rysunku + wspolrzedne poczatku ukladu
    int width, height;
    int begX, begY;

    //
    QVector<int> x;
    QVector<int> y;
    int sel;                // indeks zaznaczonego punktu

    bool drawMode, moveMode, delMode;

    // potrzebne funkcje
    void clearImage();
    void drawBezier(QVector<int> x, QVector<int> y, int k);
    void paintPixel(int x, int y, int R, int G, int B);
    void paintAround(int x, int y, int R, int G, int B);

private slots:
    void mousePressEvent(QMouseEvent *event);
    //void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent*);
    void on_clearButton_clicked();
    void on_exitButton_clicked();
    void on_drawButton_clicked();
    void on_moveButton_clicked();
    void on_delButton_clicked();
};
#endif // MAINWINDOW_H

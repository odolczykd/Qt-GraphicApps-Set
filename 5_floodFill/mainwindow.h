#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QImage>
#include <QMouseEvent>
#include <QStack>
#include <QPair>
#include <QColorDialog>

#include <cmath>

#define EPSILON 0.001

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
    QImage *img, *imgTmp;

    // wymiary rysunku + wspolrzedne poczatku ukladu
    int width, height;
    int begX, begY;

    // wspolrzedne poczatku (x0) i konca (x1) punktow wskazanych przez uzytkownika
    int x0, y0, x1, y1;

    bool drawLineMode, drawCircleMode, drawPolygonMode, drawEllipseMode, floodFillMode;
    int howManySides;

    QColor white;
    QColor fillColor;

    // potrzebne funkcje
    void clearImage();
    void drawLine(int x0, int y0, int x1, int y1);
    void drawCircle(int x0, int y0, int x1, int y1);
    void drawEllipse(int x0, int y0, int x1, int y1);
    void drawPolygon(int x0, int y0, int x1, int y1);
    void paintPixel(int x, int y, QColor color);
    void floodFill(int x, int y, QColor curr, QColor neww);
    QColor getPixelColor(int x, int y);

private slots:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent*);
    void on_clearButton_clicked();
    void on_exitButton_clicked();
    void on_radioButtonLine_clicked();
    void on_radioButtonPolygon_clicked();
    void on_radioButtonCircle_clicked();
    void on_radioButtonEllipse_clicked();
    void on_sidesCounter_valueChanged(int arg1);
    void on_changeColorButton_clicked();
};
#endif // MAINWINDOW_H

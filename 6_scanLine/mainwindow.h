#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QImage>
#include <QMouseEvent>
#include <QColorDialog>

#include <cmath>

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

    // wspolrzedne poczatku (x0) i konca (x1) rysowanego odcinka
    int x0, y0, x1, y1;

    QVector<QPair<int,int>> vert;
    QColor fillColor, white;

    // potrzebne funkcje
    void clearImage();
    void drawLine(int x0, int y0, int x1, int y1, QColor c);
    void scanLine();
    void paintPixel(int x, int y, QColor c);
    QVector<int> findCrossPts(int y);

private slots:
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent*);
    void on_clearButton_clicked();
    void on_exitButton_clicked();
    void on_colorButton_clicked();
};
#endif // MAINWINDOW_H

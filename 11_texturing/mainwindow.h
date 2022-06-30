#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QImage>
#include <QImageReader>
#include <QSize>
#include <QMouseEvent>
#include <QFileDialog>

#include <QVector>
#include <QPair>
#include <algorithm>

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
    QImage *img, *imgT, *bliss;

    // wymiary rysunku + wspolrzedne poczatku ukladu
    int width, height;
    int begX, begY, begTX, begTY;

    QVector<QPair<int,int>> vert, vertT;
    bool isTriangleEnabled, isTriangleEnabledT;

    // sciezka do wczytanej grafiki
    QString filePath;

    // potrzebne funkcje
    QColor getColor(QImage *im, int x, int y);
    QColor bilinearInterpolation(QImage *im, double x, double y);
    void clearImage();
    void paintPixel(QImage *im, int x, int y, QColor c);
    void paintAround(QImage *im, int x, int y);
    void drawLine(QImage *im, int x0, int y0, int x1, int y1);
    int getVert(QVector<QPair<int,int>> v, int minOrMax, int which);
    void texturing();

private slots:
    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent *event);
    void on_exitButton_clicked();
    void on_resetButton_clicked();
};
#endif // MAINWINDOW_H

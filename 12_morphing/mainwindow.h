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
    QImage *imgL, *imgR, *imgW;
    QImage *imgLcp, *imgRcp;

    int width, height;
    int begLX, begLY, begRX, begRY, begWX, begWY;
    int N;

    QVector<QPair<float,float>> vertL, vertR;
    bool isTriangleEnabledL, isTriangleEnabledR;

    // sciezka do wczytanej grafiki
    QString filePath;

    // potrzebne funkcje
    void clearImage(QImage *im);
    void paintPixel(QImage *im, int x, int y, QColor c);
    void paintAround(QImage *im, int x, int y);
    void drawLine(QImage *im, int x0, int y0, int x1, int y1);
    void morphing();
    int getVert(QVector<QPair<float,float>> v, int minOrMax, int which);
    void texturing(QImage *im, QVector<QPair<float,float>> src, QImage *srcImg, QVector<QPair<float,float>> dst);
    void blend(int n, QImage *im, QImage* A, QImage *B);
    QColor bilinearInterpolation(QImage *im, double x, double y);

private slots:
    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent *event);
    void on_exitButton_clicked();
    //void on_resetButton_clicked();
    void on_imgLeftButton_clicked();
    void on_imgRightButton_clicked();
    void on_resetLButton_clicked();
    void on_resetRButton_clicked();
    void on_resetButton_clicked();
    void on_startButton_clicked();
    void on_framesSpinBox_valueChanged(int arg1);
};
#endif // MAINWINDOW_H

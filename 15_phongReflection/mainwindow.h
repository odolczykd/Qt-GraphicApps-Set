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
#include <QList>
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

    QImage *img;
    QImage *txtrFront, *txtrBack, *txtrTop, *txtrBottom, *txtrLeft, *txtrRight;

    int width, height, widthT, heightT;
    int begX, begY;
    int d;

    QVector3D observer;
    QVector3D lightBulb1, lightBulb2;
    QVector<QVector3D> cube;
    QVector<float> T, Rx, Ry, Rz, S, Tr;

    void setCube();
    void resetTrMatrix();
    void resetMatrices();
    void setTranslation(int tx, int ty, int tz);
    void setScaling(float sx, float sy, float sz);
    void setRotationX(int alpha);
    void setRotationY(int alpha);
    void setRotationZ(int alpha);
    QVector<float> multiply(QVector<float> A, QVector<float> B);
    float dotProduct(QVector3D A, QVector3D B);
    QVector3D crossProduct(QVector3D A, QVector3D B);
    void transformAll();
    void paintPixel(int x, int y, QColor c);
    void clearImage();
    int findMinMax(int which, int A, int B, int C);
    void drawLine(int x0, int y0, int x1, int y1, QColor c);
    void texture(QPair<int,int> A, QPair<int,int> B, QPair<int,int> C, QPair<int,int> At, QPair<int,int> Bt, QPair<int,int> Ct, QImage *txtr);
    void phongShading();
    QColor bilinearInterpolation(QImage *im, double x, double y);

private slots:
    void paintEvent(QPaintEvent*);
    void on_resetButton_clicked();
    void on_exitButton_clicked();
    void on_distanceSlider_valueChanged(int value);
    void on_translationXSlider_valueChanged(int value);
    void on_translationYSlider_valueChanged(int value);
    void on_translationZSlider_valueChanged(int value);
    void on_rotationXSlider_valueChanged(int value);
    void on_rotationYSlider_valueChanged(int value);
    void on_rotationZSlider_valueChanged(int value);
    void on_scalingXSlider_valueChanged(int value);
    void on_scalingYSlider_valueChanged(int value);
    void on_scalingZSlider_valueChanged(int value);
};
#endif // MAINWINDOW_H

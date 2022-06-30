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

    int width, height;
    int begX, begY;
    int d;

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
    void transformAll();
    void paintPixel(int x, int y);
    void clearImage();
    void drawLine(int x0, int y0, int x1, int y1);

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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QImage>
#include <QImageReader>
#include <QSize>
#include <QPair>
#include <QMouseEvent>
#include <QFileDialog>

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
    QImage *img, *orig;

    // wymiary rysunku + wspolrzedne poczatku ukladu
    int width, height;
    int begX, begY;
    int centerX, centerY;
    int recentMatrix;

    QVector<float> T, Tr, S, R, SH, P;
    QVector<QPair<int,QVector<float>>> v;

    // potrzebne funkcje
    void clearImage(QImage *im);
    void resetTrMatrix();
    void resetMatrices();
    QVector<float> multiply(QVector<float> A, QVector<float> B);
    void transformAll();
    void setTranslation(int tx, int ty);
    void setScaling(float sx, float sy);
    void setRotation(int alpha);
    void setShearing(float shx, float shy);
    QColor bilinearInterpolation(QImage *im, double x, double y);

private slots:
    void paintEvent(QPaintEvent*);
    void on_exitButton_clicked();
    void on_resetButton_clicked();

    void on_translationYSlider_valueChanged(int value);
    void on_translationXSlider_valueChanged(int value);
    void on_rotationSlider_valueChanged(int value);
    void on_scalingYSlider_valueChanged(int value);
    void on_scalingXSlider_valueChanged(int value);
    void on_shearingYSlider_valueChanged(int value);
    void on_shearingXSlider_valueChanged(int value);
};
#endif // MAINWINDOW_H

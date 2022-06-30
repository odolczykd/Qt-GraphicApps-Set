#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QImage>
#include <QMouseEvent>

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

    // wymiary rysunku + wspolrzedne poczatku ukladu
    int width, height;
    int begX, begY;

    bool RGBMode, HSVMode;

    // potrzebne funkcje
    void clearImage();
    void paintRGB(int whichIsConst, int constValue);
    void paintHSV(int x, int y, int H, int S, int V);
    void paintPixel(int x, int y, int R, int G, int B);

private slots:
    void paintEvent(QPaintEvent*);
    void on_exitButton_clicked();

    void on_RGBModeButton_clicked();
    void on_HSVModeButton_clicked();
    /*
    void on_sliderRed_valueChanged(int value);
    void on_sliderGreen_valueChanged(int value);
    void on_sliderBlue_valueChanged(int value);
    */
    void on_sliderRed_sliderMoved(int position);
    void on_sliderGreen_sliderMoved(int position);
    void on_sliderBlue_sliderMoved(int position);
    void on_sliderHue_sliderMoved(int position);
    void on_sliderSat_sliderMoved(int position);
    void on_sliderVal_sliderMoved(int position);
};
#endif // MAINWINDOW_H

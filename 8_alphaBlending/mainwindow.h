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

struct Layer{
    QImage *image;
    float alpha;
    int mode;       // 0 = normal, 1 = lighten, 2 = darken, 3 = average, 4 = multiply, 5 = XOR
    bool isActive;
};

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

    int blendMode[6];
    bool activeLayers[6];
    Layer layers[6];

    void paintPixel(int x, int y, QColor c);
    void display();
    void blend(QImage *B, QImage *F, float a, int m);

    // blend modes
    QColor blendNormal(QColor back, QColor front);
    QColor blendLighten(QColor back, QColor front);
    QColor blendDarken(QColor back, QColor front);
    QColor blendScreen(QColor back, QColor front);
    QColor blendMultiply(QColor back, QColor front);
    QColor blendXOR(QColor back, QColor front);

private slots:
    void paintEvent(QPaintEvent*);
    void on_exitButton_clicked();
    void on_checkBox_layer1_clicked();
    void on_checkBox_layer2_clicked();
    void on_checkBox_layer3_clicked();
    void on_checkBox_layer4_clicked();
    void on_checkBox_layer5_clicked();
    void on_slider_layer1_sliderMoved(int position);
    void on_slider_layer2_sliderMoved(int position);
    void on_slider_layer3_sliderMoved(int position);
    void on_slider_layer4_sliderMoved(int position);
    void on_slider_layer5_sliderMoved(int position);
    void on_comboBox_layer1_activated(int index);
    void on_comboBox_layer2_activated(int index);
    void on_comboBox_layer3_activated(int index);
    void on_comboBox_layer4_activated(int index);
    void on_comboBox_layer5_activated(int index);
};
#endif // MAINWINDOW_H

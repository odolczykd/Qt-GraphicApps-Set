#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QImage>
#include <QImageReader>
#include <QSize>
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
    QImage *img, *imgCopy, *orig;

    // wymiary rysunku + wspolrzedne poczatku ukladu
    int width, height;
    int begX, begY;
    int maskSize;

    // sciezka do wczytanej grafiki
    QString filePath;

    // potrzebne funkcje
    QImage* RGBtoGrey(QImage *im);
    QImage* thresholding(QImage *im);
    void loadImage(QString path);
    void dilation();
    void erosion();
    void opening();
    void closing();

private slots:
    void paintEvent(QPaintEvent*);
    void on_exitButton_clicked();
    void on_loadImgButton_clicked();
    void on_resetButton_clicked();
    void on_op1Button_clicked();
    void on_op2Button_clicked();
    void on_op3Button_clicked();
    void on_pushButton_clicked();
    void on_maskSizeSpinBox_valueChanged(int arg1);
};
#endif // MAINWINDOW_H

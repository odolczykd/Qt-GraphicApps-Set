#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    width = ui->frame->width();
    height = ui->frame->height();
    begX = ui->frame->x();
    begY = ui->frame->y();

    // tworzenie obrazow
    img = new QImage(width, height, QImage::Format_RGB32);


    // wczytanie grafik
    layers[0].image = new QImage(":/img/blank");
    layers[1].image = new QImage(":/img/winter");
    layers[2].image = new QImage(":/img/bliss");
    layers[3].image = new QImage(":/img/sunset");
    layers[4].image = new QImage(":/img/island");
    layers[5].image = new QImage(":/img/minecraft");

    *img = *(layers[0].image);

    for(int i=0; i<6; i++){
        layers[i].alpha = 1.0;
        layers[i].isActive = false;
        layers[i].mode = 0;
    }
    layers[0].isActive = true;
}

MainWindow::~MainWindow(){

}

void MainWindow::paintEvent(QPaintEvent *){
    QPainter p(this);
    p.drawImage(begX, begY, *img);
}

void MainWindow::on_exitButton_clicked(){
    qApp->quit();
}

// wyswietlanie warstw
void MainWindow::display(){
    for(int i=0; i<6; i++){
        if(layers[i].isActive) blend(img, layers[i].image, layers[i].alpha, layers[i].mode);
    }
    update();
}

// "blending" na warstwach (w roznych trybach)
void MainWindow::blend(QImage *B, QImage *F, float a, int m){
    QColor c, b, f;
    unsigned char *pb, *pf;
    pb = B->bits();
    pf = F->bits();

    for(int i=0; i<width; i++){
        for(int j=0; j<height; j++){

            b.setBlue(pb[width*4*j + 4*i]);
            b.setGreen(pb[width*4*j + 4*i + 1]);
            b.setRed(pb[width*4*j + 4*i + 2]);
            f.setBlue(pf[width*4*j + 4*i]);
            f.setGreen(pf[width*4*j + 4*i + 1]);
            f.setRed(pf[width*4*j + 4*i + 2]);

            switch(m){
                case 0: c = blendNormal(b,f);       break;
                case 1: c = blendLighten(b,f);      break;
                case 2: c = blendDarken(b,f);       break;
                case 3: c = blendScreen(b,f);       break;
                case 4: c = blendMultiply(b,f);     break;
                case 5: c = blendXOR(b,f);          break;
                default:
                    qDebug() << "Mode " << m << " does not exist!";
                    return;
                    break;
            }

            pb[width*4*j + 4*i] = a*c.blue() + (1-a)*b.blue();
            pb[width*4*j + 4*i + 1] = a*c.green() + (1-a)*b.green();
            pb[width*4*j + 4*i + 2] = a*c.red() + (1-a)*b.red();

        }
    }
}

/* Funkcje blendujace */

QColor MainWindow::blendNormal(QColor back, QColor front){
    return front;
}

QColor MainWindow::blendLighten(QColor back, QColor front){
    QColor m;
    m.setRed(back.red()>front.red() ? back.red() : front.red());
    m.setGreen(back.green()>front.green() ? back.green() : front.green());
    m.setBlue(back.blue()>front.blue() ? back.blue() : front.blue());
    return m;
}

QColor MainWindow::blendDarken(QColor back, QColor front){
    QColor m;
    m.setRed(back.red()<front.red() ? back.red() : front.red());
    m.setGreen(back.green()<front.green() ? back.green() : front.green());
    m.setBlue(back.blue()<front.blue() ? back.blue() : front.blue());
    return m;
}

QColor MainWindow::blendScreen(QColor back, QColor front){
    QColor scr;
    scr.setRed(255 - (((255-back.red())*(255-front.red())) >> 8));
    scr.setGreen(255 - (((255-back.green())*(255-front.green())) >> 8));
    scr.setBlue(255 - (((255-back.blue())*(255-front.blue())) >> 8));
    return scr;
}

QColor MainWindow::blendMultiply(QColor back, QColor front){
    QColor mult;
    mult.setRed((back.red()*front.red()) >> 8);
    mult.setGreen((back.green()*front.green()) >> 8);
    mult.setBlue((back.blue()*front.blue()) >> 8);
    return mult;
}

QColor MainWindow::blendXOR(QColor back, QColor front){
    QColor x;
    x.setRed(back.red()^front.red());
    x.setGreen(back.green()^front.green());
    x.setBlue(back.blue()^front.blue());
    return x;
}

void MainWindow::paintPixel(int x, int y, QColor c){
    unsigned char *ptr;
    ptr = img->bits();

    ptr[width*4*y + 4*x]     = c.blue();
    ptr[width*4*y + 4*x + 1] = c.green();
    ptr[width*4*y + 4*x + 2] = c.red();
}

void MainWindow::on_checkBox_layer1_clicked() { layers[1].isActive = 1-layers[1].isActive; display(); }
void MainWindow::on_checkBox_layer2_clicked() { layers[2].isActive = 1-layers[2].isActive; display(); }
void MainWindow::on_checkBox_layer3_clicked() { layers[3].isActive = 1-layers[3].isActive; display(); }
void MainWindow::on_checkBox_layer4_clicked() { layers[4].isActive = 1-layers[4].isActive; display(); }
void MainWindow::on_checkBox_layer5_clicked() { layers[5].isActive = 1-layers[5].isActive; display(); }

void MainWindow::on_slider_layer1_sliderMoved(int position){ layers[1].alpha = position/100.0; display(); }
void MainWindow::on_slider_layer2_sliderMoved(int position){ layers[2].alpha = position/100.0; display(); }
void MainWindow::on_slider_layer3_sliderMoved(int position){ layers[3].alpha = position/100.0; display(); }
void MainWindow::on_slider_layer4_sliderMoved(int position){ layers[4].alpha = position/100.0; display(); }
void MainWindow::on_slider_layer5_sliderMoved(int position){ layers[5].alpha = position/100.0; display(); }

void MainWindow::on_comboBox_layer1_activated(int index) { layers[1].mode = index; display(); }
void MainWindow::on_comboBox_layer2_activated(int index) { layers[2].mode = index; display(); }
void MainWindow::on_comboBox_layer3_activated(int index) { layers[3].mode = index; display(); }
void MainWindow::on_comboBox_layer4_activated(int index) { layers[4].mode = index; display(); }
void MainWindow::on_comboBox_layer5_activated(int index) { layers[5].mode = index; display(); }


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

    // tworzenie obrazow (w tym obrazu pomocniczego do efektu "dynamicznego" rysowania linii)
    img = new QImage(width, height, QImage::Format_RGB32);

    RGBMode = true;
    HSVMode = false;
    ui->HSVGroupBox->setEnabled(false);

}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *){
    QPainter p(this);
    p.drawImage(begX, begY, *img);
}

void MainWindow::paintPixel(int x, int y, int R, int G, int B){
    unsigned char *ptr;
    ptr = img->bits();

    ptr[width*4*y + 4*x]     = B;
    ptr[width*4*y + 4*x + 1] = G;
    ptr[width*4*y + 4*x + 2] = R;
}

// whichIsConst: 1 - Red, 2 - Green, 3 - Blue
void MainWindow::paintRGB(int whichIsConst, int constValue){
    int i, j;
    for(i=0; i<width; i++){
        for(j=0; j<height; j++){
            if(whichIsConst == 1) paintPixel(i, j, constValue, i/2, j/2);
            if(whichIsConst == 2) paintPixel(i, j, i/2, constValue, j/2);
            if(whichIsConst == 3) paintPixel(i, j, i/2, j/2, constValue);
        }
    }
    update();
}

void MainWindow::paintHSV(int x, int y, int H, int S, int V){
    double C = ((double)V/(width-1)) * ((double)S/(width-1));
    double X = C * (1 - fabs(fmod(H/60.0,2)-1));
    double m = (double)V/(width-1) - C;

    double r,g,b;

    if(H>=0 && H<60)    { r=C; g=X; b=0; }
    if(H>=60 && H<120)  { r=X; g=C; b=0; }
    if(H>=120 && H<180) { r=0; g=C; b=X; }
    if(H>=180 && H<240) { r=0; g=X; b=C; }
    if(H>=240 && H<300) { r=X; g=0; b=C; }
    if(H>=300 && H<360) { r=C; g=0; b=X; }

    int R = (r+m)*255;
    int G = (g+m)*255;
    int B = (b+m)*255;

    paintPixel(x,y,R,G,B);
    //update();
}

void MainWindow::on_exitButton_clicked(){
    qApp->quit();
}

void MainWindow::on_RGBModeButton_clicked()
{
    RGBMode = true;
    HSVMode = false;
    ui->RGBGroupBox->setEnabled(true);
    ui->HSVGroupBox->setEnabled(false);
    ui->RGBModeButton->setChecked(true);
    ui->HSVModeButton->setChecked(false);
}

void MainWindow::on_HSVModeButton_clicked()
{
    RGBMode = false;
    HSVMode = true;
    ui->RGBGroupBox->setEnabled(false);
    ui->HSVGroupBox->setEnabled(true);
    ui->RGBModeButton->setChecked(false);
    ui->HSVModeButton->setChecked(true);
}

void MainWindow::on_sliderRed_sliderMoved(int position)
{
    ui->sliderGreen->setSliderPosition(0);
    ui->sliderGreen->setValue(0);
    ui->sliderBlue->setSliderPosition(0);
    ui->sliderBlue->setValue(0);
    paintRGB(1, position);
}

void MainWindow::on_sliderGreen_sliderMoved(int position)
{
    ui->sliderRed->setSliderPosition(0);
    ui->sliderRed->setValue(0);
    ui->sliderBlue->setSliderPosition(0);
    ui->sliderBlue->setValue(0);
    paintRGB(2, position);
}

void MainWindow::on_sliderBlue_sliderMoved(int position)
{
    ui->sliderRed->setSliderPosition(0);
    ui->sliderRed->setValue(0);
    ui->sliderGreen->setSliderPosition(0);
    ui->sliderGreen->setValue(0);
    paintRGB(3, position);
}

void MainWindow::on_sliderHue_sliderMoved(int position)
{
    ui->sliderSat->setSliderPosition(0);
    ui->sliderSat->setValue(0);
    ui->sliderVal->setSliderPosition(0);
    ui->sliderVal->setValue(0);
    for(int i=0; i<width; i++){
        for(int j=0; j<height; j++){
            paintHSV(i,j,position,i,j);
        }
    }
    update();
}


void MainWindow::on_sliderSat_sliderMoved(int position)
{
    ui->sliderHue->setSliderPosition(0);
    ui->sliderHue->setValue(0);
    ui->sliderVal->setSliderPosition(0);
    ui->sliderVal->setValue(0);
    for(int i=0; i<width; i++){
        for(int j=0; j<height; j++){
            paintHSV(i,j,(int)((double)i/width*360),position,j);
        }
    }
    update();
}


void MainWindow::on_sliderVal_sliderMoved(int position)
{
    ui->sliderHue->setSliderPosition(0);
    ui->sliderHue->setValue(0);
    ui->sliderSat->setSliderPosition(0);
    ui->sliderSat->setValue(0);
    for(int i=0; i<width; i++){
        for(int j=0; j<height; j++){
            paintHSV(i,j,(int)((double)i/width*360),j,position);
        }
    }
    update();
}


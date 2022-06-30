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

    // tworzenie obrazu
    img = new QImage(width, height, QImage::Format_RGB32);

    // zmienna przechowujaca kolor wypelnienia (domylsnie czerwony (#ff0000))
    fillColor = Qt::red;
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *){
    QPainter p(this);
    p.drawImage(begX, begY, *img);
}

void MainWindow::on_clearButton_clicked(){
    vert.clear();
    clearImage();
    update();
}

void MainWindow::on_exitButton_clicked(){
    qApp->quit();
}

void MainWindow::clearImage(){
    unsigned char *ptr;
    ptr = img->bits();

    int i, j;
    // ustaw wszystkie piksele na czarno (RGB: #000000)
    for(i=0; i<height; i++){
        for(j=0; j<width; j++){
            ptr[width*4*i + 4*j]     = 0; // skladowa BLUE
            ptr[width*4*i + 4*j + 1] = 0; // skladowa GREEN
            ptr[width*4*i + 4*j + 2] = 0; // skladowa RED
        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event){
    int x = event->position().x();
    int y = event->position().y();
    x -= begX;
    y -= begY;

    // ustawiamy wartosci wierzcholka wielokata
    x0 = x;
    y0 = y;

    QPair<int,int> pt;
    pt.first = x0;
    pt.second = y0;

    vert.push_back(pt);

    for(int i=0; i<vert.size()-1; i++){
        //narysuj bok wielokata
        drawLine(vert[i].first, vert[i].second, vert[i+1].first, vert[i+1].second, Qt::white);

        // jesli kliknieto w poblizu pierwszego wierzcholka...
        if(abs(vert[vert.size()-1].first - vert[0].first)<=5 && abs(vert[vert.size()-1].second - vert[0].second)<=5){
            vert.pop_back();
            // ...narysuj ostatni bok...
            //drawLine(vert[vert.size()-1].first, vert[vert.size()-1].second, vert[0].first, vert[0].second, Qt::white);
            for(int j=0; j<vert.size()-1; j++){
                drawLine(vert[j].first, vert[j].second, vert[j+1].first, vert[j+1].second, fillColor);
            }
            drawLine(vert[vert.size()-1].first, vert[vert.size()-1].second, vert[0].first, vert[0].second, fillColor);

            // ...i wypelnij wielokat kolorem metoda ScanLine
            scanLine();
            vert.clear();
        }
    }

    update();
}

void MainWindow::drawLine(int x0, int y0, int x1, int y1, QColor c){
    int i;
    double a = (double)(y1-y0)/(x1-x0);
    double b = (double)y0 - a*x0;

    // obsluga rysowania pionowych kresek (kat nachylenia = 90)
    if(x0 == x1){
        if(y0>y1) std::swap(y0, y1);
        for(i=y0; i<=y1; i++){
            paintPixel(x0, i, c);
        }
        return;
    }

    // jesli kat nachylenia jest z przedzialu [0,45]
    if(a>=0 && a<=1){
        if(x0>x1) std::swap(x0, x1);
        for(i=x0; i<=x1; i++){
            double y = (double)a*i + b;
            paintPixel(i, (int)round(y), c);
        }
    }
    // jesli kat nachylenia jest z przedzialu [45,90)
    else if(a>1){
        if(y0>y1) std::swap(y0, y1);
        for(i=y0; i<=y1; i++){
            double x = (double)(i-b)/a;
            paintPixel((int)round(x), i, c);
        }
    }
    // jesli kat nachylenia jest z przedzialu (90, 135]
    else if(a<=-1){
        if(y0>y1) std::swap(y0, y1);
        for(i=y0; i<=y1; i++){
            double x = (double)(i-b)/a;
            paintPixel((int)round(x), i, c);
        }
    }
    // jesli kat nachylenia jest z przedzialu (135, 180)
    else{
        if(x0>x1) std::swap(x0, x1);
        for(i=x0; i<=x1; i++){
            double y = (double)a*i + b;
            paintPixel(i, (int)round(y), c);
        }
    }
}

void MainWindow::scanLine(){
    int i, x, y;
    QVector<int> crossPts;      // wektor punktow przeciecia prostej y z bokami wielokata

    // wyznaczanie "rozsadnych" granic poszukiwania
    int minY = vert[0].second;
    int maxY = vert[0].second;
    for(int i=1; i<vert.size(); i++){
        if(vert[i].second < minY) minY = vert[i].second;
        if(vert[i].second > maxY) maxY = vert[i].second;
    }

    for(y=minY; y<=maxY; y++){
        crossPts = findCrossPts(y);
        std::sort(crossPts.begin(), crossPts.end());
        for(i=0; i<crossPts.size()-1; i+=2){
            for(x=crossPts[i]+1; x<crossPts[i+1]; x++){
                paintPixel(x, y, fillColor);
            }
        }
    }
}

QVector<int> MainWindow::findCrossPts(int y){
    int i;
    double xTmp;
    QPair<int,int> A, B;
    QVector<int> x;

    for(i=0; i<vert.size(); i++){
        if(i==vert.size()-1){
            A = vert[i];
            B = vert[0];
        }
        else{
            A = vert[i];
            B = vert[i+1];
        }
        if(A.second > B.second) std::swap(A,B);
        if(A.second < y && y <= B.second){
            xTmp = (y-A.second)*(B.first-A.first)/(double)(B.second-A.second);
            xTmp += A.first;
            x.push_back((int)round(xTmp));
        }
    }
    return x;
}

void MainWindow::paintPixel(int x, int y, QColor c){
    unsigned char *ptr;
    ptr = img->bits();

    // zamaluj piksel na kolor bialy (RGB: #ffffff)
    ptr[width*4*y + 4*x]     = c.blue();
    ptr[width*4*y + 4*x + 1] = c.green();
    ptr[width*4*y + 4*x + 2] = c.red();
}

void MainWindow::on_colorButton_clicked()
{
    QColor c = QColorDialog::getColor(Qt::white, this);
    if(c.isValid()){
        fillColor = c;
    }
}


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
    begTX = ui->frameTexture->x();
    begTY = ui->frameTexture->y();

    img = new QImage(width, height, QImage::Format_RGB32);
    imgT = new QImage(width, height, QImage::Format_RGB32);
    imgT = new QImage(":/img/bliss");
    bliss = new QImage(":/img/bliss");

    isTriangleEnabled = isTriangleEnabledT = true;
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *){
    QPainter p(this);
    p.drawImage(begX, begY, *img);
    p.drawImage(begTX, begTY, *imgT);
}

void MainWindow::on_exitButton_clicked(){
    qApp->quit();
}

void MainWindow::mousePressEvent(QMouseEvent *event){

    int x = event->position().x() - begX;
    int y = event->position().y() - begY;

    int Tx = event->position().x() - begTX;
    int Ty = event->position().y() - begTY;

    QPair<int,int> p;

    // rysowanie trojkata na teksturze
    if(Tx>0 && Tx<width && Ty>0 && Ty<height && isTriangleEnabledT){
        p.first = Tx;
        p.second = Ty;
        vertT.push_back(p);
        //qDebug() << Tx << " " << Ty;

        paintAround(imgT, Tx, Ty);

        for(int i=0; i<vertT.size()-1; i++){
            drawLine(imgT, vertT[i].first, vertT[i].second, vertT[i+1].first, vertT[i+1].second);
            if(vertT.size() == 3) {
                drawLine(imgT, vertT[2].first, vertT[2].second, vertT[0].first, vertT[0].second);
                isTriangleEnabledT = false;
            }
        }
    }

    // rysowanie trojkata na prawym oknie
    if(x>0 && x<width && y>0 && y<height && isTriangleEnabled){
        p.first = x;
        p.second = y;
        vert.push_back(p);
        //qDebug() << x << " " << y;

        paintAround(img, x, y);

        for(int i=0; i<vert.size()-1; i++){
            drawLine(img, vert[i].first, vert[i].second, vert[i+1].first, vert[i+1].second);
            if(vert.size() == 3) {
                drawLine(img, vert[2].first, vert[2].second, vert[0].first, vert[0].second);
                isTriangleEnabled = false;
            }
        }
    }

    if(vert.size() == 3 && vertT.size() == 3){
        texturing();
    }

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

QColor MainWindow::getColor(QImage *im, int x, int y){
    QColor c;
    unsigned char *ptr;
    ptr = im->bits();

    c.setBlue(ptr[width*4*y + 4*x]);
    c.setGreen(ptr[width*4*y + 4*x + 1]);
    c.setRed(ptr[width*4*y + 4*x + 2]);

    return c;
}

QColor MainWindow::bilinearInterpolation(QImage *im, double x, double y){
    QColor c;

    int Px = (int)x, Py = (int)y;
    double a = x-Px, b = y-Py;
    int value;

    unsigned char *ptr;
    ptr = im->bits();

    value = b*((1.-a)*ptr[width*4*Py + 4*Px] + a*ptr[width*4*Py + 4*(Px+1)])
            + (1.-b)*((1.-a)*ptr[width*4*(Py-1) + 4*Px] + a*ptr[width*4*(Py-1) + 4*(Px+1)]);
    c.setBlue(value);

    value = b*((1.-a)*ptr[width*4*Py + 4*Px + 1] + a*ptr[width*4*Py + 4*(Px+1) + 1])
            + (1.-b)*((1.-a)*ptr[width*4*(Py-1) + 4*Px + 1] + a*ptr[width*4*(Py-1) + 4*(Px+1) + 1]);
    c.setGreen(value);

    value = b*((1.-a)*ptr[width*4*Py + 4*Px + 2] + a*ptr[width*4*Py + 4*(Px+1) + 2])
            + (1.-b)*((1.-a)*ptr[width*4*(Py-1) + 4*Px + 2] + a*ptr[width*4*(Py-1) + 4*(Px+1) + 2]);
    c.setRed(value);

    return c;
}

void MainWindow::paintPixel(QImage *im, int x, int y, QColor c){
    unsigned char *ptr;
    ptr = im->bits();

    ptr[width*4*y + 4*x]     = c.blue();
    ptr[width*4*y + 4*x + 1] = c.green();
    ptr[width*4*y + 4*x + 2] = c.red();
}

void MainWindow::paintAround(QImage *im, int x, int y){
    int i, j;
    for(i=-3; i<=3; i++){
        for(j=-3; j<=3; j++){
            paintPixel(im, x+i, y+j, Qt::red);
        }
    }
    update();
}

void MainWindow::drawLine(QImage *im, int x0, int y0, int x1, int y1){
    int i;
    double a = (double)(y1-y0)/(x1-x0);
    double b = (double)y0 - a*x0;

    // obsluga rysowania pionowych kresek (kat nachylenia = 90)
    if(x0 == x1){
        if(y0>y1) std::swap(y0, y1);
        for(i=y0; i<=y1; i++){
            paintPixel(im, x0, i, Qt::red);
        }
        return;
    }

    // jesli kat nachylenia jest z przedzialu [0,45]
    if(a>=0 && a<=1){
        if(x0>x1) std::swap(x0, x1);
        for(i=x0; i<=x1; i++){
            double y = (double)a*i + b;
            paintPixel(im, i, (int)round(y), Qt::red);
        }
    }

    // jesli kat nachylenia jest z przedzialu [45,90)
    else if(a>1){
        if(y0>y1) std::swap(y0, y1);
        for(i=y0; i<=y1; i++){
            double x = (double)(i-b)/a;
            paintPixel(im, (int)round(x), i, Qt::red);
        }
    }

    // jesli kat nachylenia jest z przedzialu (90, 135]
    else if(a<=-1){
        if(y0>y1) std::swap(y0, y1);
        for(i=y0; i<=y1; i++){
            double x = (double)(i-b)/a;
            paintPixel(im, (int)round(x), i, Qt::red);
        }
    }

    // jesli kat nachylenia jest z przedzialu (135, 180)
    else{
        if(x0>x1) std::swap(x0, x1);
        for(i=x0; i<=x1; i++){
            double y = (double)a*i + b;
            paintPixel(im, i, (int)round(y), Qt::red);
        }
    }
}

// minOrMax == 1 -> min, minOrMax == 2 -> max
// which == 1 -> wspolrzedna x, which == 2 -> wspolrzedna y
int MainWindow::getVert(QVector<QPair<int,int>> v, int minOrMax, int which){
    QList<int> l;

    if(which == 1){
        l << v[0].first << v[1].first << v[2].first;
        std::sort(l.begin(), l.end());
        if(minOrMax == 1) return l[0];
        if(minOrMax == 2) return l[2];
    }

    if(which == 2){
        l << v[0].second << v[1].second << v[2].second;
        std::sort(l.begin(), l.end());
        if(minOrMax == 1) return l[0];
        if(minOrMax == 2) return l[2];
    }
}

void MainWindow::texturing(){
    double u, v, w, xT, yT;

    long W = (vert[1].first-vert[0].first)*(vert[2].second-vert[0].second) - (vert[2].first-vert[0].first)*(vert[1].second-vert[0].second);
    long Wv, Ww;

    int minX = getVert(vert, 1, 1);
    int minY = getVert(vert, 1, 2);

    int maxX = getVert(vert, 2, 1);
    int maxY = getVert(vert, 2, 2);

    for(int i=minX; i<=maxX; i++){
        for(int j=minY; j<=maxY; j++){
            Wv = (i-vert[0].first)*(vert[2].second-vert[0].second) - (vert[2].first-vert[0].first)*(j-vert[0].second);
            Ww = (vert[1].first-vert[0].first)*(j-vert[0].second) - (i-vert[0].first)*(vert[1].second-vert[0].second);

            v = (double)Wv/(double)W;
            w = (double)Ww/(double)W;
            u = 1.0-v-w;

            //jesli punkt lezy wewnatrz trojkata
            if(u>0. && u<1. && v>0. && v<1. && w>0. && w<1.){
                xT = u*vertT[0].first + v*vertT[1].first + w*vertT[2].first;
                yT = u*vertT[0].second + v*vertT[1].second + w*vertT[2].second;
                paintPixel(img, i, j, bilinearInterpolation(bliss, xT, yT));
            }
        }
    }

    /*
    paintAround(img, vert[0].first, vert[0].second);
    paintAround(img, vert[1].first, vert[1].second);
    paintAround(img, vert[2].first, vert[2].second);

    drawLine(img, vert[0].first, vert[0].second, vert[1].first, vert[1].second);
    drawLine(img, vert[1].first, vert[1].second, vert[2].first, vert[2].second);
    drawLine(img, vert[2].first, vert[2].second, vert[0].first, vert[0].second);
    */

    update();
}

void MainWindow::on_resetButton_clicked()
{
    imgT = new QImage(":/img/bliss");
    clearImage();
    vert.clear();
    vertT.clear();
    isTriangleEnabled = isTriangleEnabledT = true;
    update();
}


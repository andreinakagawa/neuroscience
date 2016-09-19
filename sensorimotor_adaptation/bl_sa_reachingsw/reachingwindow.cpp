/*
 * ----------------------------------------------------------------------------
 * FEDERAL UNIVERSITY OF UBERLÂNDIA
 * Faculty of Electrical Engineering
 * Biomedical Engineering Laboratory
 * Author: Andrei Nakagawa, MSc
 * contact: andrei.ufu@gmail.com
 * ----------------------------------------------------------------------------
 * Description:
 * ----------------------------------------------------------------------------
 *
*/

#include <QScreen>
#include <QPaintDevice>

#include "reachingwindow.h"
#include "ui_reachingwindow.h"
#include "protocolcontroller.h"

bool flagStarted = false;
const int numbTrials = 30; //Number of trials
const int numbSessions = 4; //Number of sessions
const int distanceCenter = 400; //Distance of the target to the center
int trialCounter = 1;
int sessionCounter = 1;
QString subjectName = "andrei_nakagawa";

ReachingWindow::ReachingWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReachingWindow)
{
    ui->setupUi(this);

    //Sets the background color of the form
    //Default: black
    this->setStyleSheet("background-color: black;");

    //Maximizes the window to fullscreen
    //Necessary, so the only thing on screen is the protocol
    this->showFullScreen();

    //Hides the mouse cursor
    this->setCursor(Qt::BlankCursor);

    //Enables mouse tracking
    this->setMouseTracking(true);

    //Defines the type of the timer
    //Precise timer guarantees a better time resolution
    this->timer->setTimerType(Qt::PreciseTimer);
    this->timer->setInterval(1); // 1000 Hz sampling frequency

    //Connects the timer to the desired method
    connect(this->timer,SIGNAL(timeout()),SLOT(timerTick()));    

    //Sets the cursor to the center of the screen
    QCursor::setPos((this->geometry().width()/2), this->geometry().height()/2);

    //Protocol
    //One target to the right
    this->protocolController->setNumberTargets(1);
    //Sets the number of trials per target
    this->protocolController->setNumberTrialsTarget(numbTrials);

    //Creates the file header
    QString fileheader = subjectName + "_header.txt";
    this->writeHeader(fileheader.toStdString());
}

void ReachingWindow::paintEvent(QPaintEvent *e)
{
    //Painter
    QPainter painter(this);
    //Pen
    QPen pen(Qt::black);

    if(!this->formInit)
    {
        pen.setColor(Qt::blue);
        painter.setPen(pen);
        pen.setWidth(15);

        //this->protocolController->DrawTargets(this,&painter);

        //Origin
        painter.setBrush(Qt::yellow);
        this->protocolController->targetX = (this->geometry().width()/2) - distanceCenter;
        this->protocolController->targetY = this->geometry().height()/2;
        this->protocolController->DrawTarget(this,&painter);

        //Target
        painter.setBrush(Qt::blue);
        this->protocolController->targetX = (this->geometry().width()/2) + distanceCenter;
        this->protocolController->targetY = this->geometry().height()/2;
        this->protocolController->DrawTarget(this,&painter);

        //Cursor
        pen.setColor(Qt::green);
        pen.setWidth(15);
        painter.setPen(pen);
    }
    else
        this->formInit=true;


    this->mutex->lock();
    painter.setBrush(Qt::green);
    this->cursorController->DrawPoint(&painter, &pen);
    this->mutex->unlock();

    update();
}

void ReachingWindow::mousePressEvent(QMouseEvent *e)
{
    //If the left button was pressed -> left click
    if(e->button() == Qt::LeftButton)
    {
        if(!this->trialStarted)
        {
            QString filename = subjectName + "_" + QString::number(sessionCounter) + "_" + QString::number(trialCounter) + ".txt";
            this->datafileController = new DataFileController(filename.toStdString());
            this->datafileController->Open();
            this->timer->start();
            this->trialStarted = true;
        }
        else
        {
            this->timer->stop();
            this->trialStarted = false;            
            for (int i=0; i<this->vData.size(); i++)
            {
                this->datafileController->WriteData(QString::number(vData.at(i)[0]) + "\t" + QString::number(vData.at(i)[1]));
                //this->datafileController->WriteData(QString::number(vData.at(i)) + "\n");
            }
            this->vData.clear();
            trialCounter++;
            if(trialCounter > numbTrials)
                this->close();

            this->datafileController->Close();            
        }
    }
}

void ReachingWindow::mouseMoveEvent(QMouseEvent *e)
{
    this->mutex->lock();
    QCursor cursor;
    this->posX = cursor.pos().x();
    this->posY = cursor.pos().y();
    //Sets the points of the cursor controller    
    this->cursorController->setX(cursor.pos().x());
    this->cursorController->setY(cursor.pos().y());
    this->mutex->unlock();
}

void ReachingWindow::writeHeader(std::string _filename)
{
    //Saves the header file
    QString header;
    this->datafileController = new DataFileController(_filename);
    this->datafileController->Open();
    header += "Reaching Analysis\n";
    header += "-------------------------------------\n";
    header += "Number of sessions: " + QString::number(numbSessions) + "\n";
    header += "Number of trials: " + QString::number(numbTrials) + "\n";
    header += "Sampling frequency (Hz): " + QString::number(1000) + "\n";
    header += "Monitor width (pixels) : " + QString::number(this->geometry().width()) + "\n";
    header += "Monitor height (pixels) : " + QString::number(this->geometry().height()) + "\n";
    header += "-------------------------------------\n";
    header += "Task parameters\n";
    header += "Number of targets: " + QString::number(1) + "\n";
    header += "Center of target in X: " + QString::number((this->geometry().width()/2) + distanceCenter) + "\n";
    header += "Center of target in Y: " + QString::number(this->geometry().height()/2) + "\n";
    header += "Target width: " + QString::number(20) + "\n";
    header += "Target height: " + QString::number(20) + "\n";
    header += "-------------------------------------\n";
    header += "Origin\n";
    header += "Center of Origin in X: " + QString::number((this->geometry().width()/2) - distanceCenter) + "\n";
    header += "Center of Origin in Y: " + QString::number(this->geometry().height()/2) + "\n";
    header += "Origin width: " + QString::number(20) + "\n";
    header += "Origin height: " + QString::number(20) + "\n";
    header += "-------------------------------------\n";
    this->datafileController->WriteData(header);
    this->datafileController->Close();
}

void ReachingWindow::timerTick()
{
    this->mutex->lock();

    QCursor qc;
    int* cursorPoints = (int*)malloc(2*sizeof(int));
    cursorPoints[0] = qc.pos().x();
    cursorPoints[1] = qc.pos().y();
    this->vData.push_back(cursorPoints);

    this->mutex->unlock();
}

ReachingWindow::~ReachingWindow()
{
    delete ui;
}

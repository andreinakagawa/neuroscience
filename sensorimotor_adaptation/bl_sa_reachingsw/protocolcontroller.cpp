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

#include "protocolcontroller.h"

#include <QDebug>

ProtocolController::ProtocolController(QWidget *p)
{
    //Sets the background color of the form
    //Default: black
    p->setStyleSheet("background-color: black;");
    //Maximizes the window to fullscreen
    //Necessary, so the only thing on screen is the protocol
    p->showFullScreen();
    //Hides the mouse cursor
    p->setCursor(Qt::BlankCursor);
    //Enables mouse tracking
    p->setMouseTracking(true);

    //Defines the center of the screen
    this->centerX = p->geometry().width()/2;
    this->centerY = p->geometry().height()/2;

    //Defines the position of the origin
    this->originX = this->centerX - this->distanceTarget;
    this->originY = this->centerY;

    //Defines the position of the target
    this->targetX = this->centerX + this->distanceTarget;
    this->targetY = this->centerY;

    //Sets the cursor to the center of the screen
    QCursor::setPos(this->centerX,this->centerY);

    //Creates a new thread
    this->workerThread = new QThread;
    //Starts the thread
    this->workerThread->start();
    //Creates a new timer
    this->timer = new QTimer(this);
    //Precise timer is prefered
    this->timer->setTimerType(Qt::PreciseTimer);
    //Finds the timer interval (ms) according to the sampling frequency
    int interval = (int)(1000 * (1.0 / ((double)this->samplingFrequency)));
    this->timer->setInterval(interval);
    //Connects the timer to the processing event
    connect(this->timer,SIGNAL(timeout()),this,SLOT(timerTick()));
    //Moves the protocol controller to a different Thread
    this->moveToThread(workerThread);

    //Writes the header file
    this->writeHeader();
}

//This method updates the objects that needs to be drawn in the GUI
//These objects can be targets, origin or even the visual feedback position
std::vector<GUIObject*> ProtocolController::updateGUI()
{
    std::vector<GUIObject*> vobj;

    if(this->flagStarted)
    {
        GUIObject* x = new GUIObject();
        x->point = new QPointF(this->originX,this->originY);
        x->pen = new QPen(Qt::blue);
        x->pen->setWidth(0);
        x->width = this->targetWidth;
        x->height = this->targetHeight;
        vobj.push_back(x);

        GUIObject* y = new GUIObject();
        y->point = new QPointF(this->targetX,this->targetY);
        y->pen = new QPen(Qt::red);
        y->pen->setWidth(0);
        y->width = this->targetWidth;
        y->height = this->targetHeight;
        vobj.push_back(y);

        GUIObject *z = new GUIObject();
        z->point = new QPointF(QCursor::pos().x(),QCursor::pos().y());
        z->pen = new QPen(Qt::green);
        z->pen->setWidth(0);
        z->width = this->targetWidth;
        z->height = this->targetHeight;
        vobj.push_back(z);

    }
    else
    {
        GUIObject* x = new GUIObject();
        x->point = new QPointF(this->originX,this->originY);
        x->pen = new QPen(Qt::blue);
        x->pen->setWidth(0);
        x->width = this->targetWidth;
        x->height = this->targetHeight;
        vobj.push_back(x);

        GUIObject* y = new GUIObject();
        y->point = new QPointF(this->targetX,this->targetY);
        y->pen = new QPen(Qt::red);
        y->pen->setWidth(0);
        y->width = this->targetWidth;
        y->height = this->targetHeight;
        vobj.push_back(y);

        this->flagStarted = true;
    }

    return vobj;
}

void ProtocolController::timerTick()
{
    this->mutex->lock();

    this->mutex->unlock();
}

void ProtocolController::MouseMove()
{
    this->mutex->lock();
    this->cursorController->setX(QCursor::pos().x());
    this->cursorController->setY(QCursor::pos().y());
    this->mutex->unlock();
}

//Creates a header file
void ProtocolController::writeHeader()
{
    QString headername = fileprefix + "_header.txt";
    fileController = new DataFileController(headername.toStdString());
    if(fileController->Open())
    {
        QString header = "";

        header += "Federal University of Uberlandia - Brazil\n";
        header += "Biomedical Engineering Lab\n";
        header += "---------------------------------------------\n";
        header += "Sensorimotor Adaptation Task\n";
        header += "Date: " + QDate::currentDate().toString("dd/MM/yyyy")
                + " - " + QTime::currentTime().toString() + "\n";
        header += "---------------------------------------------\n";
        header += "Details of the experimental protocol\n";
        header += "Number of sessions: " + QString::number(this->numberSessions) + "\n";
        header += "Number of trials: " + QString::number(this->numberTrials) + "\n";
        fileController->WriteData(header);
        fileController->Close();
    }
}

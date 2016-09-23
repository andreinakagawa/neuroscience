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
    this->parent = p;
    this->Initialize();
}

void ProtocolController::Initialize()
{
    //Sets the background color of the form
    //Default: black
    this->parent->setStyleSheet("background-color: black;");
    //Maximizes the window to fullscreen
    //Necessary, so the only thing on screen is the protocol
    this->parent->showFullScreen();
    //Hides the mouse cursor
    this->parent->setCursor(Qt::BlankCursor);
    //Enables mouse tracking
    this->parent->setMouseTracking(true);

    //Defines the center of the screen
    this->centerX = this->parent->geometry().width()/2;
    this->centerY = this->parent->geometry().height()/2;

    //Defines the position of the origin
    this->originX = this->centerX - this->distanceTarget;
    this->originY = this->centerY;

    //Defines the position of the target
    this->targetX = this->centerX + this->distanceTarget;
    this->targetY = this->centerY;

    //Sets the cursor to the center of the screen
    QCursor::setPos(this->centerX,this->centerY);

    //Creates a new mutex
    this->mutex = new QMutex();
    //Creates a new thread
    this->workerThread = new QThread;
    //Starts the thread
    this->workerThread->start();
    //Creates a new timer
    this->timer = new QTimer(0);
    //Precise timer is prefered
    this->timer->setTimerType(Qt::PreciseTimer);
    //Finds the timer interval (ms) according to the sampling frequency
    int interval = (int)(1000 * (1.0 / ((double)this->samplingFrequency)));
    this->timer->setInterval(interval);
    //Connects the timer to the processing event
    connect(this->timer,SIGNAL(timeout()),this,SLOT(timerTick()));
    //Moves the protocol controller to a different Thread
    this->timer->moveToThread(workerThread);

    connect(this,SIGNAL(start()),this->timer,SLOT(start()),
            Qt::QueuedConnection);
    connect(this,SIGNAL(stop()),this->timer,SLOT(stop()),
            Qt::QueuedConnection);

    //Writes the header file
    this->writeHeader();

    //Initializes the cursor controller
    this->cursorController = new CursorController();
    if(this->perturbation)
    {
        this->cursorController->setPerturbation(this->perturbationDegree);
        this->cursorController->setOriginX(this->originX);
        this->cursorController->setOriginY(this->originY);
    }

    this->trialCounter=1;
    this->sessionCounter=1;

    QCursor::setPos(this->originX,this->originY);
}

ProtocolController::~ProtocolController()
{
    if(this->flagRecord)
        emit this->stop();
    free(this->timer);
    free(this->fileController);
    free(this->cursorController);
    free(this->mutex);
    this->workerThread->deleteLater();
    free(this->workerThread);
}

//This method updates the objects that needs to be drawn in the GUI
//These objects can be targets, origin or even the visual feedback position
QVector<GUIObject*> ProtocolController::updateGUI()
{
    //Creates a vector containing the GUI objects
    QVector<GUIObject*> vobj;
    double x=0;
    double y=0;

    //Creates the origin marker
    GUIObject* objOrigin = new GUIObject();
    x = this->originX - (this->objWidth/2.0);
    y = this->originY - (this->objHeight/2.0);
    objOrigin->point = new QPointF(x,y);
    objOrigin->pen = new QPen(Qt::blue);
    objOrigin->pen->setWidth(0);
    objOrigin->width = this->objWidth;
    objOrigin->height = this->objHeight;
    objOrigin->type = GUIObject::Rectangle;
    vobj.push_back(objOrigin);

    //Creates the target marker
    GUIObject* objTarget = new GUIObject();
    x = this->targetX - (this->objWidth/2.0);
    y = this->targetY - (this->objHeight/2.0);
    objTarget->point = new QPointF(x,y);
    objTarget->pen = new QPen(Qt::red);
    objTarget->pen->setWidth(0);
    objTarget->width = this->objWidth;
    objTarget->height = this->objHeight;
    objTarget->type = GUIObject::Rectangle;
    vobj.push_back(objTarget);

    //Creates an object that represents the mouse cursor
    GUIObject *objMouseCursor = new GUIObject();
    x = QCursor::pos().x() - (this->cursorWidth/2.0);
    y = QCursor::pos().y() - (this->cursorHeight/2.0);
    objMouseCursor->point = new QPointF(x,y);
    objMouseCursor->pen = new QPen(Qt::yellow);
    objMouseCursor->pen->setWidth(0);
    objMouseCursor->width = this->cursorWidth;
    objMouseCursor->height = this->cursorHeight;
    objMouseCursor->type = GUIObject::Ellipse;
    vobj.push_back(objMouseCursor);

    //Creates an object that represents the visual feedback
    //Can be different from the actual mouse movement
    GUIObject *objFeedbackCursor = new GUIObject();
    x = this->cursorController->x() - (this->cursorWidth/2.0);
    y = this->cursorController->y() - (this->cursorHeight/2.0);
    objFeedbackCursor->point = new QPointF(x,y);
    objFeedbackCursor->pen = new QPen(Qt::green);
    objFeedbackCursor->pen->setWidth(0);
    objFeedbackCursor->width = this->cursorWidth;
    objFeedbackCursor->height = this->cursorHeight;
    objFeedbackCursor->type = GUIObject::Ellipse;
    vobj.push_back(objFeedbackCursor);

    //Checks if the visual feedback cursor has collided with the origin
    //In this case, the data acquisition is initiated
    if(objFeedbackCursor->HasCollided(objOrigin) && flagRecord==false)
    {        
        emit this->start();
        this->flagRecord=true;
    }
    //Checks if the visual feedback cursor has collided with the
    //target. In this case, the data acquisition is stopped
    //and saved in a file
    else if(objFeedbackCursor->HasCollided(objTarget) && flagRecord==true)
    {        
        emit this->stop();
        this->saveData();
        this->flagRecord=false;
    }

    return vobj;
}

void ProtocolController::timerTick()
{
    this->mutex->lock();
    QString val = QString::number(QCursor::pos().x()) + "\t" +
            QString::number(QCursor::pos().y());
    vData.push_back(val);
    this->mutex->unlock();
}

void ProtocolController::MouseMove()
{
    this->mutex->lock();
    this->cursorController->setX(QCursor::pos().x());
    this->cursorController->setY(QCursor::pos().y());
    if(this->perturbation)
        this->cursorController->RotatePoint();    
    this->mutex->unlock();
}

void ProtocolController::saveData()
{
    QString datafile = this->fileprefix + "_data_" +
            QString::number(this->sessionCounter) +
            "_" + QString::number(this->trialCounter) + ".txt";
    this->fileController = new DataFileController(datafile.toStdString());
    this->fileController->Open();
    for(int i=0; i<vData.size(); i++)
        this->fileController->WriteData(vData.at(i));
    this->fileController->Close();
    this->trialCounter++;
    this->vData.clear();
}

//Creates the header file for the experiment
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
        header += "Sampling frequency (Hz): " + QString::number(1000) + "\n";
        header += "Monitor width (pixels) : " + QString::number(this->parent->geometry().width()) + "\n";
        header += "Monitor height (pixels) : " + QString::number(this->parent->geometry().height()) + "\n";
        header += "---------------------------------------------\n";
        fileController->WriteData(header);
        fileController->Close();
    }
}

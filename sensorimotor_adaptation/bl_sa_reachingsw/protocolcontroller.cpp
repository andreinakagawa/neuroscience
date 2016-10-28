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
    //this->Initialize();
}

void ProtocolController::Initialize()
{
    if(!this->initialized)
    {
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

        double x=0;
        double y=0;

        this->objOrigin = new GUIObject();
        x = this->originX;
        y = this->originY;
        this->objOrigin->point = new QPointF(x,y);
        this->objOrigin->pen = new QPen(Qt::blue);
        this->objOrigin->pen->setWidth(0);
        this->objOrigin->width = this->objWidth;
        this->objOrigin->height = this->objHeight;
        this->objOrigin->type = GUIObject::Ellipse;

        this->objTarget = new GUIObject();
        x = this->targetX;
        y = this->targetY;
        this->objTarget->point = new QPointF(x,y);
        this->objTarget->pen = new QPen(Qt::red);
        this->objTarget->pen->setWidth(0);
        this->objTarget->width = this->objWidth;
        this->objTarget->height = this->objHeight;
        this->objTarget->type = GUIObject::Ellipse;

        //Writes the header file
        this->writeHeader();

        QCursor::setPos(this->originX,this->originY);

        this->initialized = true;
    }
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
    vobj.push_back(this->objOrigin);

    //Creates the target marker   
    vobj.push_back(this->objTarget);

    //Creates an object that represents the mouse cursor
    GUIObject *objMouseCursor = new GUIObject();
    x = QCursor::pos().x();
    y = QCursor::pos().y();
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
    x = this->cursorController->x() ;
    y = this->cursorController->y();
    objFeedbackCursor->point = new QPointF(x,y);
    objFeedbackCursor->pen = new QPen(Qt::green);
    objFeedbackCursor->pen->setWidth(0);
    objFeedbackCursor->width = this->cursorWidth;
    objFeedbackCursor->height = this->cursorHeight;
    objFeedbackCursor->type = GUIObject::Ellipse;
    vobj.push_back(objFeedbackCursor);

    //Checks if the visual feedback cursor has collided with the origin
    //In this case, the data acquisition is initiated
    if(objFeedbackCursor->HasCollidedCenter(objOrigin) && flagRecord==false)
    {                
        emit this->start();
        this->flagRecord=true;
    }
    //Checks if the visual feedback cursor has collided with the
    //target. In this case, the data acquisition is stopped
    //and saved in a file
    else if(objFeedbackCursor->HasCollidedCenter(objTarget) && flagRecord==true)
    {        
        this->flagRecord=false;
        emit this->stop();
        this->saveData();
        this->vData.clear();
    }

    return vobj;
}

//Saves the samples from the visual feedback
void ProtocolController::timerTick()
{
    this->mutex->lock();
    if(this->flagRecord)
    {
        QString val = QString::number(this->cursorController->x()) + "\t" +
                QString::number(this->cursorController->y());
        vData.push_back(val);
    }
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
    this->fileController = new DataFileController(headername.toStdString());
    if(this->fileController->Open())
    {
        QString header = "";

        header += "Federal University of Uberlandia - Brazil\n";
        header += "Biomedical Engineering Lab\n";
        header += "---------------------------------------------\n";
        header += "Visuomotor Adaptation Task\n";
        header += "Date: " + QDate::currentDate().toString("dd/MM/yyyy")
                + " - " + QTime::currentTime().toString() + "\n";
        header += "---------------------------------------------\n";
        header += "Details of the experiment\n";
        header += "Number of sessions: " + QString::number(this->numberSessions) + "\n";
        header += "Number of trials: " + QString::number(this->numberTrials) + "\n";        
        header += "Sampling frequency (Hz): " + QString::number(this->samplingFrequency) + "\n";
        header += "Monitor width (pixels): " + QString::number(this->parent->geometry().width()) + "\n";
        header += "Monitor height (pixels): " + QString::number(this->parent->geometry().height()) + "\n";
        header += "Perturbation: ";
        if(this->perturbation)
            header += "Yes\n";
        else
            header += "No\n";
        header += "Perturbation degree: " + QString::number(this->perturbationDegree) + "\n";
        header += "---------------------------------------------\n";        
        header += "Task parameters\n";
        header += "-------------------------------------\n";
        header += "Origin\n";
        header += "Center of Origin in X: " + QString::number(this->objOrigin->point->x()) + "\n";
        header += "Center of Origin in Y: " + QString::number(this->objOrigin->point->y()) + "\n";
        header += "Origin width: " + QString::number(this->objWidth) + "\n";
        header += "Origin height: " + QString::number(this->objHeight) + "\n";
        header += "-------------------------------------\n";
        header += "Number of targets: " + QString::number(this->numberTargets) + "\n";
        header += "Center of target in X: " + QString::number(this->objTarget->point->x()) + "\n";
        header += "Center of target in Y: " + QString::number(this->objTarget->point->y()) + "\n";
        header += "Target width: " + QString::number(this->objWidth) + "\n";
        header += "Target height: " + QString::number(this->objHeight) + "\n";
        header += "-------------------------------------\n";
        this->fileController->WriteData(header);
        this->fileController->Close();
    }
}

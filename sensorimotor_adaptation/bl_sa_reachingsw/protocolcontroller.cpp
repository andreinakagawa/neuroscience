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

    this->numberTrialsperSession.push_back(1);
    this->numberTrialsperSession.push_back(1);
    this->numberTrialsperSession.push_back(1);
    this->numberTrialsperSession.push_back(1);

    this->perturbationSession.push_back(false);
    this->perturbationSession.push_back(false);
    this->perturbationSession.push_back(false);
    this->perturbationSession.push_back(false);

    this->vectorCursorFeedback.push_back(true);
    this->vectorCursorFeedback.push_back(true);
    this->vectorCursorFeedback.push_back(true);
    this->vectorCursorFeedback.push_back(true);
}

void ProtocolController::Initialize()
{
    if(!this->initialized)
    {
        //Defines the center of the screen
        this->centerX = this->parent->geometry().width()/2;
        this->centerY = this->parent->geometry().height()/2;

        //Defines the position of the origin
        this->originX = this->centerX;
        this->originY = this->centerY + this->distanceTarget;

        //Defines the position of the target
        this->targetX = this->centerX;
        this->targetY = this->centerY - this->distanceTarget;

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

        //Connects an event to the rest timer
        this->timerRest = new QTimer(0);
        this->timerRest->setInterval(this->restInterval);
        connect(this->timerRest,SIGNAL(timeout()),this,SLOT(timerRestTick()));

        //Connects events to the timer
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

        //Counter for the number of trials
        this->trialCounter=0;
        //Counter for the number of sessions
        this->sessionCounter=1;

        //Aux variables
        double x=0;
        double y=0;

        //Creates the object representing the origin                
        this->objOrigin = new GUIObject();
        x = this->originX;
        y = this->originY;
        this->objOrigin->point = new QPointF(x,y);
        this->objOrigin->pen = new QPen(Qt::blue);
        this->objOrigin->pen->setWidth(0);
        this->objOrigin->width = this->objWidth;
        this->objOrigin->height = this->objHeight;
        this->objOrigin->type = GUIObject::Ellipse;

        //Creates the object representing the target
        this->targetColor = Qt::red;
        this->objTarget = new GUIObject();
        x = this->targetX;
        y = this->targetY;
        this->objTarget->point = new QPointF(x,y);
        this->objTarget->pen = new QPen(this->targetColor);
        this->objTarget->pen->setWidth(0);
        this->objTarget->width = this->objWidth;
        this->objTarget->height = this->objHeight;
        this->objTarget->type = GUIObject::Ellipse;

        this->objFeedbackCursor = new GUIObject();
        this->feedbackCursorColor = Qt::green;
        this->objCursor = new GUIObject();

        //Writes the header file
        this->writeHeader();

        QCursor::setPos(this->originX,this->originY);

        this->initialized = true;
    }
}

//Destructor
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
    this->objTarget->pen = new QPen(this->targetColor);
    this->objTarget->pen->setWidth(0);
    vobj.push_back(this->objTarget);

    if(this->flagFeedback)
    {
        //Creates an object that represents the actual mouse movement
        x = QCursor::pos().x();
        y = QCursor::pos().y();
        objCursor->point = new QPointF(x,y);
        objCursor->pen = new QPen(Qt::yellow);
        objCursor->pen->setWidth(0);
        objCursor->width = this->cursorWidth;
        objCursor->height = this->cursorHeight;
        objCursor->type = GUIObject::Ellipse;

        //Creates an object that represents the visual feedback
        //Can be different from the actual mouse movement
        //GUIObject *objFeedbackCursor = new GUIObject();
        x = this->cursorController->x();
        y = this->cursorController->y();
        objFeedbackCursor->point = new QPointF(x,y);
        objFeedbackCursor->pen = new QPen(this->feedbackCursorColor);
        objFeedbackCursor->pen->setWidth(0);
        objFeedbackCursor->width = this->cursorWidth;
        objFeedbackCursor->height = this->cursorHeight;
        objFeedbackCursor->type = GUIObject::Ellipse;

        //Checks if the visual feedback cursor has collided with the origin
        //In this case, the data acquisition is initiated
        if(objFeedbackCursor->HasCollidedCenter(objOrigin) && flagRecord==false && this->flagExperiment)
        {
            emit this->start();
            this->flagRecord=true;
            this->flagPerturbation = this->perturbationSession[this->sessionCounter-1];
        }

        //Checks if the visual feedback cursor has collided with the
        //target. In this case, the data acquisition is stopped
        //and saved in a file
        else if(objFeedbackCursor->HasCollidedCenter(objTarget) && flagRecord==true && this->flagExperiment)
        {
            /*this->flagPerturbation = false;
            this->flagRecord=false;
            emit this->stop();
            this->saveData();
            this->vData.clear();*/
        }
    }

    //vobj.push_back(this->objCursor);

    if(this->vectorCursorFeedback.at(this->sessionCounter-1))
        vobj.push_back(this->objFeedbackCursor);

    return vobj;
}

//This method indicates that the experiment should be started
void ProtocolController::BeginExperiment()
{
    //Sets the "flagExperiment"
    this->flagExperiment = true;
    //Sets the cursor back to the origin
    //QCursor::setPos(this->originX,this->originY);
}

//Saves the samples from the visual feedback
void ProtocolController::timerTick()
{
    //Gets the current time -> to check the sampling frequency
    QString currentTime = QTime::currentTime().toString("hh:mm:ss.zzzz");
    //Locks the mutex for acessing the "cursorController" object for retrieving
    //the x and y position of the visual feedback cursor
    this->mutex->lock();
    if(this->flagRecord) //If the cursor should be recorded
    {
        //Creates a new string containing the current time
        //and the X and Y position of the visual feedback
        QString val = QString::number(this->cursorController->x()) + "\t" +
                QString::number(this->cursorController->y());
        //Adds the string to the QVector "vData"
        vData.push_back(val);

        QPoint aux = QPoint(this->cursorController->x(),this->cursorController->y());
        this->vectorMousePositions.push_back(aux);
        //Every X ms, check if the cursor is stationary
        if(this->vectorMousePositions.size() == this->samplesToStop)
        {
            //First mouse position
            int x0 = this->vectorMousePositions.at(0).x();
            int y0 = this->vectorMousePositions.at(0).y();
            //Final mouse position
            int xf = this->vectorMousePositions.at(this->samplesToStop-1).x();
            int yf = this->vectorMousePositions.at(this->samplesToStop-1).y();
            //If the difference is of one pixel or less, then the cursor is stationary
            //The cursor should be stationary outside the origin as well
            if(abs(xf-x0) <= 1 && abs(yf-y0) <= 1 && !this->objCursor->HasCollided(this->objOrigin))
            {
                this->feedbackCursorColor = Qt::blue;
                qDebug() << QString::number(this->vectorMousePositions.at(0).x()) << " " << QString::number(this->vectorMousePositions.at(this->samplesToStop-1).x());
                this->flagPerturbation = false;
                this->flagRecord=false;
                emit this->stop();
                this->saveData();
                this->vData.clear();
            }
            this->vectorMousePositions.clear();
        }
    }    
    //Releases the mutex
    this->mutex->unlock();
}

//This timer serves to control the rest period between trials
void ProtocolController::timerRestTick()
{
    //Stops the timer to prevent it from firing again
    this->timerRest->stop();
    //Sets the cursor back to the origin
    //QCursor::setPos(this->originX,this->originY);
    //Allows the drawing of the cursor
    this->flagFeedback=true;
    //Paints the target back to red
    this->targetColor = Qt::red;
    //Paints the cursor back to green
    this->feedbackCursorColor = Qt::green;
}

//Updates the visual feedback of the cursor according to mouse position
//If the perturbation is activated, then the visual feedback will be
//deviated
void ProtocolController::MouseMove()
{
    //Locks the mutex for acessing the "cursorController" object for setting
    //the x and y position of the visual feedback cursor
    this->mutex->lock();

    //Gets the X and Y coordinates of the cursor
    this->cursorController->setX(QCursor::pos().x());
    this->cursorController->setY(QCursor::pos().y());
    //Checks if the visual feedback should be perturbed
    //and updates it    
    //The perturbation is given by the QVector "perturbationSession" that indicates
    //whether a given session should be perturbed
    if(this->flagPerturbation)
        this->cursorController->RotatePoint(); //Applies rotation if required

    //Checks if the cursor position is within the limits of the monitor
    //X-axis
    if(this->cursorController->x() < 0)
        this->cursorController->setX(0);
    else if(this->cursorController->x() > this->parent->width())
        this->cursorController->setX(this->parent->width());
    //Y-axis
    if(this->cursorController->y() < 0)
        this->cursorController->setY(0);
    else if(this->cursorController->y() > this->parent->height())
        this->cursorController->setY(this->parent->height());

    //Releases the mutex
    this->mutex->unlock();
}

//Method for saving the visual feedback motion
void ProtocolController::saveData()
{
    //Stops painting new positions from the cursor
    this->flagFeedback=false;

    //Creates the filename
    //Example: If subject name is "subject1", for session number one and trial number one
    //datafile: subject1_1_1.txt
    QString datafile = this->fileprefix + "_data_" +
            QString::number(this->sessionCounter) +
            "_" + QString::number(this->trialCounter+1) + ".txt";
    //Creates a new instance of FileController to save data
    this->fileController = new DataFileController(datafile.toStdString());
    //Opening the file
    this->fileController->Open();
    //For each element in QVector "vData", writes it to file
    for(int i=0; i<vData.size(); i++)
        this->fileController->WriteData(vData.at(i)); //Retrieves the cursor sample
    //Closes and saves the file
    this->fileController->Close();

    //Controlling the experiment
    //Increments the trial counter
    this->trialCounter++;

    //If the total number of trials for a given session have been performed, resets
    //the counter and increments the session counter
    if(this->trialCounter >= this->numberTrialsperSession[this->sessionCounter-1])
    {
        this->trialCounter=0;
        this->sessionCounter++;
    }

    //If the total number of sessions have been performed
    //Finishes the experiment by presenting a QMessageBox and
    //closing the experiment window
    if(this->sessionCounter > this->numberSessions)
    {
        this->sessionCounter--;
        QMessageBox msgBox;
        msgBox.setText("The experiment has ended.");
        msgBox.exec();
        this->parent->close();
    }

    //Triggers the timer that controls the rest period
    this->timerRest->start();
    //Changes the target color to "Blue" indicating that
    //the target has been hit
    this->targetColor = Qt::blue;
    this->feedbackCursorColor = Qt::blue;
    //Clears the QVector "vData" so it can store new visual feeddback samples
    this->vData.clear();
}

bool ProtocolController::ExperimentIsRunning()
{
    return this->flagExperiment;
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
        header += "Sampling frequency (Hz): " + QString::number(this->samplingFrequency) + "\n";
        header += "Monitor width (pixels): " + QString::number(this->parent->geometry().width()) + "\n";
        header += "Monitor height (pixels): " + QString::number(this->parent->geometry().height()) + "\n";
        header += "---------------------------------------------\n";
        header += "Details of the sessions\n";
        header += "Number of trials: ";
        for(int i=0; i<this->numberSessions; i++)
        {
            header += QString::number(this->numberTrialsperSession[i]) + "; ";
        }
        header += "\nPerturbation of each session: ";
        for(int i=0; i<this->numberSessions; i++)
        {
            if(this->perturbationSession[i])
                header += "True; ";
            else
                header += "False; ";
        }
        header += "\n";
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

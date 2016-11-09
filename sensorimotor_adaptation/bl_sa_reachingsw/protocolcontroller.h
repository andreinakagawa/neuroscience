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
 * */

#ifndef PROTOCOLCONTROLLER_H
#define PROTOCOLCONTROLLER_H

#include <QWidget>
#include <QPainter>
#include <QCursor> //Handles the mouse cursor
#include <QDate> //Date functions
#include <QTime> //Clock time functions
#include <QThread> //Handles multi-threading
#include <QMutex> //Handles multi-threading
#include <QTimer> //Timer for saving data
#include <QMutex> //Necessary for handling multiple acess
#include <QVector> //Dynamic array
#include "datafilecontroller.h" //Imports the class that saves the experiment data
#include "cursorcontroller.h" //Handles the mouse cursor
#include "guiobject.h" //Defines the objects to be drawn in the GUI


class ProtocolController : public QObject
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------
    //Constructors
    ProtocolController(QWidget *p);
    ~ProtocolController();
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    //Methods    
    QVector<GUIObject*> updateGUI();
    void MouseMove();
    void Initialize();
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------


public slots:
    void timerTick(); //Method evoked by the timer    

signals:
    //Starts the timer
    void start();
    //Stops the timer
    void stop();

private:
    //Consts
    //Sampling frequency (Hz)
    const int samplingFrequency = 500;
    //Total number of trials
    const int numberTrials = 30;
    //Total number of sessions
    const int numberSessions = 1;
    //Number of targets
    const int numberTargets = 1;
    //Distance from center to target
    const int distanceTarget = 300;
    //Height of the target
    const int objHeight = 30;
    //Width of the target
    const int objWidth = 30;
    const int cursorWidth = 15;
    const int cursorHeight = 15;
    //Defines the session
    const bool perturbation = true;
    const int perturbationDegree = 20;
    //Filename prefix
    const QString fileprefix = "subject1";
    //Objects
    QWidget *parent;
    DataFileController *fileController;
    CursorController *cursorController;
    QMutex* mutex;
    QTimer *timer;
    QThread *workerThread;
    GUIObject* objTarget;
    GUIObject* objOrigin;
    //Methods
    //Method that updates what needs to be drawn in the GUI
    void writeHeader();
    void saveData();    
    //Properties    
    int centerX;
    int centerY;
    int originX;
    int originY;
    int targetX;
    int targetY;
    int trialCounter;
    int sessionCounter;
    bool flagRecord = false;
    bool initialized = false;
    QVector<QString> vData;
};

#endif // PROTOCOLCONTROLLER_H

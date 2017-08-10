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
#include <QMessageBox> //Display a messagebox on the screen
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
    //Method that updates what needs to be drawn in the GUI
    QVector<GUIObject*> updateGUI();
    //Mouse movement event
    void MouseMove();
    //Initialize the protocol
    void Initialize();
    //Method that indicates that the experiment should start
    void BeginExperiment();
    bool ExperimentIsRunning();
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------


public slots:
    void timerTick(); //Method evoked by the timer    
    void timerRestTick(); //Method evoked by the timer that counts rest between trials

signals:
    //Starts the timer
    void start();
    //Stops the timer
    void stop();

private:
    //Consts
    //Sampling frequency (Hz)
    const int samplingFrequency = 100;
    //Total number of trials
    const int numberTrials = 50;
    //Total number of sessions
    const int numberSessions = 4;
    //Number of targets
    const int numberTargets = 1;
    //Distance from center to target
    const int distanceTarget = 320;
    //Height of the target
    const int objHeight = 40;
    //Width of the target
    const int objWidth = 40;
    const int cursorWidth = 15;
    const int cursorHeight = 15;
    const int samplesToStop = 50; //500 ms
    //Defines the session
    const bool perturbation = true;
    const int perturbationDegree = -40;
    const int restInterval = 1500; //ms
    QVector<int> numberTrialsperSession;
    QVector<bool> perturbationSession;
    QVector<bool> vectorCursorFeedback;    
    QVector<QPoint> vectorMousePositions;
    int okcont = 0;
    //Filename prefix
    const QString fileprefix = "andrei_mesa_piloto1";
    //Objects
    QWidget *parent;
    DataFileController *fileController;
    CursorController *cursorController;
    QMutex *mutex;
    QTimer *timer;
    QTimer *timerRest;
    QThread *workerThread;
    GUIObject *objTarget;
    GUIObject *objOrigin;
    GUIObject *objFeedbackCursor;
    GUIObject *objCursor;
    QColor targetColor;
    //Methods    
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
    bool flagPerturbation = false;
    bool flagRecord = false;
    bool initialized = false;
    bool flagFeedback = true;    
    bool flagExperiment = false;
    QVector<QString> vData;
};

#endif // PROTOCOLCONTROLLER_H

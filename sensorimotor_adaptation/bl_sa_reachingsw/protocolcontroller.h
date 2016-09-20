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
#include <QDate> //Date functions
#include <QTime> //Clock time functions
#include <QTimer> //Timer for saving data
#include <QMutex> //Necessary for handling multiple acess
#include "datafilecontroller.h" //Imports the class that saves the experiment data


class ProtocolController
{

public:
    //-----------------------------------------------------------------
    //Constructors
    ProtocolController();
    ProtocolController(QWidget *p);
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    //Methods    
    void DrawTarget(QPainter *p);
    void DrawGUI(QPainter *p);
    void StartProtocol();
    void writeHeader();
    //-----------------------------------------------------------------
    //Properties    
    double targetX;
    double targetY;
    //-----------------------------------------------------------------

signals:
    void valorChanged(double valor);

private:
    //Consts
    //Total number of trials
    const int numberTrials = 30;
    //Total number of sessions
    const int numberSessions = 1;
    //Distance from center to target
    const int distanceTarget = 400;
    //Height of the target
    const int targetHeight = 20;
    //Width of the target
    const int targetWidth = 20;
    //Filename prefix
    const QString fileprefix = "andrei_nakagawa";
    //Objects
    DataFileController* fileHandler;
    QWidget* parent;
    //Methods
    //Properties
    int centerX;
    int centerY;
    int originX;
    int originY;
    int tX;
    int tY;
    double m_valor;
};

#endif // PROTOCOLCONTROLLER_H

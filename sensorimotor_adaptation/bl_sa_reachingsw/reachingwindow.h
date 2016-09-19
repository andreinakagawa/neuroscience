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

#ifndef REACHINGWINDOW_H
#define REACHINGWINDOW_H

#include <QWidget>
#include <QCursor> //Keeps track of the position of the cursor
#include <QMouseEvent> //Mouse Events for handling the visual feedback
#include <QPainter> //Painter to draw the visual feedback of the task
#include <QTimer> //Timer for saving data
#include <QMutex> //Necessary for handling multiple acess

#include <vector>

#include "datafilecontroller.h" //Imports the class that saves the cursor movement
#include "cursorcontroller.h" //Imports the class that controls the visual feedback
#include "protocolcontroller.h" //Imports the class that manages the protocol

namespace Ui {
class ReachingWindow;
}

class ReachingWindow : public QWidget
{
    Q_OBJECT

public:
    //Constructor
    explicit ReachingWindow(QWidget *parent = 0);

    //Objects and fields
    QTimer *timer = new QTimer(this);
    //Data file controller --> Handler for saving cursor position
    DataFileController *datafileController = new DataFileController("reaching1.txt");
    //Protocol controller --> Defines what is the protocol to be performed
    ProtocolController *protocolController = new ProtocolController();
    //Gets the position of the mouse cursor
    CursorController *cursorController = new CursorController();
    //Mutex
    QMutex *mutex = new QMutex();
    //Position in X and Y
    int posX, posY;
    bool formInit = false;
    bool trialStarted = false;
    std::vector<int*> vData;

    //Methods
    void paintEvent(QPaintEvent *e); //Paint Event
    void mousePressEvent(QMouseEvent *e); //Mouse press event
    void mouseMoveEvent(QMouseEvent *e); //Mouse Move Event
    void writeHeader(std::string _filename);

    ~ReachingWindow();

private:
    Ui::ReachingWindow *ui;

public slots:
    void timerTick(); //Method called by the timer

};

#endif // REACHINGWINDOW_H

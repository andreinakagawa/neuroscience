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
#include "protocolcontroller.h" //Imports the class that manages the protocol
#include "guiobject.h" //Class that manages the objects to be drawn
#include <QVector> //Vector

namespace Ui {
class ReachingWindow;
}

class ReachingWindow : public QWidget
{
    Q_OBJECT

public:
    //Constructor
    explicit ReachingWindow(QWidget *parent = 0);
    ~ReachingWindow();

    //Objects
    ProtocolController *protocolController;

    //Methods
    void paintEvent(QPaintEvent *e); //Paint Event
    void mousePressEvent(QMouseEvent *e); //Mouse press event
    void mouseMoveEvent(QMouseEvent *e); //Mouse Move Event    

    //Properties
    std::vector<GUIObject> objVector;
    GUIObject *objList;

private:
    Ui::ReachingWindow *ui;

};

#endif // REACHINGWINDOW_H

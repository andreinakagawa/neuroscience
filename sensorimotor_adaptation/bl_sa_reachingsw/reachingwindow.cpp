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
#include <QTime>

#include "reachingwindow.h"
#include "ui_reachingwindow.h"
#include "protocolcontroller.h"

bool flag = true;

ReachingWindow::ReachingWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReachingWindow)
{
    ui->setupUi(this);

    //Creates a new instance of the ProtocolController class
    protocolController = new ProtocolController(this);
}

ReachingWindow::~ReachingWindow()
{
    delete ui;
}

void ReachingWindow::paintEvent(QPaintEvent *e)
{
    //Painter
    QPainter painter(this);
    QVector<GUIObject*> vGUIObjects = this->protocolController->updateGUI();
    for(int i=0; i<vGUIObjects.size(); i++)
    {
        painter.setPen(*vGUIObjects.at(i)->pen);
        painter.setBrush(vGUIObjects.at(i)->pen->color());
        switch(vGUIObjects.at(i)->type)
        {
            case GUIObject::Ellipse:
            painter.drawEllipse(*vGUIObjects.at(i)->point,
                                vGUIObjects.at(i)->width,vGUIObjects.at(i)->height);
            break;

        case GUIObject::Rectangle:
            painter.drawRect((int)vGUIObjects.at(i)->point->x(),
                             (int)vGUIObjects.at(i)->point->y(),
                                vGUIObjects.at(i)->width,vGUIObjects.at(i)->height);
            break;
        }
    }

    update();
}

void ReachingWindow::mousePressEvent(QMouseEvent *e)
{

}

void ReachingWindow::mouseMoveEvent(QMouseEvent *e)
{
    this->protocolController->MouseMove();
}

/*
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
*/

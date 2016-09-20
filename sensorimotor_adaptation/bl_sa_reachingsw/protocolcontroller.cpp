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

ProtocolController::ProtocolController()
{

}

ProtocolController::ProtocolController(QWidget *p)
{
    this->parent = p;
    //Defines the center of the screen
    this->centerX = p->geometry().width()/2;
    this->centerY = p->geometry().height()/2;
    //Defines the position of the origin
    this->originX = this->centerX - this->distanceTarget;
    this->originY = this->centerY;
    //Defines the position of the target
    this->targetX = this->centerX + this->distanceTarget;
    this->targetY = this->centerY;
}

void ProtocolController::DrawGUI(QPainter *p)
{
    //Drawing the origin
    QPoint* pt = new QPoint(this->originX,this->originY);
    p->setBrush(Qt::blue);
    p->drawEllipse(*pt,this->targetWidth,this->targetHeight);
    //Drawing the target
    pt = new QPoint(this->targetX,this->targetY);
    p->setBrush(Qt::red);
    p->drawEllipse(*pt,this->targetWidth,this->targetHeight);
}

//Draw targets in the screen
void ProtocolController::DrawTarget(QPainter *p)
{
    QPoint pt(this->targetX,this->targetY);
    p->drawEllipse(pt,this->targetWidth,this->targetHeight);
}

//Creates a header file
void ProtocolController::writeHeader()
{
    QString headername = fileprefix + "_header.txt";
    fileHandler = new DataFileController(headername.toStdString());
    if(fileHandler->Open())
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
        fileHandler->WriteData(header);
        fileHandler->Close();
    }
}

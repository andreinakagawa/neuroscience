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

void ProtocolController::DrawTargets(QWidget *w, QPainter *p)
{
    int targets = 4; //Number of targets
    int totalWidth = w->geometry().width(); //Maximum width of the screen
    int totalHeight = w->geometry().height(); //Maximum height of the screen
    int centerX  = totalWidth / 2;
    int centerY = totalHeight / 2;

    int targetWidth = 20;
    int targetHeight = 20;

    //Adds the desired number of targets in the screen
    for(int i=0; i<this->numberTargets(); i++)
    {
        QPoint point(0,0);

        switch(i)
        {

        case 0:
            point.setX(centerX);
            point.setY(centerY-this->distanceTarget());
            p->drawEllipse(point,targetWidth,targetHeight);
            break;

        case 1:
            point.setX(centerX+this->distanceTarget());
            point.setY(centerY);
            p->drawEllipse(point,targetWidth,targetHeight);
            break;

        case 2:
            point.setX(centerX);
            point.setY(centerY+this->distanceTarget());
            p->drawEllipse(point,targetWidth,targetHeight);
            break;

        case 3:
            point.setX(centerX-this->distanceTarget());
            point.setY(centerY);
            p->drawEllipse(point,targetWidth,targetHeight);
            break;
        }
    }

    /*
    //Drawing the targets
    //Draw ellipse
    p->setBrush(Qt::blue); //To fill the ellipses

    int px=0,py=0;

    //Top target
    px = totalWidth/2 - targetWidth;
    py = 0;
    p->drawEllipse(px,py,targetWidth,targetHeight);

    //Right target
    px = totalWidth - targetWidth;
    py = totalHeight/2 - targetHeight;
    p->drawEllipse(px,py,targetWidth,targetHeight);

    //Bottom target
    px = totalWidth/2 - targetWidth;
    py = totalHeight - targetHeight;
    p->drawEllipse(px,py,targetWidth,targetHeight);

    //Left target
    px = 0;
    py = totalHeight/2 - targetHeight;
    p->drawEllipse(px,py,targetWidth,targetHeight);
    */
}

void ProtocolController::DrawTarget(QWidget *w, QPainter *p)
{
    QPoint pt(this->targetX,this->targetY);
    int targetWidth = 20;
    int targetHeight = 20;
    p->drawEllipse(pt,targetWidth,targetHeight);
}

/* FEDERAL UNIVERSITY OF UBERLÂNDIA
 * Faculty of Electrical Engineering
 * Biomedical Engineering Laboratory
 * Author: Andrei Nakagawa, MSc
 * contact: andrei.ufu@gmail.com
*/

#include "cursorcontroller.h"

CursorController::CursorController()
{

}

CursorController::CursorController(int _perturbation)
{    
    this->setPerturbation(_perturbation);
}

//Method that rotates a given point according to the degree of the perturbation
//in respect to the origin
void CursorController::RotatePoint()
{
    int _x = this->x();
    int _y = this->y();

    //Translates back to the origin
    int ax = _x - this->originX();
    int ay = _y - this->originY();

    //Applies the rotation
    double fax = (ax*cos(this->rad)) - (ay*sin(this->rad));
    double fay = (ay*cos(this->rad)) + (ax*sin(this->rad));

    //Translates the rotated point back to the original coordinates
    int rx = round(fax+this->originX());
    int ry = round(fay+this->originY());

    this->setX(rx);
    this->setY(ry);
}

void CursorController::DrawPoint(QPainter *painter, QPen *pen)
{
    QPointF fp;
    fp.setX(this->x());
    fp.setY(this->y());
    painter->drawEllipse(fp,6,6);
}

double CursorController::Deg2Rad(int _deg)
{
    return _deg * (M_PI/180.0);
}

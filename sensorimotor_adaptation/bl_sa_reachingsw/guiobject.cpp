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

#include "guiobject.h"

GUIObject::GUIObject()
{
    this->width = 20;
    this->height = 20;
    this->pen = new QPen(Qt::blue);
    this->point = new QPointF(0.0,0.0);
    this->type = this->Ellipse;
}

GUIObject::~GUIObject()
{
    free(this->point);
    free(this->pen);
}

bool GUIObject::HasCollided(GUIObject* _obj)
{
    //Measuring the Euclidean Distance
    double distance = this->EuclideanDistance(_obj);
    if(distance < (this->width+_obj->width))
        return true;
    else
        return false;
}

double GUIObject::EuclideanDistance(GUIObject* _obj)
{
    double x = (double)this->point->x() - (double)_obj->point->x();
    double y = (double)this->point->y() - (double)_obj->point->y();
    return std::sqrt(std::pow(x,2) + std::pow(y,2));
}

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

//Default constructor
GUIObject::GUIObject()
{
    this->width = 20;
    this->height = 20;
    this->pen = new QPen(Qt::blue);
    this->point = new QPointF(0.0,0.0);
    this->type = this->Ellipse;
}

//Destructor
GUIObject::~GUIObject()
{
    //Frees the allocated pointers
    free(this->point); //QPointF
    free(this->pen); //QPen
    free(this->paintColor); //QColor
}

//Method that determines whether another GUIObject
//has collided with the current object
bool GUIObject::HasCollided(GUIObject* _obj)
{
    //Measuring the Euclidean Distance
    double distance = this->EuclideanDistance(_obj);
    //The object has collided if the distance between
    //the two objects is inferior to the sum of
    //the objects' radius
    if(distance <= this->width+_obj->width)
        return true;
    else
        return false;
}

//Method that determines whether another GUIObject
//has collided with the center of the current object
bool GUIObject::HasCollidedCenter(GUIObject *_obj)
{
    //Measuring the Euclidean Distance
    double distance = this->EuclideanDistance(_obj);
    if(distance <= _obj->width/2.0)
        return true;
    else
        return false;
}

//Determines whether another GUIOBject is inside the
//current object
bool GUIObject::IsInside(GUIObject* _obj)
{
    //TODO: Implement this method
   return false;
}

//Method that measures the Euclidean distance between
//the current object and another GUIObject
double GUIObject::EuclideanDistance(GUIObject* _obj)
{
    double x = (double)this->point->x() - (double)_obj->point->x();
    double y = (double)this->point->y() - (double)_obj->point->y();
    return std::sqrt(std::pow(x,2) + std::pow(y,2));
}

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

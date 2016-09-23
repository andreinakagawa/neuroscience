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

#ifndef GUIOBJECT_H
#define GUIOBJECT_H

#include <QPen>
#include <QPoint>
#include <QPointF>

class GUIObject : public QObject
{
  Q_OBJECT

public:
    GUIObject(); //Constructor
    ~GUIObject(); //Destructor

    //Methods
    bool HasCollided(GUIObject* _obj);
    double EuclideanDistance(GUIObject* _obj);

    //Properties
    QPen *pen;
    QPointF *point;
    QColor *paintColor;
    int width;
    int height;
    enum objectType{Ellipse=1,Rectangle=2};
    objectType type;
};

#endif // GUIOBJECT_H

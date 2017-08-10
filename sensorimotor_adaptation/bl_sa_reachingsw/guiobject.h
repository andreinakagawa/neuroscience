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
    //Determines whether another GUIObject has
    //collided with the current object
    bool HasCollided(GUIObject *_obj);
    //Determines whether another GUIObject has
    //collided with the center of the current object
    bool HasCollidedCenter(GUIObject *_obj);
    //Determines whether another GUIOBject is inside the
    //current object
    bool IsInside(GUIObject* _obj);
    //Measures the euclidean distance between
    //the current object and another GUIObject
    double EuclideanDistance(GUIObject *_obj);    

    //Properties
    //Determines the parameters of the QPen object
    //that will be used by QPainter to draw the object
    QPen *pen;
    //Creates an object that contains information about
    //the origin point of the object to be drawn
    QPointF *point;
    //Determines the color of the GUIObject
    QColor *paintColor;
    //Determines the width of the GUIObject
    int width;
    //Determines the height of the GUIObject
    int height;
    //Enum used to determine the type of the object
    enum objectType{Ellipse=1,Rectangle=2};
    //Type of the GUIObject
    objectType type;
};

#endif // GUIOBJECT_H

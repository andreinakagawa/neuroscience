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

#ifndef CURSORCONTROLLER_H
#define CURSORCONTROLLER_H

#include <math.h>
#include <QPoint>

#define M_PI 3.14159265358979323846

class CursorController : public QPoint
{
    //Properties
    //The degree of the perturbation
    //Q_PROPERTY(int perturbation READ perturbation WRITE setPerturbation)
    //Q_PROPERTY(double originX READ originX WRITE setOriginX)
    //Q_PROPERTY(double originY READ originY WRITE setoriginY)

public:
    //Constructors
    CursorController(); //Default
    CursorController(int _perturbation); //Custom
    //~CursorController();

    //Methods
    void RotatePoint();

    //Getters and setters
    //perturbation
    void setPerturbation(int perturbation)
    {
        m_perturbation = perturbation;
        this->rad = this->Deg2Rad(perturbation);
    }
    int perturbation() const
    {
        return m_perturbation;
    }
    void setOriginX(int originX)
    {
        m_originX = originX;
    }
    void setOriginY(int originY)
    {
        m_originY = originY;
    }
    int originX() const
    {
        return m_originX;
    }

    int originY() const
    {
        return m_originY;
    }


private:
    //Properties
    int m_perturbation;
    int m_originX;
    int m_originY;

    //Fields
    double rad;

    //Methods
    double Deg2Rad(int _deg);
};

#endif // CURSORCONTROLLER_H

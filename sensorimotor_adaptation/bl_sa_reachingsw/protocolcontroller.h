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

#ifndef PROTOCOLCONTROLLER_H
#define PROTOCOLCONTROLLER_H

#include <QWidget>
#include <QPainter>

class ProtocolController
{
    //Properties
    //Number of targets
    Q_PROPERTY(int numberTargets READ numberTargets WRITE setNumberTargets)
    //Distance from the center to the target
    Q_PROPERTY(int distanceTarget READ distanceTarget WRITE setDistanceTarget)
    //Number of trials by target
    Q_PROPERTY(int numberTrialsTarget READ numberTrialsTarget WRITE setNumberTrialsTarget)

public:
    //-----------------------------------------------------------------
    //Constructor
    ProtocolController();
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    //Methods
    void DrawTargets(QWidget *w, QPainter *p);
    void DrawTarget(QWidget *w, QPainter *p);
    //-----------------------------------------------------------------
    // Attributes
    int targetX;
    int targetY;
    //-----------------------------------------------------------------
    //Getters
    int numberTargets() const
    {
        return m_numberTargets;
    }
    int distanceTarget() const
    {
        return m_distanceTarget;
    }

    int numberTrialsTarget() const
    {
        return m_numberTrialsTarget;
    }
    //-----------------------------------------------------------------

public slots:
    void setNumberTargets(int numberTargets)
    {
        m_numberTargets = numberTargets;
    }
    void setDistanceTarget(int distanceTarget)
    {
        m_distanceTarget = distanceTarget;
    }

    void setNumberTrialsTarget(int numberTrialsTarget)
    {
        m_numberTrialsTarget = numberTrialsTarget;
    }

private:
    int m_numberTargets;
    int m_distanceTarget;
    int m_numberTrialsTarget;
};

#endif // PROTOCOLCONTROLLER_H

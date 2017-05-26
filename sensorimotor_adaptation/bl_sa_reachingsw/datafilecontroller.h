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

#ifndef DATAFILECONTROLLER_H
#define DATAFILECONTROLLER_H

#include <QString>
#include <QFile>
#include <QTextStream>

class DataFileController
{

public:

    //Constructors
    DataFileController(std::string _filename); //Default constructor

    //Fields
    std::string filename;

    //Methods
    bool Open(); //Method for opening the file
    bool Close(); //Method for closing and saving the file
    void WriteData(QString _textline); //Method for writing data

public slots:


private:
    //Fields
    QFile fileHandler; //Handles file operations
    QTextStream fileStream; //Stream for writing data to the file
};

#endif // DATAFILECONTROLLER_H

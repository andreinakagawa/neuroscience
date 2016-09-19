/* FEDERAL UNIVERSITY OF UBERLÂNDIA
 * Faculty of Electrical Engineering
 * Biomedical Engineering Laboratory
 * Author: Andrei Nakagawa, MSc
 * contact: andrei.ufu@gmail.com
*/

#include "datafilecontroller.h"
#include "cursorcontroller.h"

//Default constructor
DataFileController::DataFileController(std::string _filename)
{
    this->filename = _filename;
    this->fileHandler.setFileName(QString::fromStdString(this->filename));
}

//Open the file
bool DataFileController::Open()
{
    bool ret = this->fileHandler.open(QIODevice::WriteOnly);
    if(ret)
    {
        this->fileStream.setDevice(&this->fileHandler);
    }
    return ret;
}

//Saves the file
bool DataFileController::Close()
{
    this->fileHandler.close();
    return true;
}

//Writes data to the file
void DataFileController::WriteData(QString _textline)
{
    this->fileStream << _textline << endl;
}

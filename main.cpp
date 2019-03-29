#include <QCoreApplication>
#include <QDir>
#include <QList>
#include <QtMath>

#include <QDebug>

struct SavePoint {
    QString strP;
    float fDist;
};

bool compareByDist(const SavePoint &a, const SavePoint &b)
{
    return a.fDist < b.fDist;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString strPath = QDir::currentPath();
    qInfo() << strPath;

    QFile file("coord.txt");
    if(!file.open(QIODevice::ReadOnly)) {
        qInfo() << "Error open file:" << file.errorString();
    }
    QTextStream in(&file);

    QString inputLine;
    QRegExp separator("[;\\s]");

    QList<SavePoint> strSave;
    SavePoint singlePoint;

    int iCount = 0;
    float xLU, yLU, // x,y left/up angle of rect
          xRD, yRD, // x,y right/down angle of rect
          xP, yP,   // x,y current point
          xC, yC;   // x,y centre of rectangle

    while(!in.atEnd()) {
        inputLine = in.readLine();
        //qInfo() << "\n\nPoint :" << iCount;
        //qInfo() << inputLine;
        QStringList fields = inputLine.split(separator, QString::SkipEmptyParts);

        if (iCount == 0) { // for first line
            float wid2, hei2;

            xC = fields[0].toFloat();
            yC = fields[1].toFloat();
            wid2 = fields[2].toFloat() / 2;
            hei2 = fields[3].toFloat() / 2;
            xLU = xC - wid2;
            yLU = yC + hei2;
            xRD = xC + wid2;
            yRD = yC - hei2;

            //qInfo() << "xLU:" << xLU << "yLU:" << yLU << "xRD:" << xRD << "yRD:" << yRD;
        }
        else {

            xP = fields[0].toFloat();
            yP = fields[1].toFloat();
            //qInfo() << "xP:" << xP << "yP:" << yP;
            if (xP >= xLU && yP >= yLU &&
                xP <= xRD && yP >= yRD)
            {
                singlePoint.strP = inputLine;
                //qInfo() << inputLine;
                singlePoint.fDist = sqrt(pow(xC - xP, 2) + pow(yC - yP, 2));
                strSave.append(singlePoint);
            }
        }
        iCount++;
        if (iCount > 100)
            break;
    }

    file.close();

    std::sort (strSave.begin(), strSave.end(), compareByDist);

    QFile fileW("coordW.txt");
    if(!fileW.open(QIODevice::WriteOnly)) {
        qInfo() << "Error create file:" << fileW.errorString();
    }
    QTextStream out(&fileW);
    QList<SavePoint>::const_iterator stlIter;
    for( stlIter = strSave.begin(); stlIter != strSave.end(); ++stlIter)
        out << stlIter->strP << "\n";

    fileW.close();
    return a.exec();
}

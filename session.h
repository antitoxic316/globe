#ifndef SESSION_H
#define SESSION_H

#include <QObject>
#include <QList>
#include <QUuid>
#include <QVector3D>

struct Marker {
    Marker(float lon, float lat)
        : longitude(lon), latitude(lat)
    {};

    QUuid uuid = QUuid::createUuid();

    float latitude; // degrees
    float longitude;
};

struct MarkerSource {
    QString name;

    QList<Marker> markers;
};

class Session : public QObject
{
Q_OBJECT

signals:
    void markerListUpdated();
    void globeRotated();
private:
    QList<Marker> markerList_;
    QList<MarkerSource> markerSourceList_;
    QVector3D globeEuler_;
public:
    Session();

    void addMarker(Marker m);
    void removeMarker(QString uuid);

    void addMarkerSource(MarkerSource ms);
    void removeMarkerSource(QString name);

    void appendMarkertoSource(Marker m, QString sourceName);

    QList<Marker> getMarkers(){
        return markerList_;
    };

    QList<MarkerSource> getMarkerSources(){
        return markerSourceList_;
    };

    QVector3D getGlobeEulers(){
        return globeEuler_;
    };

    void updateGlobeRotation(QVector3D eulers){
        globeEuler_ = eulers;
        emit globeRotated();
    };
};

#endif // SESSION_H

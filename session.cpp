#include "session.h"

Session::Session() {}

void Session::addMarker(Marker m){
    markerList_.append(m);
    emit markerListUpdated();
}
void Session::removeMarker(QString uuid){
    int ret = markerList_.removeIf([uuid](const Marker& m){ return m.uuid.toString() == uuid; });

    if (ret) emit markerListUpdated();
}

void Session::addMarkerSource(MarkerSource ms){
    for(int i = 0; i < ms.markers.size(); i++){
        markerList_.append(ms.markers.at(i));
    }

    markerSourceList_.append(ms);

    emit markerListUpdated();
}

void Session::removeMarkerSource(QString name){
    auto it = std::find_if(markerSourceList_.begin(), markerSourceList_.end(),
                           [name](const MarkerSource& ms){ return ms.name == name; });

    if (it != markerSourceList_.end()){
        MarkerSource ms = *it;

        for(int i = 0; i < ms.markers.size(); i++){
            QString uuid = ms.markers.at(i).uuid.toString();
            markerList_.removeIf([uuid](const Marker& m){ return m.uuid.toString() == uuid; });
        }

        markerSourceList_.erase(it);
        emit markerListUpdated();
    }
}


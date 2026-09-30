#include "Artist.h"
#include <algorithm>

musicus::Artist::Artist(string _name) : name(_name) {
    albums = new vector<Album*>();
}

string musicus::Artist::getName() const {
    return name;
}

void musicus::Artist::addAlbum(Album *album) {
    this->albums->push_back(album);
}

vector<musicus::Album*>* musicus::Artist::getAlbums() {
    return this->albums;
}

bool musicus::Artist::hasAlbum(Album* album) {
    auto r = std::find(albums->begin(), albums->end(), album);
    return r != albums->end();
}

bool musicus::Artist::operator<(const Artist& other) const {
    return name < other.getName();
}

//
//  ID3Tag.h
//  Musicus
//
//  Created by Philipp Otte on 20.10.22.
//
#pragma once

#include <string>
#include <vector>

namespace musicus {

enum ID3Tag_Version {
    NO_COMPATIBLE_TAG,
    ID3v23,
    ID3v24
};

struct ID3_Header {
    char major_version;
    char minor_version;
    char flags;
    unsigned int tag_size;
    int extended_header_size;
};

struct ID3_Frame {
    char frame_id[5];
    unsigned int size;
    char flags[2];
    bool unicode; // If ISO-8859-1 is used this flag is false, if Unicode is used this flag is true;
    unsigned int body_size;
    char* body;
};

enum ID3Tag_Field {
    ARTIST,
    ALBUM,
    TITLE,
    COMPOSER,
    TRACK,
    PICTURE
};

class ID3Tag {
    bool initialized = false;
    ID3_Header* header;
    std::vector<ID3_Frame*> frames;
public:
    ID3Tag(std::string file);
    ID3_Frame* getFrame(ID3Tag_Field field);
    std::string getField(ID3Tag_Field field);
    std::string getFieldAsUtf8(ID3Tag_Field field);
};
}

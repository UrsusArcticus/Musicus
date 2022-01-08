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
    char frame_id[4];
    unsigned int size;
    char flags[2];
    char* data;
};

struct ID3_Text_Frame_Content {
    int size;
    bool is_unicode; // If ISO-8859-1 is used this flag is false, if Unicode is used this flag is true;
    char* text;
};

enum ID3Tag_Field {
    ARTIST,
    ALBUM,
    TITLE
};

class ID3Tag {
    bool initialized = false;
    ID3_Header* header;
    std::vector<ID3_Frame*> frames;
public:
    ID3Tag(std::string file);
    ID3_Text_Frame_Content* getField(ID3Tag_Field field);
    std::string getFieldAsUtf8(ID3Tag_Field field);
};
}

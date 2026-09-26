//
//  ID3Tag.cpp
//  Musicus
//
//  Created by Philipp Otte on 20.10.22.
//
#include <fstream>
#include <map>
#include <codecvt>

#include "Utils.h"
#include "ID3Tag.h"

#include <iostream> // Debug output

#define ID3_HEADER_SIZE 10
#define ID3_FRAME_HEADER_SIZE 10
#define NO_COMPATIBLE_TAG_VERSION 0

using namespace std;

using musicus::ID3_Header;
using musicus::ID3Tag_Version;
using musicus::ID3_Frame;
using musicus::ID3Tag_Field;
using musicus::ID3_PictureFrame;
using musicus::read_int_from_bytes;

/*
  The ID3 Tag is given in 4 Bytes but the 7th Bit in each Byte is set to zero
 */
int tagsize_encode(int value) {
    int out, mask = 0x7F;

    while (mask ^ 0x7FFFFFFF) {
        out = value & ~mask;
        out <<= 1;
        out |= value & mask;
        mask = ((mask + 1) << 8) - 1;
        value = out;
    }

    return out;
}

unsigned int tagsize_decode(int value) {
    unsigned int a, b, c, d, result = 0x0;
    a = value & 0xFF;
    b = (value >> 8) & 0xFF;
    c = (value >> 16) & 0xFF;
    d = (value >> 24) & 0xFF;

    result = result | a;
    result = result | (b << 7);
    result = result | (c << 14);
    result = result | (d << 21);

    return result;
}

ID3_Header* parse_tag_header(char* buffer) {
    char header_beg[4];
    memcpy(header_beg, buffer, 3);
    header_beg[3] = 0;
    if ( string(header_beg) != "ID3" ) { // "ID3" indicates that it is a ID3v2-Tag
        return nullptr;
    }
    ID3_Header* header = new ID3_Header();
    // Major Version = 3 and Minor Version = 0 means it is an ID3v2.3.0 Tag
    header->major_version = buffer[3];
    header->minor_version = buffer[4];
    header->flags = buffer[5];
    header->tag_size = tagsize_decode(read_int_from_bytes(buffer, 6));
    header->has_extended_header = (header->flags&(1<<6)) == (1<<6);
    return header;
}

ID3Tag_Version get_tag_version(ID3_Header* header) {
    if (header->major_version == 3)
        return ID3Tag_Version::ID3v23;
    if (header->major_version == 4)
        return ID3Tag_Version::ID3v24;
    return ID3Tag_Version::NO_COMPATIBLE_TAG;
}

bool has_encoding_byte(char* frame_id) {
    if (strcmp(frame_id, "TPE1") == 0) {
        return true;
    }
    if (strcmp(frame_id, "TALB") == 0) {
        return true;
    }
    if (strcmp(frame_id, "TIT2") == 0) {
        return true;
    }
    if (strcmp(frame_id, "TRCK") == 0) {
        return true;
    }
    if (strcmp(frame_id, "TCOM") == 0) {
        return true;
    }
    return false;
}

ID3_Frame* parse_frame(char* bytes, int position, ID3Tag_Version version) {
    ID3_Frame* frame = new ID3_Frame();
    memcpy(frame->frame_id, bytes + position, 4);
    frame->frame_id[4] = '\0';
    if(memcmp(frame->frame_id, "\0\0\0\0", 4) == 0) {
        delete frame;
        return nullptr;
    }
    frame->size = read_int_from_bytes(bytes, position + 4);
    if (version == ID3Tag_Version::ID3v24) {
        frame->size = tagsize_decode(frame->size);
    }
    
    memcpy(frame->flags, bytes + position + 8, 2);
    bool encoding_byte = has_encoding_byte(frame->frame_id);
    frame->unicode = encoding_byte && bytes[position + 10] > 0;
    frame->body = new char [frame->size + 1];
    
    // +13 => 10 (Header) + 1 (Text Encoding byte) + 2 (Unicode BOM)
    memcpy(frame->body, bytes + position + (frame->unicode ? 13 : ( encoding_byte ? 11 : 10)),
           frame->size - (frame->unicode ? 3 : (encoding_byte ? 1 : 0)));
    frame->body_size = frame->size - (frame->unicode ? 3 : (encoding_byte ? 1 : 0)); // Das Byte für das Unicode flag und die 2 byte für das Unicode BOM wieder abziehen
    frame->body[frame->body_size] = '\0';
    return frame;
}

std::string get_frame_id(ID3Tag_Field field) {
    switch (field) {
        case ID3Tag_Field::ARTIST:
            return "TPE1";
        case ID3Tag_Field::ALBUM:
            return "TALB";
        case ID3Tag_Field::TITLE:
            return "TIT2";
        case ID3Tag_Field::COMPOSER:
            return "TCOM";
        case ID3Tag_Field::TRACK:
            return "TRCK";
        case ID3Tag_Field::PICTURE:
            return "APIC";
    }
    return "";
}

musicus::ID3Tag::ID3Tag(string file_name) {
    ifstream file (file_name, ios::in | ios::binary | ios::ate);
    if ( file.is_open() ) {
        char *header_buffer = new char [ID3_HEADER_SIZE];
        file.seekg(0, ios::beg);
        file.read(header_buffer, ID3_HEADER_SIZE);
        ID3_Header* header = parse_tag_header(header_buffer);
        delete[] header_buffer;
        if (header == nullptr || get_tag_version(header) == ID3Tag_Version::NO_COMPATIBLE_TAG) {
            delete header;
            return;
        }
        this->header = header;
        if ( header->has_extended_header ) {
            char* extended_header_buffer = new char[4];// For now we skip the extended header; read only size field to skip it
            file.read(extended_header_buffer, 4);
            int es = read_int_from_bytes(extended_header_buffer, 0);
            file.seekg(ID3_HEADER_SIZE + es);
        }
        char* tagdata_buffer = new char[header->tag_size];
        file.read(tagdata_buffer, header->tag_size);
        file.close();
        ID3_Frame* frame;
        ID3Tag_Version version = get_tag_version(header);
        int position = 0;
        while (position < header->tag_size) {
            frame = parse_frame(tagdata_buffer, position, version);
            if (frame == nullptr) {
                break;
            }
            frames.push_back(frame);
            position += frame->size + ID3_FRAME_HEADER_SIZE;
        }
        delete[] tagdata_buffer;
    }
    initialized = true;
}

ID3_Frame* musicus::ID3Tag::getFrame(ID3Tag_Field field) {
    string frame_id = get_frame_id(field);
    for (int i = 0; i < frames.size(); i++) {
        if (musicus::read_string_from_bytes(frames[i]->frame_id, 0, 4) == frame_id) {
            return frames[i];
        }
    }
    return nullptr;
}

string musicus::ID3Tag::getField(ID3Tag_Field field) {
    ID3_Frame* frame = getFrame(field);
    if ( frame == nullptr) return "";
    if ( frame->unicode ) {
        return getFieldAsUtf8(field);
    }
    return string(frame->body);
}

string musicus::ID3Tag::getFieldAsUtf8(ID3Tag_Field field) {
    ID3_Frame* frame = getFrame(field);
    if ( frame == nullptr || frame->body_size == 0 ) { return "";}
    u16string text_ucs(reinterpret_cast<const char16_t*>(frame->body));
    string text = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t>{}.to_bytes(text_ucs);
    return text;
}

ID3_PictureFrame* musicus::ID3Tag::getPicture() {
    string frame_id = get_frame_id(ID3Tag_Field::PICTURE);
    for ( int i = 0; i < frames.size(); i++) {
        if (musicus::read_string_from_bytes(frames[i]->frame_id, 0, 4) == frame_id) {
            ID3_Frame* frame = frames[i];
            ID3_PictureFrame *picture_frame = new ID3_PictureFrame();
            picture_frame->text_encoding = frame->body[0];
            picture_frame->mime_type = musicus::read_string_from_bytes(frame->body, 1);
            int i = (int)picture_frame->mime_type.length() + 2;
            picture_frame->picture_type = frame->body[i];
            i++;
            picture_frame->description = musicus::read_string_from_bytes(frame->body, i);
            i += picture_frame->description.length() + 1;
            picture_frame->size = (unsigned int)(frame->size - i);
            picture_frame->data = new char[picture_frame->size];
            memcpy(picture_frame->data, frame->body + i, picture_frame->size);
            return picture_frame;
        }
    }
    return nullptr;
}

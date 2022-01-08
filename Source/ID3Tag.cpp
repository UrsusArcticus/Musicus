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
#define NO_COMPATIBLE_TAG_VERSION 0

using namespace std;

using musicus::ID3_Header;
using musicus::ID3Tag_Version;
using musicus::ID3_Frame;
using musicus::ID3Tag_Field;
using musicus::ID3_Text_Frame_Content;
using musicus::btoi;

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
    header->tag_size = tagsize_decode(btoi(buffer, 4, 6));
    cout << "Header Size" << header->tag_size << endl;
    if (((header->flags&(1<<6)) == (1<<6))) { // Check for extended headers
        header->extended_header_size = tagsize_decode(btoi(buffer, 4, 12));
        cout << "extended header exists" << endl;
    } else {
        header->extended_header_size = 0;
    }
    return header;
}

ID3Tag_Version get_tag_version(ID3_Header* header) {
    if (header->major_version == 3)
        return ID3Tag_Version::ID3v23;
    if (header->major_version == 4)
        return ID3Tag_Version::ID3v24;
    return ID3Tag_Version::NO_COMPATIBLE_TAG;
}

ID3_Frame* parse_frame(char* bytes, int position, ID3Tag_Version version) {
    ID3_Frame* frame = new ID3_Frame();
    memcpy(frame->frame_id, bytes + position, 4);
    if(memcmp(frame->frame_id, "\0\0\0\0", 4) == 0) {
        delete frame;
        return nullptr;
    }
    frame->size = btoi(bytes, 4, position + 4);
    if (version == ID3Tag_Version::ID3v24) {
        frame->size = tagsize_decode(frame->size);
    }
    memcpy(frame->flags, bytes + position + 8, 2);
    frame->data = (char*) malloc(frame->size * sizeof(char));
    memcpy(frame->data, bytes + position + 10, frame->size);
    std::cout << "Frame ID: " << frame->frame_id << " Frame Content: " << frame->data << endl;
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
    }
    return "";
}

musicus::ID3Tag::ID3Tag(string file_name) {
    ifstream file (file_name, ios::in | ios::binary | ios::ate);
    if ( file.is_open() ) {
        streampos file_size = file.tellg();
        char *file_buffer = new char [(int)file_size];
        file.seekg(0, ios::beg);
        file.read(file_buffer, file_size);
        file.close();
        ID3_Header* header = parse_tag_header(file_buffer);
        if (header == nullptr || get_tag_version(header) == ID3Tag_Version::NO_COMPATIBLE_TAG) {
            delete[] file_buffer;
            delete header;
            return;
        }
        this->header = header;
        int position = 10 + header->extended_header_size; // 10 is the size of the header
        ID3_Frame* frame;
        while (position < header->tag_size) {
            frame = parse_frame(file_buffer, position, get_tag_version(header));
            if (frame == nullptr) {
                break;
            }
            frames.push_back(frame);
            position += frame->size + 10; // 10 is the size of the frame header
        }
        delete[] file_buffer;
    }
    initialized = true;
}

ID3_Text_Frame_Content* musicus::ID3Tag::getField(ID3Tag_Field field) {
    string frame_id = get_frame_id(field);
    std::cout << "Frame ID: " << frame_id << std::endl;
    for (int i = 0; i < frames.size(); i++) {
        if (musicus::bytes_to_string(frames[i]->frame_id, 0, 4) == frame_id) {
            ID3_Text_Frame_Content* content = new ID3_Text_Frame_Content();
            content->size = frames[i]->size - 1;
            content->is_unicode = (bool)frames[i]->data[0];
            content->text = (char*) malloc(content->size * sizeof(char));
            memcpy(content->text, frames[i]->data + 1, content->size);
            std::cout << content->text << endl;
            return content;
        }
    }
    return nullptr;
}

string musicus::ID3Tag::getFieldAsUtf8(ID3Tag_Field field) {
    ID3_Text_Frame_Content* content = getField(field);
    if ( content == nullptr || !content->is_unicode ) {
        return nullptr;
    }
    u16string field_string = u"";
    for (int i = 0; i < content->size; i += 2) {
        if ((i + 1) >= content->size) {
            break;
        }
        char16_t first_char = (char16_t)content->text[i];
        char16_t second_char = content->text[i+1];
        field_string.append(1, (first_char << 8) | second_char);
    }
    std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> converter;
    return converter.to_bytes(field_string);
}

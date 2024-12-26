// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/language.h>

#include <framework/file.h>

#include <templates/stackpool.h>
#include <templates/hashmap.h>

#include <config.h>

#include <cstring>
#include <fstream>

/**
 * @namespace tram::Language
 * 
 * Localization services.
 * 
 * Provides services for application localization, character re-encoding, 
 * translation, etc.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/framework/language.html
 */

namespace tram::Language {
    
static StackPool<char> language_string_pool("Language string pool", LANUGAGE_STRING_CHARACTER_LIMIT);
static Hashmap<const char*> language_string_map("Language string hashmap", LANGUAGE_STRING_LIMIT);

/// Loads a language file.
void Load(const char* filename){
    char path [100] = "data/";
    strcat (path, filename);
    strcat (path, ".lang");
    
    File file (path, MODE_READ);
    
    if (!file.is_open()) {
        std::cout << "Can't find language file " << path << std::endl;
    }
    
    while (file.is_continue()) {
        name_t string_name = file.read_name();
        std::string_view string_content = file.read_line();
        
        char* string_ptr = language_string_pool.AddNew(string_content.length() + 1);

        strncpy(string_ptr, string_content.data(), string_content.length());
        string_ptr[string_content.length()] = '\0';

        language_string_map.Insert(string_name, string_ptr);
    }
}

/// Retrieves a string from the loaded language files.
const char* Get(name_t name) {
    const char* string = language_string_map.Find(name);
    
    if (!string) {
        return name;
    } else {
        return string;
    }
}

/// Converts a UTF16 codepoint to a Latin7 codepoint.
char UTF16ToLatin7(uint16_t code) {
    switch (code) {
        case 0x0000: return 0x00;
        case 0x0001: return 0x01;
        case 0x0002: return 0x02;
        case 0x0003: return 0x03;
        case 0x0004: return 0x04;
        case 0x0005: return 0x05;
        case 0x0006: return 0x06;
        case 0x0007: return 0x07;
        case 0x0008: return 0x08;
        case 0x0009: return 0x09;
        case 0x000a: return 0x0a;
        case 0x000b: return 0x0b;
        case 0x000c: return 0x0c;
        case 0x000d: return 0x0d;
        case 0x000e: return 0x0e;
        case 0x000f: return 0x0f;
        case 0x0010: return 0x10;
        case 0x0011: return 0x11;
        case 0x0012: return 0x12;
        case 0x0013: return 0x13;
        case 0x0014: return 0x14;
        case 0x0015: return 0x15;
        case 0x0016: return 0x16;
        case 0x0017: return 0x17;
        case 0x0018: return 0x18;
        case 0x0019: return 0x19;
        case 0x001a: return 0x1a;
        case 0x001b: return 0x1b;
        case 0x001c: return 0x1c;
        case 0x001d: return 0x1d;
        case 0x001e: return 0x1e;
        case 0x001f: return 0x1f;
        case 0x0020: return 0x20;
        case 0x0021: return 0x21;
        case 0x0022: return 0x22;
        case 0x0023: return 0x23;
        case 0x0024: return 0x24;
        case 0x0025: return 0x25;
        case 0x0026: return 0x26;
        case 0x0027: return 0x27;
        case 0x0028: return 0x28;
        case 0x0029: return 0x29;
        case 0x002a: return 0x2a;
        case 0x002b: return 0x2b;
        case 0x002c: return 0x2c;
        case 0x002d: return 0x2d;
        case 0x002e: return 0x2e;
        case 0x002f: return 0x2f;
        case 0x0030: return 0x30;
        case 0x0031: return 0x31;
        case 0x0032: return 0x32;
        case 0x0033: return 0x33;
        case 0x0034: return 0x34;
        case 0x0035: return 0x35;
        case 0x0036: return 0x36;
        case 0x0037: return 0x37;
        case 0x0038: return 0x38;
        case 0x0039: return 0x39;
        case 0x003a: return 0x3a;
        case 0x003b: return 0x3b;
        case 0x003c: return 0x3c;
        case 0x003d: return 0x3d;
        case 0x003e: return 0x3e;
        case 0x003f: return 0x3f;
        case 0x0040: return 0x40;
        case 0x0041: return 0x41;
        case 0x0042: return 0x42;
        case 0x0043: return 0x43;
        case 0x0044: return 0x44;
        case 0x0045: return 0x45;
        case 0x0046: return 0x46;
        case 0x0047: return 0x47;
        case 0x0048: return 0x48;
        case 0x0049: return 0x49;
        case 0x004a: return 0x4a;
        case 0x004b: return 0x4b;
        case 0x004c: return 0x4c;
        case 0x004d: return 0x4d;
        case 0x004e: return 0x4e;
        case 0x004f: return 0x4f;
        case 0x0050: return 0x50;
        case 0x0051: return 0x51;
        case 0x0052: return 0x52;
        case 0x0053: return 0x53;
        case 0x0054: return 0x54;
        case 0x0055: return 0x55;
        case 0x0056: return 0x56;
        case 0x0057: return 0x57;
        case 0x0058: return 0x58;
        case 0x0059: return 0x59;
        case 0x005a: return 0x5a;
        case 0x005b: return 0x5b;
        case 0x005c: return 0x5c;
        case 0x005d: return 0x5d;
        case 0x005e: return 0x5e;
        case 0x005f: return 0x5f;
        case 0x0060: return 0x60;
        case 0x0061: return 0x61;
        case 0x0062: return 0x62;
        case 0x0063: return 0x63;
        case 0x0064: return 0x64;
        case 0x0065: return 0x65;
        case 0x0066: return 0x66;
        case 0x0067: return 0x67;
        case 0x0068: return 0x68;
        case 0x0069: return 0x69;
        case 0x006a: return 0x6a;
        case 0x006b: return 0x6b;
        case 0x006c: return 0x6c;
        case 0x006d: return 0x6d;
        case 0x006e: return 0x6e;
        case 0x006f: return 0x6f;
        case 0x0070: return 0x70;
        case 0x0071: return 0x71;
        case 0x0072: return 0x72;
        case 0x0073: return 0x73;
        case 0x0074: return 0x74;
        case 0x0075: return 0x75;
        case 0x0076: return 0x76;
        case 0x0077: return 0x77;
        case 0x0078: return 0x78;
        case 0x0079: return 0x79;
        case 0x007a: return 0x7a;
        case 0x007b: return 0x7b;
        case 0x007c: return 0x7c;
        case 0x007d: return 0x7d;
        case 0x007e: return 0x7e;
        case 0x007f: return 0x7f;
        case 0x0080: return 0x80;
        case 0x0081: return 0x81;
        case 0x0082: return 0x82;
        case 0x0083: return 0x83;
        case 0x0084: return 0x84;
        case 0x0085: return 0x85;
        case 0x0086: return 0x86;
        case 0x0087: return 0x87;
        case 0x0088: return 0x88;
        case 0x0089: return 0x89;
        case 0x008a: return 0x8a;
        case 0x008b: return 0x8b;
        case 0x008c: return 0x8c;
        case 0x008d: return 0x8d;
        case 0x008e: return 0x8e;
        case 0x008f: return 0x8f;
        case 0x0090: return 0x90;
        case 0x0091: return 0x91;
        case 0x0092: return 0x92;
        case 0x0093: return 0x93;
        case 0x0094: return 0x94;
        case 0x0095: return 0x95;
        case 0x0096: return 0x96;
        case 0x0097: return 0x97;
        case 0x0098: return 0x98;
        case 0x0099: return 0x99;
        case 0x009a: return 0x9a;
        case 0x009b: return 0x9b;
        case 0x009c: return 0x9c;
        case 0x009d: return 0x9d;
        case 0x009e: return 0x9e;
        case 0x009f: return 0x9f;
        case 0x00a0: return 0xa0;
        case 0x201d: return 0xa1;
        case 0x00a2: return 0xa2;
        case 0x00a3: return 0xa3;
        case 0x00a4: return 0xa4;
        case 0x201e: return 0xa5;
        case 0x00a6: return 0xa6;
        case 0x00a7: return 0xa7;
        case 0x00d8: return 0xa8;
        case 0x00a9: return 0xa9;
        case 0x0156: return 0xaa;
        case 0x00ab: return 0xab;
        case 0x00ac: return 0xac;
        case 0x00ad: return 0xad;
        case 0x00ae: return 0xae;
        case 0x00c6: return 0xaf;
        case 0x00b0: return 0xb0;
        case 0x00b1: return 0xb1;
        case 0x00b2: return 0xb2;
        case 0x00b3: return 0xb3;
        case 0x201c: return 0xb4;
        case 0x00b5: return 0xb5;
        case 0x00b6: return 0xb6;
        case 0x00b7: return 0xb7;
        case 0x00f8: return 0xb8;
        case 0x00b9: return 0xb9;
        case 0x0157: return 0xba;
        case 0x00bb: return 0xbb;
        case 0x00bc: return 0xbc;
        case 0x00bd: return 0xbd;
        case 0x00be: return 0xbe;
        case 0x00e6: return 0xbf;
        case 0x0104: return 0xc0;
        case 0x012e: return 0xc1;
        case 0x0100: return 0xc2;
        case 0x0106: return 0xc3;
        case 0x00c4: return 0xc4;
        case 0x00c5: return 0xc5;
        case 0x0118: return 0xc6;
        case 0x0112: return 0xc7;
        case 0x010c: return 0xc8;
        case 0x00c9: return 0xc9;
        case 0x0179: return 0xca;
        case 0x0116: return 0xcb;
        case 0x0122: return 0xcc;
        case 0x0136: return 0xcd;
        case 0x012a: return 0xce;
        case 0x013b: return 0xcf;
        case 0x0160: return 0xd0;
        case 0x0143: return 0xd1;
        case 0x0145: return 0xd2;
        case 0x00d3: return 0xd3;
        case 0x014c: return 0xd4;
        case 0x00d5: return 0xd5;
        case 0x00d6: return 0xd6;
        case 0x00d7: return 0xd7;
        case 0x0172: return 0xd8;
        case 0x0141: return 0xd9;
        case 0x015a: return 0xda;
        case 0x016a: return 0xdb;
        case 0x00dc: return 0xdc;
        case 0x017b: return 0xdd;
        case 0x017d: return 0xde;
        case 0x00df: return 0xdf;
        case 0x0105: return 0xe0;
        case 0x012f: return 0xe1;
        case 0x0101: return 0xe2;
        case 0x0107: return 0xe3;
        case 0x00e4: return 0xe4;
        case 0x00e5: return 0xe5;
        case 0x0119: return 0xe6;
        case 0x0113: return 0xe7;
        case 0x010d: return 0xe8;
        case 0x00e9: return 0xe9;
        case 0x017a: return 0xea;
        case 0x0117: return 0xeb;
        case 0x0123: return 0xec;
        case 0x0137: return 0xed;
        case 0x012b: return 0xee;
        case 0x013c: return 0xef;
        case 0x0161: return 0xf0;
        case 0x0144: return 0xf1;
        case 0x0146: return 0xf2;
        case 0x00f3: return 0xf3;
        case 0x014d: return 0xf4;
        case 0x00f5: return 0xf5;
        case 0x00f6: return 0xf6;
        case 0x00f7: return 0xf7;
        case 0x0173: return 0xf8;
        case 0x0142: return 0xf9;
        case 0x015b: return 0xfa;
        case 0x016b: return 0xfb;
        case 0x00fc: return 0xfc;
        case 0x017c: return 0xfd;
        case 0x017e: return 0xfe;
        case 0x2019: return 0xff;
        default: return code < 256 ? code : 15;
    }
}

}
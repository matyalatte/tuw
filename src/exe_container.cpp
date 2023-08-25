#include "exe_container.h"
#include <cassert>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/error/en.h"

static uint32_t ReadUint32(FILE* io) {
    unsigned char int_as_bin[4];
    fread(int_as_bin, 1, 4, io);
    uint32_t num = 0;
    for (size_t i = 0; i < 4; i++)
        num += int_as_bin[i] << (i * 8);
    return num;
}

static void WriteUint32(FILE* io, const uint32_t& num) {
    unsigned char int_as_bin[4];
    for (size_t i = 0; i < 4; i++)
        int_as_bin[i] = num >> (i * 8);
    fwrite(int_as_bin, 1, 4, io);
}

static void ReadStr(FILE* io, std::string& str, const uint32_t& size) {
    // assert str == "";
    char buff[1025];
    buff[1024] = 0;
    uint32_t pos = 0;
    while (pos < size) {
        if (size - pos < 1024) {
            fread(buff, 1, size - pos, io);
            buff[size - pos] = 0;
            str += buff;
            break;
        }
        fread(buff, 1, 1024, io);
        str += buff;
        pos += 1024;
    }
}

static void WriteStr(FILE* io, const std::string& str) {
    uint32_t size = str.length();
    uint32_t pos = 0;
    while (pos < size) {
        if (size - pos <= 1024) {
            fwrite(str.substr(pos, size).c_str(), 1, size - pos, io);
            break;
        }
        fwrite(str.substr(pos, pos + 1024).c_str(), 1, 1024, io);
        pos += 1024;
    }
    fpos_t ft;
    fgetpos(io, &ft);
    size_t padding = (8 - (ft) % 8) % 8;
    char buff[8];
    fwrite(buff, 1, padding, io);
}

static void CopyBinary(FILE* reader, FILE* writer, const uint32_t& size) {
    uint32_t pos = 0;
    char buff[1024];
    while (pos < size) {
        uint32_t rest = size - pos;
        if (rest < 1024) {
            fread(buff, 1, rest, reader);
            fwrite(buff, 1, rest, writer);
            break;
        }
        fread(buff, 1, 1024, reader);
        fwrite(buff, 1, 1024, writer);
        pos += 1024;
    }
}

static std::string ReadMagic(FILE* io) {
    char magic[5];
    magic[4] = 0;
    fread(magic, 1, 4, io);
    return std::string(magic);
}

static const uint32_t FNV_OFFSET_BASIS_32 = 2166136261U;
static const uint32_t FNV_PRIME_32 = 16777619U;

static uint32_t Fnv1Hash32(const std::string& str) {
    uint32_t hash = FNV_OFFSET_BASIS_32;
    for (const char& c : str) hash = (FNV_PRIME_32 * hash) ^ c;
    return hash;
}

static uint32_t Tell(FILE* io) {
    fpos_t cur;
    fgetpos(io, &cur);
    return cur;
}

static uint32_t Length(FILE* io) {
    uint32_t cur = Tell(io);
    fseek(io, 0, SEEK_END);
    uint32_t len = Tell(io);
    fseek(io, cur, SEEK_CUR);
    return len;
}

static const uint32_t EXE_SIZE_MAX = 20000000;  // Allowed size of exe
static const uint32_t JSON_SIZE_MAX = 1000000;  // Allowed size of json

json_utils::JsonResult ExeContainer::Read(const std::string& exe_path) {
    m_exe_path = exe_path;
    FILE* file_io = fopen(exe_path.c_str(), "rb");
    if (!file_io)
        return { false, "Failed to open " + exe_path };

    // Read the last 4 bytes
    fseek(file_io, 0, SEEK_END);
    uint32_t end_off = Tell(file_io);
    fseek(file_io, -4, SEEK_CUR);
    std::string magic = ReadMagic(file_io);

    if ( magic != "JSON" ) {
        // Json data not found
        m_exe_size = end_off;
        fclose(file_io);
        return { true };
    }

    // Read exe size
    fseek(file_io, -8, SEEK_CUR);
    m_exe_size = end_off + ReadUint32(file_io);
    if (EXE_SIZE_MAX <= m_exe_size || end_off < m_exe_size) {
        std::string msg = "Unexpected exe size. (" + std::to_string(m_exe_size) + ")";
        fclose(file_io);
        return { false, msg };
    }
    fseek(file_io, m_exe_size, SEEK_SET);

    // Read a header for json data
    magic = ReadMagic(file_io);
    if (magic != "JSON") {
        fclose(file_io);
        return { false, "Invalid magic. (" + magic + ")" };
    }

    uint32_t json_size = ReadUint32(file_io);
    uint32_t stored_hash = ReadUint32(file_io);
    if (JSON_SIZE_MAX <= json_size || end_off < m_exe_size + json_size + 20) {
        std::string msg = "Unexpected json size. (" + std::to_string(json_size) + ")";
        fclose(file_io);
        return { false, msg };
    }

    // Read json data
    std::string json_str = "";
    ReadStr(file_io, json_str, json_size);
    fclose(file_io);

    if (json_str.length() != json_size)
        return { false, "Unexpected char detected." };

    if (stored_hash != Fnv1Hash32(json_str)) {
        std::string msg = "Invalid JSON hash. (" + std::to_string(stored_hash) + ")";
        return { false, msg };
    }

    rapidjson::ParseResult ok = m_json.Parse(json_str);
    if (!ok) {
        std::string msg = std::string("Failed to parse JSON: ")
                          + rapidjson::GetParseError_En(ok.Code())
                          + " (offset: " + std::to_string(ok.Offset()) + ")";
        return { false, msg };
    }

    return { true };
}

json_utils::JsonResult ExeContainer::Write(const std::string& exe_path) {
    assert(m_exe_path != "");
    std::string json_str = "";
    if (HasJson())
        json_str = json_utils::JsonToString(m_json);

    uint32_t json_size = json_str.length();
    if (JSON_SIZE_MAX <= json_size) {
        std::string msg = "Unexpected json size. (" + std::to_string(json_size) + ")";
        return { false, msg };
    }

    FILE* old_io = fopen(m_exe_path.c_str(), "rb");
    if (!old_io) {
        std::string msg = "Failed to open a file. (" + m_exe_path + ")";
        return { false, msg };
    }
    FILE* new_io = fopen(exe_path.c_str(), "wb");
    if (!new_io) {
        std::string msg = "Failed to open a file. (" + exe_path + ")";
        fclose(old_io);
        return { false, msg };
    }
    m_exe_path = exe_path;

    CopyBinary(old_io, new_io, m_exe_size);

    if (Tell(old_io) != Length(old_io)) {
        std::string magic = ReadMagic(old_io);
        if (magic != "JSON") {
            fclose(old_io);
            fclose(new_io);
            return { false, "Invalid magic. (" + magic + ")" };
        }
    }

    fclose(old_io);
    if (json_size == 0) {
        fclose(new_io);
        return { true };
    }

    // Write json data
    fwrite("JSON", 1, 4, new_io);
    WriteUint32(new_io, json_size);
    WriteUint32(new_io, Fnv1Hash32(json_str));
    WriteStr(new_io, json_str);
    WriteUint32(new_io, m_exe_size - Tell(new_io) - 8);
    fwrite("JSON", 1, 4, new_io);
    fclose(new_io);
    return { true };
}

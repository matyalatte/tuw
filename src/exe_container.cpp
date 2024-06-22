#include "exe_container.h"
#include <cassert>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/error/en.h"

#include "string_utils.h"

static uint32_t ReadUint32(FILE* io) {
    unsigned char int_as_bin[4];
    size_t ret = fread(int_as_bin, 1, 4, io);
    if (ret != 4)
        return 0;
    uint32_t num = 0;
    for (size_t i = 0; i < 4; i++)
        num += int_as_bin[i] << (i * 8);
    return num;
}

static void WriteUint32(FILE* io, const uint32_t& num) {
    unsigned char int_as_bin[4];
    for (size_t i = 0; i < 4; i++)
        int_as_bin[i] = (unsigned char)(num >> (i * 8));
    fwrite(int_as_bin, 1, 4, io);
}

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define BUF_SIZE 1024

static std::string ReadStr(FILE* io, const uint32_t& size) {
    std::string str = "";
    char buff[BUF_SIZE + 1];
    buff[BUF_SIZE] = 0;
    uint32_t pos = 0;
    while (pos < size) {
        size_t ret;
        uint32_t rest = size - pos;
        size_t copy_size = MIN(BUF_SIZE, rest);
        ret = fread(buff, 1, copy_size, io);
        if (ret != copy_size)
            return "";
        buff[copy_size] = 0;
        str += buff;

        if (rest < BUF_SIZE)
            break;
        pos += BUF_SIZE;
    }
    return str;
}

static void WriteStr(FILE* io, const std::string& str) {
    uint32_t size = (uint32_t)str.length();
    uint32_t pos = 0;
    while (pos < size) {
        if (size - pos <= BUF_SIZE) {
            fwrite(str.substr(pos, size).c_str(), 1, size - pos, io);
            break;
        }
        fwrite(str.substr(pos, pos + BUF_SIZE).c_str(), 1, BUF_SIZE, io);
        pos += BUF_SIZE;
    }
    size_t padding = (8 - ftell(io) % 8) % 8;
    char buff[8];
    fwrite(buff, 1, padding, io);
}

static bool CopyBinary(FILE* reader, FILE* writer, const uint32_t& size) {
    uint32_t pos = 0;
    char buff[BUF_SIZE];
    while (pos < size) {
        uint32_t rest = size - pos;
        size_t copy_size = MIN(BUF_SIZE, rest);
        size_t ret = fread(buff, 1, copy_size, reader);
        if (ret != copy_size)
            return false;
        fwrite(buff, 1, copy_size, writer);

        if (rest < BUF_SIZE)
            break;
        pos += BUF_SIZE;
    }
    return true;
}

static std::string ReadMagic(FILE* io) {
    char magic[5];
    magic[4] = 0;
    size_t ret = fread(magic, 1, 4, io);
    if (ret != 4)
        return "";
    return std::string(magic);
}

static uint32_t Length(FILE* io) {
    uint32_t cur = ftell(io);
    fseek(io, 0, SEEK_END);
    uint32_t len = ftell(io);
    fseek(io, cur, SEEK_SET);
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
    uint32_t end_off = ftell(file_io);
    fseek(file_io, -4, SEEK_CUR);
    std::string magic = ReadMagic(file_io);

    if ( magic != "JSON" ) {
        // Json data not found
        m_exe_size = end_off;
        fclose(file_io);
        return JSON_RESULT_OK;
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
    std::string json_str = ReadStr(file_io, json_size);
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

    return JSON_RESULT_OK;
}

json_utils::JsonResult ExeContainer::Write(const std::string& exe_path) {
    assert(m_exe_path != "");
    std::string json_str = "";
    if (HasJson())
        json_str = json_utils::JsonToString(m_json);

    uint32_t json_size = (uint32_t)json_str.length();
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

    bool ok = CopyBinary(old_io, new_io, m_exe_size);
    if (!ok) {
        fclose(old_io);
        fclose(new_io);
        return { false, "Failed to copy the original executable (" + m_exe_path + ")" };
    }

    uint32_t pos = ftell(old_io);
    if (pos != Length(old_io)) {
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
        return JSON_RESULT_OK;
    }

    // Write json data
    fwrite("JSON", 1, 4, new_io);
    WriteUint32(new_io, json_size);
    WriteUint32(new_io, Fnv1Hash32(json_str));
    WriteStr(new_io, json_str);
    WriteUint32(new_io, m_exe_size - ftell(new_io) - 8);
    fwrite("JSON", 1, 4, new_io);
    fclose(new_io);
    return JSON_RESULT_OK;
}

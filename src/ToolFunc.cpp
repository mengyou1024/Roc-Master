#include "pch.h"

#include "ToolFunc.h"
#include <Model/UserModel.h>
#include <Thread.h>
#include <fstream>
#include <iostream>
#include <regex>

float PointToSegDist(float x, float y, float x1, float y1, float x2, float y2) {
    float cross = (x2 - x1) * (x - x1) + (y2 - y1) * (y - y1);
    if (cross <= 0)
        return sqrt((x - x1) * (x - x1) + (y - y1) * (y - y1));

    float d2 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
    if (cross >= d2)
        return sqrt((x - x2) * (x - x2) + (y - y2) * (y - y2));

    float r  = cross / d2;
    float px = x1 + (x2 - x1) * r;
    float py = y1 + (y2 - y1) * r;
    return sqrt((x - px) * (x - px) + (py - y) * (py - y));
}

void RemoveBaseline(USHORT* pInput, float* pOutput, int iSize) {
    const INT iSmooth        = 32;
    USHORT    pLast[iSmooth] = {0};

    float fSum = 0;
    for (int i = 0; i < iSmooth; i++) {
        fSum += pInput[i];
        pLast[i] = pInput[i];
    }

    for (int i = 0; i < iSize; i++) {
        int iLast    = i % iSmooth;
        fSum         = fSum + pInput[i] - pLast[iLast];
        pLast[iLast] = pInput[i];

        float fBase = fSum / iSmooth;
        pOutput[i]  = pInput[i] - fBase;
    }
}

// 线段相交
float LineIntersection(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y, glm::vec2& pt) {
    float s02_x, s02_y, s10_x, s10_y, s32_x, s32_y, s_numer, t_numer, denom, t;
    s10_x = p1_x - p0_x;
    s10_y = p1_y - p0_y;
    s32_x = p3_x - p2_x;
    s32_y = p3_y - p2_y;

    denom = s10_x * s32_y - s32_x * s10_y;
    if (denom == 0)
        return -1;
    bool denomPositive = denom > 0;

    s02_x   = p0_x - p2_x;
    s02_y   = p0_y - p2_y;
    s_numer = s10_x * s02_y - s10_y * s02_x;
    if ((s_numer < 0) == denomPositive)
        return -1;

    t_numer = s32_x * s02_y - s32_y * s02_x;
    if ((t_numer < 0) == denomPositive)
        return -1;

    if (abs(s_numer) > abs(denom) || abs(t_numer) > abs(denom))
        return -1;

    t = t_numer / denom;

    pt.x = p0_x + (t * s10_x);
    pt.y = p0_y + (t * s10_y);
    return t;
}

void GenVAO(GLuint& VAO, GLuint& VBO, GLuint& VEO) {
    if (VAO == 0)
        glGenVertexArrays(1, &VAO);
    if (VBO == 0)
        glGenBuffers(1, &VBO);
    if (VEO == 0)
        glGenBuffers(1, &VEO);
}

void GenVAO(GLuint& VAO, GLuint& VBO) {
    if (VAO == 0)
        glGenVertexArrays(1, &VAO);
    if (VBO == 0)
        glGenBuffers(1, &VBO);
}

void DeleteVAO(GLuint& VAO, GLuint& VBO, GLuint& VEO) {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }

    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    if (VEO != 0) {
        glDeleteBuffers(1, &VEO);
        VEO = 0;
    }
}

void DeleteVAO(GLuint& VAO, GLuint& VBO) {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }

    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
}

void DeleteTexture(GLuint& Texture) {
    if (Texture != 0) {
        glDeleteTextures(1, &Texture);
        Texture = 0;
    }
}

void DeleteBuffers(GLuint& Buffer) {
    if (Buffer != 0) {
        glDeleteBuffers(1, &Buffer);
        Buffer = 0;
    }
}

void GenFBO(GLuint& FBO, GLuint& FBOTEX, GLuint& RBO, GLuint& PBO, GLuint& OUTTEX, GLsizei width, GLsizei height) {
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &FBOTEX);
    glBindTexture(GL_TEXTURE_2D, FBOTEX);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBOTEX, 0);

    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenBuffers(1, &PBO);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, PBO);
    glBufferData(GL_PIXEL_PACK_BUFFER, static_cast<unsigned long long>(width) * height * sizeof(DWORD), NULL, GL_DYNAMIC_COPY);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

    glGenTextures(1, &OUTTEX);
    glBindTexture(GL_TEXTURE_2D, OUTTEX);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void DeleteFBO(GLuint& FBO, GLuint& FBOTEX, GLuint& RBO, GLuint& PBO, GLuint& OUTTEX) {
    if (FBO != 0) {
        glDeleteFramebuffers(1, &FBO);
        FBO = 0;
    }

    if (FBOTEX != 0) {
        glDeleteTextures(1, &FBOTEX);
        FBOTEX = 0;
    }

    if (RBO != 0) {
        glDeleteRenderbuffers(1, &RBO);
        RBO = 0;
    }

    if (PBO != 0) {
        glDeleteBuffers(1, &PBO);
        PBO = 0;
    }

    if (OUTTEX != 0) {
        glDeleteTextures(1, &OUTTEX);
        OUTTEX = 0;
    }
}

void AddQuadsStrip(GLuint* pEBO, int iSize) {
    int iIndex = 0;
    for (int i = 0; i < iSize - 2; i++) {
        pEBO[iIndex++] = i;
        pEBO[iIndex++] = i + 1;
        pEBO[iIndex++] = i + 2;
    }
}

glm::vec3 NormalLine(float* p0, float* p1, float* p2) {
    glm::vec3 a(p1[0] - p0[0], p1[1] - p0[1], p1[2] - p0[2]);
    glm::vec3 b(p2[0] - p0[0], p2[1] - p0[1], p2[2] - p0[2]);

    return glm::normalize(glm::cross(a, b));
}

std::string StringFromWString(std::wstring str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(str);
}

std::wstring WStringFromString(std::string str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str);
}

std::string GB2312ToUtf8(std::string gb2312) {
    char buf[1024] = {};
    int  len       = GB2312ToUtf8(gb2312.c_str(), buf);
    return std::string(buf, len - 1);
}

std::string Utf8ToGB2312(std::string utf8) {
    char buf[1024] = {};
    int  len       = Utf8ToGB2312(utf8.c_str(), buf);
    return std::string(buf, len - 1);
}

// GB2312到UTF-8的转换
int GB2312ToUtf8(const char* gb2312, char* utf8) {
    int      len  = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len + 1];
    memset(wstr, 0, static_cast<size_t>(len + 1));
    MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, utf8, len, NULL, NULL);
    if (wstr)
        delete[] wstr;
    return len;
}

// UTF-8到GB2312的转换
int Utf8ToGB2312(const char* utf8, char* gb2312) {
    int      len  = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len + 1];
    memset(wstr, 0, static_cast<size_t>(len + 1));
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
    len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
    WideCharToMultiByte(CP_ACP, 0, wstr, -1, gb2312, len, NULL, NULL);
    if (wstr)
        delete[] wstr;
    return len;
}

bool CreateMultipleDirectory(LPCTSTR szPath) {
    CString strDir(szPath);                               // 存放要创建的目录字符串
    if (strDir.GetAt(strDir.GetLength() - 1) != _T('\\')) // 确保以'\'结尾以创建最后一个目录
    {
        strDir.AppendChar(_T('\\'));
    }
    vector<CString> vPath;            // 存放每一层目录字符串
    CString         strTemp;          // 一个临时变量,存放目录字符串
    bool            bSuccess = false; // 成功标志
    // 遍历要创建的字符串
    for (int i = 0; i < strDir.GetLength(); ++i) {
        if (strDir.GetAt(i) != _T('\\')) { // 如果当前字符不是'\\'
            strTemp.AppendChar(strDir.GetAt(i));
        } else {                      // 如果当前字符是'\\'
            vPath.push_back(strTemp); // 将当前层的字符串添加到数组中
            strTemp.AppendChar(_T('\\'));
        }
    }
    // 遍历存放目录的数组,创建每层目录
    vector<CString>::const_iterator vIter;
    for (vIter = vPath.begin(); vIter != vPath.end(); vIter++) {
        if (!PathIsDirectory(*vIter)) // 判断当前目录时候存在，不存在就创建
        {
            // 如果CreateDirectory执行成功,返回true,否则返回false
            bSuccess = CreateDirectory(*vIter, NULL) ? true : false;
        }
    }
    return bSuccess;
}

bool PointInRect(RECT rc, ::CPoint pt) {
    if (pt.x >= rc.left && pt.x <= rc.right && pt.y >= rc.top && pt.y <= rc.bottom) {
        return true;
    }
    return false;
}

int GetVersionCode(std::string version) {
    std::regex  reg(R"([vV]?(\d+)\.(\d+)\.?(\d+)?)");
    std::smatch match;
    if (!std::regex_search(version, match, reg)) {
        return -1;
    }
    int curVer = 0;
    for (size_t i = 1; i < std::min(match.size(), 4ull); i++) {
        curVer |= (atol(match[i].str().data()) & 0xFF) << ((3ull - i) * 8);
    }
    return curVer;
}

bool Check4Update(std::string currentVersion, std::string newVersion) {
    int curVer = GetVersionCode(currentVersion);
    int newVer = GetVersionCode(newVersion);
    if (curVer < 0 || newVer < 0) {
        return false;
    }
    return newVer > curVer;
}

size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream) {
    string data((const char*)ptr, (size_t)size * nmemb);
    *((std::stringstream*)stream) << data << std::endl;
    return size * nmemb;
}

std::tuple<string, string, string> GetLatestReleaseNote(std::string github_api_url) {
    std::stringstream out;
    void*             curl = curl_easy_init();
    // 设置URL
    curl_easy_setopt(curl, CURLOPT_URL, github_api_url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Dark Secret Ninja/1.0");
    // 设置接收数据的处理函数和存放变量
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }

    string str_json = out.str();
    curl_easy_cleanup(curl);
    try {
        JSONCPP_STRING                          err;
        Json::Value                             root;
        Json::CharReaderBuilder                 builder;
        const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
        if (!reader->parse(str_json.c_str(), str_json.c_str() + str_json.length(), &root, &err)) {
            spdlog::error("parser json string erroor");
            return std::tuple<string, string, string>();
        }
        string url = {};
        for (size_t i = 0; i < root["assets"].size(); i++) {
            std::regex  reg(R"((.+\.exe))");
            std::smatch match;
            std::string str = root["assets"][static_cast<int>(i)]["browser_download_url"].asString();
            if (std::regex_match(str, match, reg)) {
                url = match[1].str();
            }
        }
        return std::tuple<string, string, string>(root["tag_name"].asString(), root["body"].asString(), url);
    } catch (std::exception& e) { spdlog::error(GB2312ToUtf8(e.what())); }

    return std::tuple<string, string, string>();
}

bool WordTemplateRender(std::wstring templateName, std::wstring fileName, std::map<string, string> var) {
    // 拷贝模板
    std::wregex  reg(LR"(^(.+)[/\\])");
    std::wsmatch match;
    if (std::regex_search(fileName, match, reg)) {
        std::wstring str = match[1].str();
        std::replace(str.begin(), str.end(), L'/', L'\\');
        CreateMultipleDirectory(str.data());
    }
    if (CopyFile(templateName.data(), fileName.data(), false) == 0) {
        return false;
    }
    duckx::Document doc(StringFromWString(fileName));
    doc.open();
    auto replaceFunc = [&var](duckx::Run& run) {
        std::regex  reg(R"(\$\{(.+?)\})");
        std::smatch match;
        string      str = run.get_text();
        if (std::regex_search(str, match, reg)) {
            string newStr = std::regex_replace(str, reg, var[(match[1].str())]);
            spdlog::debug("replace {: ^24} ---> {: ^24}", match[1].str(), var[match[1].str()]);
            run.set_text(newStr);
        }
    };

    for (duckx::Paragraph p : doc.paragraphs()) {
        for (duckx::Run r : p.runs()) {
            replaceFunc(r);
        }
    }

    for (duckx::Table table : doc.tables()) {
        for (duckx::TableRow row : table.rows()) {
            for (duckx::TableCell cell : row.cells()) {
                for (duckx::Paragraph paragh : cell.paragraphs()) {
                    for (duckx::Run run : paragh.runs()) {
                        replaceFunc(run);
                    }
                }
            }
        }
    }
    doc.save();
    return true;
}

ORM_Model::SystemConfig GetSystemConfig() {
    try {
        return ORM_Model::SystemConfig::storage().get<ORM_Model::SystemConfig>(1);
    } catch (std::exception& e) {
        spdlog::debug(e.what());
        spdlog::warn("不能加载默认的系统配置, 将初始化为默认值。");
        ORM_Model::SystemConfig config = {};
        config.id                      = 1;
        config.groupName               = _T(DB_UNNAMED_GROUP);
        config.userName                = _T(DB_UNNAMED_USER);
        ORM_Model::SystemConfig::storage().insert(config);
        try {
            ORM_Model::JobGroup jobGroup;
            jobGroup.groupName = _T(DB_UNNAMED_GROUP);
            ORM_Model::JobGroup::storage().insert(jobGroup);
            ORM_Model::User user;
            user.name = _T(DB_UNNAMED_USER);
            ORM_Model::User::storage().insert(user);
        } catch (std::exception& e) { spdlog::error(GB2312ToUtf8(e.what())); }
        return GetSystemConfig();
    }
}

void UpdateSystemConfig(ORM_Model::SystemConfig& config) {
    config.id = 1;
    try {
        ORM_Model::SystemConfig::storage().update(config);
    } catch (std::exception& e) {
        spdlog::error("不能写入系统配置");
        spdlog::error(e.what());
    }
}

std::string GetJobGroup() {
    return StringFromWString(GetSystemConfig().groupName);
}

void AddTaskToQueue(std::function<void(void)> func, std::string id, bool rm_same_id) {
    static TaskQueue gTaskQueue;
    gTaskQueue.AddTask(func, id, rm_same_id);
}

bool IncChinese(std::wstring str) {
    size_t  nLen = str.length();
    wchar_t ch;
    for (size_t i = 0; i != nLen; ++i) {
        ch = str[i];
        if (ch >= 0xA0) {
            return true;
        } else {
            continue;
        }
    }
    return false;
}

DetectionStateMachine::DET_RES_TYPE DetectionStateMachine::UpdateData(DetectionStateMachine::DET_IN_TYPE& ampMax) {
    for (int i = 0; i < ampMax.size(); i++) {
        switch (mRes[i]) {
            case DetectionStatus::LowLevel: {
                if (ampMax[i] == 1) {
                    mRes[i] = DetectionStatus::Rasing;
                }
                break;
            }
            case DetectionStatus::HighLevel: {
                if (ampMax[i] == 0) {
                    mRes[i] = DetectionStatus::Falling;
                }
                break;
            }
            case DetectionStatus::Rasing: {
                if (ampMax[i] == 1) {
                    mRes[i] = DetectionStatus::HighLevel;
                } else {
                    mRes[i] = DetectionStatus::Falling;
                }
                break;
            }
            case DetectionStatus::Falling: {
                if (ampMax[i] == 0) {
                    mRes[i] = DetectionStatus::LowLevel;
                } else {
                    mRes[i] = DetectionStatus::Rasing;
                }
                break;
            }
        }
    }
    return mRes;
}

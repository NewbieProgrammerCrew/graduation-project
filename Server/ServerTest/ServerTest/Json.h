#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace rapidjson;
using namespace std;

int main() {
    try {
        // JSON 파일을 읽기 모드로 열기
        ifstream ifs("example.json");
        if (!ifs.is_open()) {
            throw runtime_error("failed to open the file");
        }

        // 파일 내용을 문자열로 읽기
        string json((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));

        // JSON 문자열 파싱
        Document doc;
        doc.Parse(json.c_str());

        // JSON 객체 편집
        Value& title = doc["title"];
        title.SetString("New Title");

        Value& email = doc["authors"][0]["email"];
        email.SetString("newemail@example.com");

        // JSON 문자열 생성
        StringBuffer buffer;
        PrettyWriter<StringBuffer> writer(buffer);
        doc.Accept(writer);

        // 파일에 JSON 문자열 쓰기
        ofstream ofs("new_example.json");
        ofs << buffer.GetString();

        ofs << endl;
    }
    catch (std::exception& e) {
        // 예외 처리
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
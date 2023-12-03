#include "Json.h"

//
//
//int main() {
//    std::ifstream ifs("path_to_your_json_file.json");
//    IStreamWrapper isw(ifs);
//
//    Document d;
//    d.ParseStream(isw);
//
//    std::vector<ObjectData> objectList;
//
//    if (d.IsObject()) {
//        for (auto& m : d.GetObject()) {
//            if (m.value.IsArray()) {
//                for (const auto& item : m.value.GetArray()) {
//                    ObjectData objData;
//                    objData.readFromJSON(item);
//                    objectList.push_back(objData);
//                }
//            }
//        }
//    }
//
//    // 저장된 데이터 출력
//    for (const auto& obj : objectList) {
//        std::cout << "Location: " << obj.locationX << ", " << obj.locationY << ", " << obj.locationZ << std::endl;
//        std::cout << "Extent: " << obj.extentX << ", " << obj.extentY << ", " << obj.extentZ << std::endl;
//        std::cout << "Yaw: " << obj.yaw << ", Pitch: " << obj.pitch << ", Roll: " << obj.roll << std::endl;
//    }
//
//    return 0;
//}
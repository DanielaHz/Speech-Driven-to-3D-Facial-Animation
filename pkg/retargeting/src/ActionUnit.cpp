#include "ActionUnit.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <algorithm>

bool ActionUnit::loadMuscleIndexMapFromJSON(const char *musclesJson)
{
    std::string filePathStr = std::string(musclesJson);
    std::ifstream file(filePathStr);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePathStr << "\n";
        return false;
    }

    nlohmann::json data;
    try {
        file >> data;
    } catch (const std::exception& e) {
        std::cerr << "JSON parse error: " << e.what() << "\n";
        return false;
    }

    m_muscleIndexMap = populateMuscleIndexMap(data); 
    std::cout << "[Loader][ACTIONUNIT]: loading the file musclePatches.json to create deltatransfer.json (pre-processing)" << "\n";
    return true;
}

std::unordered_map<int, std::vector<int>>  ActionUnit::populateMuscleIndexMap(nlohmann::json& data)
{
    std::unordered_map<int, std::vector<int>> map;
    for (auto& [key, value] : data.items()) {
        int muscleId = std::stoi(key);
        std::vector<int> indices = value.get<std::vector<int>>();
        map[muscleId] = indices;
    }
    return map;
}

void ActionUnit::printMuscleIndexMap()
{
    for (auto& value: m_muscleIndexMap)
    {
        std::cout << " the first value "<< value.first << "the second value " << "\n";
        for (int second: value.second) { std::cout << second << "\n";}
    }
}

std::vector<glm::vec3> ActionUnit::getVerticesNeutralFace(const char* modelPath)
{
    std::vector<glm::vec3> vector = m_facialMesh->loadModel(modelPath);
    return vector;
}

void ActionUnit::printVector(std::vector<glm::vec3>& vector)
{
    size_t maxVertex = 5;
    for (size_t vertex = 0 ; vertex < maxVertex ; ++vertex)
    {
        std::cout << vector[vertex].x << "," << vector[vertex].y << "," << vector[vertex].z << "\n"; 
    }
}

std::vector<glm::vec3> ActionUnit::getVerticesBlendshape(const char* modelPath)
{
    std::vector<glm::vec3> vector;
    vector = m_facialMesh->loadModel(modelPath);
    return vector;
}

std::vector<MuscleDelta> ActionUnit::getMusclesVertices(const std::vector<glm::vec3>& neutralVerts, const std::vector<glm::vec3>& blendVerts, const std::vector<int>& muscleList)
{
    std::cout << "muscle list size" << muscleList.size() << "\n";

    std::vector<MuscleDelta> result;

    for (int muscleId : muscleList)
    {
        auto it = m_muscleIndexMap.find(muscleId);
        if (it == m_muscleIndexMap.end())
            continue;

        MuscleDelta md;
        md.muscleId = muscleId;

        for (int vi : it->second)
        {
            glm::vec3 basePos  = neutralVerts[vi];
            glm::vec3 blendPos = blendVerts[vi];
            glm::vec3 delta    = m_mathUtils->calculateDeltaTransfer(blendPos, basePos);

            md.deltas.emplace_back(VertexDelta{vi, blendPos, delta});
        }
        result.emplace_back(std::move(md));
    }

    std::cout << "result size: " << result.size() << "\n";

    // -- data validation ---
    std::cout << "=== getMusclesVertices result ===\n";
    for (auto const& md : result)
    {
        std::cout << "MuscleId: " << md.muscleId
                  << "  (deltas: " << md.deltas.size() << " verts)\n";
        for (auto const& vd : md.deltas)
        {
            std::cout
                << "   vertexIndex=" << vd.vertexIndex
                << "  pos=("
                    << vd.position.x << "," 
                    << vd.position.y << "," 
                    << vd.position.z << ")"
                << "  delta=("
                    << vd.delta.x << "," 
                    << vd.delta.y << "," 
                    << vd.delta.z << ")\n";
        }
    }
    return result;
}

bool ActionUnit::loadModelPathsFromJSON(const char* pathsJson, const char* basePath)
{
    std::cout << "[Loader][ACTIONUNIT]: loading the file modelsPath.json to create deltatransfer.json (pre-processing)" << "\n";
    // reading the json file
    nlohmann::json root; 
    std::ifstream ifs(pathsJson);
    ifs >> root;

    printMuscleIndexMap();
    m_neutralFaceVertices = getVerticesNeutralFace(
        (std::string(basePath) + "/" + root["NEUTRALFACE"]["path"].get<std::string>())
            .c_str());

    // parsing the json file
    for (auto& [key, node] : root.items())
    {
        if (key == "NEUTRALFACE" || key == "SKULL") 
            continue;

        int auId = parseAUId(key);  // extract the AU value 
        std::string sideStr = node.at("side").get<std::string>();

        // Uploading the blendshapes
        std::string blendPath = basePath + std::string("/") + node["path"].get<std::string>();
        auto blendVerts = getVerticesBlendshape(blendPath.c_str());

        // getting the muscles vertices of the active muscles in the blendshape
        auto active  = getMusclesVertices(m_neutralFaceVertices,
                                          blendVerts,
                                          node["active"].get<std::vector<int>>());

        // getting the muscles vertices of the passive muscles in the blenshape
        auto passive = getMusclesVertices(m_neutralFaceVertices,
                                          blendVerts,
                                          node["passive"].get<std::vector<int>>());

        // debugging purposes 
        std::cout << "active vector size " << active.size() << "\n";
        std::cout << "passive vector size " << passive.size() << "\n";

        ActionUnitDelta auDelta;
        auDelta.auId           = auId;
        Side side = sideFromString(sideStr);
        auDelta.side           = side;
        auDelta.activeMuscles  = std::move(active);
        auDelta.passiveMuscles = std::move(passive);
        m_auDeltaTable[auId].push_back(auDelta); 
    }
    printauDeltaTable();
    return true;
}

bool ActionUnit::saveDeltaTransfersToJSON(const char* outJsonPath)
{
    nlohmann::ordered_json root;
    root["actionUnits"] = nlohmann::ordered_json::array();
    
    for (auto const& [auId, deltaList] : m_auDeltaTable)
    {
        for (auto const& auDelta : deltaList)
        {
            std::string sideStr = sideToString(auDelta.side);
            nlohmann::ordered_json auJ = {
                {"auId", auId},
                {"side", sideStr},
                {"activeMuscles", nlohmann::ordered_json::array()},
                {"passiveMuscles", nlohmann::ordered_json::array()}
            };

            auto serialize = [&](nlohmann::ordered_json& targetArray,
                                const std::vector<MuscleDelta>& muscles)
            {
                for (auto const& md : muscles)
                {
                    nlohmann::ordered_json mdJ;
                    mdJ["muscleId"] = md.muscleId;
                    mdJ["deltas"]   = nlohmann::ordered_json::array();

                    for (auto const& vd : md.deltas)
                    {
                        nlohmann::ordered_json vdJ = {
                            {"vertexIndex", vd.vertexIndex},
                            {"position",    {vd.position.x,
                                            vd.position.y,
                                            vd.position.z}},
                            {"delta",       {vd.delta.x,
                                            vd.delta.y,
                                            vd.delta.z}}
                        };
                        mdJ["deltas"].push_back(std::move(vdJ));
                    }
                    targetArray.push_back(std::move(mdJ));
                }
            };
            serialize(auJ["activeMuscles"],  auDelta.activeMuscles);
            serialize(auJ["passiveMuscles"], auDelta.passiveMuscles);
            root["actionUnits"].push_back(std::move(auJ));
        }
    }

    std::ofstream ofs(outJsonPath);
    if (!ofs.is_open()) return false;
    ofs << std::setw(2) << root << std::endl;
    return true;
}

int ActionUnit::parseAUId(const std::string& auKey)
{
    std::string digits;
    digits.reserve(auKey.size());

    for (char c : auKey)
    {
        if (std::isdigit(static_cast<unsigned char>(c)))
            digits.push_back(c);
    }

    if (digits.empty())
        throw std::invalid_argument("parseAUId: no digits found in key \"" + auKey + "\"");

    return std::stoi(digits);
}

void ActionUnit::loadDeltaTransfersFromJSON(const char* deltaJson)
{
    // reading the json file
    nlohmann::json root; 
    std::ifstream ifs(deltaJson);
    ifs >> root;

    if (!ifs.is_open()) {
        std::cerr << "Error opening file: " << deltaJson << std::endl;
        return;
    }

    for (const auto& auJ : root["actionUnits"])
    {
        if (!auJ.contains("auId") || !auJ["auId"].is_number_integer()) {
            std::cerr << "Invalid auId" << std::endl;
            continue;
        }

        int auId = auJ["auId"];
        std::string sideStr = auJ["side"].get<std::string>();
        
        ActionUnitDelta auDelta;
        auDelta.auId = auId;
        auDelta.side   = sideFromString(sideStr); 

        auto parseMuscles = [](const nlohmann::json& muscleArray,
                               std::vector<MuscleDelta>& target)
        {
            for (const auto& muscleJ : muscleArray)
            {
                MuscleDelta md;
                md.muscleId = muscleJ["muscleId"];

                for (const auto& deltaJ : muscleJ["deltas"])
                {
                    VertexDelta vd;
                    vd.vertexIndex = deltaJ["vertexIndex"];
                    vd.position = {
                        deltaJ["position"][0],
                        deltaJ["position"][1],
                        deltaJ["position"][2]
                    };
                    vd.delta = {
                        deltaJ["delta"][0],
                        deltaJ["delta"][1],
                        deltaJ["delta"][2]
                    };
                    md.deltas.push_back(vd);
                }
                target.push_back(md);
            }
        };

        parseMuscles(auJ["activeMuscles"], auDelta.activeMuscles);
        parseMuscles(auJ["passiveMuscles"], auDelta.passiveMuscles);
        m_auDeltaTable[auId].push_back(std::move(auDelta)); 
    }
}

void ActionUnit::printauDeltaTable()
{
    std::cout << "-------- printing the auDeltaTable ---------" << "\n";
    int totalAUvertices = 0;
    for (auto& [key, auList] : m_auDeltaTable)
    {
        for (const auto& auDelta : auList)
        {
            std::cout << "action unit value: " << key 
                    << " the auId value: " << auDelta.auId 
                    << " side: " << sideToString(auDelta.side) << "\n";

            int totalvertex = 0;
            for (const auto& e : auDelta.activeMuscles)
            {
                totalvertex += e.deltas.size();
                std::cout << "active muscle " << e.muscleId 
                        << " the total vertices/deltas per muscles are: " << e.deltas.size() << "\n";
            }

            for (const auto& e : auDelta.passiveMuscles)
            {
                totalvertex += e.deltas.size();
                std::cout << "passive muscle " << e.muscleId 
                        << " the total vertices/deltas per muscles are: " << e.deltas.size() << "\n";
            }
            std::cout << "the total vertices and deltas for the action unit are: " << totalvertex << "\n";
            totalAUvertices += totalvertex;
        }
    }
    int average =  totalAUvertices/50;
    std::cout << "the average of vertices in the AU are: " << average;
}
 
std::unordered_map<int, std::vector<ActionUnitDelta>> ActionUnit::getAuDeltaTable()
{
    return m_auDeltaTable;
}

std::unordered_map<int, std::vector<int>> ActionUnit::getMuscleIndexMap()
{
    if (m_muscleIndexMap.empty()) {
        std::cout << "[ActionUnit] Muscle index map is empty.\n";
    } else {
        std::cout << "[ActionUnit] Muscle index map contains " << m_muscleIndexMap.size() << " entries.\n";
    }
    return m_muscleIndexMap;
}

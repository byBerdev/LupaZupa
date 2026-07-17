#include "Scene.hpp"
#include "../Graphics/Sprite.hpp"
#include <fstream>
#include <sstream>
#include <cstdio>

// -----------------------------------------------------------------------
// Este arquivo inclui um parser/escritor JSON minimalista, feito sob medida
// para o formato de cena da LupaZupaEngine (ver Examples/HelloWorld/scene.json).
// Ele evita depender de uma biblioteca JSON externa apenas para esta
// estrutura simples de dados. Para projetos maiores, recomenda-se trocar
// por nlohmann::json (basta adicionar via FetchContent no CMakeLists.txt).
// -----------------------------------------------------------------------

namespace LupaZupa {

Entity& Scene::CreateEntity(const std::string& name) {
    auto entity = std::make_unique<Entity>(name, m_nextId++);
    m_entities.push_back(std::move(entity));
    return *m_entities.back();
}

Entity* Scene::FindEntityByName(const std::string& name) {
    for (auto& e : m_entities) {
        if (e->GetName() == name) return e.get();
    }
    return nullptr;
}

namespace {

std::string ReadFile(const std::string& path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Extrai o valor de uma string entre aspas para uma dada chave "key": "valor"
// dentro de um bloco de texto. Retorna string vazia se não encontrar.
std::string ExtractString(const std::string& block, const std::string& key) {
    std::string pattern = "\"" + key + "\"";
    size_t pos = block.find(pattern);
    if (pos == std::string::npos) return "";
    pos = block.find(':', pos);
    if (pos == std::string::npos) return "";
    size_t firstQuote = block.find('"', pos);
    if (firstQuote == std::string::npos) return "";
    size_t secondQuote = block.find('"', firstQuote + 1);
    if (secondQuote == std::string::npos) return "";
    return block.substr(firstQuote + 1, secondQuote - firstQuote - 1);
}

float ExtractNumber(const std::string& block, const std::string& key, float defaultValue = 0.0f) {
    std::string pattern = "\"" + key + "\"";
    size_t pos = block.find(pattern);
    if (pos == std::string::npos) return defaultValue;
    pos = block.find(':', pos);
    if (pos == std::string::npos) return defaultValue;
    pos++;
    while (pos < block.size() && std::isspace(static_cast<unsigned char>(block[pos]))) pos++;
    size_t end = pos;
    while (end < block.size() && (std::isdigit(static_cast<unsigned char>(block[end])) ||
           block[end] == '-' || block[end] == '.')) end++;
    if (end == pos) return defaultValue;
    return std::stof(block.substr(pos, end - pos));
}

// Divide o array de entidades em blocos individuais delimitados por { }.
std::vector<std::string> SplitEntityBlocks(const std::string& json) {
    std::vector<std::string> blocks;
    size_t arrayStart = json.find("\"entities\"");
    if (arrayStart == std::string::npos) return blocks;
    arrayStart = json.find('[', arrayStart);
    size_t arrayEnd = json.find(']', arrayStart);
    if (arrayStart == std::string::npos || arrayEnd == std::string::npos) return blocks;

    int depth = 0;
    size_t blockStart = std::string::npos;
    for (size_t i = arrayStart; i < arrayEnd; ++i) {
        if (json[i] == '{') {
            if (depth == 0) blockStart = i;
            depth++;
        } else if (json[i] == '}') {
            depth--;
            if (depth == 0 && blockStart != std::string::npos) {
                blocks.push_back(json.substr(blockStart, i - blockStart + 1));
                blockStart = std::string::npos;
            }
        }
    }
    return blocks;
}

} // namespace

std::unique_ptr<Scene> Scene::LoadFromFile(const std::string& path) {
    std::string json = ReadFile(path);
    if (json.empty()) {
        std::fprintf(stderr, "[LupaZupaEngine] Falha ao ler cena '%s'\n", path.c_str());
        return nullptr;
    }

    std::string sceneName = ExtractString(json, "name");
    if (sceneName.empty()) sceneName = "Untitled";

    auto scene = std::make_unique<Scene>(sceneName);

    for (const auto& block : SplitEntityBlocks(json)) {
        std::string entName = ExtractString(block, "name");
        if (entName.empty()) entName = "Entity";

        Entity& entity = scene->CreateEntity(entName);
        auto& t = entity.GetTransform();
        t.x = ExtractNumber(block, "x", 0.0f);
        t.y = ExtractNumber(block, "y", 0.0f);
        t.rotation = ExtractNumber(block, "rotation", 0.0f);
        t.scaleX = ExtractNumber(block, "scaleX", 1.0f);
        t.scaleY = ExtractNumber(block, "scaleY", 1.0f);

        std::string spritePath = ExtractString(block, "sprite");
        if (!spritePath.empty()) {
            Sprite sprite;
            sprite.texturePath = spritePath;
            entity.SetComponent<Sprite>("sprite", sprite);
        }

        std::string scriptPath = ExtractString(block, "script");
        if (!scriptPath.empty()) {
            entity.SetComponent<std::string>("script", scriptPath);
        }
    }

    return scene;
}

bool Scene::SaveToFile(const std::string& path) const {
    std::ofstream file(path);
    if (!file.is_open()) return false;

    file << "{\n";
    file << "  \"name\": \"" << m_name << "\",\n";
    file << "  \"entities\": [\n";

    for (size_t i = 0; i < m_entities.size(); ++i) {
        const auto& e = m_entities[i];
        const auto& t = e->GetTransform();
        file << "    {\n";
        file << "      \"name\": \"" << e->GetName() << "\",\n";
        file << "      \"x\": " << t.x << ",\n";
        file << "      \"y\": " << t.y << ",\n";
        file << "      \"rotation\": " << t.rotation << ",\n";
        file << "      \"scaleX\": " << t.scaleX << ",\n";
        file << "      \"scaleY\": " << t.scaleY << "\n";
        file << "    }" << (i + 1 < m_entities.size() ? "," : "") << "\n";
    }

    file << "  ]\n";
    file << "}\n";
    return true;
}

} // namespace LupaZupa

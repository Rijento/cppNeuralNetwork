#include "Synapse.hpp"
#include "Neuron.hpp"
#include <regex>
#include <rapidjson/document.h>

Synapse::Synapse() {
    from = NULL;
    to = NULL;
    weight = 0.0;
    enabled = false;
}

Synapse::Synapse(Neuron* fromIn, Neuron* toIn) {
    from = fromIn;
    to = toIn;
    weight = 1.0;
    enabled = true;
}

Synapse::Synapse(Neuron* fromIn, Neuron* toIn, float weightIn) {
    from = fromIn;
    to = toIn;
    weight = weightIn;
    enabled = true;
}

rapidjson::Value Synapse::serialize(rapidjson::Document::AllocatorType& allocator) {
    rapidjson::Value serialized(rapidjson::kObjectType);
    rapidjson::Value fromID(from->getId().c_str(), from->getId().size(), allocator);
    rapidjson::Value toID(to->getId().c_str(), to->getId().size(), allocator);
    serialized.AddMember("fromID", fromID, allocator);
    serialized.AddMember("toID", toID, allocator);
    serialized.AddMember("weight", weight, allocator);
    serialized.AddMember("enabled", enabled, allocator);
    return serialized;
}
void Synapse::deserialize(rapidjson::Value& dataIn, std::unordered_map<std::string, Neuron*> &deserializedNeurons) {
    weight = dataIn["weight"].GetFloat();
    enabled = dataIn["enabled"].GetBool();
    std::string fromId = dataIn["fromID"].GetString();
    std::string toId = dataIn["toID"].GetString();
    
    from = deserializedNeurons[fromId];
    to = deserializedNeurons[toId];
}
Synapse* Synapse::clone(std::unordered_map<std::string, Neuron*> &clonedNeurons) {
    Synapse* clone = new Synapse(clonedNeurons.at(from->getId()), clonedNeurons.at(to->getId()));
    clone->weight = weight;
    clone->enabled = enabled;
    return clone;
}
Neuron* Synapse::getFrom() {
    return from;
}
Neuron* Synapse::getTo() {
    return to;
}
float Synapse::getWeight() {
    return weight;
}
void Synapse::setWeight(float weightIn) {
    weight = weightIn;
}
bool Synapse::isEnabled() {
    return enabled;
}
void Synapse::enable() {
    enabled = true;
}
void Synapse::disable() {
    enabled = false;
}

bool Synapse::operator==(const Synapse& other) const {
    if (from->getId() == other.from->getId()) {
        if (to->getId() == other.to->getId()) {
            return true;
        }
    }
    return false;
}
bool Synapse::operator==(const Synapse* other) const {
    if (from->getId() == other->from->getId()) {
        if (to->getId() == other->to->getId()) {
            return true;
        }
    }
    return false;
}

Synapse::~Synapse() { // Clean up pointers
    delete from;
    delete to;
}
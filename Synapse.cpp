#include "Synapse.hpp"
#include "Neuron.hpp"
#include <regex>

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

std::string Synapse::serialize() {
    return "syn:(" + from->getId() + "," + to->getId() + "," + std::to_string(weight) + "," + std::to_string(enabled) + ")";
}
void Synapse::deserialize(std::string dataIn, std::unordered_map<std::string, Neuron*> &deserializedNeurons) {
    std::regex split ("\\((.*),(.*),(.*),(.*)\\)");
    std::regex_token_iterator<std::string::iterator> spl_it (dataIn.begin(), dataIn.end(), split, {1,2,3,4});

    std::string fromId = *spl_it++;
    std::string toId = *spl_it++;
    float weightIn = std::stof(*spl_it++);
    bool enabledIn = std::stoi(*spl_it);
    
    weight = weightIn;
    enabled = enabledIn;
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
Synapse::~Synapse() { // Clean up pointers
    delete from;
    delete to;
}
#include "Synapse.hpp"
#include "Neuron.hpp"

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
void Synapse::deserialize(std::unordered_map<std::string, Neuron*> &deserializedNeurons) { /*TODO: THIS*/}
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
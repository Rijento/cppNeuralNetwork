#include "Layer.hpp"
#include <rapidjson/document.h>
#include "Neuron.hpp"
#include <regex>

Layer::Layer() {}

Layer::Layer(int depthIn) {
    depth = depthIn;
}
Layer* Layer::clone(std::unordered_map<std::string, Neuron*> &clonedNeurons) {
    Layer* clone = new Layer(depth);
    std::unordered_map<std::string, Neuron*> layerNeuronClones;
    for (NeuronIterator it = neurons.begin(); it != neurons.end(); ++it) {
        Neuron* clonedNeuron = it->second->clone(clonedNeurons, clone); // Clone the neuron
        layerNeuronClones[clonedNeuron->getId()] = clonedNeuron;       
    }
    clone->neurons = layerNeuronClones;
    return clone;
}
rapidjson::Value Layer::serialize(rapidjson::Document::AllocatorType& allocator) {
    rapidjson::Value serialized(rapidjson::kObjectType);
    serialized.AddMember("depth", depth, allocator);
    rapidjson::Value nurs(rapidjson::kArrayType);
    // std::string serialized = "lay:(" + std::to_string(depth) + ",nurs:(";
    
    for (NeuronIterator it = neurons.begin(); it != neurons.end(); ++it) {
        // serialized += it->second->serialize();
        // serialized += ",";
        nurs.PushBack(it->second->serialize(allocator), allocator);
    }

    serialized.AddMember("nurs", nurs, allocator);

    return serialized;
} 
void Layer::deserialize(rapidjson::Value& dataIn, std::unordered_map<std::string, Neuron*> &deserializedNeurons) {
    neurons.clear(); // Remove anything that's already there

    depth = dataIn["depth"].GetInt();
    rapidjson::Value neuronsVal(rapidjson::kArrayType);
    neuronsVal = dataIn["nurs"].GetArray();

    for(rapidjson::SizeType i = 0; i < neuronsVal.Size(); i++) {
        Neuron* neuron = new Neuron();
        neuron->deserialize(neuronsVal[i].GetObject(), deserializedNeurons, this);
        neurons[neuron->getId()] = neuron;
    }
}

void Layer::addNeuron(Neuron* neuron) {
    neurons[neuron->getId()] = neuron;
}
void Layer::removeNeuron(Neuron* neuron) {
    neurons.erase(neuron->getId());
}
std::unordered_map<std::string, Neuron*>* Layer::getNeurons() {
    return &neurons;
}
Neuron* Layer::getRandomNeuron() {
    int randint = rand() % neurons.size();
    NeuronIterator it = neurons.begin();
    std::advance(it, randint);
    return it->second;
}
bool Layer::containsNeuron(Neuron* neuron) {
    NeuronIterator it = neurons.find(neuron->getId());
    return it != neurons.end();
}
void Layer::setLayerDepth(int depthIn) {
    depth = depthIn;
}
int Layer::getLayerDepth() {
    return depth;
}

Layer& Layer::operator++() { // prefix
    depth++;
    return *this;
}
Layer Layer::operator++(int) { // Postfix
    Layer old = *this;
    operator++();
    return old;
}
Layer& Layer::operator--() { // prefix
    depth--;
    return *this;
}
Layer Layer::operator--(int) { // Postfix
    Layer old = *this;
    operator--();
    return old;
}
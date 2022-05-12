#include "Neuron.hpp"
#include "Synapse.hpp"
#include "Layer.hpp"
#include <rapidjson/document.h>
#include <math.h>
#include <regex>

bool synapseCompare::operator()(const Synapse* lhs, const Synaspe* rhs) const {
    return (*lhs) == rhs;
}

Neuron::Neuron() {
    id = "PLACEHOLDER";
    layer = NULL;
    dxpartial = NAN;
    archivedLevel = 0.0;
    activationLevel = 0.0;
    outputLevel = 0.0;
}

Neuron::Neuron(std::string idIn) {
    id = idIn;
    activationLevel = 0.0;
    archivedLevel = 0.0;
    dxpartial = NAN;
    outputLevel = 0.0;
}
Neuron::Neuron(std::string idIn, Layer* layerIn) {
    id = idIn;
    activationLevel = 0.0;
    archivedLevel = 0.0;
    outputLevel = 0.0;
    dxpartial = NAN;
    layer = layerIn;
}

Neuron* Neuron::clone(std::unordered_map<std::string, Neuron*> &clonedNeurons, Layer* clonedLayer) {
    // Create clone and set basic values
    Neuron* clone = new Neuron(id, clonedLayer);
    clone->activationLevel = activationLevel;
    clone->outputLevel = outputLevel;

    // Add the newly cloned neuron to the list of neurons that have already been cloned
    clonedNeurons[id] = clone; 

    // Clone all of the synapses
    std::unordered_set<Synapse*> clonedSynapses;
    for(SynapseIterator it = synapses.begin(); it != synapses.end(); ++it) {
        Synapse* clonedSynapse = (*it)->clone(clonedNeurons);
        clonedSynapses.insert(clonedSynapse);
    }

    clone->synapses = clonedSynapses;

    return clone;
}
rapidjson::Value Neuron::serialize(rapidjson::Document::AllocatorType& allocator) {
    rapidjson::Value serialized(rapidjson::kObjectType);
    // std::string serialized = "nur:(" + id + ",syns:(";
    rapidjson::Value jId(id.c_str(), id.size(), allocator);
    serialized.AddMember("id", jId, allocator);
    rapidjson::Value syns(rapidjson::kArrayType);

    for(SynapseIterator it = synapses.begin(); it != synapses.end(); ++it) {
        // serialized += (*it)->serialize();
        // serialized += ",";
        syns.PushBack((*it)->serialize(allocator),allocator);
    }
    serialized.AddMember("syns", syns, allocator);

    return serialized;
}
void Neuron::deserialize(rapidjson::Value& dataIn, std::unordered_map<std::string, Neuron*> &deserializedNeurons, Layer* deserializedLayer) {
    id = dataIn["id"].GetString();
    layer = deserializedLayer;
    deserializedNeurons[id] = this;

    rapidjson::Value synsVal(rapidjson::kArrayType);
    synsVal = dataIn["syns"].GetArray();

    for(rapidjson::SizeType i = 0; i < synsVal.Size(); i++) {
        Synapse* synapse = new Synapse();
        synapse->deserialize(synsVal[i].GetObject(), deserializedNeurons);
        synapses.insert(synapse);
    }
}
std::unordered_set<Synapse*>* Neuron::getSynapses() {
    return &synapses;
}
Synapse* Neuron::getRandomSynapse() {
    int randint = rand() % synapses.size();
    SynapseIterator it = synapses.begin();
    std::advance(it, randint);
    return *it;
}
bool Neuron::isConnected(Neuron* neuron) {
    for (SynapseIterator it = synapses.begin(); it != synapses.end(); ++it) {
        if((*it)->getTo() == neuron) {
            return true;
        }
    }
    return false;
}

void Neuron::addSynapse(Synapse* synapse) {
    synapses.insert(synapse);
}

std::string Neuron::getId() {
    return id;
}
float Neuron::activationFunction(float x) { // sigmoid activation function
    float y = 1 / (1 + exp(-x)); //pow(exp(1), -4.9*x));
    return y;
}
void Neuron::activate() {
    dxpartial = NAN;
    outputLevel = activationFunction(activationLevel);
    archivedLevel = activationLevel;
    activationLevel = 0; // Need to reset this for the next pass else it will just keep increasing.
    for (SynapseIterator it = synapses.begin(); it != synapses.end(); ++it) {
        if ((*it)->isEnabled()) {
            (*it)->getTo()->activationLevel += (*it)->getWeight() * outputLevel;
        }
    }
}
void Neuron::activate(float dataIn) { // Used for input nodes
    dxpartial = NAN;
    outputLevel = activationFunction(dataIn);
    archivedLevel = dataIn;
    activationLevel = 0; // Need to reset this for the next pass else it will just keep increasing.
    for (SynapseIterator it = synapses.begin(); it != synapses.end(); ++it) {
        if ((*it)->isEnabled()) {
            (*it)->getTo()->activationLevel += (*it)->getWeight() * outputLevel;
        }
    }
}
Layer* Neuron::getLayer() {
    return layer;
}
void Neuron::setLayer(Layer* layerIn) {
    layer = layerIn;
}
float Neuron::getActivationLevel() {
    return activationLevel;
}
float Neuron::getArchivedLevel() {
    return archivedLevel;
}
float Neuron::getOutputLevel() {
    return outputLevel;
}

float Neuron::getDXPartial() {
    return dxpartial;
}
void Neuron::incrementDXPartial(float val){
    if(std::isnan(dxpartial)) {
        dxpartial = val;
    } else {
        dxpartial += val;
    }
}

Neuron::~Neuron() {
    for(Synapse* synapse : synapses) {
        delete synapse;
    }
}

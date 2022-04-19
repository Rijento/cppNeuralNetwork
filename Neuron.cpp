#include "Neuron.hpp"
#include "Synapse.hpp"
#include "Layer.hpp"
#include <math.h>
#include <regex>

Neuron::Neuron() {
    id = "PLACEHOLDER";
    layer = NULL;
    activationLevel = 0.0;
    outputLevel = 0.0;
}

Neuron::Neuron(std::string idIn) {
    id = idIn;
    activationLevel = 0.0;
    archivedLevel = 0.0;
    outputLevel = 0.0;
}
Neuron::Neuron(std::string idIn, Layer* layerIn) {
    id = idIn;
    activationLevel = 0.0;
    archivedLevel = 0.0;
    outputLevel = 0.0;
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
std::string Neuron::serialize() {
    std::string serialized = "nur:(" + id + ",syns:(";

    for(SynapseIterator it = synapses.begin(); it != synapses.end(); ++it) {
        serialized += (*it)->serialize();
        serialized += ",";
    }

    return serialized + "))";
}
void Neuron::deserialize(std::string dataIn, std::unordered_map<std::string, Neuron*> &deserializedNeurons, Layer* deserializedLayer) {
    std::regex_token_iterator<std::string::iterator> rend;
    std::regex nurSplit ("\\((.*),syns:\\((.*)\\)\\)");
    std::regex synsSplit ("syn:\\(.*?\\)");
    std::regex_token_iterator<std::string::iterator> nurSpl_it (dataIn.begin(), dataIn.end(), nurSplit, {1,2});
    std::string idString = *nurSpl_it++;
    std::string synsString = *nurSpl_it;
    
    synapses.clear();
    id = idString;
    layer = deserializedLayer;

    deserializedNeurons[idString] = this;

    std::regex_token_iterator<std::string::iterator> synsSpl_it (synsString.begin(), synsString.end(), synsSplit);
    while (synsSpl_it != rend) {
        Synapse* synapse = new Synapse();
        synapse->deserialize(*synsSpl_it++, deserializedNeurons);
        synapses.insert(synapse);
    }
    bool test = false;
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
    for (SynapseIterator it = synapses.begin(); it != synapses.end(); ++it) {
        if ((*it)->isEnabled()) {
            (*it)->getTo()->activationLevel += (*it)->getWeight() * dataIn;
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
Neuron::~Neuron() {
    delete layer;
}
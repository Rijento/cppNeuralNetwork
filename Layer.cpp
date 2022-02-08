#include "Layer.hpp"
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
std::string Layer::serialize() {
    std::string serialized = "lay:(" + std::to_string(depth) + ",nurs:(";
    
    for (NeuronIterator it = neurons.begin(); it != neurons.end(); ++it) {
        serialized += it->second->serialize();
        serialized += ",";
    }

    return serialized + "))";
} 
void Layer::deserialize(std::string dataIn, std::unordered_map<std::string, Neuron*> &deserializedNeurons) {
    neurons.clear(); // Remove anything that's already there
    std::regex_token_iterator<std::string::iterator> rend;
    std::regex dataSplitter ("lay:\\((-?\\d+),nurs:\\((.*),\\)");
    std::regex neuronSplitter ("nur:([^)]*\\))*?\\)?\\)");
    
    std::regex_token_iterator<std::string::iterator> dataSpl_it (dataIn.begin(), dataIn.end(), dataSplitter, {1,2});

    std::string depthString = *dataSpl_it++;
    std::string neuronsString = *dataSpl_it;

    depth = std::stoi(depthString);

    std::regex_token_iterator<std::string::iterator> neuronSpl_it(neuronsString.begin(), neuronsString.end(), neuronSplitter);

    std::vector<std::string> serializedNeurons;

    while (neuronSpl_it != rend) {
        Neuron* neuron = new Neuron();
        std::string neuronString = *neuronSpl_it++;
        neuron->deserialize(neuronString, deserializedNeurons, this);
        neurons[neuron->getId()] = neuron;
        // serializedNeurons.push_back(*neuronSpl_it++);
    }

bool test = false;

} //TODO: this
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
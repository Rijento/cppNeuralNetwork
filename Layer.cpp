#include "Layer.hpp"
#include "Neuron.hpp"

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
std::string Layer::serialize() {} //TODO: this
void Layer::deserialize(std::string dataIn) {} //TODO: this
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
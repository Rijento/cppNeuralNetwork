#ifndef __LAYER_H__
#define __LAYER_H__

#include <unordered_map>
#include <string>
#include <rapidjson/document.h>

#define NeuronIterator std::unordered_map<std::string, Neuron*>::iterator

class Neuron;
class Layer
{
private:
    std::unordered_map<std::string, Neuron*> neurons;
    int depth;
public:
    Layer();
    Layer(int depthIn);
    Layer* clone(std::unordered_map<std::string, Neuron*> &clonedNeurons); // creates a copy of the layer
    rapidjson::Value serialize(rapidjson::Document::AllocatorType& allocator); // serializes the layer to a string
    void deserialize(rapidjson::Value& dataIn, std::unordered_map<std::string, Neuron*> &deserializedNeurons); // loads the layer from a serialized string
    void addNeuron(Neuron* neuron);
    void removeNeuron(Neuron* neuron);
    std::unordered_map<std::string, Neuron*>* getNeurons();
    Neuron* getRandomNeuron();
    Neuron* getNeuronById(std::string id);
    bool containsNeuron(Neuron* neuron);
    void setLayerDepth(int depthIn);
    int getLayerDepth();
    Layer& operator++(); // prefix
    Layer operator++(int); // postfix
    Layer& operator--(); // prefix
    Layer operator--(int); // postfix
    ~Layer();
};

#endif
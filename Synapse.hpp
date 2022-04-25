#ifndef __SYNAPSE_H__
#define __SYNAPSE_H__

#include <string>
#include <unordered_map>
#include <rapidjson/document.h>
class Neuron;
class Synapse
{
private:
    Neuron* from;
    Neuron* to;
    float weight;
    bool enabled;
public:
    Synapse();
    Synapse(Neuron* fromIn, Neuron* toIn);
    Synapse(Neuron* fromIn, Neuron* toIn, float weightIn);
    rapidjson::Value serialize(rapidjson::Document::AllocatorType& allocator);
    void deserialize(rapidjson::Value& dataIn, std::unordered_map<std::string, Neuron*> &deserializedNeurons);
    Synapse* clone(std::unordered_map<std::string, Neuron*> &clonedNeurons);
    Neuron* getFrom();
    Neuron* getTo();
    float getWeight();
    void setWeight(float weightIn);
    bool isEnabled();
    void enable();
    void disable();
    ~Synapse(); // needed to delete the to and from pointers
};

#endif
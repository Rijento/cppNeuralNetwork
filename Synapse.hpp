#ifndef __SYNAPSE_H__
#define __SYNAPSE_H__

#include <string>
#include <unordered_map>
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
    std::string serialize();
    void deserialize(std::string dataIn, std::unordered_map<std::string, Neuron*> &deserializedNeurons);
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
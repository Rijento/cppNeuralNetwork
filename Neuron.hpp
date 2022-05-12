#ifndef __NEURON_H__
#define __NEURON_H__

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <rapidjson/document.h>

#define SynapseIterator std::unordered_set<Synapse*>::iterator

class Synapse;
class Layer;

struct synapseCompare
{
    bool operator()(const Synapse* lhs, const Synapse* rhs) const;
};


class Neuron
{
private:
    std::unordered_set<Synapse*, synapseCompare> synapses;
    std::string id;
    Layer* layer;
    float activationLevel;
    float archivedLevel;
    float outputLevel;
    float dxpartial;
    float activationFunction(float x); // used in the activate functions
public:
    Neuron();
    Neuron(std::string idIn);
    Neuron(std::string idIn, Layer* layerIn);
    Neuron* clone(std::unordered_map<std::string, Neuron*> &clonedNeurons, Layer* clonedLayer); // creates a copy of the neuron
    rapidjson::Value serialize(rapidjson::Document::AllocatorType& allocator); // serializes the neuron to a string
    void deserialize(rapidjson::Value& dataIn, std::unordered_map<std::string, Neuron*> &deserializedNeurons, Layer* deserializedLayer); // loads the neuron from a serialized string
    std::unordered_set<Synapse*>* getSynapses();
    Synapse* getRandomSynapse();
    bool isConnected(Neuron* neuron);
    void addSynapse(Synapse* synapse);
    std::string getId();
    void activate();
    void activate(float dataIn); // used for the input neurons
    Layer* getLayer();
    void setLayer(Layer* layerIn);
    float getActivationLevel();
    float getArchivedLevel();
    float getOutputLevel();
    float getDXPartial();
    void incrementDXPartial(float);
    ~Neuron(); // needed to delete the layer pointer
};
#endif
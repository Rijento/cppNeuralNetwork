#ifndef __NEURALNETWORK_H__
#define __NEURALNETWORK_H__

#include <list>
#include <vector>
#include <string>

#define LayerIterator std::list<Layer*>::iterator
#define RLayerIterator std::list<Layer*>::reverse_iterator

class Synapse;
class Neuron;
class Layer;

class NeuralNetwork
{
protected:
    Layer* inputLayer;
    Layer* outputLayer;
    std::list<Layer*> hiddenLayers;
    Neuron* biasNeuron;
    Layer* getRandomLayer();
    void createInitialConnections();
private:
    int hiddenNeuronCount;
    int inputs;
    int outputs;
public:
    NeuralNetwork(int inputsIn, int outputsIn);
    NeuralNetwork*clone(); // creates a copy of the network
    std::string serialize(); // serializes the network to a string
    void deserialize(std::string dataIn); // loads the network from a serialized string
    void insertLayer(int index);
    std::vector<float> feedForward(std::vector<float> inputData);
    ~NeuralNetwork();
};
#endif
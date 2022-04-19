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

class NeuralNetwork {
// Needs to be firends with all of the training algorithms
friend class BackpropagationTrain;
protected:
    Layer* inputLayer;
    Layer* outputLayer;
    std::list<Layer*> hiddenLayers;
    Neuron* biasNeuron;
    Layer* getRandomLayer();
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
    void addNewNode(int depth); // adds a new node to the hidden layer
    std::vector<float> feedForward(std::vector<float> inputData);
    void createInitialConnections();
    ~NeuralNetwork();
};
#endif